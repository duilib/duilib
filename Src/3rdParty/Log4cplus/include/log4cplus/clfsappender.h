// -*- C++ -*-
// Module:  Log4cplus
// File:    clfsappender.h
// Created: 5/2012
// Author:  Vaclav Zeman
//
//
//  Copyright (C) 2012-2013, Vaclav Zeman. All rights reserved.
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
   
 
//

/** @file */

#ifndef LOG4CPLUS_CLFSAPPENDER_H
#define LOG4CPLUS_CLFSAPPENDER_H

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/appender.h>


#if defined (LOG4CPLUS_CLFSAPPENDER_BUILD_DLL)
#  if defined (INSIDE_LOG4CPLUS_CLFSAPPENDER)
#    define LOG4CPLUS_CLFSAPPENDER_EXPORT __declspec(dllexport)
#  else
#    define LOG4CPLUS_CLFSAPPENDER_EXPORT __declspec(dllimport)
#  endif
#else
#  define LOG4CPLUS_CLFSAPPENDER_EXPORT
#endif


namespace log4cplus
{


class LOG4CPLUS_CLFSAPPENDER_EXPORT CLFSAppender
    : public Appender
{
public:
    CLFSAppender (tstring const & logname, unsigned long logsize,
        unsigned long buffersize);
    explicit CLFSAppender (helpers::Properties const &);
    virtual ~CLFSAppender ();

    virtual void close ();

    static void registerAppender ();

protected:
    virtual void append (spi::InternalLoggingEvent const &);

    void init (tstring const & logname, unsigned long logsize,
        unsigned long buffersize);

    struct Data;

    Data * data;

private:
    CLFSAppender (CLFSAppender const &);
    CLFSAppender & operator = (CLFSAppender const &);
};


typedef helpers::SharedObjectPtr<CLFSAppender> CLFSAppenderPtr;


} // namespace log4cplus


#endif // LOG4CPLUS_CLFSAPPENDER_H
