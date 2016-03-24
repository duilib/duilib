// -*- C++ -*-
// Module:  Log4CPLUS
// File:    loggingmacros.h
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

/** @file
 * This header defines the logging macros. */

#ifndef LOG4CPLUS_LOGGING_MACROS_HEADER_
#define LOG4CPLUS_LOGGING_MACROS_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/streams.h>
#include <log4cplus/logger.h>
#include <log4cplus/helpers/snprintf.h>
#include <log4cplus/tracelogger.h>
#include <sstream>
#include <utility>


#if defined(_MSC_VER)
#define LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()  \
    __pragma (warning (push))                 \
    __pragma (warning (disable:4127))           

#define LOG4CPLUS_RESTORE_DOWHILE_WARNING()   \
    __pragma (warning (pop))

#else
#define LOG4CPLUS_SUPPRESS_DOWHILE_WARNING() /* empty */
#define LOG4CPLUS_RESTORE_DOWHILE_WARNING() /* empty */

#endif

#define LOG4CPLUS_DOWHILE_NOTHING()                 \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()            \
    do { } while (0)                                \
    LOG4CPLUS_RESTORE_DOWHILE_WARNING()

#if defined(LOG4CPLUS_DISABLE_FATAL) && !defined(LOG4CPLUS_DISABLE_ERROR)
#define LOG4CPLUS_DISABLE_ERROR
#endif
#if defined(LOG4CPLUS_DISABLE_ERROR) && !defined(LOG4CPLUS_DISABLE_WARN)
#define LOG4CPLUS_DISABLE_WARN
#endif
#if defined(LOG4CPLUS_DISABLE_WARN) && !defined(LOG4CPLUS_DISABLE_INFO)
#define LOG4CPLUS_DISABLE_INFO
#endif
#if defined(LOG4CPLUS_DISABLE_INFO) && !defined(LOG4CPLUS_DISABLE_DEBUG)
#define LOG4CPLUS_DISABLE_DEBUG
#endif
#if defined(LOG4CPLUS_DISABLE_DEBUG) && !defined(LOG4CPLUS_DISABLE_TRACE)
#define LOG4CPLUS_DISABLE_TRACE
#endif


namespace log4cplus
{

namespace detail
{


inline
Logger
macros_get_logger (Logger const & logger)
{
    return logger;
}


inline
Logger const &
macros_get_logger (Logger & logger)
{
    return logger;
}


#if defined (LOG4CPLUS_HAVE_RVALUE_REFS)
inline
Logger
macros_get_logger (Logger && logger)
{
    return std::move (logger);
}

#endif


inline
Logger
macros_get_logger (tstring const & logger)
{
    return Logger::getInstance (logger);
}


inline
Logger
macros_get_logger (tchar const * logger)
{
    return Logger::getInstance (logger);
}


LOG4CPLUS_EXPORT void clear_tostringstream (tostringstream &);


LOG4CPLUS_EXPORT log4cplus::tostringstream & get_macro_body_oss ();
LOG4CPLUS_EXPORT log4cplus::helpers::snprintf_buf & get_macro_body_snprintf_buf ();
LOG4CPLUS_EXPORT void macro_forced_log (log4cplus::Logger const &,
    log4cplus::LogLevel, log4cplus::tstring const &, char const *, int,
    char const *);


} // namespace detail

} // namespace log4cplus


#if ! defined (LOG4CPLUS_DISABLE_FUNCTION_MACRO) \
    && ! defined (LOG4CPLUS_MACRO_FUNCTION)
#  if defined (LOG4CPLUS_HAVE_FUNCSIG_MACRO)
#    define LOG4CPLUS_MACRO_FUNCTION() __FUNCSIG__
#  elif defined (LOG4CPLUS_HAVE_PRETTY_FUNCTION_MACRO)
#    define LOG4CPLUS_MACRO_FUNCTION() __PRETTY_FUNCTION__
#  elif defined (LOG4CPLUS_HAVE_FUNCTION_MACRO)
#    define LOG4CPLUS_MACRO_FUNCTION() __FUNCTION__
#  elif defined (LOG4CPLUS_HAVE_FUNC_SYMBOL)
#    define LOG4CPLUS_MACRO_FUNCTION() __func__
#  endif
#endif

#if ! defined (LOG4CPLUS_MACRO_FUNCTION)
#  define LOG4CPLUS_MACRO_FUNCTION() 0
#endif

// Make TRACE and DEBUG log level unlikely and INFO, WARN, ERROR and
// FATAL log level likely.
#define LOG4CPLUS_MACRO_TRACE_LOG_LEVEL(pred) \
    LOG4CPLUS_UNLIKELY (pred)
#define LOG4CPLUS_MACRO_DEBUG_LOG_LEVEL(pred) \
    LOG4CPLUS_UNLIKELY (pred)
#define LOG4CPLUS_MACRO_INFO_LOG_LEVEL(pred) \
    LOG4CPLUS_LIKELY (pred)
#define LOG4CPLUS_MACRO_WARN_LOG_LEVEL(pred) \
    LOG4CPLUS_LIKELY (pred)
#define LOG4CPLUS_MACRO_ERROR_LOG_LEVEL(pred) \
    LOG4CPLUS_LIKELY (pred)
#define LOG4CPLUS_MACRO_FATAL_LOG_LEVEL(pred) \
    LOG4CPLUS_LIKELY (pred)


//! Dispatch to LOG4CPLUS_MACRO_LOGLEVEL_* depending on log level.
#define LOG4CPLUS_MACRO_LOGLEVEL_PRED(pred, logLevel)    \
    LOG4CPLUS_MACRO_ ## logLevel (pred)


// Either use temporary instances of ostringstream
// and snprintf_buf, or use thread-local instances.
#if defined (LOG4CPLUS_MACRO_DISABLE_TLS)
#  define LOG4CPLUS_MACRO_INSTANTIATE_OSTRINGSTREAM(var)    \
    log4cplus::tostringstream var

#  define LOG4CPLUS_MACRO_INSTANTIATE_SNPRINTF_BUF(var)     \
    log4cplus::helpers::snprintf_buf var

#else
#  define LOG4CPLUS_MACRO_INSTANTIATE_OSTRINGSTREAM(var)    \
    log4cplus::tostringstream & var                         \
        = log4cplus::detail::get_macro_body_oss ()

#  define LOG4CPLUS_MACRO_INSTANTIATE_SNPRINTF_BUF(var)     \
    log4cplus::helpers::snprintf_buf & var                  \
        = log4cplus::detail::get_macro_body_snprintf_buf ()

#endif


#define LOG4CPLUS_MACRO_BODY(logger, logEvent, logLevel)                \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()                                \
    do {                                                                \
        log4cplus::Logger const & _l                                    \
            = log4cplus::detail::macros_get_logger (logger);            \
        if (LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (log4cplus::logLevel), logLevel)) {     \
            LOG4CPLUS_MACRO_INSTANTIATE_OSTRINGSTREAM (_log4cplus_buf); \
            _log4cplus_buf << logEvent;                                 \
            log4cplus::detail::macro_forced_log (_l,                    \
                log4cplus::logLevel, _log4cplus_buf.str(),              \
                __FILE__, __LINE__, LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while (0)                                                         \
    LOG4CPLUS_RESTORE_DOWHILE_WARNING()


#define LOG4CPLUS_MACRO_STR_BODY(logger, logEvent, logLevel)            \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()                                \
    do {                                                                \
        log4cplus::Logger const & _l                                    \
            = log4cplus::detail::macros_get_logger (logger);            \
        if (LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (log4cplus::logLevel), logLevel)) {     \
            log4cplus::detail::macro_forced_log (_l,                    \
                log4cplus::logLevel, logEvent,                          \
                __FILE__, __LINE__, LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while(0)                                                          \
    LOG4CPLUS_RESTORE_DOWHILE_WARNING()

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_MACRO_FMT_BODY(logger, logLevel, logFmt, ...)         \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()                                \
    do {                                                                \
        log4cplus::Logger const & _l                                    \
            = log4cplus::detail::macros_get_logger (logger);            \
        if (LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (log4cplus::logLevel), logLevel)) {     \
            LOG4CPLUS_MACRO_INSTANTIATE_SNPRINTF_BUF (_snpbuf);         \
            log4cplus::tchar const * _logEvent                          \
                = _snpbuf.print (logFmt, __VA_ARGS__);                  \
            log4cplus::detail::macro_forced_log (_l,                    \
                log4cplus::logLevel, _logEvent,                         \
                __FILE__, __LINE__, LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while(0)                                                          \
    LOG4CPLUS_RESTORE_DOWHILE_WARNING()

#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_MACRO_FMT_BODY(logger, logLevel, logFmt, logArgs...)  \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()                                \
    do {                                                                \
        log4cplus::Logger const & _l                                    \
            = log4cplus::detail::macros_get_logger (logger);            \
        if (LOG4CPLUS_MACRO_LOGLEVEL_PRED (                             \
                _l.isEnabledFor (log4cplus::logLevel), logLevel)) {     \
            LOG4CPLUS_MACRO_INSTANTIATE_SNPRINTF_BUF (_snpbuf);         \
            log4cplus::tchar const * _logEvent                          \
                = _snpbuf.print (logFmt, logArgs);                      \
            log4cplus::detail::macro_forced_log (_l,                    \
                log4cplus::logLevel, _logEvent,                         \
                __FILE__, __LINE__, LOG4CPLUS_MACRO_FUNCTION ());       \
        }                                                               \
    } while(0)                                                          \
    LOG4CPLUS_RESTORE_DOWHILE_WARNING()

#endif


/**
 * @def LOG4CPLUS_TRACE(logger, logEvent) This macro creates a
 * TraceLogger to log a TRACE_LOG_LEVEL message to <code>logger</code>
 * upon entry and exiting of a method.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_TRACE)
#define LOG4CPLUS_TRACE_METHOD(logger, logEvent)                        \
    log4cplus::TraceLogger _log4cplus_trace_logger(logger, logEvent,    \
                                                   __FILE__, __LINE__);
#define LOG4CPLUS_TRACE(logger, logEvent)                               \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, TRACE_LOG_LEVEL)
#define LOG4CPLUS_TRACE_STR(logger, logEvent)                           \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, TRACE_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_TRACE_FMT(logger, logFmt, ...)                            \
    LOG4CPLUS_MACRO_FMT_BODY (logger, TRACE_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_TRACE_FMT(logger, logFmt, logArgs...)                     \
    LOG4CPLUS_MACRO_FMT_BODY(logger, TRACE_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_TRACE_METHOD(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_TRACE(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_TRACE_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_TRACE_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_TRACE_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

/**
 * @def LOG4CPLUS_DEBUG(logger, logEvent)  This macro is used to log a
 * DEBUG_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_DEBUG)
#define LOG4CPLUS_DEBUG(logger, logEvent)                               \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, DEBUG_LOG_LEVEL)
#define LOG4CPLUS_DEBUG_STR(logger, logEvent)                           \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, DEBUG_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_DEBUG_FMT(logger, logFmt, ...)                            \
    LOG4CPLUS_MACRO_FMT_BODY (logger, DEBUG_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_DEBUG_FMT(logger, logFmt, logArgs...)                     \
    LOG4CPLUS_MACRO_FMT_BODY(logger, DEBUG_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_DEBUG(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_DEBUG_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_DEBUG_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_DEBUG_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

/**
 * @def LOG4CPLUS_INFO(logger, logEvent)  This macro is used to log a
 * INFO_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_INFO)
#define LOG4CPLUS_INFO(logger, logEvent)                                \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, INFO_LOG_LEVEL)
#define LOG4CPLUS_INFO_STR(logger, logEvent)                            \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, INFO_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_INFO_FMT(logger, logFmt, ...)                             \
    LOG4CPLUS_MACRO_FMT_BODY (logger, INFO_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_INFO_FMT(logger, logFmt, logArgs...)                      \
    LOG4CPLUS_MACRO_FMT_BODY(logger, INFO_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_INFO(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_INFO_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_INFO_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_INFO_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

/**
 * @def LOG4CPLUS_WARN(logger, logEvent)  This macro is used to log a
 * WARN_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_WARN)
#define LOG4CPLUS_WARN(logger, logEvent)                                \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, WARN_LOG_LEVEL)
#define LOG4CPLUS_WARN_STR(logger, logEvent)                            \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, WARN_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_WARN_FMT(logger, logFmt, ...)                             \
    LOG4CPLUS_MACRO_FMT_BODY (logger, WARN_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_WARN_FMT(logger, logFmt, logArgs...)                      \
    LOG4CPLUS_MACRO_FMT_BODY(logger, WARN_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_WARN(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_WARN_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_WARN_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_WARN_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

/**
 * @def LOG4CPLUS_ERROR(logger, logEvent)  This macro is used to log a
 * ERROR_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_ERROR)
#define LOG4CPLUS_ERROR(logger, logEvent)                               \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, ERROR_LOG_LEVEL)
#define LOG4CPLUS_ERROR_STR(logger, logEvent)                           \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, ERROR_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_ERROR_FMT(logger, logFmt, ...)                            \
    LOG4CPLUS_MACRO_FMT_BODY (logger, ERROR_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_ERROR_FMT(logger, logFmt, logArgs...)                     \
    LOG4CPLUS_MACRO_FMT_BODY(logger, ERROR_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_ERROR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_ERROR_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_ERROR_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_ERROR_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

/**
 * @def LOG4CPLUS_FATAL(logger, logEvent)  This macro is used to log a
 * FATAL_LOG_LEVEL message to <code>logger</code>.
 * <code>logEvent</code> will be streamed into an <code>ostream</code>.
 */
#if !defined(LOG4CPLUS_DISABLE_FATAL)
#define LOG4CPLUS_FATAL(logger, logEvent)                               \
    LOG4CPLUS_MACRO_BODY (logger, logEvent, FATAL_LOG_LEVEL)
#define LOG4CPLUS_FATAL_STR(logger, logEvent)                           \
    LOG4CPLUS_MACRO_STR_BODY (logger, logEvent, FATAL_LOG_LEVEL)

#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_FATAL_FMT(logger, logFmt, ...)                            \
    LOG4CPLUS_MACRO_FMT_BODY (logger, FATAL_LOG_LEVEL, logFmt, __VA_ARGS__)
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_FATAL_FMT(logger, logFmt, logArgs...)                     \
    LOG4CPLUS_MACRO_FMT_BODY(logger, FATAL_LOG_LEVEL, logFmt, logArgs)
#endif

#else
#define LOG4CPLUS_FATAL(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#define LOG4CPLUS_FATAL_STR(logger, logEvent) LOG4CPLUS_DOWHILE_NOTHING()
#if defined (LOG4CPLUS_HAVE_C99_VARIADIC_MACROS)
#define LOG4CPLUS_FATAL_FMT(logger, logFmt, ...) LOG4CPLUS_DOWHILE_NOTHING()
#elif defined (LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS)
#define LOG4CPLUS_FATAL_FMT(logger, logFmt, logArgs...) LOG4CPLUS_DOWHILE_NOTHING()
#endif

#endif

#endif /* LOG4CPLUS_LOGGING_MACROS_HEADER_ */
