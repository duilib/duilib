// -*- C++ -*-
// Module:  Log4CPLUS
// File:    socket.h
// Created: 1/2010
// Author:  Vaclav Haisman
//
//
//  Copyright (C) 2010-2013, Vaclav Haisman. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modifica-
//  tion, are permitted provided that the following conditions are met:
//  
//  1. Redistributions of  source code must  retain the above copyright  notice,
//     this list of conditions and the following disclaimer.
//  
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  
//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL  THE
//  APACHE SOFTWARE  FOUNDATION  OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
//  DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
//  OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
//  ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
//  (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/** @file 
 * This header contains declaration internal to log4cplus. They must never be
 * visible from user accesible headers or exported in DLL/shared libray.
 */


#ifndef LOG4CPLUS_INTERNAL_SOCKET_H_
#define LOG4CPLUS_INTERNAL_SOCKET_H_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if ! defined (INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif

#if defined(_WIN32)
#include <log4cplus/config/windowsh-inc.h>
#endif
#include <log4cplus/helpers/socket.h>

#include <cerrno>
#ifdef LOG4CPLUS_HAVE_ERRNO_H
#include <errno.h>
#endif


namespace log4cplus {

namespace helpers {


#if defined(_WIN32)
typedef SOCKET os_socket_type;
#else
typedef int os_socket_type;
#endif


os_socket_type const INVALID_OS_SOCKET_VALUE
#if defined(_WIN32)
    = INVALID_SOCKET;
#else
    = -1;
#endif


static inline
os_socket_type
to_os_socket (SOCKET_TYPE const & x)
{
    return static_cast<os_socket_type>(x);
}


static inline
SOCKET_TYPE
to_log4cplus_socket (os_socket_type const & x)
{
    return static_cast<SOCKET_TYPE>(x);
}


static inline
void
set_last_socket_error (int err)
{
    errno = err;
}


static inline
int
get_last_socket_error ()
{
    return errno;
}


} // namespace helpers {

} // namespace log4cplus { 


#endif // LOG4CPLUS_INTERNAL_SOCKET_H_
