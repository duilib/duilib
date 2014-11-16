// Module:  LOG4CPLUS
// File:    nteventlogappender.cxx
// Created: 4/2003
// Author:  Michael CATANZARITI
//
// Copyright 2003-2013 Michael CATANZARITI
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
#if defined (LOG4CPLUS_HAVE_NT_EVENT_LOG)

#include <log4cplus/nteventlogappender.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <sstream>
#include <cstdlib>


namespace log4cplus
{


//////////////////////////////////////////////////////////////////////////////
// File LOCAL methods
//////////////////////////////////////////////////////////////////////////////

namespace {

    static
    bool
    copySID(SID** ppDstSid, SID* pSrcSid) 
    {
        DWORD dwLength = ::GetLengthSid(pSrcSid);

        SID * pDstSid = (SID *) std::calloc (1, dwLength);
        if (! pDstSid)
            return false;

        if (CopySid(dwLength, pDstSid, pSrcSid))
        {
            *ppDstSid = pDstSid;
            return true;
        }
        else
        {
            std::free (pDstSid);
            return false;
        }
    }


    static
    bool 
    GetCurrentUserSID(SID** ppSid) 
    {
        bool bSuccess = false;
        TOKEN_USER * ptu = 0;
        DWORD tusize = 0;
        HANDLE hProcess = ::GetCurrentProcess();
        HANDLE hToken = 0;

        if (! ::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
            goto finish;

        // Get the required size
        if (! GetTokenInformation(hToken, TokenUser, NULL, 0, &tusize))
            goto finish;

        ptu = (TOKEN_USER*) std::calloc (1, tusize);
        if (! ptu)
            goto finish;

        if (GetTokenInformation(hToken, TokenUser, (LPVOID)ptu, tusize, &tusize))
            bSuccess = copySID (ppSid, (SID *)ptu->User.Sid);

    finish:;
        if (hToken)
            CloseHandle (hToken);

        std::free (ptu);

        return bSuccess;
    }


    static
    HKEY 
    regGetKey(const tstring& subkey, DWORD* disposition)
    {
        HKEY hkey = 0;
        RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
                       subkey.c_str(), 
                       0, 
                       NULL, 
                       REG_OPTION_NON_VOLATILE, 
                       KEY_SET_VALUE, 
                       NULL, 
                       &hkey, 
                       disposition);
        return hkey;
    }


    static
    void 
    regSetString(HKEY hkey, const tstring& name, const tstring& value)
    {
        RegSetValueEx(hkey, 
                      name.c_str(), 
                      0, 
                      REG_SZ, 
                      reinterpret_cast<BYTE const *>(value.c_str()),
                      static_cast<DWORD>(value.length() * sizeof(tchar)));
    }


    static
    void 
    regSetDword(HKEY hkey, const tstring& name, DWORD value)
    {
        RegSetValueEx(hkey, 
                      name.c_str(), 
                      0, 
                      REG_DWORD, 
                      reinterpret_cast<LPBYTE>(&value), 
                      sizeof(DWORD));
    }

}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender ctor and dtor
//////////////////////////////////////////////////////////////////////////////

NTEventLogAppender::NTEventLogAppender(const tstring& server, 
                                       const tstring& log, 
                                       const tstring& source)
: server(server), 
  log(log), 
  source(source), 
  hEventLog(NULL), 
  pCurrentUserSID(NULL)
{
    init();
}



NTEventLogAppender::NTEventLogAppender(const helpers::Properties & properties)
: Appender(properties),
  hEventLog(NULL), 
  pCurrentUserSID(NULL)
{
    server = properties.getProperty( LOG4CPLUS_TEXT("server") );
    log = properties.getProperty( LOG4CPLUS_TEXT("log") );
    source = properties.getProperty( LOG4CPLUS_TEXT("source") );

    init();
}



void 
NTEventLogAppender::init()
{
    if(source.empty()) {
        helpers::getLogLog().warn(
            LOG4CPLUS_TEXT("Source option not set for appender [")
            + name 
            + LOG4CPLUS_TEXT("]."));
        return;
    }

    if(log.empty ()) {
        log = LOG4CPLUS_TEXT("Application");
    }

    // current user security identifier
    GetCurrentUserSID(&pCurrentUserSID);

    addRegistryInfo();

    hEventLog = ::RegisterEventSource(server.empty () ? 0 : server.c_str(),
        source.c_str());
    if (! hEventLog || hEventLog == HANDLE(ERROR_INVALID_HANDLE))
        helpers::getLogLog().warn (
            LOG4CPLUS_TEXT("Event source registration failed."));
}



NTEventLogAppender::~NTEventLogAppender()
{
    destructorImpl();

    if(pCurrentUserSID != NULL) {
        std::free (pCurrentUserSID);
        pCurrentUserSID = NULL;
    }
}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender public methods
//////////////////////////////////////////////////////////////////////////////

void 
NTEventLogAppender::close()
{
    if(hEventLog != NULL) {
        ::DeregisterEventSource(hEventLog);
        hEventLog = NULL;
    }
    closed = true;
}



//////////////////////////////////////////////////////////////////////////////
// NTEventLogAppender protected methods
//////////////////////////////////////////////////////////////////////////////

void 
NTEventLogAppender::append(const spi::InternalLoggingEvent& event)
{
    if(hEventLog == NULL) {
        helpers::getLogLog().warn(LOG4CPLUS_TEXT("NT EventLog not opened."));
        return;
    }

    tstring & str = formatEvent (event);

    // From MSDN documentation for ReportEvent():
    // Each string is limited to 31,839 characters.
    if (str.size () > 31839)
        str.resize (31839);

    const tchar * s = str.c_str ();
    BOOL bSuccess = ::ReportEvent(hEventLog,
                                  getEventType(event),
                                  getEventCategory(event),
                                  0x1000,
                                  pCurrentUserSID,
                                  1,
                                  0,
                                  &s,
                                  NULL);

    if(!bSuccess) {
        helpers::getLogLog().error(
            LOG4CPLUS_TEXT("Cannot report event in NT EventLog."));
    }
}




WORD 
NTEventLogAppender::getEventType(const spi::InternalLoggingEvent& event)
{
    WORD ret_val;
    LogLevel const ll = event.getLogLevel();

    if (ll >= ERROR_LOG_LEVEL) // or FATAL_LOG_LEVEL
        ret_val = EVENTLOG_ERROR_TYPE;
    else if (ll >= WARN_LOG_LEVEL)
        ret_val = EVENTLOG_WARNING_TYPE;
    else // INFO_LOG_LEVEL or DEBUG_LOG_LEVEL or TRACE_LOG_LEVEL
        ret_val = EVENTLOG_INFORMATION_TYPE;

    return ret_val;
}



WORD 
NTEventLogAppender::getEventCategory(const spi::InternalLoggingEvent& event)
{
    WORD ret_val;
    LogLevel const ll = event.getLogLevel();

    if (ll >= FATAL_LOG_LEVEL)
        ret_val = 1;
    else if (ll >= ERROR_LOG_LEVEL)
        ret_val = 2;
    else if (ll >= WARN_LOG_LEVEL)
        ret_val = 3;
    else if (ll >= INFO_LOG_LEVEL)
        ret_val = 4;
    else if (ll >= DEBUG_LOG_LEVEL)
        ret_val = 5;
    else // TRACE_LOG_LEVEL
        ret_val = 6;

    return ret_val;
}


// Add this source with appropriate configuration keys to the registry.
void 
NTEventLogAppender::addRegistryInfo()
{
    DWORD disposition;
    HKEY hkey = 0;
    tstring subkey =   LOG4CPLUS_TEXT("SYSTEM\\CurrentControlSet\\Services\\EventLog\\")
                     + log 
                     + LOG4CPLUS_TEXT("\\") 
                     + source;
    
    hkey = regGetKey(subkey, &disposition);
    if(disposition == REG_CREATED_NEW_KEY) {
        regSetString(hkey, 
                     LOG4CPLUS_TEXT("EventMessageFile"), 
                     LOG4CPLUS_TEXT("NTEventLogAppender.dll"));
        regSetString(hkey, 
                     LOG4CPLUS_TEXT("CategoryMessageFile"), 
                     LOG4CPLUS_TEXT("NTEventLogAppender.dll"));
        regSetDword(hkey, LOG4CPLUS_TEXT("TypesSupported"), (DWORD)7);
        regSetDword(hkey, LOG4CPLUS_TEXT("CategoryCount"), (DWORD)5);
    }
    
    RegCloseKey(hkey);
    return;
}


} // namespace log4cplus


#endif // LOG4CPLUS_HAVE_NT_EVENT_LOG
