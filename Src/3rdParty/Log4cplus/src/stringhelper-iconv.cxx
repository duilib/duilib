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

#if defined (LOG4CPLUS_WITH_ICONV)

#ifdef LOG4CPLUS_HAVE_ICONV_H
#include <iconv.h>
#endif

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cerrno>
#include <cstring>


// This is here because some compilers (Sun CC) think that there is a
// difference if the typedefs are not in an extern "C" block.
extern "C"
{

    //! SUSv3 iconv() type.
    typedef size_t (& iconv_func_type_1) (iconv_t cd, char * * inbuf,
	size_t * inbytesleft, char * * outbuf, size_t * outbytesleft); 


    //! GNU iconv() type.
    typedef size_t (& iconv_func_type_2) (iconv_t cd, const char * * inbuf,
	size_t * inbytesleft, char * * outbuf, size_t * outbytesleft);

} // extern "C"


namespace log4cplus
{

namespace helpers
{

namespace
{


static iconv_t const iconv_error_handle = reinterpret_cast<iconv_t>(-1);


struct iconv_handle
{
    iconv_handle (char const * to, char const * from)
        : handle (iconv_open (to, from))
    {
        if (handle == iconv_error_handle)
        {
            std::ostringstream oss;
            oss << "iconv_open(" << to << ", " << from << ") failed: " << errno;
            std::cerr << oss.str () << std::endl;
            throw std::runtime_error (oss.str ().c_str ());
        }
    }

    ~iconv_handle ()
    {
        try
        {
            if (handle != iconv_error_handle)
            {
                int ret = iconv_close (handle);
                if (ret == -1)
                {
                    std::ostringstream oss;
                    oss << "iconv_close failed: " << errno;
                    std::cerr << oss.str () << std::endl;
                    throw std::runtime_error (oss.str ().c_str ());
                }
            }
        }
        catch (std::runtime_error const &)
        { }
    }

    size_t
    call_iconv (iconv_func_type_1 iconv_func, char * * inbuf,
        size_t * inbytesleft, char * * outbuf, size_t * outbytesleft)
    {
        return iconv_func (handle, inbuf, inbytesleft, outbuf, outbytesleft);
    }

    size_t
    call_iconv (iconv_func_type_2 iconv_func, char * * inbuf,
        size_t * inbytesleft, char * * outbuf, size_t * outbytesleft)
    {
        return iconv_func (handle, const_cast<const char * *>(inbuf),
            inbytesleft, outbuf, outbytesleft);
    }

    size_t
    do_iconv (char * * inbuf, size_t * inbytesleft, char * * outbuf,
        size_t * outbytesleft)
    {
        return call_iconv (iconv, inbuf, inbytesleft, outbuf, outbytesleft);
    }

    iconv_t handle;
};


template <typename T>
struct question_mark;


template <>
struct question_mark<char>
{
    static char const value = '?';
};


template <>
struct question_mark<wchar_t>
{
    static wchar_t const value = L'?';
};


char const question_mark<char>::value;


wchar_t const question_mark<wchar_t>::value;


template <typename DestType, typename SrcType>
static
void
iconv_conv (std::basic_string<DestType> & result, char const * destenc,
    SrcType const * src, std::size_t size, char const * srcenc)
{
    iconv_handle cvt (destenc, srcenc);
    if (cvt.handle == iconv_error_handle)
    {
        // TODO: Better error handling.
        result.resize (0);
        return;
    }

    typedef DestType outbuf_type;
    typedef SrcType inbuf_type;

    std::size_t result_size = size + size / 3 + 1;
    result.resize (result_size);

    char * inbuf = const_cast<char *>(reinterpret_cast<char const *>(src));
    std::size_t inbytesleft = size * sizeof (inbuf_type);

    char * outbuf = reinterpret_cast<char *>(&result[0]);
    std::size_t outbytesleft = result_size * sizeof (outbuf_type);

    std::size_t res;
    std::size_t const error_retval = static_cast<std::size_t>(-1);

    while (inbytesleft != 0)
    {
        res = cvt.do_iconv (&inbuf, &inbytesleft, &outbuf, &outbytesleft);
        if (res == error_retval)
        {
            switch (errno)
            {
            case EILSEQ:
            case EINVAL:
                if (outbytesleft >= sizeof (outbuf_type))
                {
                    if (inbytesleft > 0)
                    {
                        ++inbuf;
                        inbytesleft -= sizeof (inbuf_type);
                    }

                    *outbuf = question_mark<outbuf_type>::value;
                    ++outbuf;
                    outbytesleft -= sizeof (outbuf_type);
                    
                    continue;
                }

                // Fall through.
                
            case E2BIG:;
                // Fall through.
            }

            std::size_t const outbuf_index = result_size;
            result_size *= 2;
            result.resize (result_size);
            outbuf = reinterpret_cast<char *>(&result[0] + outbuf_index);
            outbytesleft = (result_size - outbuf_index) * sizeof (outbuf_type);
        }
        else
            result.resize (result_size - outbytesleft / sizeof (outbuf_type));
    }
}


} // namespace


std::string
tostring (const std::wstring & src)
{
    std::string ret;
    iconv_conv (ret, "UTF-8", src.c_str (), src.size (), "WCHAR_T");
    return ret;
}


std::string
tostring (wchar_t const * src)
{
    assert (src);
    std::string ret;
    iconv_conv (ret, "UTF-8", src, std::wcslen (src), "WCHAR_T");
    return ret;
}


std::wstring
towstring (const std::string& src)
{
    std::wstring ret;
    iconv_conv (ret, "WCHAR_T", src.c_str (), src.size (), "UTF-8");
    return ret;
}


std::wstring
towstring (char const * src)
{
    assert (src);
    std::wstring ret;
    iconv_conv (ret, "WCHAR_T", src, std::strlen (src), "UTF-8");
    return ret;
}


} // namespace helpers

} // namespace log4cplus

#endif // LOG4CPLUS_WITH_ICONV
