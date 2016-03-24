// Module:  Log4CPLUS
// File:    filter.cxx
// Created: 5/2003
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

#include <log4cplus/spi/filter.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus { namespace spi {

///////////////////////////////////////////////////////////////////////////////
// global methods
///////////////////////////////////////////////////////////////////////////////

FilterResult
checkFilter(const Filter* filter, const InternalLoggingEvent& event)
{
    const Filter* currentFilter = filter;
    while(currentFilter) {
        FilterResult result = currentFilter->decide(event);
        if(result != NEUTRAL) {
            return result;
        }

        currentFilter = currentFilter->next.get();
    }

    return ACCEPT;
}



///////////////////////////////////////////////////////////////////////////////
// Filter implementation
///////////////////////////////////////////////////////////////////////////////

Filter::Filter()
{
}


Filter::~Filter()
{
}


void
Filter::appendFilter(FilterPtr filter)
{
    if (! next)
        next = filter;
    else
        next->appendFilter(filter);
}



///////////////////////////////////////////////////////////////////////////////
// DenyAllFilter implementation
///////////////////////////////////////////////////////////////////////////////

DenyAllFilter::DenyAllFilter ()
{ }


DenyAllFilter::DenyAllFilter (const helpers::Properties&)
{ }


FilterResult
DenyAllFilter::decide(const InternalLoggingEvent&) const
{
    return DENY;
}



///////////////////////////////////////////////////////////////////////////////
// LogLevelMatchFilter implementation
///////////////////////////////////////////////////////////////////////////////

LogLevelMatchFilter::LogLevelMatchFilter()
{
    init();
}



LogLevelMatchFilter::LogLevelMatchFilter(const helpers::Properties& properties)
{
    init();

    properties.getBool (acceptOnMatch = false,
        LOG4CPLUS_TEXT("AcceptOnMatch"));

    tstring const & log_level_to_match
        = properties.getProperty( LOG4CPLUS_TEXT("LogLevelToMatch") );
    logLevelToMatch = getLogLevelManager().fromString(log_level_to_match);
}


void
LogLevelMatchFilter::init()
{
    acceptOnMatch = true;
    logLevelToMatch = NOT_SET_LOG_LEVEL;
}


FilterResult
LogLevelMatchFilter::decide(const InternalLoggingEvent& event) const
{
    if(logLevelToMatch == NOT_SET_LOG_LEVEL) {
        return NEUTRAL;
    }

    bool matchOccured = (logLevelToMatch == event.getLogLevel());
       
    if(matchOccured) {
        return (acceptOnMatch ? ACCEPT : DENY);
    }
    else {
        return NEUTRAL;
    }
}



///////////////////////////////////////////////////////////////////////////////
// LogLevelRangeFilter implementation
///////////////////////////////////////////////////////////////////////////////

LogLevelRangeFilter::LogLevelRangeFilter()
{
    init();
}



LogLevelRangeFilter::LogLevelRangeFilter(const helpers::Properties& properties)
{
    init();

    properties.getBool (acceptOnMatch = false,
        LOG4CPLUS_TEXT("AcceptOnMatch"));

    tstring const & log_level_min
        = properties.getProperty( LOG4CPLUS_TEXT("LogLevelMin") );
    logLevelMin = getLogLevelManager().fromString(log_level_min);

    tstring const & log_level_max
        = properties.getProperty( LOG4CPLUS_TEXT("LogLevelMax") );
    logLevelMax = getLogLevelManager().fromString(log_level_max);
}


void
LogLevelRangeFilter::init()
{
    acceptOnMatch = true;
    logLevelMin = NOT_SET_LOG_LEVEL;
    logLevelMax = NOT_SET_LOG_LEVEL;
}


FilterResult
LogLevelRangeFilter::decide(const InternalLoggingEvent& event) const
{
    if((logLevelMin != NOT_SET_LOG_LEVEL) && (event.getLogLevel() < logLevelMin)) {
        // priority of event is less than minimum
        return DENY;
    }

    if((logLevelMax != NOT_SET_LOG_LEVEL) && (event.getLogLevel() > logLevelMax)) {
        // priority of event is greater than maximum
        return DENY;
    }

    if(acceptOnMatch) {
        // this filter set up to bypass later filters and always return
        // accept if priority in range
        return ACCEPT;
    }
    else {
        // event is ok for this filter; allow later filters to have a look...
        return NEUTRAL;
    }
}



///////////////////////////////////////////////////////////////////////////////
// StringMatchFilter implementation
///////////////////////////////////////////////////////////////////////////////

StringMatchFilter::StringMatchFilter()
{
    init();
}



StringMatchFilter::StringMatchFilter(const helpers::Properties& properties)
{
    init();

    properties.getBool (acceptOnMatch = false,
        LOG4CPLUS_TEXT("AcceptOnMatch"));
    stringToMatch = properties.getProperty( LOG4CPLUS_TEXT("StringToMatch") );
}


void
StringMatchFilter::init()
{
    acceptOnMatch = true;
}


FilterResult
StringMatchFilter::decide(const InternalLoggingEvent& event) const
{
    const tstring& message = event.getMessage();

    if(stringToMatch.empty () || message.empty ()) {
        return NEUTRAL;
    }

    if(message.find(stringToMatch) == tstring::npos) {
        return NEUTRAL;
    }
    else {  // we've got a match
        return (acceptOnMatch ? ACCEPT : DENY);
    }
}


} } // namespace log4cplus { namespace spi {
