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

#include <log4cplus/internal/env.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/streams.h>

#ifdef LOG4CPLUS_HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef LOG4CPLUS_HAVE_WCHAR_H
#include <wchar.h>
#endif


#include <cassert>
#include <cstdlib>
#include <sstream>


namespace log4cplus { namespace internal {


bool
get_env_var (tstring & value, tstring const & name)
{
#if defined (_WIN32) && defined (UNICODE)
    tchar const * val = _wgetenv (name.c_str ());
    if (val)
        value = val;

    return !! val;

#else
    char const * val
        = std::getenv (LOG4CPLUS_TSTRING_TO_STRING (name).c_str ());
    if (val)
        value = LOG4CPLUS_STRING_TO_TSTRING (val);

    return !! val;

#endif
}


bool
parse_bool (bool & val, tstring const & str)
{
    log4cplus::tistringstream iss (str);
    log4cplus::tstring word;
    if (! (iss >> word))
        return false;
    tchar ch;
    if (iss >> ch)
        return false;
    word = helpers::toLower (word);

    bool result = true;
    if (word == LOG4CPLUS_TEXT ("true"))
        val = true;
    else if (word == LOG4CPLUS_TEXT ("false"))
        val = false;
    else
    {
        iss.clear ();
        iss.seekg (0);
        assert (iss);

        long lval;
        iss >> lval;
        result = !! iss && ! (iss >> ch);
        if (result)
            val = !! lval;
    }

    return result;
}


} } // namespace log4cplus { namespace internal {
