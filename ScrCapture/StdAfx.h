
#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <objbase.h>

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib.lib")
#   endif
#endif

#define  DUI_CTR_CANVAS                          (_T("Canvas"))
#define  DUI_CTR_CANVASCONTAINER                 (_T("CanvasContainer"))

#define MENUCMD_ADDRECTANGE		200
#define MENUCMD_ADDELLIPSE		201
#define MENUCMD_ADDARROW		202
#define MENUCMD_ADDBRUSH		203
#define MENUCMD_ADDTEXT			204
#define MENUCMD_UNDO			205
#define MENUCMD_RECHOICE		206
#define MENUCMD_SAVEANDQUIT		207
#define MENUCMD_SAVE			208
#define MENUCMD_HIDETOOLBAR		209
#define MENUCMD_QUIT			210

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
