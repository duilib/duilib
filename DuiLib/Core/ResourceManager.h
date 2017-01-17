#pragma once

namespace DuiLib
{

typedef struct DUILIB_API tagTFontInfo TFontInfo;

namespace app
{

class DUILIB_API ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    void AddFont(LPCTSTR pszResName, LPCTSTR pszTypeface, long nFontStyle);

    /**
     * Load the resources from a specific path.
     * @param hWnd A handle to the parent window. This parameter is optional.
     * @param pszResourcePath The name of the file or directory to be loaded.
     **/
    void Open(HWND hWnd, LPCTSTR pszResourcePath);

    HFONT GetFont(LPCTSTR pszResFontName);
    TFontInfo *GetFontInfo(LPCTSTR pszResFontName);

protected:
private:
    void Parse(const char *const pszContent);

    /**
     * Parse font style from a null-terminal string. This parameter can be one or more of the following values:
     *  bold
     *  italic
     *  underline
     **/
    long ParseFontStyle(LPCTSTR pszStyle);

    HWND m_hAttachWnd;
    HDC m_hAttachDC;
    CDuiStringPtrMap m_Fonts;

} final;

}
}
