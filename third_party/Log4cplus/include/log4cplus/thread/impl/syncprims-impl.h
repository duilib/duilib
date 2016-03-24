// -*- C++ -*-
//  Copyright (C) 2009-2013, Vaclav Haisman. All rights reserved.
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

#ifndef LOG4CPLUS_THREAD_SYNCPRIMS_IMPL_H
#define LOG4CPLUS_THREAD_SYNCPRIMS_IMPL_H

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if ! defined (INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif

#include <stdexcept>
#include <log4cplus/thread/syncprims.h>
#if defined (_WIN32)
#  include <log4cplus/config/windowsh-inc.h>

#elif defined (LOG4CPLUS_USE_PTHREADS)
#  include <errno.h>
#  include <pthread.h>
#  include <semaphore.h>
#  if defined (LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
#    include <sstream>
#    include <string>
#    if defined (LOG4CPLUS_HAVE_SYS_TYPES_H)
#      include <sys/types.h>
#    endif
#    if defined (LOG4CPLUS_HAVE_UNISTD_H)
#      include <unistd.h>
#    endif
#  endif
#  if defined (LOG4CPLUS_HAVE_FCNTL_H)
#    include <fcntl.h>
#  endif
#  include <log4cplus/helpers/timehelper.h>

#endif


namespace log4cplus { namespace thread { namespace impl {


LOG4CPLUS_EXPORT void LOG4CPLUS_ATTRIBUTE_NORETURN
    syncprims_throw_exception (char const * const msg,
    char const * const file, int line);


#define LOG4CPLUS_THROW_RTE(msg) \
    do { syncprims_throw_exception (msg, __FILE__, __LINE__); } while (0)


class ManualResetEvent;


class Mutex
    : public MutexImplBase
{
public:
    explicit Mutex (log4cplus::thread::Mutex::Type);
    ~Mutex ();

    void lock () const;
    void unlock () const;

private:
#if defined (LOG4CPLUS_USE_PTHREADS)
    mutable pthread_mutex_t mtx;
    friend class ManualResetEvent;
#elif defined (LOG4CPLUS_USE_WIN32_THREADS)
    mutable CRITICAL_SECTION cs;
#endif
    
    Mutex (Mutex const &);
    Mutex & operator = (Mutex &);
};


typedef SyncGuard<Mutex> MutexGuard;


class Semaphore
    : public SemaphoreImplBase
{
public:
    Semaphore (unsigned max, unsigned initial);
    ~Semaphore ();

    void lock () const;
    void unlock () const;

private:
#if defined (LOG4CPLUS_USE_PTHREADS)
#  if defined (LOG4CPLUS_USE_NAMED_POSIX_SEMAPHORE)
    sem_t * sem;
#  else
    mutable sem_t sem;
#  endif
#elif defined (LOG4CPLUS_USE_WIN32_THREADS)
    HANDLE sem;
#endif

    Semaphore (Semaphore const &);
    Semaphore & operator = (Semaphore const &);
};


typedef SyncGuard<Semaphore> SemaphoreGuard;


class FairMutex
    : public FairMutexImplBase
{
public:
    FairMutex ();
    ~FairMutex ();

    void lock () const;
    void unlock () const;

private:
#if defined (LOG4CPLUS_USE_PTHREADS)
    Semaphore sem;
#elif defined (LOG4CPLUS_USE_WIN32_THREADS)
    HANDLE mtx;
#endif

    FairMutex (FairMutex const &);
    FairMutex & operator = (FairMutex &);
};


typedef SyncGuard<FairMutex> FairMutexGuard;


class ManualResetEvent
    : public ManualResetEventImplBase
{
public:
    ManualResetEvent (bool = false);
    ~ManualResetEvent ();

    void signal () const;
    void wait () const;
    bool timed_wait (unsigned long msec) const;
    void reset () const;

private:
#if defined (LOG4CPLUS_USE_PTHREADS)
    mutable pthread_cond_t cv;
    mutable Mutex mtx;
    mutable volatile unsigned sigcount;
    mutable volatile bool signaled;
#elif defined (LOG4CPLUS_USE_WIN32_THREADS)
    HANDLE ev;
#endif

    ManualResetEvent (ManualResetEvent const &);
    ManualResetEvent & operator = (ManualResetEvent const &);
};


class SharedMutex
    : public SharedMutexImplBase
{
public:
    SharedMutex ();
    ~SharedMutex ();

    void rdlock () const;
    void wrlock () const;
    void rdunlock () const;
    void wrunlock () const;

private:
#if defined (LOG4CPLUS_POOR_MANS_SHAREDMUTEX)
    Mutex m1;
    Mutex m2;
    Mutex m3;
    Semaphore w;
    mutable unsigned writer_count;
    Semaphore r;
    mutable unsigned reader_count;

#elif defined (LOG4CPLUS_USE_PTHREADS)
    void unlock () const;

    mutable pthread_rwlock_t rwl;

#elif defined (LOG4CPLUS_USE_SRW_LOCK)
    mutable SRWLOCK srwl;

#endif

    SharedMutex (SharedMutex const &);
    SharedMutex & operator = (SharedMutex const &);
};


} } } // namespace log4cplus { namespace thread { namespace impl {


// Include the appropriate implementations of the classes declared
// above.

#if defined (LOG4CPLUS_USE_PTHREADS)
#  include <log4cplus/thread/impl/syncprims-pthreads.h>
#elif defined (LOG4CPLUS_USE_WIN32_THREADS)
#  include <log4cplus/thread/impl/syncprims-win32.h>
#endif


#undef LOG4CPLUS_THROW_RTE


#endif // LOG4CPLUS_THREAD_SYNCPRIMS_IMPL_H
