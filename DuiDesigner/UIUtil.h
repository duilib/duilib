//
//
// DirectUI - UI Library
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2006-2007 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. These
// source files may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
////
// Acknowledgements :
// Bjarke Viksoe (http://www.viksoe.dk/code/windowless1.htm)
//
//
//
// Beware of bugs.
//
//
//
////////////////////////////////////////////////////////
#ifndef __UIUTIL_H__
#define __UIUTIL_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace StringConvertor {

#pragma warning(disable: 4996)

LPSTR  WideToAnsi(LPCWSTR str, int len = -1);
LPSTR  WideToUtf8(LPCWSTR str, int len = -1);
LPWSTR WideStrDup(LPCWSTR str, int len = -1);

LPWSTR AnsiToWide(LPCSTR  str, int len = -1);
__declspec(deprecated) LPSTR  AnsiToUtf8(LPCSTR  str, int len = -1);
__declspec(deprecated) LPSTR  AnsiStrDup(LPCSTR  str, int len = -1);

LPWSTR Utf8ToWide(LPCSTR  str, int len = -1);
__declspec(deprecated) LPSTR  Utf8ToAnsi(LPCSTR  str, int len = -1);
static __inline LPSTR Utf8StrDup(LPCSTR str, int len = -1)    { return AnsiStrDup(str, len); }

//StringAlloc takes the number of characters, not bytes!
__declspec(deprecated) LPWSTR StringAllocW(size_t len);
__declspec(deprecated) LPSTR  StringAllocA(size_t len);
void   StringFree(LPWSTR str);
void   StringFree(LPSTR str);

#define StringFreeW StringFree
#define StringFreeA StringFree

#ifdef UNICODE
static __declspec(deprecated) __inline LPTSTR WideToTchar(LPCWSTR str, int len = -1) { return WideStrDup(str, len);  }
static __declspec(deprecated) __inline LPTSTR AnsiToTchar(LPCSTR  str, int len = -1) { return AnsiToWide(str, len);  }
static __declspec(deprecated) __inline LPTSTR Utf8ToTchar(LPCSTR  str, int len = -1) { return Utf8ToWide(str, len);  }
static __declspec(deprecated) __inline LPWSTR TcharToWide(LPCTSTR str, int len = -1) { return WideStrDup(str, len);  }
static __declspec(deprecated) __inline LPSTR  TcharToAnsi(LPCTSTR str, int len = -1) { return WideToAnsi(str, len);  }
static __declspec(deprecated) __inline LPSTR  TcharToUtf8(LPCTSTR str, int len = -1) { return WideToUtf8(str, len);  }
static __declspec(deprecated) __inline LPTSTR TcharStrDup(LPCTSTR str, int len = -1) { return WideStrDup(str, len);  }
static __declspec(deprecated) __inline LPTSTR StringAlloc(size_t  len)               { return StringAllocW(len); }
#else
static __declspec(deprecated) __inline LPTSTR WideToTchar(LPCWSTR str, int len = -1) { return WideToAnsi(str, len);  }
static __declspec(deprecated) __inline LPTSTR AnsiToTchar(LPCSTR  str, int len = -1) { return AnsiStrDup(str, len);  }
static __declspec(deprecated) __inline LPTSTR Utf8ToTchar(LPCSTR  str, int len = -1) { return Utf8ToAnsi(str, len);  }
static __declspec(deprecated) __inline LPWSTR TcharToWide(LPCTSTR str, int len = -1) { return AnsiToWide(str, len);  }
static __declspec(deprecated) __inline LPSTR  TcharToAnsi(LPCTSTR str, int len = -1) { return AnsiStrDup(str, len);  }
static __declspec(deprecated) __inline LPSTR  TcharToUtf8(LPCTSTR str, int len = -1) { return AnsiToUtf8(str, len);  }
static __declspec(deprecated) __inline LPTSTR TcharStrDup(LPCTSTR str, int len = -1) { return AnsiStrDup(str, len);  }
static __declspec(deprecated) __inline LPTSTR StringAlloc(size_t  len)               { return StringAllocA(len); }
#endif

#pragma warning(default: 4996)

} // namespace StringConvertor

#endif //__UIUTIL_H__
