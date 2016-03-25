// -*- C++ -*-
// Module:  Log4CPLUS
// File:    nteventlogappender.h
// Created: 4/2003
// Author:  Michael CATANZARITI
//
// Copyright 2003-2013 Michael CATANZARITI
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

/** @file */

#ifndef LOG4CPLUS_NT_EVENT_LOG_APPENDER_HEADER_
#define LOG4CPLUS_NT_EVENT_LOG_APPENDER_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if defined (LOG4CPLUS_HAVE_NT_EVENT_LOG)

#include <log4cplus/appender.h>
#include <log4cplus/config/windowsh-inc.h>


namespace log4cplus {

    /**
     * Appends log events to NT EventLog. 
     */
    class LOG4CPLUS_EXPORT NTEventLogAppender : public Appender {
    public:
      // ctors
        NTEventLogAppender(const log4cplus::tstring& server, 
                           const log4cplus::tstring& log, 
                           const log4cplus::tstring& source);
        NTEventLogAppender(const log4cplus::helpers::Properties & properties);

      // dtor
        virtual ~NTEventLogAppender();

      // public Methods
        virtual void close();

    protected:
        virtual void append(const spi::InternalLoggingEvent& event);
        virtual WORD getEventType(const spi::InternalLoggingEvent& event);
        virtual WORD getEventCategory(const spi::InternalLoggingEvent& event);
        void init();

        /*
         * Add this source with appropriate configuration keys to the registry.
         */
        void addRegistryInfo();

      // Data
        log4cplus::tstring server;
        log4cplus::tstring log;
        log4cplus::tstring source;
        HANDLE hEventLog;
        SID* pCurrentUserSID;

    private:
      // Disallow copying of instances of this class
        NTEventLogAppender(const NTEventLogAppender&);
        NTEventLogAppender& operator=(const NTEventLogAppender&);
    };

} // end namespace log4cplus

#endif // LOG4CPLUS_HAVE_NT_EVENT_LOG
#endif //LOG4CPLUS_NT_EVENT_LOG_APPENDER_HEADER_
