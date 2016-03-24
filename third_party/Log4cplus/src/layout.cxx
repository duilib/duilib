// Module:  Log4CPLUS
// File:    layout.cxx
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
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/internal/internal.h>
#include <ostream>
#include <iomanip>


namespace log4cplus
{

void
formatRelativeTimestamp (log4cplus::tostream & output,
    log4cplus::spi::InternalLoggingEvent const & event)
{
    helpers::Time const rel_time
        = event.getTimestamp () - getTTCCLayoutTimeBase ();
    tchar const old_fill = output.fill ();
    helpers::time_t const sec = rel_time.sec ();
 
    if (sec != 0)
        output << sec << std::setfill (LOG4CPLUS_TEXT ('0')) << std::setw (3);
 
    output << rel_time.usec () / 1000;
    output.fill (old_fill);
}

//
//
//


Layout::Layout ()
    : llmCache(getLogLevelManager())
{ }


Layout::Layout (const log4cplus::helpers::Properties&)
    : llmCache(getLogLevelManager())
{ }


Layout::~Layout()
{ }


///////////////////////////////////////////////////////////////////////////////
// log4cplus::SimpleLayout public methods
///////////////////////////////////////////////////////////////////////////////

SimpleLayout::SimpleLayout ()
{ }


SimpleLayout::SimpleLayout (const helpers::Properties& properties)
    : Layout (properties)
{ }


SimpleLayout::~SimpleLayout()
{ }


void
SimpleLayout::formatAndAppend(log4cplus::tostream& output, 
                              const log4cplus::spi::InternalLoggingEvent& event)
{
    output << llmCache.toString(event.getLogLevel()) 
           << LOG4CPLUS_TEXT(" - ")
           << event.getMessage() 
           << LOG4CPLUS_TEXT("\n");
}



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout ctors and dtor
///////////////////////////////////////////////////////////////////////////////

TTCCLayout::TTCCLayout(bool use_gmtime_)
    : dateFormat()
    , use_gmtime(use_gmtime_)
{
}


TTCCLayout::TTCCLayout(const log4cplus::helpers::Properties& properties)
    : Layout(properties)
    , dateFormat(properties.getProperty (LOG4CPLUS_TEXT("DateFormat"),
            internal::empty_str))
    , use_gmtime(false)
{
    properties.getBool (use_gmtime, LOG4CPLUS_TEXT("Use_gmtime"));
}


TTCCLayout::~TTCCLayout()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::TTCCLayout public methods
///////////////////////////////////////////////////////////////////////////////

void
TTCCLayout::formatAndAppend(log4cplus::tostream& output, 
                            const log4cplus::spi::InternalLoggingEvent& event)
{
     if (dateFormat.empty ())
         formatRelativeTimestamp (output, event);
     else
         output << event.getTimestamp().getFormattedTime(dateFormat,
             use_gmtime);

    output << LOG4CPLUS_TEXT(" [")
           << event.getThread()
           << LOG4CPLUS_TEXT("] ")
           << llmCache.toString(event.getLogLevel()) 
           << LOG4CPLUS_TEXT(" ")
           << event.getLoggerName()
           << LOG4CPLUS_TEXT(" <")
           << event.getNDC() 
           << LOG4CPLUS_TEXT("> - ")
           << event.getMessage()
           << LOG4CPLUS_TEXT("\n");
}


} // namespace log4cplus
