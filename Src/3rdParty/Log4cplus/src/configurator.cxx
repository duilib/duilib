// Module:  LOG4CPLUS
// File:    configurator.cxx
// Created: 3/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2013 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <log4cplus/configurator.h>
#include <log4cplus/hierarchylocker.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/sleep.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/spi/loggerimpl.h>
#include <log4cplus/internal/env.h>

#ifdef LOG4CPLUS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef LOG4CPLUS_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef LOG4CPLUS_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if defined (_WIN32)
#include <tchar.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <functional>


namespace log4cplus
{


void initializeLog4cplus();


namespace
{
    static tchar const DELIM_START[] = LOG4CPLUS_TEXT("${");
    static tchar const DELIM_STOP[] = LOG4CPLUS_TEXT("}");
    static std::size_t const DELIM_START_LEN = 2;
    static std::size_t const DELIM_STOP_LEN = 1;


    /**
     * Perform variable substitution in string <code>val</code> from
     * environment variables.
     *
     * <p>The variable substitution delimeters are <b>${</b> and <b>}</b>.
     *
     * <p>For example, if the System properties contains "key=value", then
     * the call
     * <pre>
     * string s;
     * substEnvironVars(s, "Value of key is ${key}.");
     * </pre>
     *
     * will set the variable <code>s</code> to "Value of key is value.".
     *
     * <p>If no value could be found for the specified key, then
     * substitution defaults to the empty string.
     *
     * <p>For example, if there is no environment variable "inexistentKey",
     * then the call
     *
     * <pre>
     * string s;
     * substEnvironVars(s, "Value of inexistentKey is [${inexistentKey}]");
     * </pre>
     * will set <code>s</code> to "Value of inexistentKey is []"
     *
     * @param val The string on which variable substitution is performed.
     * @param dest The result.
     */
    static
    bool
    substVars (tstring & dest, const tstring & val,
        helpers::Properties const & props, helpers::LogLog& loglog,
        unsigned flags)
    {
        tstring::size_type i = 0;
        tstring::size_type var_start, var_end;
        tstring pattern (val);
        tstring key;
        tstring replacement;
        bool changed = false;
        bool const empty_vars
            = !! (flags & PropertyConfigurator::fAllowEmptyVars);
        bool const shadow_env
            = !! (flags & PropertyConfigurator::fShadowEnvironment);
        bool const rec_exp
            = !! (flags & PropertyConfigurator::fRecursiveExpansion);

        while (true)
        {
            // Find opening paren of variable substitution.
            var_start = pattern.find(DELIM_START, i);
            if (var_start == tstring::npos)
            {
                dest = pattern;
                return changed;
            }

            // Find closing paren of variable substitution.
            var_end = pattern.find(DELIM_STOP, var_start);
            if (var_end == tstring::npos)
            {
                tostringstream buffer;
                buffer << '"' << pattern
                       << "\" has no closing brace. "
                       << "Opening brace at position " << var_start << ".";
                loglog.error(buffer.str());
                dest = val;
                return false;
            }
            
            key.assign (pattern, var_start + DELIM_START_LEN,
                var_end - (var_start + DELIM_START_LEN));
            replacement.clear ();
            if (shadow_env)
                replacement = props.getProperty (key);
            if (! shadow_env || (! empty_vars && replacement.empty ()))
                internal::get_env_var (replacement, key);
            
            if (empty_vars || ! replacement.empty ())
            {
                // Substitute the variable with its value in place.
                pattern.replace (var_start, var_end - var_start + DELIM_STOP_LEN,
                    replacement);
                changed = true;
                if (rec_exp)
                    // Retry expansion on the same spot.
                    continue;
                else
                    // Move beyond the just substituted part.
                    i = var_start + replacement.size ();
            }
            else
                // Nothing has been subtituted, just move beyond the
                // unexpanded variable.
                i = var_end + DELIM_STOP_LEN;
        } // end while loop

    } // end substVars()


    //! Translates encoding in ProtpertyConfigurator::PCFlags
    //! to helpers::Properties::PFlags
    static
    unsigned
    pcflag_to_pflags_encoding (unsigned pcflags)
    {
        switch (pcflags
            & (PropertyConfigurator::fEncodingMask
                << PropertyConfigurator::fEncodingShift))
        {
#if defined (LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) && defined (UNICODE)
        case PropertyConfigurator::fUTF8:
            return helpers::Properties::fUTF8;
#endif

#if (defined (LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) || defined (WIN32)) \
    && defined (UNICODE)
        case PropertyConfigurator::fUTF16:
            return helpers::Properties::fUTF16;
#endif

#if defined (LOG4CPLUS_HAVE_CODECVT_UTF32_FACET) && defined (UNICODE)
        case PropertyConfigurator::fUTF32:
            return helpers::Properties::fUTF32;
#endif

        case PropertyConfigurator::fUnspecEncoding:;
        default:
            return 0;
        }
    }

} // namespace



//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator ctor and dtor
//////////////////////////////////////////////////////////////////////////////

PropertyConfigurator::PropertyConfigurator(const tstring& propertyFile,
    Hierarchy& hier, unsigned f)
    : h(hier)
    , propertyFilename(propertyFile)
    , properties(propertyFile, pcflag_to_pflags_encoding (f))
    , flags (f)
{
    init();
}


PropertyConfigurator::PropertyConfigurator(const helpers::Properties& props,
    Hierarchy& hier, unsigned f)
    : h(hier)
    , propertyFilename( LOG4CPLUS_TEXT("UNAVAILABLE") )
    , properties( props )
    , flags (f)
{
    init();
}


PropertyConfigurator::PropertyConfigurator(tistream& propertyStream,
    Hierarchy& hier, unsigned f)
    : h(hier)
    , propertyFilename( LOG4CPLUS_TEXT("UNAVAILABLE") )
    , properties(propertyStream)
    , flags (f)
{
    init();
}


void
PropertyConfigurator::init()
{
    replaceEnvironVariables();
    properties = properties.getPropertySubset( LOG4CPLUS_TEXT("log4cplus.") );
}


PropertyConfigurator::~PropertyConfigurator()
{
}



//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator static methods
//////////////////////////////////////////////////////////////////////////////

void
PropertyConfigurator::doConfigure(const tstring& file, Hierarchy& h,
    unsigned flags)
{
    PropertyConfigurator tmp(file, h, flags);
    tmp.configure();
}



//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator public methods
//////////////////////////////////////////////////////////////////////////////

void
PropertyConfigurator::configure()
{
    // Configure log4cplus internals.
    bool internal_debugging = false;
    if (properties.getBool (internal_debugging, LOG4CPLUS_TEXT ("configDebug")))
        helpers::getLogLog ().setInternalDebugging (internal_debugging);

    bool quiet_mode = false;
    if (properties.getBool (quiet_mode, LOG4CPLUS_TEXT ("quietMode")))
        helpers::getLogLog ().setQuietMode (quiet_mode);

    bool disable_override = false;
    properties.getBool (disable_override, LOG4CPLUS_TEXT ("disableOverride"));

    initializeLog4cplus();
    configureAppenders();
    configureLoggers();
    configureAdditivity();

    if (disable_override)
        h.disable (Hierarchy::DISABLE_OVERRIDE);

    // Erase the appenders so that we are not artificially keeping them "alive".
    appenders.clear ();
}


helpers::Properties const &
PropertyConfigurator::getProperties () const
{
    return properties;
}


log4cplus::tstring const &
PropertyConfigurator::getPropertyFilename () const
{
    return propertyFilename;
}


//////////////////////////////////////////////////////////////////////////////
// PropertyConfigurator protected methods
//////////////////////////////////////////////////////////////////////////////

void
PropertyConfigurator::reconfigure()
{
    properties = helpers::Properties(propertyFilename);
    init();
    configure();
}


void
PropertyConfigurator::replaceEnvironVariables()
{
    tstring val, subKey, subVal;
    std::vector<tstring> keys;
    bool const rec_exp
        = !! (flags & PropertyConfigurator::fRecursiveExpansion);
    bool changed;

    do 
    {
        changed = false;
        properties.propertyNames().swap (keys);
        for (std::vector<tstring>::const_iterator it = keys.begin();
            it != keys.end(); ++it)
        {
            tstring const & key = *it;
            val = properties.getProperty(key);

            subKey.clear ();
            if (substVars(subKey, key, properties, helpers::getLogLog(), flags))
            {
                properties.removeProperty(key);
                properties.setProperty(subKey, val);
                changed = true;
            }

            subVal.clear ();
            if (substVars(subVal, val, properties, helpers::getLogLog(), flags))
            {
                properties.setProperty(subKey, subVal);
                changed = true;
            }
        }
    }
    while (changed && rec_exp);
}



void
PropertyConfigurator::configureLoggers()
{
    if(properties.exists( LOG4CPLUS_TEXT("rootLogger") ))
    {
        Logger root = h.getRoot();
        configureLogger(root,
                        properties.getProperty(LOG4CPLUS_TEXT("rootLogger")));
    }

    helpers::Properties loggerProperties
        = properties.getPropertySubset(LOG4CPLUS_TEXT("logger."));
    std::vector<tstring> loggers = loggerProperties.propertyNames();
    for(std::vector<tstring>::iterator it=loggers.begin(); it!=loggers.end();
        ++it)
    {
        Logger log = getLogger(*it);
        configureLogger(log, loggerProperties.getProperty(*it));
    }
}



void
PropertyConfigurator::configureLogger(Logger logger, const tstring& config)
{
    // Remove all spaces from config
    tstring configString;
    std::remove_copy_if(config.begin(), config.end(),
        std::back_inserter (configString),
        std::bind1st(std::equal_to<tchar>(), LOG4CPLUS_TEXT(' ')));

    // "Tokenize" configString
    std::vector<tstring> tokens;
    helpers::tokenize(configString, LOG4CPLUS_TEXT(','),
        std::back_insert_iterator<std::vector<tstring> >(tokens));

    if (tokens.empty ())
    {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT("PropertyConfigurator::configureLogger()")
            LOG4CPLUS_TEXT("- Invalid config string(Logger = ")
            + logger.getName()
            + LOG4CPLUS_TEXT("): \"")
            + config
            + LOG4CPLUS_TEXT("\""));
        return;
    }

    // Set the loglevel
    tstring const & loglevel = tokens[0];
    if (loglevel != LOG4CPLUS_TEXT("INHERITED"))
        logger.setLogLevel( getLogLevelManager().fromString(loglevel) );
    else
        logger.setLogLevel (NOT_SET_LOG_LEVEL);

    // Remove all existing appenders first so that we do not duplicate output.
    logger.removeAllAppenders ();

    // Set the Appenders
    for(std::vector<tstring>::size_type j=1; j<tokens.size(); ++j)
    {
        AppenderMap::iterator appenderIt = appenders.find(tokens[j]);
        if (appenderIt == appenders.end())
        {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("PropertyConfigurator::configureLogger()")
                LOG4CPLUS_TEXT("- Invalid appender: ")
                + tokens[j]);
            continue;
        }
        addAppender(logger, appenderIt->second);
    }
}



void
PropertyConfigurator::configureAppenders()
{
    helpers::Properties appenderProperties =
        properties.getPropertySubset(LOG4CPLUS_TEXT("appender."));
    std::vector<tstring> appendersProps = appenderProperties.propertyNames();
    tstring factoryName;
    for(std::vector<tstring>::iterator it=appendersProps.begin();
        it != appendersProps.end(); ++it)
    {
        if( it->find( LOG4CPLUS_TEXT('.') ) == tstring::npos )
        {
            factoryName = appenderProperties.getProperty(*it);
            spi::AppenderFactory* factory 
                = spi::getAppenderFactoryRegistry().get(factoryName);
            if (! factory)
            {
                tstring err =
                    LOG4CPLUS_TEXT("PropertyConfigurator::configureAppenders()")
                    LOG4CPLUS_TEXT("- Cannot find AppenderFactory: ");
                helpers::getLogLog().error(err + factoryName);
                continue;
            }

            helpers::Properties props_subset
                = appenderProperties.getPropertySubset((*it)
                + LOG4CPLUS_TEXT("."));
            try
            {
                SharedAppenderPtr appender
                    = factory->createObject(props_subset);
                if (! appender)
                {
                    tstring err =
                        LOG4CPLUS_TEXT("PropertyConfigurator::")
                        LOG4CPLUS_TEXT("configureAppenders()")
                        LOG4CPLUS_TEXT("- Failed to create appender: ");
                    helpers::getLogLog().error(err + *it);
                }
                else
                {
                    appender->setName(*it);
                    appenders[*it] = appender;
                }
            }
            catch(std::exception const & e)
            {
                tstring err =
                    LOG4CPLUS_TEXT("PropertyConfigurator::")
                    LOG4CPLUS_TEXT("configureAppenders()")
                    LOG4CPLUS_TEXT("- Error while creating Appender: ");
                helpers::getLogLog().error(err + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
            }
        }
    } // end for loop
}


void
PropertyConfigurator::configureAdditivity()
{
    helpers::Properties additivityProperties =
        properties.getPropertySubset(LOG4CPLUS_TEXT("additivity."));
    std::vector<tstring> additivitysProps
        = additivityProperties.propertyNames();

    for(std::vector<tstring>::const_iterator it = additivitysProps.begin();
        it != additivitysProps.end(); ++it)
    {
        Logger logger = getLogger(*it);
        bool additivity;
        if (additivityProperties.getBool (additivity, *it))
            logger.setAdditivity (additivity);
    }
}



Logger
PropertyConfigurator::getLogger(const tstring& name)
{
    return h.getInstance(name);
}


void
PropertyConfigurator::addAppender(Logger &logger, SharedAppenderPtr& appender)
{
    logger.addAppender(appender);
}



//////////////////////////////////////////////////////////////////////////////
// BasicConfigurator ctor and dtor
//////////////////////////////////////////////////////////////////////////////

log4cplus::tstring DISABLE_OVERRIDE_KEY (
    LOG4CPLUS_TEXT ("log4cplus.disableOverride"));

BasicConfigurator::BasicConfigurator(Hierarchy& hier, bool logToStdErr)
    : PropertyConfigurator( LOG4CPLUS_TEXT(""), hier )
{
    properties.setProperty(LOG4CPLUS_TEXT("rootLogger"),
                           LOG4CPLUS_TEXT("DEBUG, STDOUT"));
    properties.setProperty(LOG4CPLUS_TEXT("appender.STDOUT"),
                           LOG4CPLUS_TEXT("log4cplus::ConsoleAppender"));
    properties.setProperty(LOG4CPLUS_TEXT("appender.STDOUT.logToStdErr"),
                           logToStdErr ? LOG4CPLUS_TEXT("1")
                           : LOG4CPLUS_TEXT("0"));
}




BasicConfigurator::~BasicConfigurator()
{
}


//////////////////////////////////////////////////////////////////////////////
// BasicConfigurator static methods
//////////////////////////////////////////////////////////////////////////////

void
BasicConfigurator::doConfigure(Hierarchy& h, bool logToStdErr)
{
    BasicConfigurator tmp(h, logToStdErr);
    tmp.configure();
}


#if !defined(LOG4CPLUS_SINGLE_THREADED)

//////////////////////////////////////////////////////////////////////////////
// ConfigurationWatchDogThread implementation
//////////////////////////////////////////////////////////////////////////////

class ConfigurationWatchDogThread 
    : public thread::AbstractThread,
      public PropertyConfigurator
{
public:
    ConfigurationWatchDogThread(const tstring& file, unsigned int millis)
        : PropertyConfigurator(file)
        , waitMillis(millis < 1000 ? 1000 : millis)
        , shouldTerminate(false)
        , lock(NULL)
    {
        lastFileInfo.mtime = helpers::Time::gettimeofday ();
        lastFileInfo.size = 0;
        lastFileInfo.is_link = false;

        updateLastModInfo();
    }

    virtual ~ConfigurationWatchDogThread ()
    { }
    
    void terminate ()
    {
        shouldTerminate.signal ();
        join ();
    }

protected:
    virtual void run();
    virtual Logger getLogger(const tstring& name);
    virtual void addAppender(Logger &logger, SharedAppenderPtr& appender);
    
    bool checkForFileModification();
    void updateLastModInfo();
    
private:
    ConfigurationWatchDogThread (ConfigurationWatchDogThread const &);
    ConfigurationWatchDogThread & operator = (
        ConfigurationWatchDogThread const &);

    unsigned int const waitMillis;
    thread::ManualResetEvent shouldTerminate;
    helpers::FileInfo lastFileInfo;
    HierarchyLocker* lock;
};


void
ConfigurationWatchDogThread::run()
{
    while (! shouldTerminate.timed_wait (waitMillis))
    {
        bool modified = checkForFileModification();
        if(modified) {
            // Lock the Hierarchy
            HierarchyLocker theLock(h);
            lock = &theLock;

            // reconfigure the Hierarchy
            theLock.resetConfiguration();
            reconfigure();
            updateLastModInfo();

            // release the lock
            lock = NULL;
        }
    }
}


Logger
ConfigurationWatchDogThread::getLogger(const tstring& name)
{
    if(lock)
        return lock->getInstance(name);
    else
        return PropertyConfigurator::getLogger(name);
}


void
ConfigurationWatchDogThread::addAppender(Logger& logger,
                                         SharedAppenderPtr& appender)
{
    if(lock)
        lock->addAppender(logger, appender);
    else
        PropertyConfigurator::addAppender(logger, appender);
}


bool
ConfigurationWatchDogThread::checkForFileModification()
{
    helpers::FileInfo fi;

    if (helpers::getFileInfo (&fi, propertyFilename) != 0)
        return false;

    bool modified = fi.mtime > lastFileInfo.mtime
        || fi.size != lastFileInfo.size;

#if defined(LOG4CPLUS_HAVE_LSTAT)
    if (!modified && fi.is_link)
    {
        struct stat fileStatus;
        if (lstat(LOG4CPLUS_TSTRING_TO_STRING(propertyFilename).c_str(),
                &fileStatus) == -1)
            return false;

        helpers::Time linkModTime(fileStatus.st_mtime);
        modified = (linkModTime > fi.mtime);
    }
#endif

    return modified;
}



void
ConfigurationWatchDogThread::updateLastModInfo()
{
    helpers::FileInfo fi;

    if (helpers::getFileInfo (&fi, propertyFilename) == 0)
        lastFileInfo = fi;
}



//////////////////////////////////////////////////////////////////////////////
// PropertyConfiguratorWatchDog ctor and dtor
//////////////////////////////////////////////////////////////////////////////

ConfigureAndWatchThread::ConfigureAndWatchThread(const tstring& file,
    unsigned int millis)
    : watchDogThread(0)
{
    watchDogThread = new ConfigurationWatchDogThread(file, millis);
    watchDogThread->addReference ();
    watchDogThread->configure();
    watchDogThread->start();
}


ConfigureAndWatchThread::~ConfigureAndWatchThread()
{
    if (watchDogThread)
    {
        watchDogThread->terminate();
        watchDogThread->removeReference ();
    }
}


#endif


} // namespace log4cplus
