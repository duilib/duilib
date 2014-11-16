// -*- C++ -*-
// Module:  Log4CPLUS
// File:    config-macosx.h
// Created: 7/2003
// Author:  Christopher R. Bailey
//
//
// Copyright 2003-2013 Christopher R. Bailey
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

#ifndef LOG4CPLUS_CONFIG_MACOSX_HEADER_
#define LOG4CPLUS_CONFIG_MACOSX_HEADER_

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if (defined(__APPLE__) || (defined(__MWERKS__) && defined(__MACOS__)))

#define LOG4CPLUS_HAVE_GETTIMEOFDAY 1
#define socklen_t int

#endif // MACOSX
#endif // LOG4CPLUS_CONFIG_MACOSX_HEADER_
