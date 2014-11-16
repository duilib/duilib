// Module:  Log4cplus
// File:    clfsappender.cxx
// Created: 5/2012
// Author:  Vaclav Zeman
//
//
//  Copyright (C) 2012-2013, Vaclav Zeman. All rights reserved.
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
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/clfsappender.h>
#include <log4cplus/spi/factory.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <log4cplus/config/windowsh-inc.h>
#include <clfsw32.h>
#include <clfsmgmtw32.h>


// Forward Declarations
namespace log4cplus
{

const unsigned CLFS_APPENDER_INITIAL_LOG_SIZE = 512 * 1024;
const ULONG CLFS_APPENDER_DEFAULT_BUFFER_SIZE = 1024 * 64;


namespace
{

static
void
loglog_win32_error (tchar const * msg)
{
    DWORD err = GetLastError ();
    tostringstream oss;
    oss << LOG4CPLUS_TEXT ("CLFSAppender: ") << msg << LOG4CPLUS_TEXT(": ")
        << err << LOG4CPLUS_TEXT (" / 0x")
        << std::setw (8) << std::setfill (LOG4CPLUS_TEXT ('0')) << std::hex
        << err;
    helpers::getLogLog ().error (oss.str ());
}

}


struct CLFSAppender::Data
{
    Data ()
        : log_name ()
        , log_handle (INVALID_HANDLE_VALUE)
        , buffer (0)
        , buffer_size (0)
    { }

    tstring log_name;
    HANDLE log_handle;
    void * buffer;
    ULONG buffer_size;
};


CLFSAppender::CLFSAppender (tstring const & logname, unsigned long logsize,
    unsigned long buffersize)
    : Appender ()
    , data (new Data)
{
    init (logname, logsize, buffersize);
}


CLFSAppender::CLFSAppender (helpers::Properties const & props)
    : Appender (props)
    , data (new Data)
{
    tstring logname = props.getProperty (LOG4CPLUS_TEXT ("LogName"));

    unsigned long logsize = CLFS_APPENDER_INITIAL_LOG_SIZE;
    props.getULong (logsize, LOG4CPLUS_TEXT ("LogSize"));

    unsigned long buffersize = CLFS_APPENDER_DEFAULT_BUFFER_SIZE;
    props.getULong (buffersize, LOG4CPLUS_TEXT ("BufferSize"));

    init (logname, logsize, buffersize);
}


CLFSAppender::~CLFSAppender ()
{
    destructorImpl ();
    delete data;
}


void
CLFSAppender::init (tstring const & logname, unsigned long logsize,
    unsigned long buffersize)
{
    data->log_name = logname;
    data->buffer_size = buffersize;

    if (data->log_name.empty ())
        helpers::getLogLog ().error (
            LOG4CPLUS_TEXT ("CLFSAppender: empty log name"), true);

    CLFS_MGMT_POLICY log_policy;
    std::memset (&log_policy, 0, sizeof (log_policy));
    log_policy.Version = CLFS_MGMT_POLICY_VERSION;
    log_policy.LengthInBytes = sizeof (log_policy);
    log_policy.PolicyFlags = 0;

    CLFS_INFORMATION log_info;
    ULONG info_size = sizeof (log_info);
    ULONGLONG desired_size;
    ULONGLONG resulting_size;

    data->log_handle = CreateLogFile (
        helpers::towstring (data->log_name).c_str (), GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ, 0,
        OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE);

    if (data->log_handle == INVALID_HANDLE_VALUE)
    {
        loglog_win32_error (LOG4CPLUS_TEXT ("CreateLogFile()"));
        goto error;
    }

    if (! RegisterManageableLogClient (data->log_handle, 0))
    {
        loglog_win32_error (LOG4CPLUS_TEXT ("RegisterManageableLogClient()"));
        goto error;
    }


    if (! GetLogFileInformation (data->log_handle, &log_info, &info_size))
    {
        loglog_win32_error (LOG4CPLUS_TEXT ("GetLogFileInformation()"));
        goto error;
    }

    if (log_info.TotalContainers == 0)
    {
        log_policy.PolicyType = ClfsMgmtPolicyNewContainerSize;
        log_policy.PolicyParameters.NewContainerSize.SizeInBytes = logsize;
        if (! InstallLogPolicy (data->log_handle, &log_policy))
        {
            loglog_win32_error (
                LOG4CPLUS_TEXT ("InstallLogPolicy(ClfsMgmtPolicyNewContainerSize)"));
            goto error;
        }
    }

    desired_size = 0;
    resulting_size = 0;
    if (! SetLogFileSizeWithPolicy (data->log_handle, &desired_size,
        &resulting_size))
    {
        loglog_win32_error (LOG4CPLUS_TEXT ("SetLogFileSizeWithPolicy()"));
        goto error;
    }

    log_policy.PolicyType = ClfsMgmtPolicyAutoGrow;
    log_policy.PolicyParameters.AutoGrow.Enabled = true;
    if (! InstallLogPolicy (data->log_handle, &log_policy))
    {
        loglog_win32_error (
            LOG4CPLUS_TEXT ("InstallLogPolicy(ClfsMgmtPolicyAutoGrow)"));
        goto error;
    }

    log_policy.PolicyType = ClfsMgmtPolicyGrowthRate;
    log_policy.PolicyParameters.GrowthRate.AbsoluteGrowthInContainers = 0;
    log_policy.PolicyParameters.GrowthRate.RelativeGrowthPercentage = 10;
    if (! InstallLogPolicy (data->log_handle, &log_policy))
    {
        loglog_win32_error (
            LOG4CPLUS_TEXT ("InstallLogPolicy(ClfsMgmtPolicyGrowthRate)"));
        goto error;
    }

    // TODO: Get underlying media sector size using GetDiskFreeSpace().
    // TODO: What are reasonable values for cMaxWriteBuffers
    // and cMaxReadBuffers?
    if (! CreateLogMarshallingArea (data->log_handle, 0, 0, 0,
        data->buffer_size, 8, 1, &data->buffer))
    {
        loglog_win32_error (LOG4CPLUS_TEXT ("CreateLogMarshallingArea"));
        goto error;
    }

    return;

error:
    if (data->log_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle (data->log_handle);
        data->log_handle = INVALID_HANDLE_VALUE;
    }

    return;
}


void
CLFSAppender::close ()
{
    if (data->log_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle (data->log_handle);
        data->log_handle = INVALID_HANDLE_VALUE;
    }
}


void
CLFSAppender::append (spi::InternalLoggingEvent const & ev)
{
    if (data->log_handle == INVALID_HANDLE_VALUE)
        return;

    // TODO: Expose log4cplus' internal TLS to use here.
    tostringstream oss;    
    layout->formatAndAppend(oss, ev);

    tstring str;
    oss.str ().swap (str);
    if ((str.size () + 1) * sizeof (tchar) > data->buffer_size)
        str.resize (data->buffer_size / sizeof (tchar));

    CLFS_WRITE_ENTRY clfs_write_entry;
    clfs_write_entry.Buffer = const_cast<tchar *>(str.c_str ());
    clfs_write_entry.ByteLength
        = static_cast<ULONG>((str.size () + 1) * sizeof (tchar));

    if (! ReserveAndAppendLog (data->buffer, &clfs_write_entry, 1, 0, 0, 0, 0,
        CLFS_FLAG_FORCE_APPEND, 0, 0))
        loglog_win32_error (LOG4CPLUS_TEXT ("ReserveAndAppendLog"));
}


void
CLFSAppender::registerAppender ()
{
    log4cplus::spi::AppenderFactoryRegistry & reg
        = log4cplus::spi::getAppenderFactoryRegistry ();
    LOG4CPLUS_REG_APPENDER (reg, CLFSAppender);
}


} // namespace log4cplus


extern "C"
BOOL WINAPI DllMain(LOG4CPLUS_DLLMAIN_HINSTANCE,  // handle to DLL module
                    DWORD fdwReason,     // reason for calling function
                    LPVOID)  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
    case DLL_PROCESS_ATTACH:
    {
        log4cplus::CLFSAppender::registerAppender ();
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
