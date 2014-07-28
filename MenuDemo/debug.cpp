//
// debug.cpp
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

#include "stdafx.h"
#include "debug.hpp"
#include <winbase.h>

#if defined(DBG_THREADSAFE)

class OS_CLockable 
{
public:
	// ## Constructor / Destructor
	OS_CLockable();
	~OS_CLockable();

	// ....................................................................
	// METHOD NAME:	Acquire
	// DESCRIPTION:
	//		Acquires a lock, blocking.
	// PARAMETERS:
	//		none
	// RETURN VALUE:
	//		none
	// ....................................................................
	void Acquire();

	// ....................................................................
	// METHOD NAME:	Release
	// DESCRIPTION:
	//		Releases the lock.
	//		object.
	// PARAMETERS:
	//		none
	// RETURN VALUE:
	//		none
	// ....................................................................
	void Release();

private:
	DWORD dwVersion;
	DWORD dwWindowsMajorVersion;
	DWORD dwWindowsMinorVersion;
	// ## Instance Variables

	CRITICAL_SECTION m_critSec;

	// ## declarations with no implementations (prevent accidental use)

	OS_CLockable (const OS_CLockable&);
	OS_CLockable& operator= (const OS_CLockable&);
}; 

// ............................................................................
// CLASS NAME: OS_CAutoLock
//
// RESPONSIBILITIES:
//		Provides automatic locking and unlocking of an OS_CLockable object
//		within a code scope.
//
// NOTES:
// ............................................................................
class OS_CAutoLock
{
public:

	// ## Constructors
	OS_CAutoLock(OS_CLockable &lockable) : m_lockable(lockable)
	{lockable.Acquire(); };

	// ## Destructor
	~OS_CAutoLock() { m_lockable.Release();};

private:

	// The controlled lockable object
	OS_CLockable &m_lockable;

	// ## declarations with no implementations (prevent accidental use)

	OS_CAutoLock ();
	OS_CAutoLock (const OS_CAutoLock&);
	OS_CAutoLock& operator= (const OS_CAutoLock&);
};

// ----------------------------------------------------------------------------
//	              OS_CLockable METHOD DEFINITIONS
// ----------------------------------------------------------------------------

// ...........................................................................
//
// NAME:	OS_CLockable::OS_CLockable		(Constructor)
//
// IMPLEMENTATION NOTES:
// ...........................................................................
OS_CLockable::OS_CLockable()
{
	OSVERSIONINFO osvi = {0};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	dwWindowsMajorVersion =  osvi.dwMajorVersion;
	dwWindowsMinorVersion =  osvi.dwMinorVersion;
	if(dwWindowsMajorVersion < 6)
		::InitializeCriticalSection(&m_critSec);

	return;
}

// ...........................................................................
//
// NAME:	OS_CLockable::~OS_CLockable		(Destructor)
//
// IMPLEMENTATION NOTES:
// ...........................................................................
OS_CLockable::~OS_CLockable()
{
	if(dwWindowsMajorVersion < 6)
		::DeleteCriticalSection(&m_critSec);

	return;
}

// ...........................................................................
//
// NAME:	OS_CLockable::Acquire
//
// IMPLEMENTATION NOTES:
// ...........................................................................
void OS_CLockable::Acquire()
{
	if(dwWindowsMajorVersion < 6)
		::EnterCriticalSection(&m_critSec);
}

// ...........................................................................
//
// NAME:	OS_CLockable::Release
//
// IMPLEMENTATION NOTES:
// ...........................................................................
void OS_CLockable::Release()
{
	if(dwWindowsMajorVersion < 6)
		::LeaveCriticalSection(&m_critSec);
}
#endif

#define MAX_LOG_FILE_SIZE	static_cast<int>(1024*1024/2)

#ifdef _DEBUG
int		g_iDebugLevel	=	DBG_VERBOSE;
#else
int		g_iDebugLevel	=	DBG_ERROR;
#endif

bool	g_bSaveLogFile	=	false;

TCHAR	g_bLogSavePath[MAX_PATH] = _T("C:\\log");

static	TCHAR			s_szLogFile[MAX_PATH] = {0};
static	bool			s_bLogPathInit = false;

#if defined(DBG_THREADSAFE)
static	OS_CLockable	s_mtxEntry;
#endif

BOOL DbgPrint(__in LPCTSTR lpszFormatString, ...)
{
	BOOL    bResult = TRUE;

	va_list VAList;
	va_start(VAList, lpszFormatString);

	if (g_bSaveLogFile)
	{
#if defined(DBG_THREADSAFE)
		OS_CAutoLock lock(s_mtxEntry);
#endif
		if (!s_bLogPathInit)
		{
			SYSTEMTIME stime = {0};			
			GetLocalTime(&stime);
#if defined(UNDER_CE)
			_stprintf(s_szLogFile, _T("%s\\log_%04d%02d%02d_%d.log"), g_bLogSavePath, stime.wYear, stime.wMonth, stime.wDay, GetTickCount());
#else
			_stprintf_s(s_szLogFile, MAX_PATH, _T("%s\\log_%04d%02d%02d_%d.log"), g_bLogSavePath, stime.wYear, stime.wMonth, stime.wDay, GetTickCount());
#endif
			s_bLogPathInit = true;
		}

		FILE* pFile = _tfopen(s_szLogFile, _T("a"));
		if (pFile != NULL)
		{
			fseek(pFile,SEEK_END,0);
			long cbSize = ftell(pFile);
			if (cbSize > MAX_LOG_FILE_SIZE)
			{
				fclose(pFile);
				{
					SYSTEMTIME stime = {0};			
					GetLocalTime(&stime);
#if defined(UNDER_CE)
					_stprintf(s_szLogFile, _T("%s\\log_%04d%02d%02d_%d.log"), g_bLogSavePath, stime.wYear, stime.wMonth, stime.wDay, GetTickCount());
#else
					_stprintf_s(s_szLogFile, MAX_PATH, _T("%s\\log_%04d%02d%02d_%d.log"), g_bLogSavePath, stime.wYear, stime.wMonth, stime.wDay, GetTickCount());
#endif
					s_bLogPathInit = true;
				}
				pFile = _tfopen(s_szLogFile, _T("a"));
				if (pFile == NULL)
				{
					return FALSE;
				}
			}
			_vftprintf(pFile, lpszFormatString, VAList);
			fflush(pFile);
			fclose(pFile);
			pFile = NULL;
		}
	}
	else
	{
#if defined(UNDER_CE)
		_vtprintf(lpszFormatString, VAList);
#else
		TCHAR szBuf[MAX_PATH * 2] = {0};
		_vstprintf_s(szBuf, MAX_PATH, lpszFormatString, VAList);
		OutputDebugString(szBuf);
#endif
	}
	va_end(VAList);

	return bResult;
}