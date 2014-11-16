// -*- C++ -*-
//
//  Copyright (C) 2012-2013, Vaclav Zeman. All rights reserved.
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
#include <log4cplus/helpers/fileinfo.h>

#ifdef LOG4CPLUS_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef LOG4CPLUS_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if defined (_WIN32)
#include <tchar.h>
#include <log4cplus/config/windowsh-inc.h>
#endif


namespace log4cplus { namespace helpers {


int
getFileInfo (FileInfo * fi, tstring const & name)
{
#if defined (_WIN32)
    struct _stat fileStatus;
    if (_tstat (name.c_str (), &fileStatus) == -1)
        return -1;
    
    fi->mtime = helpers::Time (fileStatus.st_mtime);
    fi->is_link = false;
    fi->size = fileStatus.st_size;
    
#else
    struct stat fileStatus;
    if (stat (LOG4CPLUS_TSTRING_TO_STRING (name).c_str (),
            &fileStatus) == -1)
        return -1;

    fi->mtime = helpers::Time (fileStatus.st_mtime);
    fi->is_link = S_ISLNK (fileStatus.st_mode);
    fi->size = fileStatus.st_size;
    
#endif
    
    return 0;
}

} } // namespace log4cplus { namespace helpers {
