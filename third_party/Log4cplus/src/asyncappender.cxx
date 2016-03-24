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

#include <log4cplus/asyncappender.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/thread/syncprims-pub-impl.h>


namespace log4cplus
{


namespace
{


class QueueThread
    : public thread::AbstractThread
{
public:
    QueueThread (AsyncAppenderPtr const &, thread::QueuePtr const &);

    virtual void run();

private:
    AsyncAppenderPtr appenders;
    thread::QueuePtr queue;
};


QueueThread::QueueThread (AsyncAppenderPtr const & aai,
    thread::QueuePtr const & q)
    : appenders (aai)
    , queue (q)
{ }


void
QueueThread::run()
{
    typedef log4cplus::thread::Queue::queue_storage_type ev_buf_type;
    ev_buf_type ev_buf;

    while (true)
    {
        unsigned flags = queue->get_events (&ev_buf);
        if (flags & thread::Queue::EVENT)
        {
            ev_buf_type::const_iterator const ev_buf_end = ev_buf.end ();
            for (ev_buf_type::const_iterator it = ev_buf.begin ();
                it != ev_buf_end; ++it)
                appenders->appendLoopOnAppenders (*it);
        }

        if (((thread::Queue::EXIT | thread::Queue::DRAIN
                | thread::Queue::EVENT) & flags)
            == (thread::Queue::EXIT | thread::Queue::DRAIN
                | thread::Queue::EVENT))
            continue;
        else if (thread::Queue::EXIT & flags)
            break;
    }
}


} // namespace


AsyncAppender::AsyncAppender (SharedAppenderPtr const & app,
    unsigned queue_len)
{
    addAppender (app);
    init_queue_thread (queue_len);
}


AsyncAppender::AsyncAppender (helpers::Properties const & props)
{
    tstring const & appender_name (
        props.getProperty (LOG4CPLUS_TEXT ("Appender")));
    if (appender_name.empty ())
    {
        getErrorHandler ()->error (
            LOG4CPLUS_TEXT ("Unspecified appender for AsyncAppender."));
        return;
    }

    spi::AppenderFactoryRegistry & appender_registry
        = spi::getAppenderFactoryRegistry ();
    spi::AppenderFactory * factory = appender_registry.get (appender_name);
    if (! factory)
    {
        tstring const err (LOG4CPLUS_TEXT ("AsyncAppender::AsyncAppender()")
            LOG4CPLUS_TEXT (" - Cannot find AppenderFactory: "));
        helpers::getLogLog ().error (err + appender_name);
        // Add at least null appender so that we do not crash unexpectedly
        // elsewhere.
        // XXX: What about throwing an exception instead?
        factory = appender_registry.get (
            LOG4CPLUS_TEXT ("log4cplus::NullAppender"));
    }

    helpers::Properties appender_props = props.getPropertySubset (
        LOG4CPLUS_TEXT ("Appender."));
    addAppender (factory->createObject (appender_props));

    unsigned queue_len = 100;
    props.getUInt (queue_len, LOG4CPLUS_TEXT ("QueueLimit"));

    init_queue_thread (queue_len);
}


AsyncAppender::~AsyncAppender ()
{
    destructorImpl ();
}


void
AsyncAppender::init_queue_thread (unsigned queue_len)
{
    queue = new thread::Queue (queue_len);
    queue_thread = new QueueThread (AsyncAppenderPtr (this), queue);
    queue_thread->start ();
    helpers::getLogLog ().debug (LOG4CPLUS_TEXT("Queue thread started."));
}


void
AsyncAppender::close ()
{
    unsigned ret = queue->signal_exit ();
    if (ret & (thread::Queue::ERROR_BIT | thread::Queue::ERROR_AFTER))
        getErrorHandler ()->error (
            LOG4CPLUS_TEXT ("Error in AsyncAppender::close"));
    queue_thread->join ();
}


void
AsyncAppender::append (spi::InternalLoggingEvent const & ev)
{
    if (queue_thread && queue_thread->isRunning ())
    {
        unsigned ret = queue->put_event (ev);
        if (ret & (thread::Queue::ERROR_BIT | thread::Queue::ERROR_AFTER))
        {
            getErrorHandler ()->error (
                LOG4CPLUS_TEXT ("Error in AsyncAppender::append,")
                LOG4CPLUS_TEXT (" event queue has been lost."));
            // Exit the queue consumer thread without draining
            // the events queue.
            queue->signal_exit (false);
            queue_thread->join ();
            queue_thread = 0;
            queue = 0;
            appendLoopOnAppenders (ev);
        }
    }
    else
    {
        // If the thread has died for any reason, fall back to synchronous
        // operation.
        appendLoopOnAppenders (ev);
    }
}


} // namespace log4cplus


#endif // #ifndef LOG4CPLUS_SINGLE_THREADED
