// Module:  Log4CPLUS
// File:    loglog.cxx
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

#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/internal/env.h>
#include <log4cplus/consoleappender.h>
#include <ostream>
#include <stdexcept>


namespace log4cplus { namespace helpers {

namespace
{

static tchar const PREFIX[] = LOG4CPLUS_TEXT("log4cplus: ");
static tchar const WARN_PREFIX[] = LOG4CPLUS_TEXT("log4cplus:WARN ");
static tchar const ERR_PREFIX[] = LOG4CPLUS_TEXT("log4cplus:ERROR ");

} // namespace


LogLog *
LogLog::getLogLog()
{
    return &helpers::getLogLog ();
}


LogLog::LogLog()
    : debugEnabled(TriUndef)
    , quietMode(TriUndef)
{ }


LogLog::~LogLog()
{ }


void
LogLog::setInternalDebugging(bool enabled)
{
    thread::MutexGuard guard (mutex);

    debugEnabled = enabled ? TriTrue : TriFalse;
}


void
LogLog::setQuietMode(bool quietModeVal)
{
    thread::MutexGuard guard (mutex);

    quietMode = quietModeVal ? TriTrue : TriFalse;
}


void
LogLog::debug(const log4cplus::tstring& msg) const
{
    logging_worker (tcout, &LogLog::get_debug_mode, PREFIX, msg);
}


void
LogLog::debug(tchar const * msg) const
{
    logging_worker (tcout, &LogLog::get_debug_mode, PREFIX, msg);
}


void
LogLog::warn(const log4cplus::tstring& msg) const
{
    logging_worker (tcerr, &LogLog::get_not_quiet_mode, WARN_PREFIX, msg);
}


void
LogLog::warn(tchar const * msg) const
{
    logging_worker (tcerr, &LogLog::get_not_quiet_mode, WARN_PREFIX, msg);
}


void
LogLog::error(const log4cplus::tstring& msg, bool throw_flag) const
{
    logging_worker (tcerr, &LogLog::get_not_quiet_mode, ERR_PREFIX, msg,
        throw_flag);
}


void
LogLog::error(tchar const * msg, bool throw_flag) const
{
    logging_worker (tcerr, &LogLog::get_not_quiet_mode, ERR_PREFIX, msg,
        throw_flag);
}


bool
LogLog::get_quiet_mode () const
{
    if (quietMode == TriUndef)
        set_tristate_from_env (&quietMode,
            LOG4CPLUS_TEXT ("LOG4CPLUS_LOGLOG_QUIETMODE"));

    return quietMode == TriTrue;
}


bool
LogLog::get_not_quiet_mode () const
{
    return ! get_quiet_mode ();
}


bool
LogLog::get_debug_mode () const
{
    if (debugEnabled == TriUndef)
        set_tristate_from_env (&debugEnabled,
            LOG4CPLUS_TEXT ("LOG4CPLUS_LOGLOG_DEBUGENABLED"));

    return debugEnabled && ! get_quiet_mode ();
}


void
LogLog::set_tristate_from_env (TriState * result, tchar const * envvar_name)
{
    tstring envvar_value;
    bool exists = internal::get_env_var (envvar_value, envvar_name);
    bool value = false;
    if (exists && internal::parse_bool (value, envvar_value) && value)
        *result = TriTrue;
    else
        *result = TriFalse;
}


template <typename StringType>
void
LogLog::logging_worker (tostream & os, bool (LogLog:: * cond) () const,
    tchar const * prefix, StringType const & msg, bool throw_flag) const
{
    bool output;
    {
        thread::MutexGuard guard (mutex);
        output = (this->*cond) ();
    }

    if (LOG4CPLUS_UNLIKELY (output))
    {
        // XXX This is potential recursive lock of
        // ConsoleAppender::outputMutex.
        thread::MutexGuard outputGuard (ConsoleAppender::getOutputMutex ());
        os << prefix << msg << std::endl;
    }

    if (LOG4CPLUS_UNLIKELY (throw_flag))
        throw std::runtime_error (LOG4CPLUS_TSTRING_TO_STRING (msg));
}


} } // namespace log4cplus { namespace helpers {
