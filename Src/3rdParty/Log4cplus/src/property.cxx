// Module:  Log4CPLUS
// File:    property.cxx
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright 2002-2013 Tad E. Smith
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

#include <log4cplus/config.hxx>

#include <cstring>
#if defined (UNICODE)
#  include <cwctype>
#else
#  include <cctype>
#endif
#if defined (LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) \
    || defined (LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) \
    || defined (LOG4CPLUS_HAVE_CODECVT_UTF32_FACET)
#  include <codecvt>
#endif
#include <locale>
#include <fstream>
#include <sstream>
#include <log4cplus/streams.h>
#include <log4cplus/fstreams.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/internal/internal.h>
#include <log4cplus/internal/env.h>
#include <log4cplus/helpers/loglog.h>


namespace log4cplus { namespace helpers {


const tchar Properties::PROPERTIES_COMMENT_CHAR = LOG4CPLUS_TEXT('#');


namespace
{


static
int
is_space (tchar ch)
{
#if defined (UNICODE)
    return std::iswspace (ch);
#else
    return std::isspace (static_cast<unsigned char>(ch));
#endif
}


static
void
trim_leading_ws (tstring & str)
{
    tstring::iterator it = str.begin ();
    for (; it != str.end (); ++it)
    {
        if (! is_space (*it))
            break;
    }
    str.erase (str.begin (), it);
}


static
void
trim_trailing_ws (tstring & str)
{
    tstring::reverse_iterator rit = str.rbegin ();
    for (; rit != str.rend (); ++rit)
    {
        if (! is_space (*rit))
            break;
    }
    str.erase (rit.base (), str.end ());
}


static
void
trim_ws (tstring & str)
{
    trim_trailing_ws (str);
    trim_leading_ws (str);
}


void
imbue_file_from_flags (tistream & file, unsigned flags)
{
    switch (flags & (Properties::fEncodingMask << Properties::fEncodingShift))
    {
#if defined (LOG4CPLUS_HAVE_CODECVT_UTF8_FACET) && defined (UNICODE)
    case Properties::fUTF8:
        file.imbue (
            std::locale (file.getloc (),
                new std::codecvt_utf8<tchar, 0x10FFFF,
                    static_cast<std::codecvt_mode>(std::consume_header | std::little_endian)>));
        break;
#endif

#if defined (LOG4CPLUS_HAVE_CODECVT_UTF16_FACET) && defined (UNICODE)
    case Properties::fUTF16:
        file.imbue (
            std::locale (file.getloc (),
                new std::codecvt_utf16<tchar, 0x10FFFF,
                    static_cast<std::codecvt_mode>(std::consume_header | std::little_endian)>));
        break;

#elif defined (UNICODE) && defined (WIN32)
    case Properties::fUTF16:
        file.imbue (
            std::locale (file.getloc (),
                // TODO: This should actually be a custom "null" facet
                // that just copies the chars to wchar_t buffer.
                new std::codecvt<wchar_t, char, std::mbstate_t>));
    break;

#endif

#if defined (LOG4CPLUS_HAVE_CODECVT_UTF32_FACET) && defined (UNICODE)
    case Properties::fUTF32:
        file.imbue (
            std::locale (file.getloc (),
                new std::codecvt_utf32<tchar, 0x10FFFF,
                    static_cast<std::codecvt_mode>(std::consume_header | std::little_endian)>));
        break;
#endif

    case Properties::fUnspecEncoding:;
    default:
        // Do nothing.
        ;
    }
}


} // namespace



///////////////////////////////////////////////////////////////////////////////
// Properties ctors and dtor
///////////////////////////////////////////////////////////////////////////////

Properties::Properties()
    : flags (0)
{
}



Properties::Properties(tistream& input)
    : flags (0)
{
    init(input);
}



Properties::Properties(const tstring& inputFile, unsigned f)
    : flags (f)
{
    if (inputFile.empty ())
        return;

    tifstream file;
    imbue_file_from_flags (file, flags);

    file.open(LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(inputFile).c_str(),
        std::ios::binary);
    if (! file.good ())
        helpers::getLogLog ().error (LOG4CPLUS_TEXT ("could not open file ")
            + inputFile);

    init(file);
}



void 
Properties::init(tistream& input) 
{
    if (! input)
        return;

    tstring buffer;
    while (std::getline (input, buffer))
    {
        trim_leading_ws (buffer);

        tstring::size_type const buffLen = buffer.size ();
        if (buffLen == 0 || buffer[0] == PROPERTIES_COMMENT_CHAR)
            continue;
        
        // Check if we have a trailing \r because we are 
        // reading a properties file produced on Windows.
        if (buffer[buffLen-1] == LOG4CPLUS_TEXT('\r'))
            // Remove trailing 'Windows' \r.
            buffer.resize (buffLen - 1);

        tstring::size_type const idx = buffer.find('=');
        if (idx != tstring::npos)
        {
            tstring key = buffer.substr(0, idx);
            tstring value = buffer.substr(idx + 1);
            trim_trailing_ws (key);
            trim_ws (value);
            setProperty(key, value);
        }
        else if (buffer.compare (0, 7, LOG4CPLUS_TEXT ("include")) == 0
            && buffer.size () >= 7 + 1 + 1
            && is_space (buffer[7]))
        {
            tstring included (buffer, 8) ;
            trim_ws (included);

            tifstream file;
            imbue_file_from_flags (file, flags);

            file.open (LOG4CPLUS_FSTREAM_PREFERED_FILE_NAME(included).c_str(),
                std::ios::binary);
            if (! file.good ())
                helpers::getLogLog ().error (
                    LOG4CPLUS_TEXT ("could not open file ") + included);

            init (file);
        }
    }
}



Properties::~Properties() 
{
}



///////////////////////////////////////////////////////////////////////////////
// helpers::Properties public methods
///////////////////////////////////////////////////////////////////////////////


bool
Properties::exists(const log4cplus::tstring& key) const
{
    return data.find(key) != data.end();
}


bool
Properties::exists(tchar const * key) const
{
    return data.find(key) != data.end();
}


tstring const &
Properties::getProperty(const tstring& key) const 
{
    return get_property_worker (key);
}


log4cplus::tstring const &
Properties::getProperty(tchar const * key) const
{
    return get_property_worker (key);
}


tstring
Properties::getProperty(const tstring& key, const tstring& defaultVal) const
{
    StringMap::const_iterator it (data.find (key));
    if (it == data.end ())
        return defaultVal;
    else
        return it->second;
}


std::vector<tstring>
Properties::propertyNames() const 
{
    std::vector<tstring> tmp;
    for (StringMap::const_iterator it=data.begin(); it!=data.end(); ++it)
        tmp.push_back(it->first);

    return tmp;
}



void
Properties::setProperty(const log4cplus::tstring& key,
    const log4cplus::tstring& value)
{
    data[key] = value;
}


bool
Properties::removeProperty(const log4cplus::tstring& key)
{
    return data.erase(key) > 0;
}


Properties 
Properties::getPropertySubset(const log4cplus::tstring& prefix) const
{
    Properties ret;
    std::size_t const prefix_len = prefix.size ();
    std::vector<tstring> keys = propertyNames();
    for (std::vector<tstring>::iterator it=keys.begin(); it!=keys.end(); ++it)
    {
        int result = it->compare (0, prefix_len, prefix);
        if (result == 0)
            ret.setProperty (it->substr (prefix_len), getProperty(*it));
    }

    return ret;
}


bool
Properties::getInt (int & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getUInt (unsigned & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getLong (long & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getULong (unsigned long & val, log4cplus::tstring const & key) const
{
    return get_type_val_worker (val, key);
}


bool
Properties::getBool (bool & val, log4cplus::tstring const & key) const
{
    if (! exists (key))
        return false;

    log4cplus::tstring const & prop_val = getProperty (key);
    return internal::parse_bool (val, prop_val);
}


template <typename StringType>
log4cplus::tstring const &
Properties::get_property_worker (StringType const & key) const
{
    StringMap::const_iterator it (data.find (key));
    if (it == data.end ())
        return log4cplus::internal::empty_str;
    else
        return it->second;
}


template <typename ValType>
bool
Properties::get_type_val_worker (ValType & val, log4cplus::tstring const & key)
    const
{
    if (! exists (key))
        return false;

    log4cplus::tstring const & prop_val = getProperty (key);
    log4cplus::tistringstream iss (prop_val);
    ValType tmp_val;
    tchar ch;

    iss >> tmp_val;
    if (! iss)
        return false;
    iss >> ch;
    if (iss)
        return false;

    val = tmp_val;
    return true;
}


} } // namespace log4cplus { namespace helpers {
