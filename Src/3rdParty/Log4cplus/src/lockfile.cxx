// -*- C++ -*-
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

#if defined (LOG4CPLUS_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined (LOG4CPLUS_HAVE_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if defined (LOG4CPLUS_HAVE_SYS_FILE_H)
#include <sys/file.h>
#endif
#if defined (LOG4CPLUS_HAVE_SYS_LOCKING_H)
#include <sys/locking.h>
#endif
#if defined (LOG4CPLUS_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined (LOG4CPLUS_HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined (LOG4CPLUS_HAVE_IO_H)
#include <io.h>
#endif
#if defined (_WIN32)
#include <tchar.h>
#include <share.h>
#endif
#include <log4cplus/config/windowsh-inc.h>

#include <stdexcept>
#include <cerrno>
#include <limits>
#include <cstring>

#include <log4cplus/helpers/lockfile.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/loglog.h>

#if defined (_WIN32)
#  if _WIN32_WINNT < 0x0501
#    define LOG4CPLUS_USE_WIN32_LOCKING
#  else
#    define LOG4CPLUS_USE_WIN32_LOCKFILEEX
#  endif
#else
#  if defined (O_EXLOCK)
#    define LOG4CPLUS_USE_O_EXLOCK
#  elif defined (LOG4CPLUS_HAVE_FCNTL) && defined (F_SETLKW)
#    define LOG4CPLUS_USE_SETLKW
#  elif defined (LOG4CPLUS_HAVE_LOCKF)
#    define LOG4CPLUS_USE_LOCKF
#  elif defined (LOG4CPLUS_HAVE_FLOCK)
#    define LOG4CPLUS_USE_FLOCK
#  endif
#  if defined (LOG4CPLUS_USE_O_EXLOCK) || defined (LOG4CPLUS_USE_SETLKW) \
    || defined (LOG4CPLUS_USE_LOCKF) || defined (LOG4CPLUS_USE_FLOCK)
#    define LOG4CPLUS_USE_POSIX_LOCKING
#  endif
#endif

#if ! defined (LOG4CPLUS_USE_POSIX_LOCKING) && ! defined (_WIN32)
#error "no usable file locking"
#endif

namespace log4cplus { namespace helpers {


#if defined (_WIN32)
#if defined (__BORLANDC__)
int const OPEN_FLAGS = O_RDWR | O_CREAT | O_NOINHERIT;
int const OPEN_SHFLAGS = SH_DENYNO;
int const OPEN_MODE = S_IREAD | S_IWRITE;
#else
int const OPEN_FLAGS = _O_RDWR | _O_CREAT /*| _O_TEMPORARY*/ | _O_NOINHERIT;
int const OPEN_SHFLAGS = _SH_DENYNO;
int const OPEN_MODE = _S_IREAD | _S_IWRITE;
#endif

namespace
{

static
HANDLE
get_os_HANDLE (int fd, helpers::LogLog & loglog = helpers::getLogLog ())
{
    HANDLE fh = reinterpret_cast<HANDLE>(_get_osfhandle (fd));
    if (fh == INVALID_HANDLE_VALUE)
        loglog.error (tstring (LOG4CPLUS_TEXT ("_get_osfhandle() failed: "))
            + convertIntegerToString (errno), true);

    return fh;
}

} // namespace

#elif defined (LOG4CPLUS_USE_POSIX_LOCKING)
int const OPEN_FLAGS = O_RDWR | O_CREAT
#if defined (O_CLOEXEC)
    | O_CLOEXEC
#endif
    ;

mode_t const OPEN_MODE = (S_IRWXU ^ S_IXUSR)
    | (S_IRWXG ^ S_IXGRP)
    | (S_IRWXO ^ S_IXOTH);

#endif


//! Helper function that sets FD_CLOEXEC on descriptor on platforms
//! that support it.
LOG4CPLUS_PRIVATE
bool
trySetCloseOnExec (int fd, helpers::LogLog & loglog = helpers::getLogLog ())
{
#if defined (WIN32)
    int ret = SetHandleInformation (get_os_HANDLE (fd), HANDLE_FLAG_INHERIT, 0);
    if (! ret)
    {
        DWORD eno = GetLastError ();
        loglog.warn (
            tstring (
                LOG4CPLUS_TEXT ("could not unset HANDLE_FLAG_INHERIT on fd: "))
            + convertIntegerToString (fd)
            + LOG4CPLUS_TEXT (", errno: ")
            + convertIntegerToString (eno));
        return false;        
    }

#elif defined (FD_CLOEXEC)
    int ret = fcntl (fd, F_SETFD, FD_CLOEXEC);
    if (ret == -1)
    {
        int eno = errno;
        loglog.warn (
            tstring (LOG4CPLUS_TEXT ("could not set FD_CLOEXEC on fd: "))
            + convertIntegerToString (fd)
            + LOG4CPLUS_TEXT (", errno: ")
            + convertIntegerToString (eno));
        return false;
    }
#else
    return false;

#endif

    return true;
}


//
//
//

struct LockFile::Impl
{
#if defined (LOG4CPLUS_USE_POSIX_LOCKING) \
    || defined (_WIN32)
    int fd;

#endif
};


//
//
//

LockFile::LockFile (tstring const & lf)
    : lock_file_name (lf)
    , data (new LockFile::Impl)
{
#if defined (LOG4CPLUS_USE_O_EXLOCK)
    data->fd = -1;

#else
    open (OPEN_FLAGS);

#endif
}


LockFile::~LockFile ()
{
    close ();
    delete data;
}


void
LockFile::open (int open_flags) const
{
    LogLog & loglog = getLogLog ();

#if defined (_WIN32)
#  if defined (LOG4CPLUS_HAVE__TSOPEN_S) && defined (_tsopen_s)
    errno_t eno = _tsopen_s (&data->fd, lock_file_name.c_str (), open_flags,
        OPEN_SHFLAGS, OPEN_MODE);
    if (eno != 0)
#  elif defined (LOG4CPLUS_HAVE__TSOPEN) && defined (_tsopen)
    data->fd = _tsopen (lock_file_name.c_str (), open_flags, OPEN_SHFLAGS,
        OPEN_MODE);
    if (data->fd == -1)
#  else
#    error "Neither _tsopen_s() nor _tsopen() is available."
#  endif
        loglog.error (tstring (LOG4CPLUS_TEXT("could not open or create file "))
            + lock_file_name, true);

#elif defined (LOG4CPLUS_USE_POSIX_LOCKING)
    data->fd = ::open (LOG4CPLUS_TSTRING_TO_STRING (lock_file_name).c_str (),
        open_flags, OPEN_MODE);
    if (data->fd == -1)
        loglog.error (
            tstring (LOG4CPLUS_TEXT ("could not open or create file "))
            + lock_file_name, true);

#if ! defined (O_CLOEXEC)
    if (! trySetCloseOnExec (data->fd, loglog))
        loglog.warn (
            tstring (LOG4CPLUS_TEXT("could not set FD_CLOEXEC on file "))
            + lock_file_name);

#endif
#endif
}


void
LockFile::close () const
{
#if defined (_WIN32)
    if (data->fd >= 0)
        _close (data->fd);

    data->fd = -1;

#elif defined (LOG4CPLUS_USE_POSIX_LOCKING)
    if (data->fd >= 0)
        ::close (data->fd);

    data->fd = -1;

#endif
}


void
LockFile::lock () const
{
    LogLog & loglog = getLogLog ();
    int ret = 0;
    (void) loglog;
    (void) ret;

#if defined (LOG4CPLUS_USE_WIN32_LOCKFILEEX)
    HANDLE fh = get_os_HANDLE (data->fd, loglog);

    OVERLAPPED overlapped;
    std::memset (&overlapped, 0, sizeof (overlapped));
    overlapped.hEvent = 0;

    ret = LockFileEx(fh, LOCKFILE_EXCLUSIVE_LOCK, 0,
        (std::numeric_limits<DWORD>::max) (),
        (std::numeric_limits<DWORD>::max) (), &overlapped);
    if (! ret)
        loglog.error (tstring (LOG4CPLUS_TEXT ("LockFileEx() failed: "))
            + convertIntegerToString (GetLastError ()), true);

#elif defined (LOG4CPLUS_USE_WIN32_LOCKING)
    ret = _locking (data->fd, _LK_LOCK, (std::numeric_limits<long>::max) ());
    if (ret != 0)
        loglog.error (tstring (LOG4CPLUS_TEXT ("_locking() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (LOG4CPLUS_USE_O_EXLOCK)
    open (OPEN_FLAGS | O_EXLOCK);

#elif defined (LOG4CPLUS_USE_SETLKW)
    do
    {
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        ret = fcntl (data->fd, F_SETLKW, &fl);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (LOG4CPLUS_TEXT("fcntl(F_SETLKW) failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#elif defined (LOG4CPLUS_USE_LOCKF)
    do
    {
        ret = lockf (data->fd, F_LOCK, 0);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (LOG4CPLUS_TEXT("lockf() failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#elif defined (LOG4CPLUS_USE_FLOCK)
    do
    {
        ret = flock (data->fd, LOCK_EX);
        if (ret == -1 && errno != EINTR)
            loglog.error (tstring (LOG4CPLUS_TEXT("flock() failed: "))
                + convertIntegerToString (errno), true);
    }
    while (ret == -1);

#endif
}


void LockFile::unlock () const
{
    LogLog & loglog = getLogLog ();
    int ret = 0;

#if defined (LOG4CPLUS_USE_WIN32_LOCKFILEEX)
    HANDLE fh = get_os_HANDLE (data->fd, loglog);

    ret = UnlockFile(fh, 0, 0, (std::numeric_limits<DWORD>::max) (),
        (std::numeric_limits<DWORD>::max) ());
    if (! ret)
        loglog.error (tstring (LOG4CPLUS_TEXT ("UnlockFile() failed: "))
            + convertIntegerToString (GetLastError ()), true);

#elif defined (LOG4CPLUS_USE_WIN32_LOCKING)
    ret = _locking (data->fd, _LK_UNLCK, (std::numeric_limits<long>::max) ());
    if (ret != 0)
        loglog.error (tstring (LOG4CPLUS_TEXT ("_locking() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (LOG4CPLUS_USE_O_EXLOCK)
    close ();

#elif defined (LOG4CPLUS_USE_SETLKW)
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    ret = fcntl (data->fd, F_SETLKW, &fl);
    if (ret != 0)
        loglog.error (tstring (LOG4CPLUS_TEXT("fcntl(F_SETLKW) failed: "))
            + convertIntegerToString (errno), true);

#elif defined (LOG4CPLUS_USE_LOCKF)
    ret = lockf (data->fd, F_ULOCK, 0);
    if (ret != 0)
        loglog.error (tstring (LOG4CPLUS_TEXT("lockf() failed: "))
            + convertIntegerToString (errno), true);

#elif defined (LOG4CPLUS_USE_FLOCK)
    ret = flock (data->fd, LOCK_UN);
    if (ret != 0)
        loglog.error (tstring (LOG4CPLUS_TEXT("flock() failed: "))
            + convertIntegerToString (errno), true);

#endif

}



} } // namespace log4cplus { namespace helpers {
