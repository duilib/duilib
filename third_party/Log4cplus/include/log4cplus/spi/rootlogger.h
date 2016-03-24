// -*- C++ -*-
// Module:  Log4CPLUS
// File:    rootlogger.h
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

/** @file */

#ifndef LOG4CPLUS_SPI_ROOT_LOGGER_HEADER_
#define LOG4CPLUS_SPI_ROOT_LOGGER_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/spi/loggerimpl.h>

namespace log4cplus {
    namespace spi {

        /**
         * RootLogger sits at the top of the logger hierachy. It is a
         * regular logger except that it provides several guarantees.
         *
         * First, it cannot be assigned a <code>NOT_SET_LOG_LEVEL</code>
         * LogLevel. Second, since root logger cannot have a parent, the
         * getChainedLogLevel method always returns the value of the
         * ll field without walking the hierarchy.
         */
        class LOG4CPLUS_EXPORT RootLogger : public LoggerImpl {
        public:
          // Ctors
            /**
             * The root logger names itself as "root". However, the root
             * logger cannot be retrieved by name.  
             */
            RootLogger(Hierarchy& h, LogLevel ll);

          // Methods
            /**
             * Return the assigned LogLevel value without walking the logger
             * hierarchy.
             */
            virtual LogLevel getChainedLogLevel() const;

            /**
             * Setting a NOT_SET_LOG_LEVEL value to the LogLevel of the root logger 
             * may have catastrophic results. We prevent this here.
             */
            void setLogLevel(LogLevel ll);

        };

    } // end namespace spi
} // end namespace log4cplus

#endif // LOG4CPLUS_SPI_ROOT_LOGGER_HEADER_

