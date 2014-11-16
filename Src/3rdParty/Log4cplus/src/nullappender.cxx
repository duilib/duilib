// Module:  Log4CPLUS
// File:    nullappender.cxx
// Created: 6/2003
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

#include <log4cplus/nullappender.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus
{


///////////////////////////////////////////////////////////////////////////////
// NullAppender ctors and dtor
///////////////////////////////////////////////////////////////////////////////

NullAppender::NullAppender()
{
}


NullAppender::NullAppender(const helpers::Properties& properties)
: Appender(properties)
{
}



NullAppender::~NullAppender()
{
    destructorImpl();
}



///////////////////////////////////////////////////////////////////////////////
// NullAppender public methods
///////////////////////////////////////////////////////////////////////////////

void
NullAppender::close()
{
}



///////////////////////////////////////////////////////////////////////////////
// NullAppender protected methods
///////////////////////////////////////////////////////////////////////////////

// This method does not need to be locked since it is called by
// doAppend() which performs the locking
void
NullAppender::append(const spi::InternalLoggingEvent&)
{
}


} // namespace log4cplus
