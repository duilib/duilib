// -*- C++ -*-
// Module:  Log4CPLUS
// File:    tstring.h
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

#ifndef LOG4CPLUS_TSTRING_HEADER_
#define LOG4CPLUS_TSTRING_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <string>
#include <log4cplus/tchar.h>

namespace log4cplus
{

typedef std::basic_string<tchar> tstring;


namespace helpers
{

inline
std::string
tostring (char const * str)
{
    return std::string (str);
}

inline
std::string
tostring (std::string const & str)
{
    return str;
}

inline
std::string const &
tostring (std::string & str)
{
    return str;
}

#ifdef LOG4CPLUS_HAVE_RVALUE_REFS
inline
std::string
tostring (std::string && str)
{
    return std::move (str);
}

#endif



inline
std::wstring
towstring (wchar_t const * str)
{
    return std::wstring (str);
}

inline
std::wstring
towstring (std::wstring const & str)
{
    return str;
}

inline
std::wstring const & 
towstring (std::wstring & str)
{
    return str;
}

#ifdef LOG4CPLUS_HAVE_RVALUE_REFS
inline
std::wstring 
towstring (std::wstring && str)
{
    return std::move (str);
}

#endif

LOG4CPLUS_EXPORT std::string tostring(const std::wstring&);
LOG4CPLUS_EXPORT std::string tostring(wchar_t const *);

LOG4CPLUS_EXPORT std::wstring towstring(const std::string&);
LOG4CPLUS_EXPORT std::wstring towstring(char const *);

} // namespace helpers

#ifdef UNICODE

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) log4cplus::helpers::towstring(STRING)
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) log4cplus::helpers::tostring(STRING)

#else // UNICODE

#define LOG4CPLUS_C_STR_TO_TSTRING(STRING) std::string(STRING)
#define LOG4CPLUS_STRING_TO_TSTRING(STRING) STRING
#define LOG4CPLUS_TSTRING_TO_STRING(STRING) STRING

#endif // UNICODE

} // namespace log4cplus


#endif // LOG4CPLUS_TSTRING_HEADER_
