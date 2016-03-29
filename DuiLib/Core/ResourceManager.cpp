#include "StdAfx.h"
#include <Shlwapi.h>
#include <rapidxml.hpp>


namespace DuiLib
{
namespace app
{

static const int FONT_STYLE_BOLD =          0x0001;
static const int FONT_STYLE_UNDERLINE =     0x0002;
static const int FONT_STYLE_ITALIC =        0x0004;
static const int FONT_STYLE_STRIKE_OUT =    0x0008;


ResourceManager::ResourceManager()
    : m_hAttachWnd(NULL)
    , m_hAttachDC(NULL)
{

}


ResourceManager::~ResourceManager()
{
    // remove fonts
    for (int i = 0; i < m_Fonts.GetSize(); i++) {
        if (LPCTSTR key = m_Fonts.GetAt(i)) {
            auto pFontInfo = static_cast<TFontInfo*>(m_Fonts.Find(key, false));
            if (pFontInfo) {
                ::DeleteObject(pFontInfo->hFont);
                delete pFontInfo;
            }
        }
    }
    m_Fonts.RemoveAll();

    ::ReleaseDC(m_hAttachWnd, m_hAttachDC);
}


void ResourceManager::Open(HWND hWnd, LPCTSTR pszResourcePath)
{
    if (!::PathFileExists(pszResourcePath))
        return;

    m_hAttachWnd = hWnd;
    m_hAttachDC = ::GetDC(hWnd);

    if (::PathIsDirectory(pszResourcePath)) {
        TCHAR szResXml[MAX_PATH];
        ::PathCombine(szResXml, pszResourcePath, _T("resources.xml"));
        if (::PathFileExists(szResXml)) {
            HANDLE hResFile = ::CreateFile(szResXml, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
            if (hResFile == INVALID_HANDLE_VALUE)
                return;
            unsigned long dwFileSize = ::GetFileSize(hResFile, nullptr);
            char *pBuffer = new char[dwFileSize + 1];
            unsigned long dwRead = 0;
            ::ReadFile(hResFile, pBuffer, dwFileSize, &dwRead, nullptr);
            pBuffer[dwRead] = 0;
            Parse(pBuffer);
            delete[] pBuffer;
            ::CloseHandle(hResFile);
        }
    } else {

    }
}


void ResourceManager::Parse(const char *const pszContent)
{
    char *pXmlContent = nullptr;

    const size_t nNeedWideChar = ::MultiByteToWideChar(CP_UTF8, 0, pszContent, -1, nullptr, 0);
    wchar_t *pBufferW = new wchar_t[nNeedWideChar + 1];
    int nConverted = ::MultiByteToWideChar(CP_UTF8, 0, pszContent, -1, pBufferW, nNeedWideChar);
    pBufferW[nConverted] = _T('\0');

    pXmlContent = ( char *)pBufferW;

#if !defined(UNICODE)
    const size_t nNeedByte = ::WideCharToMultiByte(CP_ACP, 0, pBufferW, -1, nullptr, 0, nullptr, nullptr);
    char *pBufferA = new char[nNeedByte];
    ::WideCharToMultiByte(CP_ACP, 0, pBufferW, -1, pBufferA, nNeedByte, nullptr, nullptr);
    pXmlContent = pBufferA;
    delete[] pBufferW;
#endif

    rapidxml::xml_document<TCHAR> xmlDoc;
    xmlDoc.parse<0>((LPTSTR)pXmlContent);
    for (auto node = xmlDoc.first_node()->first_node(); node; node = node->next_sibling()) {
        LPCTSTR pszNodeName = node->name();
        if (_tcscmp(pszNodeName, _T("font")) == 0) {
            LPCTSTR pszName = nullptr, pszTypeface = nullptr, pszSize = nullptr, pszStyle = nullptr;
            for (auto attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
                LPCTSTR pszAttrName = attr->name();
                if (_tcscmp(pszAttrName, _T("name")) == 0) {
                    pszName = attr->value();
                } else if (_tcscmp(pszAttrName, _T("typeface")) == 0) {
                    pszTypeface = attr->value();
                } else if (_tcscmp(pszAttrName, _T("size")) == 0) {
                    pszSize = attr->value();
                } else if (_tcscmp(pszAttrName, _T("style")) == 0) {
                    pszStyle = attr->value();
                }
            }
            AddFont(pszName, pszTypeface, (pszSize ? _ttol(pszSize) : 12) | ParseFontStyle(pszStyle));
        }
    }

    delete[] (TCHAR *)pXmlContent;
}


long ResourceManager::ParseFontStyle(LPCTSTR pszStyle)
{
    if (!pszStyle)
        return 0;

    long nStyle = 0;
    const size_t nBufferSize = _tcslen(pszStyle) + 1;
    TCHAR *pBuffer = new TCHAR[nBufferSize];
    _tcscpy_s(pBuffer, nBufferSize, pszStyle);
    TCHAR *pContext = nullptr;
    TCHAR delims[] = _T("| ");
    TCHAR *pszToken = _tcstok_s(pBuffer, delims, &pContext);
    while (pszToken != nullptr) {
        if (_tcscmp(pszToken, _T("bold")) == 0) {
            nStyle |= FONT_STYLE_BOLD;
        } else if (_tcscmp(pszToken, _T("italic")) == 0) {
            nStyle |= FONT_STYLE_ITALIC;
        } else if (_tcscmp(pszToken, _T("underline")) == 0) {
            nStyle |= FONT_STYLE_UNDERLINE;
        } else if (_tcscmp(pszToken, _T("line-through")) == 0) {
            nStyle |= FONT_STYLE_STRIKE_OUT;
        }
        pszToken = _tcstok_s(nullptr, delims, &pContext);
    }

    delete[] pBuffer;

    return MAKELONG(0, nStyle);
}


void ResourceManager::AddFont(LPCTSTR pszResName, LPCTSTR pszTypeface, long nFontStyle)
{
    ASSERT(pszResName != nullptr);
    ASSERT(pszTypeface != nullptr);

    int nFontSize = LOWORD(nFontStyle);
    int nStyle = HIWORD(nFontStyle);

    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy_s(lf.lfFaceName, pszTypeface, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nFontSize;
    if (nStyle & FONT_STYLE_BOLD)
        lf.lfWeight += FW_BOLD;
    if (nStyle & FONT_STYLE_ITALIC)
        lf.lfItalic = TRUE;
    if (nStyle & FONT_STYLE_UNDERLINE)
        lf.lfUnderline = TRUE;
    if (nStyle & FONT_STYLE_STRIKE_OUT)
        lf.lfStrikeOut = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    ASSERT(hFont != NULL);

    TFontInfo *pFontInfo = new TFontInfo;
    ::ZeroMemory(pFontInfo, sizeof(TFontInfo));
    pFontInfo->hFont = hFont;

    if (m_hAttachDC) {
        HFONT hOldFont = (HFONT)::SelectObject(m_hAttachDC, hFont);
        ::GetTextMetrics(m_hAttachDC, &pFontInfo->tm);
        ::SelectObject(m_hAttachDC, hOldFont);
    }

    TFontInfo *pOldFontInfo = static_cast<TFontInfo *>(m_Fonts.Find(pszResName));
    if (pOldFontInfo) {
        ::DeleteObject(pOldFontInfo->hFont);
        delete pOldFontInfo;
        m_Fonts.Remove(pszResName);
    }

    m_Fonts.Insert(pszResName, pFontInfo);
}


HFONT ResourceManager::GetFont(LPCTSTR pszResFontName)
{
    TFontInfo *pFontInfo = GetFontInfo(pszResFontName);
    return pFontInfo ? pFontInfo->hFont : NULL;
}


TFontInfo *ResourceManager::GetFontInfo(LPCTSTR pszResFontName)
{
    TFontInfo *pFontInfo = static_cast<TFontInfo *>(m_Fonts.Find(pszResFontName));
    return pFontInfo;
}

}
}
