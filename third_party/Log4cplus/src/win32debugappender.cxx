// Module:  Log4CPLUS
// File:    win32debugappender.cxx
// Created: 12/2003
// Author:  Eduardo Francos, Odalio SARL
//
//
// Copyright 2003-2013 Odalio SARL
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

#include <log4cplus/config.hxx>
#if defined (LOG4CPLUS_HAVE_OUTPUTDEBUGSTRING)

#include <log4cplus/config/windowsh-inc.h>
#include <log4cplus/win32debugappender.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus
{


///////////////////////////////////////////////////////////////////////////////
// Win32DebugAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Win32DebugAppender::Win32DebugAppender()
{
}


Win32DebugAppender::Win32DebugAppender(
    const helpers::Properties& properties)
    : Appender(properties)
{
}



Win32DebugAppender::~Win32DebugAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// Win32DebugAppender public methods
///////////////////////////////////////////////////////////////////////////////

void
Win32DebugAppender::close()
{
    closed = true;
}



///////////////////////////////////////////////////////////////////////////////
// Win32DebugAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
Win32DebugAppender::append(const spi::InternalLoggingEvent& event)
{
    const tchar * s = formatEvent (event).c_str();
    ::OutputDebugString(s);
}


} // namespace log4cplus


#endif // LOG4CPLUS_HAVE_OUTPUTDEBUGSTRING
