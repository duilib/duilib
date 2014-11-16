// Module:  Log4CPLUS
// File:    log4judpappender.cxx
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

#include <log4cplus/log4judpappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <iomanip>
#include <cstring>
#if defined (UNICODE)
#include <cwctype>
#else
#include <cctype>
#endif


namespace log4cplus
{

namespace
{


static inline bool
is_control (tchar ch)
{
#if defined (UNICODE)
    return !! std::iswcntrl (std::char_traits<tchar>::to_int_type (ch));
#else
    return !! std::iscntrl (std::char_traits<tchar>::to_int_type (ch));
#endif
}


//! Outputs str with reserved XML characters escaped.
static
void
output_xml_escaped (tostream & os, tstring const & str)
{
    for (tstring::const_iterator it = str.begin (); it != str.end (); ++it)
    {
        tchar const & ch = *it;
        switch (ch)
        {
        case LOG4CPLUS_TEXT ('<'):
            os << LOG4CPLUS_TEXT ("&lt;");
            break;

        case LOG4CPLUS_TEXT ('>'):
            os << LOG4CPLUS_TEXT ("&gt;");
            break;

        case LOG4CPLUS_TEXT ('&'):
            os << LOG4CPLUS_TEXT ("&amp;");
            break;

        case LOG4CPLUS_TEXT ('\''):
            os << LOG4CPLUS_TEXT ("&apos;");
            break;

        case LOG4CPLUS_TEXT ('"'):
            os << LOG4CPLUS_TEXT ("&quot;");
            break;

        default:
            if (is_control (ch))
            {
                tchar const prev_fill = os.fill ();
                std::ios_base::fmtflags const prev_flags = os.flags ();
                os.flags (std::ios_base::hex | std::ios_base::right);
                os.fill (LOG4CPLUS_TEXT ('0'));

                os << std::setw (0) << LOG4CPLUS_TEXT ("&#x")
                    << std::setw (2) << std::char_traits<tchar>::to_int_type (ch)
                    << std::setw (0) << LOG4CPLUS_TEXT (";");

                os.fill (prev_fill);
                os.flags (prev_flags);
            }
            else
                os.put (ch);
        }
    }
}


//! Helper manipulator like class for escaped XML output.
struct outputXMLEscaped
{
    outputXMLEscaped (tstring const & s)
        : str (s)
    { }

    tstring const & str;
};


//! Overload stream insertion for outputXMLEscaped.
static
tostream &
operator << (tostream & os, outputXMLEscaped const & x)
{
    output_xml_escaped (os, x.str);
    return os;
}


} // namespace


//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender ctors and dtor
//////////////////////////////////////////////////////////////////////////////

Log4jUdpAppender::Log4jUdpAppender(const tstring& host_, int port_)
    : host(host_)
    , port(port_)
{
    layout.reset (new PatternLayout (LOG4CPLUS_TEXT ("%m")));
    openSocket();
}



Log4jUdpAppender::Log4jUdpAppender(const helpers::Properties & properties)
    : Appender(properties)
    , port(5000)
{
    host = properties.getProperty( LOG4CPLUS_TEXT("host"),
        LOG4CPLUS_TEXT ("localhost") );
    properties.getInt (port, LOG4CPLUS_TEXT ("port"));

    openSocket();
}



Log4jUdpAppender::~Log4jUdpAppender()
{
    destructorImpl();
}



//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender public methods
//////////////////////////////////////////////////////////////////////////////

void 
Log4jUdpAppender::close()
{
    helpers::getLogLog().debug(
        LOG4CPLUS_TEXT("Entering Log4jUdpAppender::close()..."));

    socket.close();
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// Log4jUdpAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void
Log4jUdpAppender::openSocket()
{
    if(!socket.isOpen()) {
        socket = helpers::Socket(host, port, true);
    }
}

void
Log4jUdpAppender::append(const spi::InternalLoggingEvent& event)
{
    if(!socket.isOpen()) {
        openSocket();
        if(!socket.isOpen()) {
            helpers::getLogLog().error(
                LOG4CPLUS_TEXT("Log4jUdpAppender::append()- Cannot connect to server"));
            return;
        }
    }

    tstring & str = formatEvent (event);

    internal::appender_sratch_pad & appender_sp
        = internal::get_appender_sp ();
    tostringstream & buffer = appender_sp.oss;
    detail::clear_tostringstream (buffer);

    buffer << LOG4CPLUS_TEXT("<log4j:event logger=\"")
           << outputXMLEscaped (event.getLoggerName())
           << LOG4CPLUS_TEXT("\" level=\"")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (getLogLevelManager().toString(event.getLogLevel()))
           << LOG4CPLUS_TEXT("\" timestamp=\"")
           << event.getTimestamp().getFormattedTime(LOG4CPLUS_TEXT("%s%q"))
           << LOG4CPLUS_TEXT("\" thread=\"") << event.getThread()
           << LOG4CPLUS_TEXT("\">")

           << LOG4CPLUS_TEXT("<log4j:message>")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (str)
           << LOG4CPLUS_TEXT("</log4j:message>")

           << LOG4CPLUS_TEXT("<log4j:NDC>")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (event.getNDC())
           << LOG4CPLUS_TEXT("</log4j:NDC>")

           << LOG4CPLUS_TEXT("<log4j:locationInfo class=\"\" file=\"")
           // TODO: Some escaping of special characters is needed here.
           << outputXMLEscaped (event.getFile())
           << LOG4CPLUS_TEXT("\" method=\"")
           << outputXMLEscaped (event.getFunction())
           << LOG4CPLUS_TEXT("\" line=\"")
           << event.getLine()
           << LOG4CPLUS_TEXT("\"/>")
           << LOG4CPLUS_TEXT("</log4j:event>");

    LOG4CPLUS_TSTRING_TO_STRING (buffer.str ()).swap (appender_sp.chstr);

    bool ret = socket.write(appender_sp.chstr);
    if (!ret)
    {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT(
                "Log4jUdpAppender::append()- Cannot write to server"));
    }
}

} // namespace log4cplus
