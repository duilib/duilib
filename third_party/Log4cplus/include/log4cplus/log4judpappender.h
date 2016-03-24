// -*- C++ -*-
// Module:  LOG4CPLUS
// File:    log4judpappender.h
// Created: 7/2012
// Author:  Siva Chandran P
//
//
// Copyright 2012-2013 Siva Chandran P
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

#ifndef LOG4CPLUS_LOG4J_UDP_APPENDER_HEADER_
#define LOG4CPLUS_LOG4J_UDP_APPENDER_HEADER_

#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <log4cplus/helpers/socket.h>

namespace log4cplus {

    /**
     * Sends log events as Log4j XML to a remote a log server.
     *
     * The Log4jUdpAppender has the following properties:
     *
     * <ul>
     *   <li>Remote logging is non-intrusive as far as the log event
     *   is concerned. In other words, the event will be logged with
     *   the same time stamp, NDC, location info as if it were logged
     *   locally by the client.</li>
     *
     *   <li>Remote logging uses the UDP protocol.</li>
     * </ul>
     *
     * <h3>Properties</h3>
     * <dl>
     * <dt><tt>host</tt></dt>
     * <dd>Remote host name to connect and send events to.</dd>
     *
     * <dt><tt>port</tt></dt>
     * <dd>Port on remote host to send events to.</dd>
     *
     * </dl>
     */
    class LOG4CPLUS_EXPORT Log4jUdpAppender : public Appender {
    public:
      // Ctors
        Log4jUdpAppender(const log4cplus::tstring& host, int port);
        Log4jUdpAppender(const log4cplus::helpers::Properties & properties);

      // Dtor
        ~Log4jUdpAppender();

      // Methods
        virtual void close();

    protected:
        void openSocket();
        virtual void append(const spi::InternalLoggingEvent& event);

      // Data
        log4cplus::helpers::Socket socket;
        log4cplus::tstring host;
        int port;

    private:
      // Disallow copying of instances of this class
        Log4jUdpAppender(const Log4jUdpAppender&);
        Log4jUdpAppender& operator=(const Log4jUdpAppender&);
    };
} // end namespace log4cplus

#endif // LOG4CPLUS_LOG4J_UDP_APPENDER_HEADER_

