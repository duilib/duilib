// Module:  Log4CPLUS
// File:    logloguser.cxx
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

#include <log4cplus/helpers/logloguser.h>
#include <log4cplus/helpers/loglog.h>


namespace log4cplus { namespace helpers {


LogLogUser::LogLogUser()
{ }


LogLogUser::LogLogUser(const LogLogUser&)
{ }


LogLogUser::~LogLogUser()
{ }


LogLog&
LogLogUser::getLogLog() const
{
    return *LogLog::getLogLog ();
}


LogLogUser&
LogLogUser::operator=(const LogLogUser&)
{
    return *this;
}

} } // namespace log4cplus { namespace helpers {
