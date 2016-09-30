#include "StringTools.h"

std::string UnicodeToAnsi(const std::wstring& strSource)
{
    int nLength = ::WideCharToMultiByte(CP_ACP, 0, strSource.data(), -1, NULL, 0, NULL, FALSE);

    if (nLength < 1)
    {
        return "";
    }

    std::vector<char> vecResult(nLength);
    ::WideCharToMultiByte(CP_ACP, 0, strSource.data(), -1, &vecResult[0], nLength, NULL, FALSE);

    return std::string(vecResult.begin(), vecResult.end()-1);
}


std::wstring AnsiToUnicode(const std::string& strSource)
{
    int nLength = ::MultiByteToWideChar(CP_ACP, 0, strSource.data(), -1, NULL, 0);

    if (nLength < 1)
    {
        return L"";
    }

    std::wstring strResult(nLength, 0);
    ::MultiByteToWideChar(CP_ACP, 0, strSource.data(), strSource.size(), &strResult[0], nLength);

    return std::wstring(strResult.data(), nLength - 1);
}


std::wstring Utf8ToUnicode(const std::string& strSrouce)
{
    int nLength = ::MultiByteToWideChar(CP_UTF8, 0, strSrouce.data(), -1, NULL, 0);

    if (nLength < 1)
    {
        return L"";
    }

    std::wstring strResult(nLength, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, strSrouce.data(), strSrouce.size(), &strResult[0], nLength);

    return std::wstring(strResult.data(), nLength - 1);
}

std::string UnicodeToUtf8(const std::wstring& strSource)
{
    int nLength = ::WideCharToMultiByte(CP_UTF8, 0, strSource.data(), -1, NULL, 0, NULL, FALSE);

    if (nLength < 1)
    {
        return "";
    }

    std::string strResult(nLength, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, strSource.data(), -1, &strResult[0], nLength, NULL, FALSE);

    return std::string(strResult.data(), nLength - 1);
}

std::string AnsiToUtf8(const std::string& strSource)
{
    std::wstring temp = AnsiToUnicode(strSource);
    return UnicodeToUtf8(temp);
}

std::string Utf8ToAnsi(const std::string& strSource)
{
    std::wstring strTmp = Utf8ToUnicode(strSource);
    return UnicodeToAnsi(strTmp);
}

//UrlEncode
bool UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase)
{
    if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
        return false;

    size_t len_ascii = strlen(szSrc);
    if(len_ascii == 0)
    {
        pBuf[0] = 0;
		return true;
    }

    //先转换到UTF-8
    char baseChar = bUpperCase ? 'A' : 'a';
    int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
    LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
    if(pUnicode == NULL)
        return false;
    MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

    int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
    LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
    if(pUTF8 == NULL)
    {
        free(pUnicode);
        return false;
    }
    WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
    pUTF8[cbUTF8] = '\0';

    unsigned char c;
    int cbDest = 0; //累加
    unsigned char *pSrc = (unsigned char*)pUTF8;
    unsigned char *pDest = (unsigned char*)pBuf;
    while(*pSrc && cbDest < cbBufLen - 1)
    {
        c = *pSrc;
        if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
        {
            *pDest = c;
            ++pDest;
            ++cbDest;
        }
        else if(c == ' ')
        {
            *pDest = '+';
            ++pDest;
            ++cbDest;
        }
        else
        {
            //检查缓冲区大小是否够用？
            if(cbDest + 3 > cbBufLen - 1)
                break;
            pDest[0] = '%';
            pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
            pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
            pDest += 3;
            cbDest += 3;
        }
        ++pSrc;
    }
    //null-terminator
    *pDest = '\0';
    free(pUnicode);
    free(pUTF8);
    return true;
}

//解码后是utf-8编码
bool UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
    if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
        return false;

    size_t len_ascii = strlen(szSrc);
    if(len_ascii == 0)
    {
        pBuf[0] = 0;
        return true;
    }

    char *pUTF8 = (char*)malloc(len_ascii + 1);
    if(pUTF8 == NULL)
        return false;

    int cbDest = 0; //累加
    unsigned char *pSrc = (unsigned char*)szSrc;
    unsigned char *pDest = (unsigned char*)pUTF8;
    while(*pSrc)
    {
        if(*pSrc == '%')
        {
            *pDest = 0;
            //高位
            if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
                *pDest += (pSrc[1] - 'A' + 10) * 0x10;
            else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
                *pDest += (pSrc[1] - 'a' + 10) * 0x10;
            else
                *pDest += (pSrc[1] - '0') * 0x10;

            //低位
            if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
                *pDest += (pSrc[2] - 'A' + 10);
            else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
                *pDest += (pSrc[2] - 'a' + 10);
            else
                *pDest += (pSrc[2] - '0');

            pSrc += 3;
        }
        else if(*pSrc == '+')
        {
            *pDest = ' ';
            ++pSrc;
        }
        else
        {
            *pDest = *pSrc;
            ++pSrc;
        }
        ++pDest;
        ++cbDest;
    }
    //null-terminator
    *pDest = '\0';
    ++cbDest;

    int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
    LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
    if(pUnicode == NULL)
    {
        free(pUTF8);
        return false;
    }
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
    WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
    free(pUTF8);
    free(pUnicode);
    return true;
}

typedef unsigned char BYTE;

inline BYTE toHex(const BYTE &x)
{

    return x > 9 ? x + 55: x + 48; 

}

std::string URLEncodeGB2312(const char* szSrc/*, char* pBuf, int cbBufLen*/)
{
    std::string sOut;
	for( size_t ix = 0; ix < strlen(szSrc); ix++ )
    {       
        BYTE buf[4]; 
        memset( buf, 0, 4 ); 
        if( isalnum( (BYTE)szSrc[ix] ) || ispunct((BYTE)szSrc[ix]))
        {       
            buf[0] = szSrc[ix];
        }
        else if ( isspace( (BYTE)szSrc[ix] ) )
        {
            buf[0] = '+';
        }
        else
        {
            buf[0] = '%';
            buf[1] = toHex( (BYTE)szSrc[ix] >> 4 );
            buf[2] = toHex( (BYTE)szSrc[ix] % 16);
        }
        sOut += (char *)buf;
    }
	//strncpy(pBuf,sOut.data(),cbBufLen);
    return sOut;
};

char* Utf8ToGBK(const char* strUtf8)
{
    int len=MultiByteToWideChar(CP_UTF8, 0, /*(LPCTSTR)*/strUtf8, -1, NULL,0); 
    unsigned short * wszGBK = new unsigned short[len+1];       
    memset(wszGBK, 0, len * 2 + 2); 
    MultiByteToWideChar(CP_UTF8, 0, /*(LPCTSTR)*/strUtf8, -1, (LPWSTR)wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK=new char[len + 1]; 
    memset(szGBK, 0, len + 1); 
    WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL);
    return szGBK; 
}

char* GBKToUtf8(const char* strGBK)
{ 
    int len=MultiByteToWideChar(CP_ACP, 0, /*(LPCTSTR)*/strGBK, -1, NULL,0); 
    unsigned short * wszUtf8 = new unsigned short[len+1]; 
    memset(wszUtf8, 0, len * 2 + 2); 
    MultiByteToWideChar(CP_ACP, 0, /*(LPCTSTR)*/strGBK, -1, (LPWSTR)wszUtf8, len);
    len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
    char *szUtf8=new char[len + 1]; 
    memset(szUtf8, 0, len + 1); 
    WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, (LPSTR)szUtf8, len, NULL,NULL);
    return szUtf8; 
}

std::string EscapeToAnsi(const std::string& strSource)
{
	try
	{
		string strAnsi = strSource;					//"\u50ab",另外一种"%u50ab"暂时没有做处理
		string strValue,strvalue1;
		wchar_t wch;

		int npos = strAnsi.find("\\u",0);
		while (npos != string::npos)
		{
			strvalue1 = strAnsi.substr(npos,6);		//"\u50ab"
			strValue = strAnsi.substr(npos+2,4);	//"50ab"
			if (strValue.length() == 4)
			{
				if ( (strValue.at(0)>='0'&& strValue.at(0)<='9') || (strValue.at(0)>='a'&& strValue.at(0)<='f') || (strValue.at(0)>='A'&& strValue.at(0)<='F') )
				{
					if ( (strValue.at(1)>='0'&&strValue.at(1)<='9') || (strValue.at(1)>='a'&&strValue.at(1)<='f') || (strValue.at(1)>='A'&&strValue.at(1)<='F') )
					{
						if ( (strValue.at(2)>='0'&& strValue.at(2)<='9') || ((strValue.at(2)>='a'&& strValue.at(2)<='f')) || (strValue.at(2)>='A'&& strValue.at(2)<='F') )
						{
							if ( (strValue.at(3)>='0'&& strValue.at(3)<='9') || ((strValue.at(3)>='a'&& strValue.at(3)<='f')) || (strValue.at(3)>='A'&& strValue.at(3)<='F') )
							{
								sscanf(strValue.c_str(),"%4x",&wch);
								strAnsi.replace(strAnsi.begin()+npos,strAnsi.begin()+npos+6,_A(&wch));

								npos = strAnsi.find("\\u",0);
							}
							else
								npos = strAnsi.find("\\u",npos+2);
						}
						else
							npos = strAnsi.find("\\u",npos+2);
					}
					else
						npos = strAnsi.find("\\u",npos+2);
				}
				else
					npos = strAnsi.find("\\u",npos+2);
			}
			else
				break;			
		}
		return strAnsi;
	}
	catch (...)
	{
		return "";
	}
}