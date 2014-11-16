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

#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/streams.h>

#include <locale>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <cctype>

#include <cassert>
#include <vector>


namespace log4cplus
{

namespace helpers
{

void clear_mbstate (std::mbstate_t &);

#ifdef LOG4CPLUS_WORKING_LOCALE

static
void
towstring_internal (std::wstring & outstr, const char * src, std::size_t size,
    std::locale const & loc)
{
    if (size == 0)
    {
        outstr.clear ();
        return;
    }

    typedef std::codecvt<wchar_t, char, std::mbstate_t> CodeCvt;
    const CodeCvt & cdcvt = std::use_facet<CodeCvt>(loc);
    std::mbstate_t state;
    clear_mbstate (state);

    char const * from_first = src;
    std::size_t const from_size = size;
    char const * const from_last = from_first + from_size;
    char const * from_next = from_first;

    std::vector<wchar_t> dest (from_size);

    wchar_t * to_first = &dest.front ();
    std::size_t to_size = dest.size ();
    wchar_t * to_last = to_first + to_size;
    wchar_t * to_next = to_first;

    CodeCvt::result result;
    std::size_t converted = 0;
    while (true)
    {
        result = cdcvt.in (
            state, from_first, from_last,
            from_next, to_first, to_last,
            to_next);
        // XXX: Even if only half of the input has been converted the
        // in() method returns CodeCvt::ok. I think it should return
        // CodeCvt::partial.
        if ((result == CodeCvt::partial || result == CodeCvt::ok)
            && from_next != from_last)
        {
            to_size = dest.size () * 2;
            dest.resize (to_size);
            converted = to_next - to_first;
            to_first = &dest.front ();
            to_last = to_first + to_size;
            to_next = to_first + converted;
            continue;
        }
        else if (result == CodeCvt::ok && from_next == from_last)
            break;
        else if (result == CodeCvt::error
            && to_next != to_last && from_next != from_last)
        {
            clear_mbstate (state);
            ++from_next;
            from_first = from_next;
            *to_next = L'?';
            ++to_next;
            to_first = to_next;
        }
        else
            break;
    }
    converted = to_next - &dest[0];

    outstr.assign (dest.begin (), dest.begin () + converted);
}


std::wstring 
towstring(const std::string& src)
{
    std::wstring ret;
    towstring_internal (ret, src.c_str (), src.size (), std::locale ());
    return ret;
}


std::wstring 
towstring(char const * src)
{
    std::wstring ret;
    towstring_internal (ret, src, std::strlen (src), std::locale ());
    return ret;
}


static
void
tostring_internal (std::string & outstr, const wchar_t * src, std::size_t size,
    std::locale const & loc)
{
    if (size == 0)
    {
        outstr.clear ();
        return;
    }

    typedef std::codecvt<wchar_t, char, std::mbstate_t> CodeCvt;
    const CodeCvt & cdcvt = std::use_facet<CodeCvt>(loc);
    std::mbstate_t state;
    clear_mbstate (state);

    wchar_t const * from_first = src;
    std::size_t const from_size = size;
    wchar_t const * const from_last = from_first + from_size;
    wchar_t const * from_next = from_first;

    std::vector<char> dest (from_size);

    char * to_first = &dest.front ();
    std::size_t to_size = dest.size ();
    char * to_last = to_first + to_size;
    char * to_next = to_first;

    CodeCvt::result result;
    std::size_t converted = 0;
    while (from_next != from_last)
    {
        result = cdcvt.out (
            state, from_first, from_last,
            from_next, to_first, to_last,
            to_next);
        // XXX: Even if only half of the input has been converted the
        // in() method returns CodeCvt::ok with VC8. I think it should
        // return CodeCvt::partial.
        if ((result == CodeCvt::partial || result == CodeCvt::ok)
            && from_next != from_last)
        {
            to_size = dest.size () * 2;
            dest.resize (to_size);
            converted = to_next - to_first;
            to_first = &dest.front ();
            to_last = to_first + to_size;
            to_next = to_first + converted;
        }
        else if (result == CodeCvt::ok && from_next == from_last)
            break;
        else if (result == CodeCvt::error
            && to_next != to_last && from_next != from_last)
        {
            clear_mbstate (state);
            ++from_next;
            from_first = from_next;
            *to_next = '?';
            ++to_next;
            to_first = to_next;
        }
        else
            break;
    }
    converted = to_next - &dest[0];

    outstr.assign (dest.begin (), dest.begin () + converted);
}


std::string 
tostring(const std::wstring& src)
{
    std::string ret;
    tostring_internal (ret, src.c_str (), src.size (), std::locale ());
    return ret;
}


std::string 
tostring(wchar_t const * src)
{
    std::string ret;
    tostring_internal (ret, src, std::wcslen (src), std::locale ());
    return ret;
}


#endif // LOG4CPLUS_WORKING_LOCALE

} // namespace helpers

} // namespace log4cplus
