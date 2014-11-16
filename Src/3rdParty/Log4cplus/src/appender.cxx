// Module:  Log4CPLUS
// File:    appender.cxx
// Created: 6/2001
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

#include <log4cplus/appender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/pointer.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <stdexcept>


namespace log4cplus
{


///////////////////////////////////////////////////////////////////////////////
// log4cplus::ErrorHandler dtor
///////////////////////////////////////////////////////////////////////////////

ErrorHandler::ErrorHandler ()
{ }


ErrorHandler::~ErrorHandler()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::OnlyOnceErrorHandler 
///////////////////////////////////////////////////////////////////////////////

OnlyOnceErrorHandler::OnlyOnceErrorHandler()
    : firstTime(true)
{ }


OnlyOnceErrorHandler::~OnlyOnceErrorHandler ()
{ }


void
OnlyOnceErrorHandler::error(const log4cplus::tstring& err)
{
    if(firstTime) {
        helpers::getLogLog().error(err);
        firstTime = false;
    }
}



void
OnlyOnceErrorHandler::reset()
{
    firstTime = true;
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::Appender ctors
///////////////////////////////////////////////////////////////////////////////

Appender::Appender()
 : layout(new SimpleLayout()),
   name( LOG4CPLUS_TEXT("") ),
   threshold(NOT_SET_LOG_LEVEL),
   errorHandler(new OnlyOnceErrorHandler),
   useLockFile(false),
   closed(false)
{
}



Appender::Appender(const log4cplus::helpers::Properties & properties)
    : layout(new SimpleLayout())
    , name()
    , threshold(NOT_SET_LOG_LEVEL)
    , errorHandler(new OnlyOnceErrorHandler)
    , useLockFile(false)
    , closed(false)
{
    if(properties.exists( LOG4CPLUS_TEXT("layout") ))
    {
        log4cplus::tstring const & factoryName
            = properties.getProperty( LOG4CPLUS_TEXT("layout") );
        spi::LayoutFactory* factory
            = spi::getLayoutFactoryRegistry().get(factoryName);
        if(factory == 0) {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Cannot find LayoutFactory: \"")
                + factoryName
                + LOG4CPLUS_TEXT("\"") );
            return;
        }

        helpers::Properties layoutProperties =
                properties.getPropertySubset( LOG4CPLUS_TEXT("layout.") );
        try {
            std::auto_ptr<Layout> newLayout(factory->createObject(layoutProperties));
            if(newLayout.get() == 0) {
                helpers::getLogLog().error(
                    LOG4CPLUS_TEXT("Failed to create appender: ")
                    + factoryName);
            }
            else {
                layout = newLayout;
            }
        }
        catch(std::exception const & e) {
            helpers::getLogLog().error( 
                LOG4CPLUS_TEXT("Error while creating Layout: ")
                + LOG4CPLUS_C_STR_TO_TSTRING(e.what()));
            return;
        }

    }

    // Support for appender.Threshold in properties configuration file
    if(properties.exists(LOG4CPLUS_TEXT("Threshold"))) {
        tstring tmp = properties.getProperty(LOG4CPLUS_TEXT("Threshold"));
        tmp = log4cplus::helpers::toUpper(tmp);
        threshold = log4cplus::getLogLevelManager().fromString(tmp);
    }

    // Configure the filters
    helpers::Properties filterProps
        = properties.getPropertySubset( LOG4CPLUS_TEXT("filters.") );
    unsigned filterCount = 0;
    spi::FilterPtr filterChain;
    tstring filterName;
    while (filterProps.exists(
        filterName = helpers::convertIntegerToString (++filterCount)))
    {
        tstring const & factoryName = filterProps.getProperty(filterName);
        spi::FilterFactory* factory
            = spi::getFilterFactoryRegistry().get(factoryName);

        if(! factory)
        {
            tstring err = LOG4CPLUS_TEXT("Appender::ctor()- Cannot find FilterFactory: ");
            helpers::getLogLog().error(err + factoryName);
            continue;
        }
        spi::FilterPtr tmpFilter = factory->createObject (
            filterProps.getPropertySubset(filterName + LOG4CPLUS_TEXT(".")));
        if (! tmpFilter)
        {
            tstring err = LOG4CPLUS_TEXT("Appender::ctor()- Failed to create filter: ");
            helpers::getLogLog().error(err + filterName);
        }
        if (! filterChain)
            filterChain = tmpFilter;
        else
            filterChain->appendFilter(tmpFilter);
    }
    setFilter(filterChain);

    properties.getBool (useLockFile, LOG4CPLUS_TEXT("UseLockFile"));
    if (useLockFile)
    {
        tstring const & lockFileName
            = properties.getProperty (LOG4CPLUS_TEXT ("LockFile"));
        if (! lockFileName.empty ())
        {
            try
            {
                lockFile.reset (new helpers::LockFile (lockFileName));
            }
            catch (std::runtime_error const &)
            {
                return;
            }
        }
        else
        {
            helpers::getLogLog ().debug (
                LOG4CPLUS_TEXT (
                    "UseLockFile is true but LockFile is not specified"));
        }
    }
}


Appender::~Appender()
{
    helpers::LogLog & loglog = helpers::getLogLog ();

    loglog.debug(LOG4CPLUS_TEXT("Destroying appender named [") + name
        + LOG4CPLUS_TEXT("]."));

    if (! closed)
        loglog.error (
            LOG4CPLUS_TEXT ("Derived Appender did not call destructorImpl()."));
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::Appender public methods
///////////////////////////////////////////////////////////////////////////////

void
Appender::destructorImpl()
{
    // An appender might be closed then destroyed. There is no point
    // in closing twice. It can actually be a wrong thing to do, e.g.,
    // files get rolled more than once.
    if (closed)
        return;

    close();
    closed = true;
}


bool Appender::isClosed() const
{
    return closed;
}


void
Appender::doAppend(const log4cplus::spi::InternalLoggingEvent& event)
{
    thread::MutexGuard guard (access_mutex);

    if(closed) {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT("Attempted to append to closed appender named [")
            + name
            + LOG4CPLUS_TEXT("]."));
        return;
    }

    // Check appender's threshold logging level.

    if (! isAsSevereAsThreshold(event.getLogLevel()))
        return;

    // Evaluate filters attached to this appender.

    if (checkFilter(filter.get(), event) == spi::DENY)
        return;

    // Lock system wide lock.

    helpers::LockFileGuard lfguard;
    if (useLockFile && lockFile.get ())
    {
        try
        {
            lfguard.attach_and_lock (*lockFile);
        }
        catch (std::runtime_error const &)
        {
            return;
        }
    }

    // Finally append given event.

    append(event);
}


tstring &
Appender::formatEvent (const spi::InternalLoggingEvent& event) const
{
    internal::appender_sratch_pad & appender_sp = internal::get_appender_sp ();
    detail::clear_tostringstream (appender_sp.oss);
    layout->formatAndAppend(appender_sp.oss, event);
    appender_sp.oss.str().swap (appender_sp.str);
    return appender_sp.str;
}


log4cplus::tstring
Appender::getName()
{
    return name;
}



void
Appender::setName(const log4cplus::tstring& n)
{
    this->name = n;
}


ErrorHandler*
Appender::getErrorHandler()
{
    return errorHandler.get();
}



void
Appender::setErrorHandler(std::auto_ptr<ErrorHandler> eh)
{
    if (! eh.get())
    {
        // We do not throw exception here since the cause is probably a
        // bad config file.
        helpers::getLogLog().warn(
            LOG4CPLUS_TEXT("You have tried to set a null error-handler."));
        return;
    }

    thread::MutexGuard guard (access_mutex);

    this->errorHandler = eh;
}



void
Appender::setLayout(std::auto_ptr<Layout> lo)
{
    thread::MutexGuard guard (access_mutex);

    this->layout = lo;
}



Layout*
Appender::getLayout()
{
    return layout.get();
}


} // namespace log4cplus
