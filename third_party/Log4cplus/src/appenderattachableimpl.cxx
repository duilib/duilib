// Module:  Log4CPLUS
// File:    appenderattachableimpl.cxx
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


#include <log4cplus/appender.h>
#include <log4cplus/helpers/appenderattachableimpl.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/thread/syncprims-pub-impl.h>

#include <algorithm>


namespace log4cplus
{


namespace spi
{


AppenderAttachable::~AppenderAttachable()
{ }


} // namespace spi


namespace helpers
{


//////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl ctor and dtor
//////////////////////////////////////////////////////////////////////////////

AppenderAttachableImpl::AppenderAttachableImpl()
{ }


AppenderAttachableImpl::~AppenderAttachableImpl()
{ }



///////////////////////////////////////////////////////////////////////////////
// log4cplus::helpers::AppenderAttachableImpl public methods
///////////////////////////////////////////////////////////////////////////////

void
AppenderAttachableImpl::addAppender(SharedAppenderPtr newAppender)
{
    if(newAppender == NULL) {
        getLogLog().warn( LOG4CPLUS_TEXT("Tried to add NULL appender") );
        return;
    }

    thread::MutexGuard guard (appender_list_mutex);

    ListType::iterator it = 
        std::find(appenderList.begin(), appenderList.end(), newAppender);
    if(it == appenderList.end()) {
        appenderList.push_back(newAppender);
    }
}



AppenderAttachableImpl::ListType
AppenderAttachableImpl::getAllAppenders()
{
    thread::MutexGuard guard (appender_list_mutex);
    
    return appenderList;
}



SharedAppenderPtr 
AppenderAttachableImpl::getAppender(const log4cplus::tstring& name)
{
    thread::MutexGuard guard (appender_list_mutex);

    for(ListType::iterator it=appenderList.begin(); 
        it!=appenderList.end(); 
        ++it)
    {
        if((*it)->getName() == name) {
            return *it;
        }
    }

    return SharedAppenderPtr(NULL);
}



void 
AppenderAttachableImpl::removeAllAppenders()
{
    thread::MutexGuard guard (appender_list_mutex);

    appenderList.erase(appenderList.begin(), appenderList.end());
}



void 
AppenderAttachableImpl::removeAppender(SharedAppenderPtr appender)
{
    if(appender == NULL) {
        getLogLog().warn( LOG4CPLUS_TEXT("Tried to remove NULL appender") );
        return;
    }

    thread::MutexGuard guard (appender_list_mutex);

    ListType::iterator it =
        std::find(appenderList.begin(), appenderList.end(), appender);
    if(it != appenderList.end()) {
        appenderList.erase(it);
    }
}



void 
AppenderAttachableImpl::removeAppender(const log4cplus::tstring& name)
{
    removeAppender(getAppender(name));
}



int 
AppenderAttachableImpl::appendLoopOnAppenders(const spi::InternalLoggingEvent& event) const
{
    int count = 0;

    thread::MutexGuard guard (appender_list_mutex);

    for(ListType::const_iterator it=appenderList.begin();
        it!=appenderList.end();
        ++it)
    {
        ++count;
        (*it)->doAppend(event);
    }

    return count;
}


} // namespace helpers


} // namespace log4cplus
