// -*- C++ -*-
// Module:  Log4CPLUS
// File:    loggingevent.h
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

#ifndef LOG4CPLUS_SPI_INTERNAL_LOGGING_EVENT_HEADER_
#define LOG4CPLUS_SPI_INTERNAL_LOGGING_EVENT_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <memory>
#include <log4cplus/loglevel.h>
#include <log4cplus/ndc.h>
#include <log4cplus/mdc.h>
#include <log4cplus/tstring.h>
#include <log4cplus/helpers/timehelper.h>
#include <log4cplus/thread/threads.h>

namespace log4cplus {
    namespace spi {
        /**
         * The internal representation of logging events. When an affirmative
         * decision is made to log then a <code>InternalLoggingEvent</code>
         * instance is created. This instance is passed around to the
         * different log4cplus components.
         *
         * This class is of concern to those wishing to extend log4cplus.
         */
        class LOG4CPLUS_EXPORT InternalLoggingEvent {
        public:
          // Ctors
             /**
              * Instantiate a LoggingEvent from the supplied parameters.
              *
              * @param logger   The logger of this event.
              * @param loglevel The LogLevel of this event.
              * @param message  The message of this event.
              * @param filename Name of file where this event has occurred,
              * can be NULL.
              * @param line     Line number in file specified by
              *                 the <code>filename</code> parameter.
              */
            InternalLoggingEvent(const log4cplus::tstring& logger,
                LogLevel loglevel, const log4cplus::tstring& message,
                const char* filename, int line);

            InternalLoggingEvent(const log4cplus::tstring& logger,
                LogLevel loglevel, const log4cplus::tstring& ndc,
                MappedDiagnosticContextMap const & mdc,
                const log4cplus::tstring& message,
                const log4cplus::tstring& thread,
                log4cplus::helpers::Time time, const log4cplus::tstring& file,
                int line);

            InternalLoggingEvent ();

            InternalLoggingEvent(
                const log4cplus::spi::InternalLoggingEvent& rhs);

            virtual ~InternalLoggingEvent();

            void setLoggingEvent (const log4cplus::tstring & logger,
                LogLevel ll, const log4cplus::tstring & message,
                const char * filename, int line);

            void setFunction (char const * func);
            void setFunction (log4cplus::tstring const &);


          // public virtual methods
            /** The application supplied message of logging event. */
            virtual const log4cplus::tstring& getMessage() const;

            /** Returns the 'type' of InternalLoggingEvent.  Derived classes
             *  should override this method.  (NOTE: Values <= 1000 are
             *  reserved for log4cplus and should not be used.)
             */
            virtual unsigned int getType() const;

           /** Returns a copy of this object.  Derived classes
             *  should override this method.
             */
            virtual std::auto_ptr<InternalLoggingEvent> clone() const;



          // public methods
            /** The logger of the logging event. It is set by
             *  the LoggingEvent constructor.
             */
            const log4cplus::tstring& getLoggerName() const
            {
                return loggerName;
            }

            /** LogLevel of logging event. */
            LogLevel getLogLevel() const
            {
                return ll;
            }

            /** The nested diagnostic context (NDC) of logging event. */
            const log4cplus::tstring& getNDC() const
            {
                if (!ndcCached)
                {
                    ndc = log4cplus::getNDC().get();
                    ndcCached = true;
                }
                return ndc;
            }

            MappedDiagnosticContextMap const & getMDCCopy () const
            {
                if (!mdcCached)
                {
                    mdc = log4cplus::getMDC().getContext ();
                    mdcCached = true;
                }
                return mdc;
            }

            tstring const & getMDC (tstring const & key) const;

            /** The name of thread in which this logging event was generated. */
            const log4cplus::tstring& getThread() const
            {
                if (! threadCached)
                {
                    thread = thread::getCurrentThreadName ();
                    threadCached = true;
                }
                return thread;
            }

            //! The alternative name of thread in which this logging event
            //! was generated.
            const log4cplus::tstring& getThread2() const
            {
                if (! thread2Cached)
                {
                    thread2 = thread::getCurrentThreadName2 ();
                    thread2Cached = true;
                }
                return thread2;
            }


            /** The number of milliseconds elapsed from 1/1/1970 until
             *  logging event was created. */
            const log4cplus::helpers::Time& getTimestamp() const
            {
                return timestamp;
            }

            /** The is the file where this log statement was written */
            const log4cplus::tstring& getFile() const
            {
                return file;
            }

            /** The is the line where this log statement was written */
            int getLine() const { return line; }

            log4cplus::tstring const & getFunction () const
            {
                return function;
            }

            void gatherThreadSpecificData () const;

            void swap (InternalLoggingEvent &);

          // public operators
            log4cplus::spi::InternalLoggingEvent&
            operator=(const log4cplus::spi::InternalLoggingEvent& rhs);

          // static methods
            static unsigned int getDefaultType();

        protected:
          // Data
            log4cplus::tstring message;
            log4cplus::tstring loggerName;
            LogLevel ll;
            mutable log4cplus::tstring ndc;
            mutable MappedDiagnosticContextMap mdc;
            mutable log4cplus::tstring thread;
            mutable log4cplus::tstring thread2;
            log4cplus::helpers::Time timestamp;
            log4cplus::tstring file;
            log4cplus::tstring function;
            int line;
            /** Indicates whether or not the Threadname has been retrieved. */
            mutable bool threadCached;
            mutable bool thread2Cached;
            /** Indicates whether or not the NDC has been retrieved. */
            mutable bool ndcCached;
            /** Indicates whether or not the MDC has been retrieved. */
            mutable bool mdcCached;
        };

    } // end namespace spi
} // end namespace log4cplus

#endif // LOG4CPLUS_SPI_INTERNAL_LOGGING_EVENT_HEADER_
