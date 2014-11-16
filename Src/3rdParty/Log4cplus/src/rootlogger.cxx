// Module:  Log4CPLUS
// File:    rootlogger.cxx
// Created: 4/2003
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

#include <log4cplus/spi/rootlogger.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus { namespace spi {


//////////////////////////////////////////////////////////////////////////////
// RootLogger Constructor
//////////////////////////////////////////////////////////////////////////////

RootLogger::RootLogger(Hierarchy& h, LogLevel loglevel)
: LoggerImpl(LOG4CPLUS_TEXT("root"), h)
{
    setLogLevel(loglevel);
}



//////////////////////////////////////////////////////////////////////////////
// Logger Methods
//////////////////////////////////////////////////////////////////////////////

LogLevel 
RootLogger::getChainedLogLevel() const
{
    return ll;
}


void 
RootLogger::setLogLevel(LogLevel loglevel)
{
    if(loglevel == NOT_SET_LOG_LEVEL) {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT("You have tried to set NOT_SET_LOG_LEVEL to root."));
    }
    else {
        LoggerImpl::setLogLevel(loglevel);
    }
}


} } // namespace log4cplus { namespace spi {
