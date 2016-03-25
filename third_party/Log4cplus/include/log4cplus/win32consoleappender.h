// -*- C++ -*-
//  Copyright (C) 2009-2013, Vaclav Haisman. All rights reserved.
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

#ifndef LOG4CPLUS_WIN32CONSOLEAPPENDER_H
#define LOG4CPLUS_WIN32CONSOLEAPPENDER_H

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if defined(_WIN32) && defined (LOG4CPLUS_HAVE_WIN32_CONSOLE)

#include <log4cplus/appender.h>


namespace log4cplus
{

   /**
    * Prints events to Win32 console.
    *
    * <h3>Properties</h3>
    * <dl>
    * <dt><tt>AllocConsole</tt></dt>
    * <dd>This boolean property specifies whether or not this appender
    * will try to allocate new console using the
    * <code>AllocConsole()</code> Win32 function.</dd>
    *
    * <dt><tt>logToStdErr</tt></dt>
    * <dd>When it is set true, the output will be into
    * <code>STD_ERROR_HANDLE</code> instead of <code>STD_OUTPUT_HANDLE</code>.
    * </dd>
    *
    * <dt><tt>TextColor</tt></dt>
    * <dd>See MSDN documentation for
    * <a href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms682088(v=vs.85).aspx#_win32_character_attributes">
    * Character Attributes</a>.
    * </dl>
    */
    class LOG4CPLUS_EXPORT Win32ConsoleAppender
        : public Appender
    {
    public:
        explicit Win32ConsoleAppender (bool allocConsole = true,
            bool logToStdErr = false, unsigned int textColor = 0);
        Win32ConsoleAppender (helpers::Properties const & properties);
        virtual ~Win32ConsoleAppender ();

        virtual void close ();

    protected:
        virtual void append (spi::InternalLoggingEvent const &);

        void write_handle (void *, tchar const *, std::size_t);
        void write_console (void *, tchar const *, std::size_t);

        bool alloc_console;
        bool log_to_std_err;
        unsigned int text_color;

    private:
        Win32ConsoleAppender (Win32ConsoleAppender const &);
        Win32ConsoleAppender & operator = (Win32ConsoleAppender const &);
    };

} // namespace log4cplus

#endif

#endif // LOG4CPLUS_WIN32CONSOLEAPPENDER_H
