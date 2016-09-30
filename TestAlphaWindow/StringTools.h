#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#pragma warning(disable:4190)
using namespace std;

std::string  UnicodeToAnsi(const std::wstring& strSource);
std::wstring AnsiToUnicode(const std::string& strSource);
std::wstring Utf8ToUnicode(const std::string& strSrouce);
std::string UnicodeToUtf8(const std::wstring& strSource);
std::string AnsiToUtf8(const std::string& strSource);
std::string Utf8ToAnsi(const std::string& strSource);
std::string URLEncodeGB2312(const char* szSrc/*, char* pBuf, int cbBufLen*/);
char* Utf8ToGBK(const char* strSource);
char* GBKToUtf8(const char* strSource);
bool UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase);
bool UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);
std::string EscapeToAnsi(const std::string& strSource);

#ifndef _W
#define _W(x) AnsiToUnicode(x).data()
#endif

#ifndef _A
#define _A(x) UnicodeToAnsi(x).data()
#endif

inline bool IsGB2312(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0xB0 && *pszIn<=0xF7 && *(pszIn+1)>=0xA0 && *(pszIn+1)<=0xFE) 
		return true; 
	else 
		return false; 
}

inline bool IsGBK(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0x81 && *pszIn<=0xFE && *(pszIn+1)>=0x40 && *(pszIn+1)<=0xFE) 
		return true; 
	else 
		return false; 
}

inline bool IsBIG5(const unsigned char *pszIn) 
{ 
	if (*pszIn>=0x81 && *pszIn<=0xFE && ((*(pszIn+1)>=0x40 && *(pszIn+1)<=0x7E) || (*(pszIn+1)>=0xA1 && *(pszIn+1)<=0xFE)))
		return true; 
	else 
		return false; 
}

inline bool IsValidEnglishChar(const unsigned char *pszIn) 
{
	if((*pszIn>='0' && *pszIn <= '9') || (*pszIn>='a' && *pszIn <= 'z') || (*pszIn>='A' && *pszIn <= 'Z'))
	{
		return true;
	}
	else
		return false;
}

inline bool IsValidName(const unsigned char *pszIn, int nLen) 
{
	for (int i=0; i < nLen; )
	{
		if(IsValidEnglishChar(pszIn+i))
		{
			i++;
			continue;
		}
		else if(((i+1) < nLen) && IsGBK(pszIn+i))
		{
			i += 2;
			continue;
		}
		else
			return false;
	}
	return true;
}
inline void CutChineseChar(char *pszSrc,unsigned int nNeedLen)
{
	if (strlen(pszSrc) >= nNeedLen)
	{
		//如果后两位不是英文字符，也不是一个有效的汉字，则后三位是一个汉字多最后一个字节
		if(!IsValidEnglishChar((const unsigned char *)(pszSrc+nNeedLen-2)) && !IsGBK((const unsigned char *)(pszSrc+nNeedLen-2)))
		{
			*(pszSrc+nNeedLen-3) = '.';
		}
		*(pszSrc+nNeedLen-2) = '.';
		*(pszSrc+nNeedLen-1) = '.';
		*(pszSrc+nNeedLen) = '\0';
	}
}
