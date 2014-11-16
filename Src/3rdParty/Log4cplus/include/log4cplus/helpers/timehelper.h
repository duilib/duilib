// -*- C++ -*-
// Module:  Log4CPLUS
// File:    timehelper.h
// Created: 6/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2013 Tad E. Smith
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

/** @file */

#ifndef LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_
#define LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_

#include <log4cplus/config.hxx>

#if defined (LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include <log4cplus/tstring.h>

#if defined (LOG4CPLUS_HAVE_TIME_H)
#include <time.h>
#endif

#include <ctime>


namespace log4cplus {

namespace helpers {


using std::time_t;
using std::tm;


/**
 * This class represents a Epoch time with microsecond accuracy.
 */
class LOG4CPLUS_EXPORT Time {
public:
    Time();
    Time(time_t tv_sec, long tv_usec);
    explicit Time(time_t time);

    /**
     * Returns the current time using the <code>gettimeofday()</code>
     * method if it is available on the current platform.  (Not on 
     * WIN32.)
     */
    static Time gettimeofday();

  // Methods
    /**
     * Returns <i>seconds</i> value.
     */
    time_t sec() const { return tv_sec; }

    /**
     * Returns <i>microseconds</i> value.
     */
    long usec() const { return tv_usec; }

    /**
     * Sets the <i>seconds</i> value.
     */
    void sec(time_t s) { tv_sec = s; }

    /**
     * Sets the <i>microseconds</i> value.
     */
    void usec(long us) { tv_usec = us; }

    /**
     * Sets this Time using the <code>mktime</code> function.
     */
    time_t setTime(tm* t);

    /**
     * Returns this Time as a <code>time_t</code> value.
     */
    time_t getTime() const LOG4CPLUS_ATTRIBUTE_PURE;

    /**
     * Populates <code>tm</code> using the <code>gmtime()</code>
     * function.
     */
    void gmtime(tm* t) const;

    /**
     * Populates <code>tm</code> using the <code>localtime()</code>
     * function.
     */
    void localtime(tm* t) const;

    /**
     * Returns a string with a "formatted time" specified by
     * <code>fmt</code>.  It used the <code>strftime()</code>
     * function to do this.  
     * 
     * Look at your platform's <code>strftime()</code> documentation
     * for the formatting options available.
     * 
     * The following additional options are provided:<br>
     * <code>%q</code> - 3 character field that provides milliseconds
     * <code>%Q</code> - 7 character field that provides fractional 
     * milliseconds.
     */
    log4cplus::tstring getFormattedTime(const log4cplus::tstring& fmt,
                                        bool use_gmtime = false) const;

  // Operators
    Time& operator+=(const Time& rhs);
    Time& operator-=(const Time& rhs);
    Time& operator/=(long rhs);
    Time& operator*=(long rhs);

private:
  // Data
    time_t tv_sec;  /* seconds */
    long tv_usec;  /* microseconds */
};


LOG4CPLUS_EXPORT const log4cplus::helpers::Time operator+
                                   (const log4cplus::helpers::Time& lhs,
                                    const log4cplus::helpers::Time& rhs);
LOG4CPLUS_EXPORT const log4cplus::helpers::Time operator-
                                   (const log4cplus::helpers::Time& lhs,
                                    const log4cplus::helpers::Time& rhs);
LOG4CPLUS_EXPORT const log4cplus::helpers::Time operator/
                                   (const log4cplus::helpers::Time& lhs,
                                    long rhs);
LOG4CPLUS_EXPORT const log4cplus::helpers::Time operator*
                                   (const log4cplus::helpers::Time& lhs,
                                    long rhs);

LOG4CPLUS_EXPORT bool operator<(const log4cplus::helpers::Time& lhs,
                                const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;
LOG4CPLUS_EXPORT bool operator<=(const log4cplus::helpers::Time& lhs,
                                 const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;

LOG4CPLUS_EXPORT bool operator>(const log4cplus::helpers::Time& lhs,
                                const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;
LOG4CPLUS_EXPORT bool operator>=(const log4cplus::helpers::Time& lhs,
                                 const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;

LOG4CPLUS_EXPORT bool operator==(const log4cplus::helpers::Time& lhs,
                                 const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;
LOG4CPLUS_EXPORT bool operator!=(const log4cplus::helpers::Time& lhs,
                                 const log4cplus::helpers::Time& rhs)
    LOG4CPLUS_ATTRIBUTE_PURE;

} // namespace helpers

} // namespace log4cplus


#endif // LOG4CPLUS_HELPERS_TIME_HELPER_HEADER_

