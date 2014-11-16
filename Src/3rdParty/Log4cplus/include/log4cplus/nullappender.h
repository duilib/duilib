// -*- C++ -*-
// Module:  Log4CPLUS
// File:    nullappender.h
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

/** @file */

#ifndef LOG4CPLUS_NULL_APPENDER_HEADER_
#define LOG4CPLUS_NULL_APPENDER_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/appender.h>


namespace log4cplus {

    /**
     * Appends log events to a file. 
     */
    class LOG4CPLUS_EXPORT NullAppender : public Appender {
    public:
      // Ctors
        NullAppender();
        NullAppender(const log4cplus::helpers::Properties&);

      // Dtor
        virtual ~NullAppender();

      // Methods
        virtual void close();

    protected:
        virtual void append(const log4cplus::spi::InternalLoggingEvent& event);

    private:
      // Disallow copying of instances of this class
        NullAppender(const NullAppender&);
        NullAppender& operator=(const NullAppender&);
    };

} // end namespace log4cplus

#endif // LOG4CPLUS_NULL_APPENDER_HEADER_

