// Module:  Log4CPLUS
// File:    loggingevent.cxx
// Created: 6/2003
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

#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/internal/internal.h>
#include <algorithm>


namespace log4cplus {  namespace spi {


static const int LOG4CPLUS_DEFAULT_TYPE = 1;


///////////////////////////////////////////////////////////////////////////////
// InternalLoggingEvent ctors and dtor
///////////////////////////////////////////////////////////////////////////////

InternalLoggingEvent::InternalLoggingEvent(const log4cplus::tstring& logger,
    LogLevel loglevel, const log4cplus::tstring& message_, const char* filename,
    int line_)
    : message(message_)
    , loggerName(logger)
    , ll(loglevel)
    , ndc()
    , mdc()
    , thread()
    , timestamp(log4cplus::helpers::Time::gettimeofday())
    , file(filename
        ? LOG4CPLUS_C_STR_TO_TSTRING(filename) 
        : log4cplus::tstring())
    , function ()
    , line(line_)
    , threadCached(false)
    , thread2Cached(false)
    , ndcCached(false)
    , mdcCached(false)
{
}


InternalLoggingEvent::InternalLoggingEvent(const log4cplus::tstring& logger,
    LogLevel loglevel, const log4cplus::tstring& ndc_,
    MappedDiagnosticContextMap const & mdc_, const log4cplus::tstring& message_,
    const log4cplus::tstring& thread_, log4cplus::helpers::Time time,
    const log4cplus::tstring& file_, int line_)
    : message(message_)
    , loggerName(logger)
    , ll(loglevel)
    , ndc(ndc_)
    , mdc(mdc_)
    , thread(thread_)
    , timestamp(time)
    , file(file_)
    , function ()
    , line(line_)
    , threadCached(true)
    , thread2Cached(true)
    , ndcCached(true)
    , mdcCached(true)
{
}


InternalLoggingEvent::InternalLoggingEvent ()
    : ll (NOT_SET_LOG_LEVEL)
    , function ()
    , line (0)
    , threadCached(false)
    , thread2Cached(false)
    , ndcCached(false)
    , mdcCached(false)
{ }


InternalLoggingEvent::InternalLoggingEvent(
    const log4cplus::spi::InternalLoggingEvent& rhs)
    : message(rhs.getMessage())
    , loggerName(rhs.getLoggerName())
    , ll(rhs.getLogLevel())
    , ndc(rhs.getNDC())
    , mdc(rhs.getMDCCopy())
    , thread(rhs.getThread())
    , timestamp(rhs.getTimestamp())
    , file(rhs.getFile())
    , function(rhs.getFunction())
    , line(rhs.getLine())
    , threadCached(true)
    , thread2Cached(true)
    , ndcCached(true)
    , mdcCached(true)
{
}


InternalLoggingEvent::~InternalLoggingEvent()
{
}



///////////////////////////////////////////////////////////////////////////////
// InternalLoggingEvent static methods
///////////////////////////////////////////////////////////////////////////////

unsigned int
InternalLoggingEvent::getDefaultType()
{
    return LOG4CPLUS_DEFAULT_TYPE;
}



///////////////////////////////////////////////////////////////////////////////
// InternalLoggingEvent implementation
///////////////////////////////////////////////////////////////////////////////

void
InternalLoggingEvent::setLoggingEvent (const log4cplus::tstring & logger,
    LogLevel loglevel, const log4cplus::tstring & msg, const char * filename,
    int fline)
{
    // This could be imlemented using the swap idiom:
    //
    // InternalLoggingEvent (logger, loglevel, msg, filename, fline).swap (*this);
    //
    // But that defeats the optimization of using thread local instance
    // of InternalLoggingEvent to avoid memory allocation.

    loggerName = logger;
    ll = loglevel;
    message = msg;
    timestamp = helpers::Time::gettimeofday();
    if (filename)
        file = LOG4CPLUS_C_STR_TO_TSTRING (filename);
    else
        file.clear ();
    line = fline;
    threadCached = false;
    thread2Cached = false;
    ndcCached = false;
    mdcCached = false;
}


void
InternalLoggingEvent::setFunction (char const * func)
{
    function = LOG4CPLUS_C_STR_TO_TSTRING (func);
}


void
InternalLoggingEvent::setFunction (log4cplus::tstring const & func)
{
    function = func;
}


const log4cplus::tstring& 
InternalLoggingEvent::getMessage() const
{
    return message;
}


unsigned int
InternalLoggingEvent::getType() const
{
    return LOG4CPLUS_DEFAULT_TYPE;
}



std::auto_ptr<InternalLoggingEvent>
InternalLoggingEvent::clone() const
{
    std::auto_ptr<InternalLoggingEvent> tmp(new InternalLoggingEvent(*this));
    return tmp;
}


tstring const &
InternalLoggingEvent::getMDC (tstring const & key) const
{
    MappedDiagnosticContextMap const & mdc_ = getMDCCopy ();
    MappedDiagnosticContextMap::const_iterator it = mdc_.find (key);
    if (it != mdc_.end ())
        return it->second;
    else
        return internal::empty_str;
}



InternalLoggingEvent &
InternalLoggingEvent::operator = (const InternalLoggingEvent& rhs)
{
    InternalLoggingEvent (rhs).swap (*this);
    return *this;
}


void
InternalLoggingEvent::gatherThreadSpecificData () const
{
    getNDC ();
    getMDCCopy ();
    getThread ();
    getThread2 ();
}


void
InternalLoggingEvent::swap (InternalLoggingEvent & other)
{
    using std::swap;

    swap (message, other.message);
    swap (loggerName, other.loggerName);
    swap (ll, other.ll);
    swap (ndc, other.ndc);
    swap (mdc, other.mdc);
    swap (thread, other.thread);
    swap (thread2, other.thread2);
    swap (timestamp, other.timestamp);
    swap (file, other.file);
    swap (function, other.function);
    swap (line, other.line);
    swap (threadCached, other.threadCached);
    swap (ndcCached, other.ndcCached);
}


} } // namespace log4cplus {  namespace spi {
