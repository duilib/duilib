// -*- C++ -*-
// Module:  Log4CPLUS
// File:    socketbuffer.h
// Created: 5/2003
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

#ifndef LOG4CPLUS_HELPERS_SOCKET_BUFFER_HEADER_
#define LOG4CPLUS_HELPERS_SOCKET_BUFFER_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>


namespace log4cplus {
namespace helpers {

/**
 *
 */
class LOG4CPLUS_EXPORT SocketBuffer
{
public:
    explicit SocketBuffer(std::size_t max);
    virtual ~SocketBuffer();

    char *getBuffer() const { return buffer; }
    std::size_t getMaxSize() const { return maxsize; }
    std::size_t getSize() const { return size; }
    void setSize(std::size_t s) { size = s; }
    std::size_t getPos() const { return pos; }

    unsigned char readByte();
    unsigned short readShort();
    unsigned int readInt();
    tstring readString(unsigned char sizeOfChar);

    void appendByte(unsigned char val);
    void appendShort(unsigned short val);
    void appendInt(unsigned int val);
    void appendString(const tstring& str);
    void appendBuffer(const SocketBuffer& buffer);

private:
    // Data
    std::size_t maxsize;
    std::size_t size;
    std::size_t pos;
    char *buffer;

    SocketBuffer(SocketBuffer const & rhs);
    SocketBuffer& operator= (SocketBuffer const& rhs);
};

} // end namespace helpers
} // end namespace log4cplus

#endif // LOG4CPLUS_HELPERS_SOCKET_HEADER_
