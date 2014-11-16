// -*- C++ -*-
// Module:  Log4CPLUS
// File:    windowsh-inc.h
// Created: 4/2010
// Author:  Vaclav Zeman
//
//
//  Copyright (C) 2010-2013, Vaclav Zeman. All rights reserved.
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

// NOTE: This file is a fragment intentionally left without include guards.

#if defined (_WIN32)
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#undef NOGDICAPMASKS
#define NOGDICAPMASKS

#undef NOVIRTUALKEYCODES
#define NOVIRTUALKEYCODES

#undef NOWINMESSAGES
#define NOWINMESSAGES

#undef NOWINSTYLES
#define NOWINSTYLES

#undef NOSYSMETRICS
#define NOSYSMETRICS

#undef NOMENUS
#define NOMENUS

#undef NOICONS
#define NOICONS

#undef NOKEYSTATES
#define NOKEYSTATES

#undef NOSYSCOMMANDS
#define NOSYSCOMMANDS

#undef NORASTEROPS
#define NORASTEROPS

#undef NOSHOWWINDOW
#define NOSHOWWINDOW

#undef NOATOM
#define NOATOM

#undef NOCLIPBOARD
#define NOCLIPBOARD

#undef NOCOLOR
#define NOCOLOR

#undef NOCTLMGR
#define NOCTLMGR

#undef NODRAWTEXT
#define NODRAWTEXT

#undef NOGDI
#define NOGDI

#undef NOKERNEL
#define NOKERNEL

#undef NOUSER
#define NOUSER

#undef NONLS
#define NONLS

#undef NOMB
#define NOMB

#undef NOMEMMGR
#define NOMEMMGR

#undef NOMETAFILE
#define NOMETAFILE

#undef NOMINMAX
#define NOMINMAX

#undef NOMSG
#define NOMSG

#undef NOOPENFILE
#define NOOPENFILE

#undef NOSCROLL
#define NOSCROLL

#undef NOSERVICE
#define NOSERVICE

#undef NOSOUND
#define NOSOUND

#undef NOTEXTMETRIC
#define NOTEXTMETRIC

#undef NOWH
#define NOWH

#undef NOWINOFFSETS
#define NOWINOFFSETS

#undef NOCOMM
#define NOCOMM

#undef NOKANJI
#define NOKANJI

#undef NOHELP
#define NOHELP

#undef NOPROFILER
#define NOPROFILER

#undef NODEFERWINDOWPOS
#define NODEFERWINDOWPOS

#undef NOMCX
#define NOMCX

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#if defined (LOG4CPLUS_HAVE_INTRIN_H)
#include <intrin.h>
#endif
#endif

// NOTE: This file is a fragment intentionally left without include guards.
