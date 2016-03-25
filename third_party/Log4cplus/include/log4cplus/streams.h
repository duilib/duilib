// -*- C++ -*-
// Module:  Log4CPLUS
// File:    streams.h
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

#ifndef LOG4CPLUS_STREAMS_HEADER_
#define LOG4CPLUS_STREAMS_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tchar.h>

#include <iosfwd>


namespace log4cplus
{
    typedef std::basic_ostream<tchar> tostream;
    typedef std::basic_istream<tchar> tistream;
    typedef std::basic_ostringstream<tchar> tostringstream;
    typedef std::basic_istringstream<tchar> tistringstream;
    extern LOG4CPLUS_EXPORT tostream & tcout;
    extern LOG4CPLUS_EXPORT tostream & tcerr;
}

#if defined (UNICODE) && defined (LOG4CPLUS_ENABLE_GLOBAL_C_STRING_STREAM_INSERTER)

LOG4CPLUS_EXPORT log4cplus::tostream& operator <<(log4cplus::tostream&, const char* psz );

#endif

#endif // LOG4CPLUS_STREAMS_HEADER_

