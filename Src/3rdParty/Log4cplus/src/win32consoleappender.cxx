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


#include <log4cplus/config.hxx>

#if defined(_WIN32) && defined (LOG4CPLUS_HAVE_WIN32_CONSOLE)
#include <log4cplus/config/windowsh-inc.h>
#include <log4cplus/win32consoleappender.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <log4cplus/streams.h>
#include <sstream>

/* list of available colors which can be OR'ed together and provided as an INT in the config file, e.g.:
       log4cplus.appender.INFO_MSGS.TextColor=36
   for red text on green background

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
*/


namespace log4cplus
{


Win32ConsoleAppender::Win32ConsoleAppender (bool allocConsole, bool logToStdErr, unsigned int textColor)
    : alloc_console (allocConsole)
    , log_to_std_err (logToStdErr)
    , text_color (textColor)
{ }


Win32ConsoleAppender::Win32ConsoleAppender (
    helpers::Properties const & properties)
    : Appender (properties)
    , alloc_console (true)
    , log_to_std_err (false)
    , text_color (0)
{
    properties.getBool (alloc_console, LOG4CPLUS_TEXT ("AllocConsole"));
    properties.getBool (log_to_std_err, LOG4CPLUS_TEXT ("logToStdErr"));
    properties.getUInt (text_color, LOG4CPLUS_TEXT ("TextColor"));
}


Win32ConsoleAppender::~Win32ConsoleAppender ()
{
    destructorImpl();
}


void
Win32ConsoleAppender::close ()
{
    closed = true;
}


void
Win32ConsoleAppender::append (spi::InternalLoggingEvent const & event)
{
    if (alloc_console)
        // We ignore the return value here. If we already have a console,
        // it will fail.
        AllocConsole ();

    HANDLE const console_out = GetStdHandle (
        log_to_std_err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    if (console_out == INVALID_HANDLE_VALUE)
    {
        helpers::getLogLog ().error (
            LOG4CPLUS_TEXT ("Win32ConsoleAppender::append")
            LOG4CPLUS_TEXT ("- Unable to get STD_OUTPUT_HANDLE."));
        return;
    }

    DWORD const handle_type = GetFileType (console_out);
    if (handle_type == FILE_TYPE_UNKNOWN && GetLastError () != NO_ERROR)
    {
        helpers::getLogLog ().error (
            LOG4CPLUS_TEXT ("Win32ConsoleAppender::append")
            LOG4CPLUS_TEXT ("- Error retrieving handle type."));
        return;
    }

    tstring & str = formatEvent (event);
    std::size_t const str_len = str.size ();
    const tchar * s = str.c_str ();
    DWORD mode;

    if (handle_type == FILE_TYPE_CHAR && GetConsoleMode (console_out, &mode))
        // It seems that we have real console handle here. We can use
        // WriteConsole() directly.
        write_console (console_out, s, str_len);
    else
        // It seems that console is redirected.
        write_handle (console_out, s, str_len);
}


void
Win32ConsoleAppender::write_handle (void * outvoid, tchar const * s,
    std::size_t str_len)
{
    HANDLE out = static_cast<HANDLE>(outvoid);
#if defined (UNICODE)
    std::wstring wstr (s, str_len);
    std::string str (helpers::tostring (wstr));
    str_len = str.size ();
    char const * const cstr = str.c_str ();

#else
    char const * const cstr = s;

#endif

    DWORD const total_to_write = static_cast<DWORD>(str_len);
    DWORD total_written = 0;

    do
    {
        DWORD const to_write = total_to_write - total_written;
        DWORD written = 0;

        BOOL ret = WriteFile (out, cstr + total_written, to_write, &written,
            0);
        if (! ret)
        {
            helpers::getLogLog ().error (
                LOG4CPLUS_TEXT ("Win32ConsoleAppender::write_handle")
                LOG4CPLUS_TEXT ("- WriteFile has failed."));
            return;
        }

        total_written += written;
    }
    while (total_written != total_to_write);
}


void
Win32ConsoleAppender::write_console (void * console_void, tchar const * s,
    std::size_t str_len)
{
    HANDLE console_out = static_cast<HANDLE>(console_void);
    DWORD const total_to_write = static_cast<DWORD>(str_len);
    DWORD total_written = 0;
    BOOL ret = FALSE;
    unsigned int oldColor = 0;

    if (text_color)
    {
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        ret = GetConsoleScreenBufferInfo (console_out, &csbiInfo);
        if (! ret)
        {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Win32ConsoleAppender::write_console:")
                LOG4CPLUS_TEXT(" GetConsoleScreenBufferInfo failed"));
            // fallback to standard gray on black
            oldColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            goto output;
        }

        // store old color first
        oldColor = csbiInfo.wAttributes;

        // set new color
        ret = SetConsoleTextAttribute (console_out, text_color);
        if (! ret)
        {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Win32ConsoleAppender::write_console:")
                LOG4CPLUS_TEXT(" SetConsoleTextAttribute failed"));
        }
    }

output:;
    do
    {
        DWORD const to_write
            = (std::min<DWORD>) (64*1024 - 1, total_to_write - total_written);
        DWORD written = 0;
        
        ret = WriteConsole (console_out, s + total_written, to_write, &written,
            0);
        if (! ret)
        {
            helpers::getLogLog ().error (
                LOG4CPLUS_TEXT ("Win32ConsoleAppender::write_console")
                LOG4CPLUS_TEXT ("- WriteConsole has failed."));
            break;
        }

        total_written += written;
    }
    while (total_written != total_to_write);

    if (text_color)
    {
        // restore old color again
        ret = SetConsoleTextAttribute (console_out, oldColor);
        if (! ret)
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Win32ConsoleAppender::write_console:")
                LOG4CPLUS_TEXT(" SetConsoleTextAttribute failed"));
    }
}


} // namespace log4cplus

#endif
