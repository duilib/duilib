// -*- C++ -*-
// Module:  Log4CPLUS
// File:    loglog.h
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

#ifndef LOG4CPLUS_HELPERS_LOGLOG
#define LOG4CPLUS_HELPERS_LOGLOG

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>
#include <log4cplus/streams.h>
#include <log4cplus/thread/syncprims.h>


namespace log4cplus {
    namespace helpers {

        /**
         * This class used to output log statements from within the log4cplus package.
         *
         * Log4cplus components cannot make log4cplus logging calls. However, it is
         * sometimes useful for the user to learn about what log4cplus is
         * doing. You can enable log4cplus internal logging by defining the
         * <b>log4cplus.configDebug</b> variable.
         *
         * All log4cplus internal debug calls go to <code>cout</code>
         * where as internal error messages are sent to
         * <code>cerr</code>. All internal messages are prepended with
         * the string "log4clus: ".
         */
        class LOG4CPLUS_EXPORT LogLog
        {
        public:
            //! Return type of getLogLog().
            typedef LogLog * Ptr;

            /**
             * Returns a reference to the <code>LogLog</code> singleton.
             */
            static Ptr getLogLog();


            /**
             * Allows to enable/disable log4cplus internal logging.
             */
            void setInternalDebugging(bool enabled);

            /**
             * In quite mode no LogLog generates strictly no output, not even
             * for errors. 
             *
             * @param quietMode A true for not
             */
            void setQuietMode(bool quietMode);

            /**
             * This method is used to output log4cplus internal debug
             * statements. Output goes to <code>std::cout</code>.
             */
            void debug(const log4cplus::tstring& msg) const;
            void debug(tchar const * msg) const;

            /**
             * This method is used to output log4cplus internal error
             * statements. There is no way to disable error
             * statements.  Output goes to
             * <code>std::cerr</code>. Optionally, this method can
             * throw std::runtime_error exception too.
             */
            void error(const log4cplus::tstring& msg, bool throw_flag = false) const;
            void error(tchar const * msg, bool throw_flag = false) const;

            /**
             * This method is used to output log4cplus internal warning
             * statements. There is no way to disable warning statements.
             * Output goes to <code>std::cerr</code>.
             */
            void warn(const log4cplus::tstring& msg) const;
            void warn(tchar const * msg) const;

            // Public ctor and dtor to be used only by internal::DefaultContext.
            LogLog();
            virtual ~LogLog();

        private:
            enum TriState
            {
                TriUndef = -1,
                TriFalse,
                TriTrue
            };

            template <typename StringType>
            LOG4CPLUS_PRIVATE
            void logging_worker (tostream & os,
                bool (LogLog:: * cond) () const, tchar const *,
                StringType const &, bool throw_flag = false) const;

            LOG4CPLUS_PRIVATE static void set_tristate_from_env (TriState *,
                tchar const * envvar);

            LOG4CPLUS_PRIVATE bool get_quiet_mode () const;
            LOG4CPLUS_PRIVATE bool get_not_quiet_mode () const;
            LOG4CPLUS_PRIVATE bool get_debug_mode () const;

            // Data
            mutable TriState debugEnabled;
            mutable TriState quietMode;
            thread::Mutex mutex;

            LOG4CPLUS_PRIVATE LogLog(const LogLog&);
            LOG4CPLUS_PRIVATE LogLog & operator = (LogLog const &);
        };

        LOG4CPLUS_EXPORT LogLog & getLogLog ();

    } // end namespace helpers
} // end namespace log4cplus


#endif // LOG4CPLUS_HELPERS_LOGLOG

