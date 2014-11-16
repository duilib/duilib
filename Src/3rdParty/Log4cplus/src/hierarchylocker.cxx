// Module:  Log4CPLUS
// File:    hierarchylocker.cxx
// Created: 8/2003
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

#include <log4cplus/hierarchylocker.h>
#include <log4cplus/hierarchy.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggerimpl.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus
{


//////////////////////////////////////////////////////////////////////////////
// HierarchyLocker ctor and dtor
//////////////////////////////////////////////////////////////////////////////

HierarchyLocker::HierarchyLocker(Hierarchy& _h)
: h(_h),
  hierarchyLocker(h.hashtable_mutex),
  loggerList()
{
    // Get a copy of all of the Hierarchy's Loggers (except the Root Logger)
    h.initializeLoggerList(loggerList);

    // Lock all of the Hierarchy's Loggers' mutexs
    LoggerList::iterator it;
    try
    {
        for (it = loggerList.begin(); it != loggerList.end(); ++it)
            it->value->appender_list_mutex.lock ();
    }
    catch (...)
    {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT("HierarchyLocker::ctor()")
            LOG4CPLUS_TEXT("- An error occurred while locking"));
        LoggerList::iterator range_end = it;
        for (it = loggerList.begin (); it != range_end; ++it)
            it->value->appender_list_mutex.unlock ();
        throw;
    }
}
 

HierarchyLocker::~HierarchyLocker()
{
    try {
        for(LoggerList::iterator it=loggerList.begin(); it!=loggerList.end(); ++it) {
            it->value->appender_list_mutex.unlock ();
        }
    }
    catch(...) {
        helpers::getLogLog().error(LOG4CPLUS_TEXT("HierarchyLocker::dtor()- An error occurred while unlocking"));
        throw;
    }
}

void 
HierarchyLocker::resetConfiguration()
{
    Logger root = h.getRoot();
    h.disable(Hierarchy::DISABLE_OFF);

    // begin by closing nested appenders
    // then, remove all appenders
    root.setLogLevel(DEBUG_LOG_LEVEL);
    root.closeNestedAppenders();
    root.removeAllAppenders();

    // repeat
    for(LoggerList::iterator it=loggerList.begin(); it!=loggerList.end(); ++it)
    {
        Logger & logger = *it;
        
        logger.closeNestedAppenders();
        logger.removeAllAppenders();
        
        logger.setLogLevel(NOT_SET_LOG_LEVEL);
        logger.setAdditivity(true);
    }
}


Logger 
HierarchyLocker::getInstance(const tstring& name)
{
    return h.getInstanceImpl(name, *h.getLoggerFactory());
}


Logger 
HierarchyLocker::getInstance(const tstring& name, spi::LoggerFactory& factory)
{
    return h.getInstanceImpl(name, factory);
}


void 
HierarchyLocker::addAppender(Logger& logger, SharedAppenderPtr& appender)
{
    for(LoggerList::iterator it=loggerList.begin(); it!=loggerList.end(); ++it) {
        if((*it).value == logger.value) {
            logger.value->appender_list_mutex.unlock ();
            logger.addAppender(appender);
            logger.value->appender_list_mutex.lock ();
            return;
        }
    }
    
    // I don't have this Logger locked
    logger.addAppender(appender);
}


} // namespace log4cplus
