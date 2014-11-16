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
#include <log4cplus/helpers/loglog.h>

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cassert>
#include <vector>


namespace log4cplus
{

namespace helpers
{


void clear_mbstate (std::mbstate_t & mbs);


#if defined (LOG4CPLUS_WORKING_C_LOCALE)

static
void
tostring_internal (std::string & result, wchar_t const * src, std::size_t size)
{
    std::vector<char> result_buf (MB_CUR_MAX);

    wchar_t const * src_it = src;
    wchar_t const * const src_end_it = src + size;

    std::mbstate_t mbs;
    clear_mbstate (mbs);

    result.clear ();
    result.reserve (size + size / 3 + 1);

    while (src_it != src_end_it)
    {
        std::size_t ret = std::wcrtomb (&result_buf[0], *src_it, &mbs);
        if (ret == static_cast<std::size_t>(-1))
        {
            result.push_back ('?');
            clear_mbstate (mbs);
            ++src_it;
        }
        else
        {
            result.append (result_buf.begin (), result_buf.begin () + ret);
            ++src_it;
        }
    }
}


std::string 
tostring (const std::wstring & src)
{
    std::string ret;
    tostring_internal (ret, src.c_str (), src.size ());
    return ret;
}


std::string 
tostring (wchar_t const * src)
{
    assert (src);
    std::string ret;
    tostring_internal (ret, src, std::wcslen (src));
    return ret;
}


static
void
towstring_internal (std::wstring & result, char const * src, std::size_t size)
{
    char const * src_it = src;
    char const * const src_end_it = src + size;

    std::mbstate_t mbs;
    clear_mbstate (mbs);

    result.clear ();
    result.reserve (size);

    while (src_it != src_end_it)
    {
        std::size_t const n = size - (src - src_it);
        wchar_t result_char;
        std::size_t ret = std::mbrtowc (&result_char, src_it, n, &mbs);
        if (ret > 0)
        {
            result.push_back (result_char);
            src_it += ret;
        }
        else
        {
            result.push_back (ret == 0 ? L'\0' : L'?') ;
            clear_mbstate (mbs);
            ++src_it;
        }
    }
}


std::wstring 
towstring (const std::string& src)
{
    std::wstring ret;
    towstring_internal (ret, src.c_str (), src.size ());
    return ret;
}


std::wstring 
towstring (char const * src)
{
    assert (src);
    std::wstring ret;
    towstring_internal (ret, src, std::strlen (src));
    return ret;
}

#endif // LOG4CPLUS_WORKING_C_LOCALE

} // namespace helpers

} // namespace log4cplus
