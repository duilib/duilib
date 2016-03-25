// -*- C++ -*-
// Module:  Log4CPLUS
// File:    loggerfactory.h
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

#ifndef LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER
#define LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>


namespace log4cplus {
    // Forward Declarations
    class Logger;
    class Hierarchy;

    namespace spi {
        /**
         * Implement this interface to create new instances of Logger or
         * a sub-class of Logger.
         */
        class LOG4CPLUS_EXPORT LoggerFactory {
        public:
            /**
             * Creates a new <code>Logger</code> object.
             */
            virtual Logger makeNewLoggerInstance(const log4cplus::tstring& name,
                                                 Hierarchy& h) = 0; 
            virtual ~LoggerFactory() = 0;
        };

    } // end namespace spi
} // end namespace log4cplus

#endif // LOG4CPLUS_SPI_LOGGER_FACTORY_HEADER

