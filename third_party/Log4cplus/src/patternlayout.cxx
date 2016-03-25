// Module:  Log4CPLUS
// File:    patternlayout.cxx
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2013 Tad E. Smith
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

#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/internal/env.h>
#include <cstdlib>


namespace
{


static
log4cplus::tstring
get_basename (const log4cplus::tstring& filename)
{
#if defined(_WIN32)
    log4cplus::tchar const dir_sep(LOG4CPLUS_TEXT('\\'));
#else
    log4cplus::tchar const dir_sep(LOG4CPLUS_TEXT('/'));
#endif

    log4cplus::tstring::size_type pos = filename.rfind(dir_sep);
    if (pos != log4cplus::tstring::npos)
        return filename.substr(pos+1);
    else
        return filename;
}


} // namespace


namespace log4cplus
{

static tchar const ESCAPE_CHAR = LOG4CPLUS_TEXT('%');

extern void formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event);


namespace pattern
{


/**
 * This is used by PatternConverter class to inform them how to format
 * their output.
 */
struct FormattingInfo {
    int minLen;
    std::size_t maxLen;
    bool leftAlign;
    FormattingInfo() { reset(); }

    void reset();
    void dump(helpers::LogLog&);
};



/**
 * This is the base class of all "Converter" classes that format a
 * field of InternalLoggingEvent objects.  In fact, the PatternLayout
 * class simply uses an array of PatternConverter objects to format
 * and append a logging event.
 */
class PatternConverter
{
public:
    explicit PatternConverter(const FormattingInfo& info);
    virtual ~PatternConverter() {}
    void formatAndAppend(tostream& output, 
        const spi::InternalLoggingEvent& event);

    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event) = 0;

private:
    int minLen;
    std::size_t maxLen;
    bool leftAlign;
};


typedef std::vector<pattern::PatternConverter*> PatternConverterList;


/**
 * This PatternConverter returns a constant string.
 */
class LiteralPatternConverter : public PatternConverter
{
public:
    LiteralPatternConverter(const tstring& str);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent&)
    {
        result = str;
    }

private:
    tstring str;
};


/**
 * This PatternConverter is used to format most of the "simple" fields
 * found in the InternalLoggingEvent object.
 */
class BasicPatternConverter
    : public PatternConverter
{
public:
    enum Type { THREAD_CONVERTER,
                THREAD2_CONVERTER,
                PROCESS_CONVERTER,
                LOGLEVEL_CONVERTER,
                NDC_CONVERTER,
                MESSAGE_CONVERTER,
                NEWLINE_CONVERTER,
                BASENAME_CONVERTER,
                FILE_CONVERTER,
                LINE_CONVERTER,
                FULL_LOCATION_CONVERTER,
                FUNCTION_CONVERTER };
    BasicPatternConverter(const FormattingInfo& info, Type type);
    virtual void convert(tstring & result, 
        const spi::InternalLoggingEvent& event);

private:
  // Disable copy
    BasicPatternConverter(const BasicPatternConverter&);
    BasicPatternConverter& operator=(BasicPatternConverter&);
    
    LogLevelManager& llmCache;
    Type type;
};



/**
 * This PatternConverter is used to format the Logger field found in
 * the InternalLoggingEvent object.
 */
class LoggerPatternConverter : public PatternConverter {
public:
    LoggerPatternConverter(const FormattingInfo& info, int precision);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    int precision;
};



/**
 * This PatternConverter is used to format the timestamp field found in
 * the InternalLoggingEvent object.  It will be formatted according to
 * the specified "pattern".
 */
class DatePatternConverter : public PatternConverter {
public:
    DatePatternConverter(const FormattingInfo& info, 
                         const tstring& pattern, 
                         bool use_gmtime);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    bool use_gmtime;
    tstring format;
};


/**
 * This PatternConverter is used to format an environment variable
 */
class EnvPatternConverter : public PatternConverter {
public:
    EnvPatternConverter(const FormattingInfo& info, 
                        const log4cplus::tstring& env);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    log4cplus::tstring envKey;
};


//! This pattern is used to format miliseconds since process start.
class RelativeTimestampConverter: public PatternConverter {
public:
    RelativeTimestampConverter(const FormattingInfo& info);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);
};


/**
 * This PatternConverter is used to format the hostname field.
 */
class HostnamePatternConverter : public PatternConverter {
public:
    HostnamePatternConverter(const FormattingInfo& info, bool fqdn);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    tstring hostname_;
};


/**
 * This PatternConverter is used to format the MDC field found in
 * the InternalLoggingEvent object, optionally limited to
 * \c k Mapped diagnostic context key.
 */
class MDCPatternConverter
    : public PatternConverter
{
public:
    MDCPatternConverter(const FormattingInfo& info, tstring const & k);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    tstring key;
};


/**
 * This PatternConverter is used to format the NDC field found in
 * the InternalLoggingEvent object, optionally limited to
 * \c precision levels (using space to separate levels).
 */
class NDCPatternConverter : public PatternConverter {
public:
    NDCPatternConverter(const FormattingInfo& info, int precision);
    virtual void convert(tstring & result,
        const spi::InternalLoggingEvent& event);

private:
    int precision;
};



/**
 * This class parses a "pattern" string into an array of
 * PatternConverter objects.
 * <p>
 * @see PatternLayout for the formatting of the "pattern" string.
 */
class PatternParser
{
public:
    PatternParser(const tstring& pattern, unsigned ndcMaxDepth);
    std::vector<PatternConverter*> parse();

private:
  // Types
    enum ParserState { LITERAL_STATE, 
                       CONVERTER_STATE,
                       DOT_STATE,
                       MIN_STATE,
                       MAX_STATE };

  // Methods
    tstring extractOption();
    int extractPrecisionOption();
    void finalizeConverter(tchar c);

  // Data
    tstring pattern;
    FormattingInfo formattingInfo;
    std::vector<PatternConverter*> list;
    ParserState state;
    tstring::size_type pos;
    tstring currentLiteral;
    unsigned ndcMaxDepth;
};


////////////////////////////////////////////////
// FormattingInfo methods:
////////////////////////////////////////////////

void 
FormattingInfo::reset() {
    minLen = -1;
    maxLen = 0x7FFFFFFF;
    leftAlign = false;
}


void 
FormattingInfo::dump(helpers::LogLog& loglog) {
    tostringstream buf;
    buf << LOG4CPLUS_TEXT("min=") << minLen
        << LOG4CPLUS_TEXT(", max=") << maxLen
        << LOG4CPLUS_TEXT(", leftAlign=") << std::boolalpha << leftAlign;
    loglog.debug(buf.str());
}




////////////////////////////////////////////////
// PatternConverter methods:
////////////////////////////////////////////////

PatternConverter::PatternConverter(const FormattingInfo& i)
{
    minLen = i.minLen;
    maxLen = i.maxLen;
    leftAlign = i.leftAlign;
}



void
PatternConverter::formatAndAppend(
    tostream& output, const spi::InternalLoggingEvent& event)
{
    tstring & s = internal::get_ptd ()->faa_str;
    convert (s, event);
    std::size_t len = s.length();

    if (len > maxLen)
        output << s.substr(len - maxLen);
    else if (static_cast<int>(len) < minLen)
    {
        std::ios_base::fmtflags const original_flags = output.flags ();
        tchar const fill = output.fill (LOG4CPLUS_TEXT(' '));
        output.setf (leftAlign ? std::ios_base::left : std::ios_base::right,
            std::ios_base::adjustfield);
        output.width (minLen);
        output << s;
        output.fill (fill);
        output.flags (original_flags);
    }
    else
        output << s;
}



////////////////////////////////////////////////
// LiteralPatternConverter methods:
////////////////////////////////////////////////

LiteralPatternConverter::LiteralPatternConverter(
    const tstring& str_)
    : PatternConverter(FormattingInfo())
    , str(str_)
{
}



////////////////////////////////////////////////
// BasicPatternConverter methods:
////////////////////////////////////////////////

BasicPatternConverter::BasicPatternConverter(
    const FormattingInfo& info, Type type_)
    : PatternConverter(info)
    , llmCache(getLogLevelManager())
    , type(type_)
{
}



void
BasicPatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    switch(type)
    {
    case LOGLEVEL_CONVERTER:
        result = llmCache.toString(event.getLogLevel());
        return;

    case BASENAME_CONVERTER:
        result = get_basename(event.getFile());
        return;

    case PROCESS_CONVERTER:
        helpers::convertIntegerToString(result, internal::get_process_id ()); 
        return;

    case NDC_CONVERTER:
        result = event.getNDC();
        return;

    case MESSAGE_CONVERTER:
        result = event.getMessage();
        return;

    case NEWLINE_CONVERTER:
        result = LOG4CPLUS_TEXT("\n");
        return; 

    case FILE_CONVERTER:
        result = event.getFile();
        return;

    case THREAD_CONVERTER:
        result = event.getThread();
        return;

    case THREAD2_CONVERTER:
        result = event.getThread2();
        return;

    case LINE_CONVERTER:
        {
            if(event.getLine() != -1)
                helpers::convertIntegerToString(result, event.getLine());
            else
                result.clear ();
            return;
        }

    case FULL_LOCATION_CONVERTER:
        {
            tstring const & file = event.getFile();
            if (! file.empty ())
            {
                result = file;
                result += LOG4CPLUS_TEXT(":");
                result += helpers::convertIntegerToString(event.getLine());
            }
            else
                result = LOG4CPLUS_TEXT(":");
            return;
        }
        
    case FUNCTION_CONVERTER:
        result = event.getFunction ();
        return;
    }

    result = LOG4CPLUS_TEXT("INTERNAL LOG4CPLUS ERROR");
}



////////////////////////////////////////////////
// LoggerPatternConverter methods:
////////////////////////////////////////////////

LoggerPatternConverter::LoggerPatternConverter(
    const FormattingInfo& info, int prec)
    : PatternConverter(info)
    , precision(prec)
{
}



void
LoggerPatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    const tstring& name = event.getLoggerName();
    if (precision <= 0) {
        result = name;
    }
    else {
        std::size_t len = name.length();

        // We substract 1 from 'len' when assigning to 'end' to avoid out of
        // bounds exception in return r.substring(end+1, len). This can happen
        // if precision is 1 and the logger name ends with a dot. 
        tstring::size_type end = len - 1;
        for (int i = precision; i > 0; --i)
        {
            end = name.rfind(LOG4CPLUS_TEXT('.'), end - 1);
            if(end == tstring::npos) {
                result = name;
                return;
            }
        }
        result = name.substr(end + 1);
    }
}



////////////////////////////////////////////////
// DatePatternConverter methods:
////////////////////////////////////////////////


DatePatternConverter::DatePatternConverter(
    const FormattingInfo& info, const tstring& pattern,
    bool use_gmtime_)
    : PatternConverter(info)
    , use_gmtime(use_gmtime_)
    , format(pattern)
{
}



void
DatePatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent& event)
{
    result = event.getTimestamp().getFormattedTime(format, use_gmtime);
}


////////////////////////////////////////////////
// EnvPatternConverter methods:
////////////////////////////////////////////////


EnvPatternConverter::EnvPatternConverter(
    const FormattingInfo& info, const tstring& env)
    : PatternConverter(info)
    , envKey(env)
{ }


void
EnvPatternConverter::convert(tstring & result,
    const spi::InternalLoggingEvent&)
{
    if (! internal::get_env_var (result, envKey))
    {
        // Variable doesn't exist, return empty string.
        result.clear ();
    }
}


//
//
//

RelativeTimestampConverter::RelativeTimestampConverter (FormattingInfo const & info)
    : PatternConverter (info)
{ }


void
RelativeTimestampConverter::convert (tstring & result,
    spi::InternalLoggingEvent const & event)
{
    tostringstream & oss = internal::get_ptd ()->layout_oss;
    detail::clear_tostringstream (oss);
    log4cplus::formatRelativeTimestamp (oss, event);
    oss.str ().swap (result);
}


////////////////////////////////////////////////
// HostnamePatternConverter methods:
////////////////////////////////////////////////

HostnamePatternConverter::HostnamePatternConverter (
    const FormattingInfo& info, bool fqdn)
    : PatternConverter(info)
    , hostname_ (helpers::getHostname (fqdn))
{ }


void
HostnamePatternConverter::convert (
    tstring & result, const spi::InternalLoggingEvent&)
{
    result = hostname_;
}



////////////////////////////////////////////////
// MDCPatternConverter methods:
////////////////////////////////////////////////

log4cplus::pattern::MDCPatternConverter::MDCPatternConverter (
    const FormattingInfo& info, tstring const & k)
    : PatternConverter(info)
    , key (k)
{ }


void
log4cplus::pattern::MDCPatternConverter::convert (tstring & result,
    const spi::InternalLoggingEvent& event)
{
    if (!key.empty())
    {
        result = event.getMDC (key);
    }
    else
    {
        result.clear ();

        MappedDiagnosticContextMap const & mdcMap = event.getMDCCopy();
                
        for (MappedDiagnosticContextMap::const_iterator it = mdcMap.begin(); 
             it != mdcMap.end(); ++it)
        {           
            tstring const & name(it->first);
            tstring const & value(it->second);

            result += LOG4CPLUS_TEXT("{");
            result += name;
            result += LOG4CPLUS_TEXT(", ");
            result += value;
            result += LOG4CPLUS_TEXT("}"); 
            
        }
    }
}


////////////////////////////////////////////////
// NDCPatternConverter methods:
////////////////////////////////////////////////

log4cplus::pattern::NDCPatternConverter::NDCPatternConverter (
    const FormattingInfo& info, int precision_)
    : PatternConverter(info)
    , precision(precision_)
{ }


void
log4cplus::pattern::NDCPatternConverter::convert (tstring & result,
    const spi::InternalLoggingEvent& event)
{
    const log4cplus::tstring& text = event.getNDC();
    if (precision <= 0)
        result = text;
    else
    {
        tstring::size_type p = text.find(LOG4CPLUS_TEXT(' '));
        for (int i = 1; i < precision && p != tstring::npos; ++i)
            p = text.find(LOG4CPLUS_TEXT(' '), p + 1);

        result = text.substr(0, p);
    }
}



////////////////////////////////////////////////
// PatternParser methods:
////////////////////////////////////////////////

PatternParser::PatternParser(
    const tstring& pattern_, unsigned ndcMaxDepth_)
    : pattern(pattern_)
    , state(LITERAL_STATE)
    , pos(0)
    , ndcMaxDepth (ndcMaxDepth_)
{
}



tstring 
PatternParser::extractOption() 
{
    if (   (pos < pattern.length()) 
        && (pattern[pos] == LOG4CPLUS_TEXT('{'))) 
    {
        tstring::size_type end = pattern.find_first_of(LOG4CPLUS_TEXT('}'), pos);
        if (end != tstring::npos) {
            tstring r = pattern.substr(pos + 1, end - pos - 1);
            pos = end + 1;
            return r;
        }
        else {
            log4cplus::tostringstream buf;
            buf << LOG4CPLUS_TEXT("No matching '}' found in conversion pattern string \"")
                << pattern
                << LOG4CPLUS_TEXT("\"");
            helpers::getLogLog().error(buf.str());
            pos = pattern.length();
        }
    }

    return LOG4CPLUS_TEXT("");
}


int 
PatternParser::extractPrecisionOption() 
{
    tstring opt = extractOption();
    int r = 0;
    if (! opt.empty ())
        r = std::atoi(LOG4CPLUS_TSTRING_TO_STRING(opt).c_str());

    return r;
}



PatternConverterList
PatternParser::parse() 
{
    tchar c;
    pos = 0;
    while(pos < pattern.length()) {
        c = pattern[pos++];
        switch (state) {
        case LITERAL_STATE :
            // In literal state, the last char is always a literal.
            if(pos == pattern.length()) {
                currentLiteral += c;
                continue;
            }
            if(c == ESCAPE_CHAR) {
                // peek at the next char. 
                switch (pattern[pos]) {
                case ESCAPE_CHAR:
                    currentLiteral += c;
                    pos++; // move pointer
                    break;
                default:
                    if(! currentLiteral.empty ()) {
                        list.push_back
                             (new LiteralPatternConverter(currentLiteral));
                        //getLogLog().debug("Parsed LITERAL converter: \"" 
                        //                  +currentLiteral+"\".");
                    }
                    currentLiteral.resize(0);
                    currentLiteral += c; // append %
                    state = CONVERTER_STATE;
                    formattingInfo.reset();
                }
            }
            else {
                currentLiteral += c;
            }
            break;

        case CONVERTER_STATE:
            currentLiteral += c;
            switch (c) {
            case LOG4CPLUS_TEXT('-'):
                formattingInfo.leftAlign = true;
                break;
            case LOG4CPLUS_TEXT('.'):
                state = DOT_STATE;
                break;
            default:
                if(c >= LOG4CPLUS_TEXT('0') && c <= LOG4CPLUS_TEXT('9')) {
                    formattingInfo.minLen = c - LOG4CPLUS_TEXT('0');
                    state = MIN_STATE;
                }
                else {
                    finalizeConverter(c);
                }
            } // switch
            break;

        case MIN_STATE:
            currentLiteral += c;
            if (c >= LOG4CPLUS_TEXT('0') && c <= LOG4CPLUS_TEXT('9')) {
                formattingInfo.minLen = formattingInfo.minLen * 10 + (c - LOG4CPLUS_TEXT('0'));
            }
            else if(c == LOG4CPLUS_TEXT('.')) {
                state = DOT_STATE;
            }
            else {
                finalizeConverter(c);
            }
            break;

        case DOT_STATE:
            currentLiteral += c;
            if(c >= LOG4CPLUS_TEXT('0') && c <= LOG4CPLUS_TEXT('9')) {
                formattingInfo.maxLen = c - LOG4CPLUS_TEXT('0');
                state = MAX_STATE;
            }
            else {
                tostringstream buf;
                buf << LOG4CPLUS_TEXT("Error occured in position ")
                    << pos
                    << LOG4CPLUS_TEXT(".\n Was expecting digit, instead got char \"")
                    << c
                    << LOG4CPLUS_TEXT("\".");
                helpers::getLogLog().error(buf.str());
                state = LITERAL_STATE;
            }
            break;

         case MAX_STATE:
            currentLiteral += c;
            if (c >= LOG4CPLUS_TEXT('0') && c <= LOG4CPLUS_TEXT('9'))
                formattingInfo.maxLen = formattingInfo.maxLen * 10 + (c - LOG4CPLUS_TEXT('0'));
            else {
                finalizeConverter(c);
                state = LITERAL_STATE;
            }
            break;
        } // end switch
    } // end while

    if(! currentLiteral.empty ()) {
        list.push_back(new LiteralPatternConverter(currentLiteral));
      //getLogLog().debug("Parsed LITERAL converter: \""+currentLiteral+"\".");
    }

    return list;
}



void
PatternParser::finalizeConverter(tchar c) 
{
    PatternConverter* pc = 0;
    switch (c) {
        case LOG4CPLUS_TEXT('b'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::BASENAME_CONVERTER);
            //getLogLog().debug("BASENAME converter.");
            //formattingInfo.dump(getLogLog());      
            break;
            
        case LOG4CPLUS_TEXT('c'):
            pc = new LoggerPatternConverter(formattingInfo, 
                                            extractPrecisionOption());
            //getLogLog().debug( LOG4CPLUS_TEXT("LOGGER converter.") );
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('d'):
        case LOG4CPLUS_TEXT('D'):
            {
                tstring dOpt = extractOption();
                if(dOpt.empty ()) {
                    dOpt = LOG4CPLUS_TEXT("%Y-%m-%d %H:%M:%S");
                }
                bool use_gmtime = c == LOG4CPLUS_TEXT('d');
                pc = new DatePatternConverter(formattingInfo, dOpt, use_gmtime);
                //if(use_gmtime) {
                //    getLogLog().debug("GMT DATE converter.");
                //}
                //else {
                //    getLogLog().debug("LOCAL DATE converter.");
                //}
                //formattingInfo.dump(getLogLog());      
            }
            break;

        case LOG4CPLUS_TEXT('E'):
            pc = new EnvPatternConverter(formattingInfo, extractOption());
            //getLogLog().debug("Environment converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('F'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::FILE_CONVERTER);
            //getLogLog().debug("FILE NAME converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('h'):
        case LOG4CPLUS_TEXT('H'):
            {
                bool fqdn = (c == LOG4CPLUS_TEXT('H'));
                pc = new HostnamePatternConverter(formattingInfo, fqdn);
                // getLogLog().debug( LOG4CPLUS_TEXT("HOSTNAME converter.") );
                // formattingInfo.dump(getLogLog());
            }
            break;

        case LOG4CPLUS_TEXT('i'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::PROCESS_CONVERTER);
            //getLogLog().debug("PROCESS_CONVERTER converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('l'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::FULL_LOCATION_CONVERTER);
            //getLogLog().debug("FULL LOCATION converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('L'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::LINE_CONVERTER);
            //getLogLog().debug("LINE NUMBER converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('m'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::MESSAGE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('M'):
            pc = new BasicPatternConverter (
                formattingInfo, BasicPatternConverter::FUNCTION_CONVERTER);
            //getLogLog().debug("METHOD (function name) converter.");
            //formattingInfo.dump(getLogLog());   
            break;

        case LOG4CPLUS_TEXT('n'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::NEWLINE_CONVERTER);
            //getLogLog().debug("MESSAGE converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('p'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::LOGLEVEL_CONVERTER);
            //getLogLog().debug("LOGLEVEL converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case LOG4CPLUS_TEXT('r'):
            pc = new RelativeTimestampConverter (formattingInfo);
            //getLogLog().debug("RELATIVE converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case LOG4CPLUS_TEXT('t'):
            pc = new BasicPatternConverter
                          (formattingInfo, 
                           BasicPatternConverter::THREAD_CONVERTER);
            //getLogLog().debug("THREAD converter.");
            //formattingInfo.dump(getLogLog());      
            break;

        case LOG4CPLUS_TEXT('T'):
            pc = new BasicPatternConverter
                          (formattingInfo,
                           BasicPatternConverter::THREAD2_CONVERTER);
            //getLogLog().debug("THREAD2 converter.");
            //formattingInfo.dump(getLogLog());
            break;

        case LOG4CPLUS_TEXT('x'):
            pc = new NDCPatternConverter (formattingInfo, ndcMaxDepth);
            //getLogLog().debug("NDC converter.");      
            break;

        case LOG4CPLUS_TEXT('X'):
            pc = new MDCPatternConverter (formattingInfo, extractOption ());
            //getLogLog().debug("MDC converter.");
            break;

        default:
            tostringstream buf;
            buf << LOG4CPLUS_TEXT("Unexpected char [")
                << c
                << LOG4CPLUS_TEXT("] at position ")
                << pos
                << LOG4CPLUS_TEXT(" in conversion patterrn.");
            helpers::getLogLog().error(buf.str());
            pc = new LiteralPatternConverter(currentLiteral);
    }

    list.push_back(pc);
    currentLiteral.resize(0);
    state = LITERAL_STATE;
    formattingInfo.reset();
}


} // namespace pattern


typedef pattern::PatternConverterList PatternConverterList;


////////////////////////////////////////////////
// PatternLayout methods:
////////////////////////////////////////////////

PatternLayout::PatternLayout(const tstring& pattern_)
{
    init(pattern_, 0);
}


PatternLayout::PatternLayout(const helpers::Properties& properties)
{
    unsigned ndcMaxDepth = 0;
    properties.getUInt (ndcMaxDepth, LOG4CPLUS_TEXT ("NDCMaxDepth"));

    bool hasPattern = properties.exists( LOG4CPLUS_TEXT("Pattern") );
    bool hasConversionPattern = properties.exists( LOG4CPLUS_TEXT("ConversionPattern") );
    
    if(hasPattern) {
        helpers::getLogLog().warn(
            LOG4CPLUS_TEXT("PatternLayout- the \"Pattern\" property has been")
            LOG4CPLUS_TEXT(" deprecated.  Use \"ConversionPattern\" instead."));
    }
    
    if(hasConversionPattern) {
        init(properties.getProperty( LOG4CPLUS_TEXT("ConversionPattern") ),
            ndcMaxDepth);
    }
    else if(hasPattern) {
        init(properties.getProperty( LOG4CPLUS_TEXT("Pattern") ), ndcMaxDepth);
    }
    else {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT ("ConversionPattern not specified in properties"),
            true);
    }

}


void
PatternLayout::init(const tstring& pattern_, unsigned ndcMaxDepth)
{
    pattern = pattern_;
    parsedPattern = pattern::PatternParser(pattern, ndcMaxDepth).parse();

    // Let's validate that our parser didn't give us any NULLs.  If it did,
    // we will convert them to a valid PatternConverter that does nothing so
    // at least we don't core.
    for(PatternConverterList::iterator it=parsedPattern.begin(); 
        it!=parsedPattern.end(); 
        ++it)
    {
        if( (*it) == 0 ) {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Parsed Pattern created a NULL PatternConverter"));
            (*it) = new pattern::LiteralPatternConverter( LOG4CPLUS_TEXT("") );
        }
    }
    if(parsedPattern.empty ()) {
        helpers::getLogLog().warn(
            LOG4CPLUS_TEXT("PatternLayout pattern is empty.  Using default..."));
        parsedPattern.push_back (
            new pattern::BasicPatternConverter(pattern::FormattingInfo(), 
            pattern::BasicPatternConverter::MESSAGE_CONVERTER));
    }
}



PatternLayout::~PatternLayout()
{
    for(PatternConverterList::iterator it=parsedPattern.begin(); 
        it!=parsedPattern.end(); 
        ++it)
    {
        delete (*it);
    }
}



void
PatternLayout::formatAndAppend(tostream& output, 
                               const spi::InternalLoggingEvent& event)
{
    for(PatternConverterList::iterator it=parsedPattern.begin(); 
        it!=parsedPattern.end(); 
        ++it)
    {
        (*it)->formatAndAppend(output, event);
    }
}


} // namespace log4cplus
