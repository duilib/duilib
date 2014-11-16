// -*- C++ -*-
//  Copyright (C) 2010-2013, Vaclav Haisman. All rights reserved.
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

#ifndef LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H
#define LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#if (defined (LOG4CPLUS_INLINES_ARE_EXPORTED)           \
    && defined (LOG4CPLUS_BUILD_DLL))                   \
    || defined (LOG4CPLUS_ENABLE_SYNCPRIMS_PUB_IMPL)
#include <log4cplus/thread/syncprims.h>

#if defined (LOG4CPLUS_SINGLE_THREADED)
#  define LOG4CPLUS_THREADED(x)
#else
#  include <log4cplus/thread/impl/syncprims-impl.h>
#  define LOG4CPLUS_THREADED(x) (x)
#endif


namespace log4cplus { namespace thread {


//
//
//

LOG4CPLUS_INLINE_EXPORT
MutexImplBase::~MutexImplBase ()
{ }


//
//
//

LOG4CPLUS_INLINE_EXPORT
Mutex::Mutex (Mutex::Type t)
    : mtx (LOG4CPLUS_THREADED (new impl::Mutex (t)) + 0)
{ }


LOG4CPLUS_INLINE_EXPORT
Mutex::~Mutex ()
{
    LOG4CPLUS_THREADED (delete static_cast<impl::Mutex *>(mtx));
}


LOG4CPLUS_INLINE_EXPORT
void
Mutex::lock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::Mutex *>(mtx)->lock ());
}


LOG4CPLUS_INLINE_EXPORT
void
Mutex::unlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::Mutex *>(mtx)->unlock ());
}


//
//
//

LOG4CPLUS_INLINE_EXPORT
SemaphoreImplBase::~SemaphoreImplBase ()
{ }


//
//
//

LOG4CPLUS_INLINE_EXPORT
Semaphore::Semaphore (unsigned LOG4CPLUS_THREADED (max),
    unsigned LOG4CPLUS_THREADED (initial))
    : sem (LOG4CPLUS_THREADED (new impl::Semaphore (max, initial)) + 0)
{ }


LOG4CPLUS_INLINE_EXPORT
Semaphore::~Semaphore ()
{
    LOG4CPLUS_THREADED (delete static_cast<impl::Semaphore *>(sem));
}


LOG4CPLUS_INLINE_EXPORT
void
Semaphore::lock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::Semaphore *>(sem)->lock ());
}


LOG4CPLUS_INLINE_EXPORT
void
Semaphore::unlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::Semaphore *>(sem)->unlock ());
}


//
//
//

LOG4CPLUS_INLINE_EXPORT
FairMutexImplBase::~FairMutexImplBase ()
{ }


//
//
//

LOG4CPLUS_INLINE_EXPORT
FairMutex::FairMutex ()
    : mtx (LOG4CPLUS_THREADED (new impl::FairMutex) + 0)
{ }


LOG4CPLUS_INLINE_EXPORT
FairMutex::~FairMutex ()
{
    LOG4CPLUS_THREADED (delete static_cast<impl::FairMutex *>(mtx));
}


LOG4CPLUS_INLINE_EXPORT
void
FairMutex::lock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::FairMutex *>(mtx)->lock ());
}


LOG4CPLUS_INLINE_EXPORT
void
FairMutex::unlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::FairMutex *>(mtx)->unlock ());
}


//
//
//

LOG4CPLUS_INLINE_EXPORT
ManualResetEventImplBase::~ManualResetEventImplBase ()
{ }


//
//
//

LOG4CPLUS_INLINE_EXPORT
ManualResetEvent::ManualResetEvent (bool LOG4CPLUS_THREADED (sig))
    : ev (LOG4CPLUS_THREADED (new impl::ManualResetEvent (sig)) + 0)
{ }


LOG4CPLUS_INLINE_EXPORT
ManualResetEvent::~ManualResetEvent ()
{
    LOG4CPLUS_THREADED (delete static_cast<impl::ManualResetEvent *>(ev));
}


LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::signal () const
{
    LOG4CPLUS_THREADED (static_cast<impl::ManualResetEvent *>(ev)->signal ());
}


LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::wait () const
{
    LOG4CPLUS_THREADED (static_cast<impl::ManualResetEvent *>(ev)->wait ());
}


LOG4CPLUS_INLINE_EXPORT
bool
ManualResetEvent::timed_wait (unsigned long LOG4CPLUS_THREADED (msec)) const
{
#if defined (LOG4CPLUS_SINGLE_THREADED)
    return true;
#else
    return static_cast<impl::ManualResetEvent *>(ev)->timed_wait (msec);
#endif
}


LOG4CPLUS_INLINE_EXPORT
void
ManualResetEvent::reset () const
{
    LOG4CPLUS_THREADED (static_cast<impl::ManualResetEvent *>(ev)->reset ());
}


//
//
//

LOG4CPLUS_INLINE_EXPORT
SharedMutexImplBase::~SharedMutexImplBase ()
{ }


//
//
//

LOG4CPLUS_INLINE_EXPORT
SharedMutex::SharedMutex ()
    : sm (LOG4CPLUS_THREADED (new impl::SharedMutex) + 0)
{ }


LOG4CPLUS_INLINE_EXPORT
SharedMutex::~SharedMutex ()
{ }


LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::rdlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::SharedMutex *>(sm)->rdlock ());
}


LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::wrlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::SharedMutex *>(sm)->wrlock ());
}


LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::rdunlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::SharedMutex *>(sm)->rdunlock ());
}


LOG4CPLUS_INLINE_EXPORT
void
SharedMutex::wrunlock () const
{
    LOG4CPLUS_THREADED (static_cast<impl::SharedMutex *>(sm)->wrunlock ());
}


} } // namespace log4cplus { namespace thread { 

#endif // LOG4CPLUS_ENABLE_SYNCPRIMS_PUB_IMPL

#endif // LOG4CPLUS_THREAD_SYNCPRIMS_PUB_IMPL_H
