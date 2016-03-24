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

#include <log4cplus/config.hxx>
#ifndef LOG4CPLUS_SINGLE_THREADED

#include <log4cplus/helpers/queue.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/syncprims-pub-impl.h>
#include <stdexcept>
#include <algorithm>
#include <iterator>


namespace log4cplus { namespace thread {


Queue::Queue (unsigned len)
    : mutex (Mutex::DEFAULT)
    , ev_consumer (false)
    , sem (len, len) 
    , flags (DRAIN)
{ }


Queue::~Queue ()
{ }


Queue::flags_type
Queue::put_event (spi::InternalLoggingEvent const & ev)
{
    flags_type ret_flags = ERROR_BIT;
    try
    {
        ev.gatherThreadSpecificData ();
        
        SemaphoreGuard semguard (sem);
        MutexGuard mguard (mutex);

        ret_flags |= flags;
        
        if (flags & EXIT)
        {
            ret_flags &= ~(ERROR_BIT | ERROR_AFTER);
            return ret_flags;
        }
        else
        {
            queue.push_back (ev);
            ret_flags |= ERROR_AFTER;
            semguard.detach ();
            flags |= QUEUE;
            ret_flags |= flags;
            mguard.unlock ();
            mguard.detach ();
            ev_consumer.signal ();
        }
    }
    catch (std::runtime_error const & e)
    {
        (void)e;
        return ret_flags;
    }

    ret_flags &= ~(ERROR_BIT | ERROR_AFTER);
    return ret_flags;
}


Queue::flags_type
Queue::signal_exit (bool drain)
{
    flags_type ret_flags = 0;

    try
    {
        MutexGuard mguard (mutex);

        ret_flags |= flags;

        if (! (flags & EXIT))
        {
            if (drain)
                flags |= DRAIN;
            else
                flags &= ~DRAIN;
            flags |= EXIT;
            ret_flags = flags;
            mguard.unlock ();
            mguard.detach ();
            ev_consumer.signal ();
        }
    }
    catch (std::runtime_error const & e)
    {
        (void)e;
        ret_flags |= ERROR_BIT;
        return ret_flags;
    }

    return ret_flags;
}


Queue::flags_type
Queue::get_events (queue_storage_type * buf)
{
    flags_type ret_flags = 0;

    try
    {
        while (true)
        {
            MutexGuard mguard (mutex);

            ret_flags = flags;

            if (((QUEUE & flags) && ! (EXIT & flags))
                || ((EXIT | DRAIN | QUEUE) & flags) == (EXIT | DRAIN | QUEUE))
            {
                assert (! queue.empty ());

                std::size_t const count = queue.size ();
                queue.swap (*buf);
                queue.clear ();
                flags &= ~QUEUE;
                for (std::size_t i = 0; i != count; ++i)
                    sem.unlock ();

                ret_flags = flags | EVENT;
                break;
            }
            else if (((EXIT | QUEUE) & flags) == (EXIT | QUEUE))
            {
                assert (! queue.empty ());
                queue.clear ();
                flags &= ~QUEUE;
                ev_consumer.reset ();
                sem.unlock ();
                ret_flags = flags;
                break;
            }
            else if (EXIT & flags)
                break;
            else
            {
                ev_consumer.reset ();
                mguard.unlock ();
                mguard.detach ();
                ev_consumer.wait ();
            }
        }
    }
    catch (std::runtime_error const & e)
    {
        (void)e;
        ret_flags |= ERROR_BIT;
    }

    return ret_flags;
}


} } // namespace log4cplus { namespace thread {


#endif // LOG4CPLUS_SINGLE_THREADED
