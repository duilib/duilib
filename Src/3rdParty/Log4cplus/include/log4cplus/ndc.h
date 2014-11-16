// -*- C++ -*-
// Module:  Log4CPLUS
// File:    ndc.h
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright 2001-2013 Tad E. Smith
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

/** @file 
 * This header defined the NDC class.
 */

#ifndef _LO4CPLUS_NDC_HEADER_
#define _LO4CPLUS_NDC_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>

#include <map>
#include <deque>


namespace log4cplus {
    // Forward declarations
    struct DiagnosticContext;
    typedef std::deque<DiagnosticContext> DiagnosticContextStack;

    /**
     * The NDC class implements <i>nested diagnostic contexts</i> as
     * defined by Neil Harrison in the article "Patterns for Logging
     * Diagnostic Messages" part of the book "<i>Pattern Languages of
     * Program Design 3</i>" edited by Martin et al.
     *
     * A Nested Diagnostic Context, or NDC in short, is an instrument
     * to distinguish interleaved log output from different sources. Log
     * output is typically interleaved when a server handles multiple
     * clients near-simultaneously.
     *
     * Interleaved log output can still be meaningful if each log entry
     * from different contexts had a distinctive stamp. This is where NDCs
     * come into play.
     *
     * <em><b>Note that NDCs are managed on a per thread
     * basis</b></em>. NDC operations such as {@link #push}, {@link
     * #pop}, {@link #clear}, {@link #getDepth} and {@link #setMaxDepth}
     * affect the NDC of the <em>current</em> thread only. NDCs of other
     * threads remain unaffected.
     *
     * For example, a server can build a per client request NDC
     * consisting the clients host name and other information contained in
     * the the request. <em>Cookies</em> are another source of distinctive
     * information. To build an NDC one uses the {@link #push}
     * operation. Simply put,
     *
     * <ul>
     *   <li>Contexts can be nested.
     *
     *   <li>When entering a context, call <code>getNDC().push()</code>. As a
     *   side effect, if there is no nested diagnostic context for the
     *   current thread, this method will create it.
     *
     *   <li>When leaving a context, call <code>getNDC().pop()</code>.
     *
     *   <li><b>When exiting a thread make sure to call {@link #remove
     *   NDC.remove()}</b>.  
     * </ul>
     *                                          
     * There is no penalty for forgetting to match each
     * <code>push</code> operation with a corresponding <code>pop</code>,
     * except the obvious mismatch between the real application context
     * and the context set in the NDC.  Use of the {@link NDCContextCreator}
     * class can automate this process and make your code exception-safe.
     *
     * If configured to do so, {@link log4cplus::PatternLayout} and
     * {@link log4cplus::TTCCLayout} instances automatically retrieve
     * the nested diagnostic context for the current thread without
     * any user intervention.  Hence, even if a server is serving
     * multiple clients simultaneously, the logs emanating from the
     * same code (belonging to the same logger) can still be
     * distinguished because each client request will have a different
     * NDC tag.
     *
     * Heavy duty systems should call the {@link #remove} method when
     * leaving the run method of a thread. This ensures that the memory
     * used by the thread can be freed.
     *
     * A thread may inherit the nested diagnostic context of another
     * (possibly parent) thread using the {@link #inherit inherit}
     * method. A thread may obtain a copy of its NDC with the {@link
     * #cloneStack cloneStack} method and pass the reference to any other
     * thread, in particular to a child.
     */
    class LOG4CPLUS_EXPORT NDC
    {
    public:
        /**
         * Clear any nested diagnostic information if any. This method is
         * useful in cases where the same thread can be potentially used
         * over and over in different unrelated contexts.
         *
         * This method is equivalent to calling the {@link #setMaxDepth}
         * method with a zero <code>maxDepth</code> argument.
         */
        void clear();

        /**
         * Clone the diagnostic context for the current thread.
         *
         * Internally a diagnostic context is represented as a stack.  A
         * given thread can supply the stack (i.e. diagnostic context) to a
         * child thread so that the child can inherit the parent thread's
         * diagnostic context.
         *
         * The child thread uses the {@link #inherit inherit} method to
         * inherit the parent's diagnostic context.
         *                                        
         * @return Stack A clone of the current thread's  diagnostic context.
         */
        DiagnosticContextStack cloneStack() const;

        /**
         * Inherit the diagnostic context of another thread.
         *
         * The parent thread can obtain a reference to its diagnostic
         * context using the {@link #cloneStack} method.  It should
         * communicate this information to its child so that it may inherit
         * the parent's diagnostic context.
         *
         * The parent's diagnostic context is cloned before being
         * inherited. In other words, once inherited, the two diagnostic
         * contexts can be managed independently.
         *
         * @param stack The diagnostic context of the parent thread.
         */
        void inherit(const DiagnosticContextStack& stack);

        /**
         * Used when printing the diagnostic context.
         */
        log4cplus::tstring const & get() const;

        /**
         * Get the current nesting depth of this diagnostic context.
         *
         * @see #setMaxDepth
         */
        std::size_t getDepth() const;

        /**
         * Clients should call this method before leaving a diagnostic
         * context.
         *
         * The returned value is the value that was pushed last. If no
         * context is available, then the empty string "" is
         * returned. If each call to push() is paired with a call to
         * pop() (even in presence of thrown exceptions), the last
         * pop() call frees the memory used by NDC for this
         * thread. Otherwise, remove() must be called at the end of
         * the thread to free the memory used by NDC for the thread.
         *
         * @return String The innermost diagnostic context.
         *
         * @see NDCContextCreator, remove(), push()
         */
        log4cplus::tstring pop();

        /**
         * Same as pop() but without the return value.
         */
        void pop_void ();

        /**
         * Looks at the last diagnostic context at the top of this NDC
         * without removing it.
         *
         * The returned value is the value that was pushed last. If no
         * context is available, then the empty string "" is returned.
         *                          
         * @return String The innermost diagnostic context.
         */
        log4cplus::tstring const & peek() const;

        /**
         * Push new diagnostic context information for the current thread.
         *
         * The contents of the <code>message</code> parameter is
         * determined solely by the client. Each call to push() should
         * be paired with a call to pop().
         *
         * @param message The new diagnostic context information.
         *
         * @see NDCContextCreator, pop(), remove()
         */
        void push(const log4cplus::tstring& message);
        void push(tchar const * message);

        /**
         * Remove the diagnostic context for this thread.
         *
         * Each thread that created a diagnostic context by calling
         * push() should call this method before exiting. Otherwise,
         * the memory used by the thread cannot be reclaimed. It is
         * possible to omit this call if and only if each push() call
         * is always paired with a pop() call (even in presence of
         * thrown exceptions). Then the memory used by NDC will be
         * returned by the last pop() call and a call to remove() will
         * be no-op.
         */
        void remove();

        /**
         * Set maximum depth of this diagnostic context. If the current
         * depth is smaller or equal to <code>maxDepth</code>, then no
         * action is taken.
         *
         * This method is a convenient alternative to multiple {@link
         * #pop} calls. Moreover, it is often the case that at the end of
         * complex call sequences, the depth of the NDC is
         * unpredictable. The <code>setMaxDepth</code> method circumvents
         * this problem.
         *
         * For example, the combination
         * <pre>
         *    void foo() {
         *    &nbsp;  std::size_t depth = NDC.getDepth();
         *
         *    &nbsp;  ... complex sequence of calls
         *
         *    &nbsp;  NDC.setMaxDepth(depth);
         *    }
         * </pre>
         *
         * ensures that between the entry and exit of foo the depth of the
         * diagnostic stack is conserved.
         * 
         * <b>Note:</b>  Use of the {@link NDCContextCreator} class will solve
         * this particular problem.
         *
         * @see #getDepth
         */
        void setMaxDepth(std::size_t maxDepth);

        // Public ctor but only to be used by internal::DefaultContext.
        NDC();

      // Dtor
        virtual ~NDC();

    private:
      // Methods
        LOG4CPLUS_PRIVATE static DiagnosticContextStack* getPtr();

        template <typename StringType>
        LOG4CPLUS_PRIVATE
        void push_worker (StringType const &);

      // Disallow construction (and copying) except by getNDC()
        NDC(const NDC&);
        NDC& operator=(const NDC&);
    };


    /**
     * Return a reference to the singleton object.
     */
    LOG4CPLUS_EXPORT NDC& getNDC();


    /**
     * This is the internal object that is stored on the NDC stack.
     */
    struct LOG4CPLUS_EXPORT DiagnosticContext
    {
      // Ctors
        DiagnosticContext(const log4cplus::tstring& message,
            DiagnosticContext const * parent);
        DiagnosticContext(tchar const * message,
            DiagnosticContext const * parent);
        DiagnosticContext(const log4cplus::tstring& message);
        DiagnosticContext(tchar const * message);
        DiagnosticContext(DiagnosticContext const &);
        DiagnosticContext & operator = (DiagnosticContext const &);

#if defined (LOG4CPLUS_HAVE_RVALUE_REFS)
        DiagnosticContext(DiagnosticContext &&);
        DiagnosticContext & operator = (DiagnosticContext &&);
#endif

        void swap (DiagnosticContext &);

      // Data
        log4cplus::tstring message; /*!< The message at this context level. */
        log4cplus::tstring fullMessage; /*!< The entire message stack. */
    };


    /**
     * This class ensures that a {@link NDC#push} call is always matched with
     * a {@link NDC#pop} call even in the face of exceptions.
     */
    class LOG4CPLUS_EXPORT NDCContextCreator {
    public:
        /** Pushes <code>msg</code> onto the NDC stack. */
        NDCContextCreator(const log4cplus::tstring& msg);
        NDCContextCreator(tchar const * msg);

        /** Pops the NDC stack. */
        ~NDCContextCreator();
    };

} // end namespace log4cplus


#endif // _LO4CPLUS_NDC_HEADER_
