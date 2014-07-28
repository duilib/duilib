//
// debug.hpp
// ~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <stdio.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifdef UNICODE
#define __TFILE__	__WFILE__
#else
#define __TFILE__	__FILE__
#endif

#define DBG_THREADSAFE

#define DBG_VERBOSE 1
#define DBG_TERSE   2
#define DBG_WARNING 3
#define DBG_ERROR   4

extern int		g_iDebugLevel;
extern bool		g_bSaveLogFile;
extern TCHAR	g_bLogSavePath[MAX_PATH];

BOOL DbgPrint(__in LPCTSTR lpszFormatString,...);

#define DBGMSG(level, prefix, msg) { \
	INT dbgLevel = level; \
	if (g_iDebugLevel <= (dbgLevel)) { \
	DbgPrint(_T("%s\t\tFILE:%s\t\tLINE:%d\t\t"), prefix, __TFILE__, __LINE__);\
	DbgPrint(msg); \
	} \
}

#define DBGPRINT(level, msg) { \
	INT dbgLevel = level; \
	if (g_iDebugLevel <= (dbgLevel)) { \
	DbgPrint(_T("FILE:%s\t\tLINE:%d\t\t"), __TFILE__, __LINE__);\
	DbgPrint(msg); \
	} \
}

#ifdef _DEBUG

#define VERBOSE(...) DBGPRINT(DBG_VERBOSE, ##__VA_ARGS__)
#define TERSE(...) DBGPRINT(DBG_TERSE, ##__VA_ARGS__)

#else // !DBG

#define VERBOSE(...)				__noop
#define TERSE(...)					__noop
//#define WARNING(...)				__noop
//#define ERR(...)					__noop
//#define DBGMSG(level, prefix, ...)	__noop
//#define DBGPRINT(level, ...)		__noop

#endif
#define WARNING(...) DBGMSG(DBG_WARNING,_T("WRN"), ##__VA_ARGS__)
#define ERR(...) DBGMSG(DBG_ERROR,_T("ERR"), ##__VA_ARGS__)

#endif // _DEBUG_HPP