// -*- C++ -*-
// Module:  Log4CPLUS
// File:    config-win32.h
// Created: 4/2003
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

#ifndef LOG4CPLUS_CONFIG_WIN32_HEADER_
#define LOG4CPLUS_CONFIG_WIN32_HEADER_

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if defined (__MINGW32__)  || defined (__MINGW64__)
#  include <_mingw.h>
#endif

#ifdef _WIN32

#if (defined (_MSC_VER) && _MSC_VER > 1400) \
    || (defined (__MINGW64_VERSION_MAJOR) && __MINGW64_VERSION_MAJOR >= 3)
#  define LOG4CPLUS_HAVE_INTRIN_H
#endif

// Time related functions and headers.
#define LOG4CPLUS_HAVE_TIME_H
#define LOG4CPLUS_HAVE_SYS_TIMEB_H
#define LOG4CPLUS_HAVE_FTIME
#if defined (_MSC_VER) || defined (__BORLANDC__) 
#define LOG4CPLUS_HAVE_GMTIME_S
#endif

// Use Winsock on Windows.
#define LOG4CPLUS_USE_WINSOCK

// Enable Win32DebugAppender
#define LOG4CPLUS_HAVE_OUTPUTDEBUGSTRING

// Enable Win32ConsoleAppender.
#define LOG4CPLUS_HAVE_WIN32_CONSOLE

#define LOG4CPLUS_HAVE_SYS_TYPES_H
#define LOG4CPLUS_HAVE_SYS_LOCKING_H
#define LOG4CPLUS_HAVE_FCNTL_H
#define LOG4CPLUS_HAVE_IO_H
#define LOG4CPLUS_HAVE_STDIO_H
#define LOG4CPLUS_HAVE_WCHAR_H
#define LOG4CPLUS_HAVE_STDARG_H
#define LOG4CPLUS_HAVE_STDLIB_H
#define LOG4CPLUS_HAVE_ERRNO_H
#define LOG4CPLUS_HAVE_SYS_STAT_H
#define LOG4CPLUS_HAVE_TIME_H
#define LOG4CPLUS_HAVE_STDLIB_H

// MSVC has both and so does MinGW.
#define LOG4CPLUS_HAVE_VSNPRINTF
#define LOG4CPLUS_HAVE__VSNPRINTF
#define LOG4CPLUS_HAVE__VSNWPRINTF

// Limit the use of foo_s() functions to builds using Visual Studio
// 2005 and its run time library. In MinGW land, limit the foo_s()
// functions to MinGw-w64 toolchain and __MSVCRT_VERSION__ >= 0x0900.
#if (defined (_MSC_VER) && _MSC_VER >= 1400)                         \
    || (defined (__MSVCRT_VERSION__) && __MSVCRT_VERSION__ >= 0x0900 \
        && defined (__MINGW64_VERSION_MAJOR) && __MINGW64_VERSION_MAJOR >= 2)
// MS secure versions of vprintf().
#  define LOG4CPLUS_HAVE_VSPRINTF_S
#  define LOG4CPLUS_HAVE_VSWPRINTF_S

// MS secure versions of vfprintf().
#  define LOG4CPLUS_HAVE_VFPRINTF_S
#  define LOG4CPLUS_HAVE_VFWPRINTF_S

// MS secure versions of vsnprintf().
#  define LOG4CPLUS_HAVE_VSNPRINTF_S
#  define LOG4CPLUS_HAVE__VSNPRINTF_S
#  define LOG4CPLUS_HAVE__VSNWPRINTF_S

// MS printf-like functions supporting positional parameters.
#  define LOG4CPLUS_HAVE__VSPRINTF_P
#  define LOG4CPLUS_HAVE__VSWPRINTF_P

// MS secure version of _tsopen().
#  define LOG4CPLUS_HAVE__TSOPEN_S
#endif

#define LOG4CPLUS_HAVE__TSOPEN

#define LOG4CPLUS_DLLMAIN_HINSTANCE HINSTANCE
#define LOG4CPLUS_HAVE_NT_EVENT_LOG

// log4cplus_EXPORTS is used by the CMake build system.  DLL_EXPORT is
// used by the autotools build system.
#if (defined (log4cplus_EXPORTS) || defined (log4cplusU_EXPORTS) \
     || defined (DLL_EXPORT))                                    \
    && ! defined (LOG4CPLUS_STATIC)
#  undef LOG4CPLUS_BUILD_DLL
#  define LOG4CPLUS_BUILD_DLL
#endif

#if ! defined (LOG4CPLUS_BUILD_DLL)
#  undef LOG4CPLUS_STATIC
#  define LOG4CPLUS_STATIC
#endif

#if defined (LOG4CPLUS_STATIC) && defined (LOG4CPLUS_BUILD_DLL)
#  error LOG4CPLUS_STATIC and LOG4CPLUS_BUILD_DLL cannot be defined both.
#endif

#if defined (LOG4CPLUS_BUILD_DLL)
#  if defined (INSIDE_LOG4CPLUS)
#    define LOG4CPLUS_EXPORT __declspec(dllexport)
#  else
#    define LOG4CPLUS_EXPORT __declspec(dllimport)
#  endif
#else
#  define LOG4CPLUS_EXPORT
#endif

#ifndef LOG4CPLUS_SINGLE_THREADED
#  define LOG4CPLUS_USE_WIN32_THREADS
#endif

#if defined (_WIN32_WINNT) && _WIN32_WINNT < 0x0600
#  define LOG4CPLUS_POOR_MANS_SHAREDMUTEX
#endif

#if defined(_MSC_VER)
  // Warning about: identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4786 )
  // Warning about: <type1> needs to have dll-interface to be used by clients of class <type2>
#  pragma warning( disable : 4251 )

#  define LOG4CPLUS_INLINES_ARE_EXPORTED

#  if _MSC_VER >= 1400
#    define LOG4CPLUS_WORKING_LOCALE
#    define LOG4CPLUS_HAVE_FUNCTION_MACRO
#    define LOG4CPLUS_HAVE_FUNCSIG_MACRO
#    define LOG4CPLUS_HAVE_C99_VARIADIC_MACROS
#    define LOG4CPLUS_ATTRIBUTE_NORETURN __declspec(noreturn)
#  endif
#  if _MSC_VER >= 1700
#    define LOG4CPLUS_HAVE_CXX11_ATOMICS
#  endif
#endif

#if defined (__GNUC__)
#  undef LOG4CPLUS_INLINES_ARE_EXPORTED
#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#    define LOG4CPLUS_HAVE_PRETTY_FUNCTION_MACRO
#    define LOG4CPLUS_HAVE_FUNC_SYMBOL
#  endif
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#    if defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#      define LOG4CPLUS_HAVE___SYNC_SUB_AND_FETCH
#      define LOG4CPLUS_HAVE___SYNC_ADD_AND_FETCH
#    endif
#  endif
#  if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
#    if defined (__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#      define LOG4CPLUS_HAVE___ATOMIC_ADD_FETCH
#      define LOG4CPLUS_HAVE___ATOMIC_SUB_FETCH
#    endif
// This has worked for some versions of MinGW with GCC 4.7+ but it
// appears to be broken again in 4.8.x. Thus, we disable this for GCC
// completely forever.
//
//#    define LOG4CPLUS_INLINES_ARE_EXPORTED
#  endif
#  define LOG4CPLUS_HAVE_FUNCTION_MACRO
#  define LOG4CPLUS_HAVE_GNU_VARIADIC_MACROS
#  define LOG4CPLUS_HAVE_C99_VARIADIC_MACROS
#  if defined (__MINGW32__)
#    define LOG4CPLUS_WORKING_C_LOCALE
#  endif
#endif

#if defined (__BORLANDC__) && __BORLANDC__ >= 0x0650
#  define LOG4CPLUS_HAVE_FUNCTION_MACRO
#  define LOG4CPLUS_HAVE_C99_VARIADIC_MACROS
#endif // __BORLANDC__

#if ! defined (LOG4CPLUS_DISABLE_DLL_RUNTIME_WARNING)
#  if defined (LOG4CPLUS_STATIC) && defined (_MSC_VER) && ! defined (_DLL)
#    pragma message("You are not using DLL C run time library. " \
    "You must call log4cplus::initialize() once before " \
    "you use any other log4cplus API.")
#  endif
#endif

#endif // _WIN32
#endif // LOG4CPLUS_CONFIG_WIN32_HEADER_
