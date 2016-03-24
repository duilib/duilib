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

//! @file
//! This file contains implementations of reader-writer locking
//! primitive using other primitives, IOW poor man's rwlock.
//! It does not contain any include guards because it is only a fragment
//! to be included by syncprims-{pthreads,win32}.h.


#if ! defined (INSIDE_LOG4CPLUS)
#  error "This header must not be be used outside log4cplus' implementation files."
#endif


// This implements algorithm described in "Concurrent Control with "Readers"
// and "Writers"; P.J. Courtois, F. Heymans, and D.L. Parnas;
// MBLE Research Laboratory; Brussels, Belgium"


inline
SharedMutex::SharedMutex ()
    : m1 (log4cplus::thread::Mutex::DEFAULT)
    , m2 (log4cplus::thread::Mutex::DEFAULT)
    , m3 (log4cplus::thread::Mutex::DEFAULT)
    , w (1, 1)
    , writer_count (0)
    , r (1, 1)
    , reader_count (0)
{ }


inline
SharedMutex::~SharedMutex ()
{ }


inline
void
SharedMutex::rdlock () const
{
    MutexGuard m3_guard (m3);
    SemaphoreGuard r_guard (r);
    MutexGuard m1_guard (m1);
    if (reader_count + 1 == 1)
        w.lock ();

    reader_count += 1;
}


inline
void
SharedMutex::rdunlock () const
{
    MutexGuard m1_guard (m1);
    if (reader_count - 1 == 0)
        w.unlock ();

    reader_count -= 1;
}


inline
void
SharedMutex::wrlock () const
{
    {
        MutexGuard m2_guard (m2);
        if (writer_count + 1 == 1)
            r.lock ();

        writer_count += 1;
    }
    try
    {
        w.lock ();
    }
    catch (...)
    {
        MutexGuard m2_guard (m2);
        writer_count -= 1;
        throw;
    }
}


inline
void
SharedMutex::wrunlock () const
{
    w.unlock ();
    MutexGuard m2_guard (m2);
    if (writer_count - 1 == 0)
        r.unlock ();

    writer_count -= 1;
}
