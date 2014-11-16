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

#include <log4cplus/mdc.h>
#include <log4cplus/internal/internal.h>


namespace log4cplus
{


MDC::MDC ()
{ }


MDC::~MDC ()
{ }


MappedDiagnosticContextMap *
MDC::getPtr ()
{
    return &internal::get_ptd ()->mdc_map;
}


void
MDC::clear()
{
    MappedDiagnosticContextMap * const dc = getPtr ();
    MappedDiagnosticContextMap ().swap (*dc);
}


void
MDC::put (tstring const & key, tstring const & value)
{
    MappedDiagnosticContextMap * const dc = getPtr ();
    (*dc)[key] = value;
}


bool
MDC::get (tstring * value, tstring const & key) const
{
    assert (value);

    MappedDiagnosticContextMap * const dc = getPtr ();
    MappedDiagnosticContextMap::const_iterator it = dc->find (key);
    if (it != dc->end ())
    {
        *value = it->second;
        return true;
    }
    else
        return false;
}


void
MDC::remove (tstring const & key)
{
    MappedDiagnosticContextMap * const dc = getPtr ();
    dc->erase (key);
}


MappedDiagnosticContextMap const &
MDC::getContext () const
{
    return *getPtr ();
}


} // namespace log4cplus
