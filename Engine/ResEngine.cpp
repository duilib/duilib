#include "stdafx.h"
#include "ResEngine.h"
#include "Utils/stb_image.h"
#include <Shlwapi.h>

namespace DuiLib
{

EXTERN_C
{
    IResEngine * GetResEngine()
    {
        return static_cast<IResEngine *>(CResEngine::GetInstance());
    }

    LPCTSTR GetI18NString(LPCTSTR lpszItem)
    {
        return GetResEngine()->GetI18NString(lpszItem);
    }
};

CResEngine::CResEngine(void)
    : m_pDefaultFont(NULL)
    , m_pResDecoder(NULL)
    , m_strDefaultLanguage(_T("zh_cn"))
{
	InitBezierPoint();
}

CResEngine::~CResEngine(void)
{
    RemoveCachedTemplate();
    RemoveCahcedI18N();
    RemoveAllCachedImage();
    RemoveAllCachedFont();
    RemoveAllCachedRDB();
    RemoveCachedStyle();
	RemoveCurveObj();
}

void CResEngine::RemoveCachedTemplate()
{
    // 释放布局描述表
    TemplateMap::iterator iter = m_mapLayouts.begin();
    TemplateMap::iterator end = m_mapLayouts.end();

    while(iter != end)
    {
        delete iter->second;
        ++iter;
    }
}

DuiLib::CResEngine *CResEngine::s_pResEngine = NULL;

CResEngine *CResEngine::GetInstance()
{
    if(CResEngine::s_pResEngine == NULL)
    {
        CResEngine::s_pResEngine = new CResEngine();
    }

    return CResEngine::s_pResEngine;
}

void CResEngine::ReleaseInstance()
{
    if(CResEngine::s_pResEngine != NULL)
    {
        delete CResEngine::s_pResEngine;
        CResEngine::s_pResEngine = NULL;
    }
}

void CResEngine::SetLanguage(LPCTSTR lpszLanguage)
{
    m_strDefaultLanguage = lpszLanguage;
}

LPCTSTR CResEngine::GetLanguage()
{
    return m_strDefaultLanguage.c_str();
}

void CResEngine::SetDefaultFontID(LPCTSTR lpszDefaultFontID)
{
    m_pDefaultFont = GetFont(lpszDefaultFontID);
}

void CResEngine::LoadResFromPath(LPCTSTR lpszResPath, bool bDefaultSkin/*= false*/)
{
    // 1.检查目录下res.xml是否合法
    // 2.保存PackageDir路径
    // 3.分别循环解析描述表
    // 4.判断type，决定添加类型
    // 4.1 type=window，view，窗口与控件布局表
    // 4.2 type=font，字体定义表
    // 4.3 type=Language，多国语字符串表
    // 1. 检查目录是否合法
    CDuiString strTemp(lpszResPath);
    strTemp.append(_T("\\res.xml"));

    if(!::PathFileExists(strTemp.c_str()))
    {
        // 不存在，非法资源包
        return;
    }

    pugi::xml_document doc;

    if(!doc.load_file(strTemp.c_str()))
    {
        return;
    }

    LPCTSTR pszSkinPackage = NULL;
    pugi::xml_node root  =  doc.root().first_child();

    if(!root)
    {
        // xml 文件内容非法
        return;
    }
    else
    {
        pugi::xml_attribute attr = root.attribute(_T("component"));

        if(!attr)
        {
            // xml 文件内容非法
            return;
        }

        pszSkinPackage = attr.value();
    }

    // 2.保存PackageDir路径
    m_mapSkinPackage[pszSkinPackage] = lpszResPath;
    CrackResXml(root);
}

void CResEngine::LoadResFromRDB(LPCTSTR lpszRDBPath, bool bDefaultSkin/*= false*/)
{
    // 1.检查rdb资源包是否存在
    // 2.载入rdb资源包到内存缓存
    // 3.检查rdb资源包是否符合结构
    // 4.执行资源载入过程，同LoadResFromPath

    // 1.检查rdb资源包是否存在
    if(!::PathFileExists(lpszRDBPath))
    {
        // 文件未找到
        return;
    }

    // 2.载入rdb资源包到内存缓存
    LPBYTE pRDBData;
    DWORD dwRDBSize;
    CDuiFileOperation::readFile(lpszRDBPath, &pRDBData, &dwRDBSize);

    if(m_pResDecoder)
    {
        m_pResDecoder->ResDecoder(pRDBData, dwRDBSize);
    }

    // 打开zip
    HZIP hRDBHandle = OpenZip(pRDBData, dwRDBSize, 3);
    // 3.检查rdb资源包是否符合结构
    CBufferPtr xmlBuffer;

    if(!CDuiZipOperation::GetItemFromZip(hRDBHandle, _T("res.xml"), xmlBuffer))
    {
        // 资源中不存在res.xml，格式非法
        // 销毁资源
        CloseZip(hRDBHandle);
        delete [] pRDBData;
        return;
    }

    // 4.执行资源载入过程，同LoadResFromPath
    pugi::xml_document doc;

    if(! doc.load_buffer(xmlBuffer.Ptr(), xmlBuffer.Size()))
    {
        // res.xml 解析错误，失败
        return;
    }

    LPCTSTR pszSkinPackage = NULL;
    pugi::xml_node root  =  doc.root().first_child();

    if(!root)
    {
        // xml 文件内容非法
        return;
    }
    else
    {
        pugi::xml_attribute attr = root.attribute(_T("component"));

        if(!attr)
        {
            // xml 文件内容非法
            return;
        }

        pszSkinPackage = attr.value();
    }

    // 保存RDB缓存
    PSkinRDB pskinRDB = new SkinRDB;
    pskinRDB->lpByte = pRDBData;
    pskinRDB->dwSize = dwRDBSize;
    pskinRDB->hZip = hRDBHandle;
    m_mapRDB.Insert(pszSkinPackage, pskinRDB);
    CrackResXml(root);
}

void CResEngine::LoadResFromRes(UINT nResID, LPCTSTR lpszResType, bool bDefaultSkin/*= false*/)
{
    DUI__Trace(_T("暂未实现，请使用LoadResFromRDB"));
}

void CResEngine::FreeResPackage(LPCTSTR lpszPackageName)
{
}

LPCTSTR CResEngine::GetResDir(LPCTSTR lpszPackageName)
{
    if(lpszPackageName == NULL)
    {
        return m_strDefaultSkinPath.c_str();
    }

    StringMap::iterator iter = m_mapSkinPackage.find(lpszPackageName);

    if(iter != m_mapSkinPackage.end())
    {
        return iter->second.c_str();
    }

    return NULL;
}

void CResEngine::AddFont(FontObject *pFontObject)
{
    m_mapFonts.Insert(pFontObject->strFontID.c_str(), pFontObject);
}

FontObject *CResEngine::GetFont(LPCTSTR lpszFontID)
{
    FontObject *pFont = static_cast<FontObject *>(m_mapFonts.Find(lpszFontID));

    if(pFont == NULL)
    {
        return GetDefaultFont();
    }

    return pFont;
}

LPCTSTR CResEngine::GetFont(LPCTSTR pstrFace, int nSize, bool bBold/*=false*/, bool bUnderline/*=false*/, bool bItalic/*=false*/, bool bStrikeout/*=false*/)
{
    int nCount = m_mapFonts.GetSize();

    for(int i = 0; i < nCount; ++i)
    {
        LPCTSTR pstrKey = m_mapFonts.GetAt(i);
        FontObject *pFont = static_cast<FontObject *>(m_mapFonts.Find(pstrKey));

        if(CDuiStringOperation::compareNoCase(pFont->strFaceName.c_str(), pstrFace)
                && pFont->nSize == nSize
                && pFont->bBold == bBold
                && pFont->bUnderline == bUnderline
                && pFont->bItalic == bItalic
                && pFont->bStrikeout == bStrikeout)
        {
            return pstrKey;
        }
    }

    return _T("");
}

FontObject *CResEngine::GetDefaultFont()
{
    return m_pDefaultFont;
}

LPCTSTR CResEngine::GetI18NString(LPCTSTR lpszID)
{
    StringMap::iterator iter = m_mapI18NCached.find(lpszID);

    if(iter != m_mapI18NCached.end())
    {
        return iter->second.c_str();
    }

    return _T("");
}

bool CResEngine::ApplyResTemplate(CControlUI *pControl, LPCTSTR lpszTemplateID)
{
    TemplateObject *pStyleObj = static_cast<TemplateObject *>(m_mapStyle.Find(lpszTemplateID));

    if(pStyleObj == NULL)
    {
        return false;
    }

    AttributeMap::iterator iter = pStyleObj->m_mapAttribute.begin();
    AttributeMap::iterator end = pStyleObj->m_mapAttribute.end();

    while(iter != end)
    {
        pControl->SetAttribute(iter->first.c_str(), iter->second.c_str());
        ++iter;
    }

    return true;
}

void CResEngine::GetHSL(short *H, short *S, short *L)
{
    *H = m_H;
    *S = m_S;
    *L = m_L;
}

void CResEngine::SetHSL(bool bUseHSL, short H, short S, short L)
{
    if(H == m_H && S == m_S && L == m_L)
    {
        return;
    }

    m_H = CLAMP(H, 0, 360);
    m_S = CLAMP(S, 0, 200);
    m_L = CLAMP(L, 0, 200);
    //int iCount = m_arrayDirectUI.GetSize();
    //for(int i = 0; i < iCount; i++)
    //{
    //  CWindowUI *pManager = static_cast<CWindowUI *>(m_arrayDirectUI.GetAt(i));
    //  if(pManager != NULL && pManager->GetRoot() != NULL)
    //  {
    //      pManager->GetRoot()->Invalidate();
    //  }
    //}
}

DWORD CResEngine::GetDefaultColor(LPCTSTR lpszID)
{
    TemplateObject *pStyleObj = static_cast<TemplateObject *>(m_mapStyle.Find(_T("DefaultColor")));

    if(pStyleObj == NULL)
    {
        return false;
    }

    AttributeMap::iterator iter = pStyleObj->m_mapAttribute.begin();
    AttributeMap::iterator end = pStyleObj->m_mapAttribute.end();

    while(iter != end)
    {
        if(_tcsicmp(iter->first.c_str(), lpszID) == 0)
        {
            return CDuiCodeOperation::StringToColor(iter->second.c_str());
        }

        ++iter;
    }

    return true;
    return 0;
}

CCurveObject* CResEngine::GetCurveObj(LPCTSTR lpszID)
{
	if ( lpszID != NULL)
		return static_cast<CCurveObject*>(m_mapCurvesTable.Find(lpszID));
	return NULL;
}

void CResEngine::ProcessString(CDuiString & strText, LPCTSTR lpszText)
{
    strText = lpszText;
    size_t nPos;

    while((nPos = strText.find(_T("{@"))) != -1)
    {
        if(nPos != -1)
        {
            size_t nEnd = strText.find(_T('}'));
            CDuiString strPlaceholder = strText.substr(nPos, nEnd - nPos + 1);
            CDuiString strID = strPlaceholder.substr(2, strPlaceholder.length() - 3);
            LPCTSTR pstrTarget = this->GetI18NString(strID.c_str());

            if(*pstrTarget == '\0')
            {
                DUI__Trace(_T("I18NString Not Found:%s"), strPlaceholder.c_str());
                return;
            }

            CDuiStringOperation::replace(strText, strPlaceholder.c_str(), pstrTarget, false);
        }
    }
}

void CResEngine::SetDefaultShadowImage(LPCTSTR lpszShadowString)
{
    m_strDefaultShadowImage = lpszShadowString;
}

LPCTSTR CResEngine::GetDefaultShadowImage()
{
    return m_strDefaultShadowImage.c_str();
}

void CResEngine::SetDefaultShadowCorner(LPCRECT lprcCorner)
{
    m_rcDefaultShadowCorner = *lprcCorner;
}

RECT CResEngine::GetDefaultShadowCorner()
{
    return m_rcDefaultShadowCorner;
}

void CResEngine::SetDefaultShadowRoundCorner(LPSIZE lpszRoundCorner)
{
    m_szDefaultShadowRoundCorner = *lpszRoundCorner;
}

SIZE CResEngine::GetDefaultShadowRoundCorner()
{
    return m_szDefaultShadowRoundCorner;
}

bool CResEngine::CreateWindowWithTemplate(CWindowUI *pWindow, LPCTSTR lpszWindowTemplate)
{
    // 1.解析Window相关属性，设置到窗口上
    // 2.递归创建UI控件
    TemplateObject *pWindowTemplate = GetTemplate(lpszWindowTemplate);

    if(pWindowTemplate)
    {
        AttributeMap::iterator iter = pWindowTemplate->m_mapAttribute.begin();
        AttributeMap::iterator end = pWindowTemplate->m_mapAttribute.end();

        while(iter != end)
        {
            pWindow->SetAttribute(iter->first.c_str(), iter->second.c_str());
            ++iter;
        }

        TemplateObject *pControl = pWindowTemplate->GetChild(0);
        pWindow->m_pRootControl =  CreateControlFromTemplate(pControl, pWindow);
        return true;
    }

    return false;
}

CControlUI *CResEngine::CreateViewWithTemplate(CControlUI *pParent, LPCTSTR lpszViewTemplate, CControlUI *pNext /*= NULL*/)
{
    // 根据lpszViewTemplate查找Template
    // 将Template实例化为对象pView
    // 如果pNext不为null，执行pView->Add(pNext);
    TemplateObject *pWindowTemplate = GetTemplate(lpszViewTemplate);

    if(pWindowTemplate)
    {
        CControlUI *pView = CreateControlFromTemplate(pWindowTemplate, pParent->GetManager(), pParent);

        if(pView)
        {
            return pView;
        }
        else
        {
            return NULL;
        }
    }

    return NULL;
}

TemplateObject *CResEngine::GetTemplate(LPCTSTR lpszWindowTemplateName)
{
    TemplateMap::iterator iter;
    iter = m_mapLayouts.find(lpszWindowTemplateName);

    if(iter != m_mapLayouts.end())
    {
        return iter->second;
    }

    return NULL;
}

CControlUI *CResEngine::CreateControlFromTemplate(TemplateObject *pTemplate, CWindowUI *pManager, CControlUI *pParent/*=NULL*/)
{
    CControlUI *pReturnControl = NULL;
    pReturnControl = CreateControl(pTemplate->GetType());

    if(pReturnControl != NULL)
    {
        // 控件创建成功，设置管理
        pReturnControl->SetManager(pManager, pParent);
        // 设置控件属性
        AttributeMap::iterator iter = pTemplate->m_mapAttribute.begin();
        AttributeMap::iterator end = pTemplate->m_mapAttribute.end();

        while(iter != end)
        {
            pReturnControl->SetAttribute(iter->first.c_str(), iter->second.c_str());
            ++iter;
        }

        // 提供IContainter接口的布局控件才继续解析并创建子控件
        IContainerUI *pContainer = static_cast<IContainerUI *>(pReturnControl->GetInterface(_T("IContainer")));
        int nCount = pTemplate->GetChildCount();

        if(pContainer != NULL)
        {
            // 有子控件
            for(int i = 0; i < nCount; ++i)
            {
                CControlUI *pChildControl = this->CreateControlFromTemplate(pTemplate->GetChild(i), pManager, pReturnControl);

                if(pChildControl != NULL)
                {
                    pContainer->Add(pChildControl);
                }
            }

            pReturnControl->SetManager(pManager, pParent);
        }
    }

    return pReturnControl;
}

TImageData *CResEngine::GetImage(LPCTSTR lpszImagePath, DWORD dwMask, bool bCached /*= true */)
{
    TImageData *pImageData = static_cast<TImageData *>(m_mapImageCache.Find(lpszImagePath));

    if(pImageData != NULL)
    {
        return pImageData;
    }

    // 处理图片路径
    CDuiString strImagePath;
    CBufferPtr dataBuffer;
    this->GetRes(dataBuffer, lpszImagePath);

    if(!dataBuffer.IsValid())
    {
        //::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    LPBYTE pImage = NULL;
    int x, y, n;
    pImage = stbi_load_from_memory(dataBuffer.Ptr(), dataBuffer.Size(), &x, &y, &n, 4);

    if(!pImage)
    {
        //::MessageBox(0, _T("解析图片失败"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = x;
    bmi.bmiHeader.biHeight = -y;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = x * y * 4;
    bool bAlphaChannel = false;
    LPBYTE pDest = NULL;
    HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pDest, NULL, 0);

    if(!hBitmap)
    {
        //::MessageBox(0, _T("CreateDIBSection失败"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    for(int i = 0; i < x * y; i++)
    {
        pDest[i * 4 + 3] = pImage[i * 4 + 3];

        if(pDest[i * 4 + 3] < 255)
        {
            // 每像素存在Alpha值，预乘，AlphaBlend函数需要
            pDest[i * 4] = (BYTE)(DWORD(pImage[i * 4 + 2]) * pImage[i * 4 + 3] / 255);
            pDest[i * 4 + 1] = (BYTE)(DWORD(pImage[i * 4 + 1]) * pImage[i * 4 + 3] / 255);
            pDest[i * 4 + 2] = (BYTE)(DWORD(pImage[i * 4]) * pImage[i * 4 + 3] / 255);
            bAlphaChannel = true;
        }
        else
        {
            pDest[i * 4] = pImage[i * 4 + 2];
            pDest[i * 4 + 1] = pImage[i * 4 + 1];
            pDest[i * 4 + 2] = pImage[i * 4];
        }

        if(*(DWORD *)(&pDest[i * 4]) == dwMask)
        {
            pDest[i * 4] = (BYTE)0;
            pDest[i * 4 + 1] = (BYTE)0;
            pDest[i * 4 + 2] = (BYTE)0;
            pDest[i * 4 + 3] = (BYTE)0;
            bAlphaChannel = true;
        }
    }

    stbi_image_free(pImage);
    pImage = NULL;
    TImageData *data = new TImageData;
    data->hBitmap = hBitmap;
    data->nX = x;
    data->nY = y;
    data->delay = 0;
    data->alphaChannel = bAlphaChannel;
    data->nCounter = 0;

    if(bCached)
    {
        m_mapImageCache.Insert(lpszImagePath, data);
    }

    return data;
}

TImageData *CResEngine::GetImage(LPCTSTR lpszImageName, bool bCached /*= true*/)
{
    return this->GetImage(lpszImageName, 0, bCached);
}

void CResEngine::FreeImageData(TImageData *pData)
{
    if(pData->hBitmap)
    {
        ::DeleteObject(pData->hBitmap) ;
    }

    delete pData;
}

void CResEngine::RemoveAllCachedImage()
{
    int iCount = m_mapImageCache.GetSize();

    if(iCount == 0)
    {
        return;
    }

    TImageData *data;

    for(int i = 0; i < iCount; i++)
    {
        if(LPCTSTR key = m_mapImageCache.GetAt(i))
        {
            data = static_cast<TImageData *>(m_mapImageCache.Find(key, false));

            if(data)
            {
                FreeImageData(data);
            }
        }
    }
}

void CResEngine::RemoveAllCachedFont()
{
    int iCount = m_mapFonts.GetSize();

    if(iCount == 0)
    {
        return;
    }

    for(int i = 0; i < iCount; i++)
    {
        if(LPCTSTR key = m_mapFonts.GetAt(i))
        {
            FontObject *pFont = static_cast<FontObject *>(m_mapFonts.Find(key, false));

            if(pFont)
            {
                delete pFont;
            }
        }
    }
}

void CResEngine::RemoveAllCachedRDB()
{
    int iCount = m_mapRDB.GetSize();

    if(iCount == 0)
    {
        return;
    }

    for(int i = 0; i < iCount; i++)
    {
        if(LPCTSTR key = m_mapRDB.GetAt(i))
        {
            PSkinRDB pSkinRDB = static_cast<PSkinRDB>(m_mapRDB.Find(key, false));

            if(pSkinRDB)
            {
                CloseZip((HZIP)pSkinRDB->hZip);
                delete [] pSkinRDB->lpByte;
                delete pSkinRDB;
            }
        }
    }
}

void CResEngine::CrackResXml(pugi::xml_node & root)
{
    LPCTSTR pstrSection = NULL;
    LPTSTR pstr = NULL;

    for(pugi::xml_node node = root.first_child(); node; node = node.next_sibling())
    {
        // 3.分别循环解析描述表
        pstrSection = node.name();
        LPCTSTR pstrName = node.attribute(_T("name")).value();
        LPCTSTR pstrType = node.attribute(_T("type")).value();
        LPCTSTR pstrPath = node.attribute(_T("path")).value();
        // 加载对应的xml文件并检查
        CBufferPtr xmlBuffer;
        GetRes(xmlBuffer, pstrPath);
        pugi::xml_document doc;

        if(! doc.load_buffer(xmlBuffer.Ptr(), xmlBuffer.Size()))
        {
            continue;
        }

        pugi::xml_node root = doc.root().first_child();

        if(_tcsicmp(pstrType, _T("window")) == 0 || _tcsicmp(pstrType, _T("view")) == 0)
        {
            TemplateObject *pTemplate = XmlToTemplate(root, NULL);

            if(pTemplate)
            {
                m_mapLayouts[(LPCTSTR)pstrName] = pTemplate;
            }
        }
        else if(_tcsicmp(pstrType, _T("font")) == 0)
        {
            for(pugi::xml_node child = root.first_child(); child; child = child.next_sibling())
            {
                FontObject *pFontObj = XmlToFontObj(child);

                if(pFontObj != NULL)
                {
                    this->AddFont(pFontObj);
                }
            }
        }
        else if(_tcsicmp(pstrType, _T("i18n")) == 0)
        {
            TemplateObject *pTemplate = XmlToTemplate(root, NULL);

            if(pTemplate)
            {
                LPCTSTR pstrLanguage = pTemplate->GetAttribute(_T("language"));

                if(pstrLanguage == NULL)
                {
                    delete pTemplate;
                    continue;
                }

                m_mapI18N.insert(StringTemplateMap::value_type(pstrLanguage, pTemplate));

                if(CDuiStringOperation::compareNoCase(m_strDefaultLanguage.c_str(), pstrLanguage) == 0)
                {
                    int nRet = TemplateToI18N(pTemplate);
                    DUI__Trace(_T("LoadI18N:%d"), nRet);
                }
            }
        }
        else if(_tcsicmp(pstrType, _T("style")) == 0)
        {
            for(pugi::xml_node child = root.first_child(); child; child = child.next_sibling())
            {
                TemplateObject *pStyleObj = XmlToStyleObj(child);

                if(pStyleObj != NULL)
                {
                    m_mapStyle.Insert(pStyleObj->GetType(), pStyleObj);
                }
            }
        }
    }
}

void CResEngine::InitBezierPoint()
{
	CCurveObject* pObj = NULL;

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(1.0f,1.0f);
	InsertCurveObj(pObj,_T("curve.liner"));

	//////////////////////////////////////////////////////////////////////////
	// quad
	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.550000f,0.085000f);
	pObj->AddPoint(0.680000f,0.530000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quad.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.250000f,0.460000f);
	pObj->AddPoint(0.450000f,0.940000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quad.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.455000f,0.030000f);
	pObj->AddPoint(0.515000f,0.955000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quad.inout"));

	//////////////////////////////////////////////////////////////////////////
	// cubic
	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.550000f,0.055000f);
	pObj->AddPoint(0.675000f,0.190000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.cubic.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.215000f,0.610000f);
	pObj->AddPoint(0.355000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.cubic.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.645000f,0.045000f);
	pObj->AddPoint(0.355000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.cubic.inout"));

	//////////////////////////////////////////////////////////////////////////
	// quart
	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.895000f,0.030000f);
	pObj->AddPoint(0.685000f,0.220000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quart.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.165000f,0.840000f);
	pObj->AddPoint(0.440000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quart.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.770000f,0.000000f);
	pObj->AddPoint(0.175000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quart.inout"));

	//////////////////////////////////////////////////////////////////////////
	// quint
	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.755000f,0.050000f);
	pObj->AddPoint(0.855000f,0.060000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quint.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.230000f,1.000000f);
	pObj->AddPoint(0.320000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quint.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.230000f,1.000000f);
	pObj->AddPoint(0.320000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.quint.inout"));

	//////////////////////////////////////////////////////////////////////////
	// sine
	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.470000f,0.000000f);
	pObj->AddPoint(0.745000f,0.715000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.sine.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.390000f,0.575000f);
	pObj->AddPoint(0.565000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.sine.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.000000f,0.000000f);
	pObj->AddPoint(0.445000f,0.050000f);
	pObj->AddPoint(0.550000f,0.950000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.sine.inout"));

	//////////////////////////////////////////////////////////////////////////
	// expo
	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.950000f,0.050000f);
	pObj->AddPoint(0.795000f,0.035000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.expo.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.190000f,1.000000f);
	pObj->AddPoint(0.220000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.expo.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(1.00000f,0.000000f);
	pObj->AddPoint(0.550000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.expo.inout"));

	//////////////////////////////////////////////////////////////////////////
	// circ
	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.600000f,0.040000f);
	pObj->AddPoint(0.980000f,0.335000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.circ.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.075000f,0.820000f);
	pObj->AddPoint(0.165000f,1.000000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.circ.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.785000f,0.135000f);
	pObj->AddPoint(0.150000f,0.860000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.circ.inout"));

	//////////////////////////////////////////////////////////////////////////
	// back
	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.600000f,-0.280000f);
	pObj->AddPoint(0.735000f,0.045000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.back.in"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.175000f,0.885000f);
	pObj->AddPoint(0.320000f,1.275000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.back.out"));

	pObj = new CCurveObject;
	pObj->AddPoint(0.0f,0.0f);
	pObj->AddPoint(0.680000f,-0.550000f);
	pObj->AddPoint(0.265000f,1.550000f);
	pObj->AddPoint(1.000000f,1.000000f);
	InsertCurveObj(pObj,_T("curve.back.inout"));
}

void CResEngine::InsertCurveObj(CCurveObject* pObj,LPCTSTR lpszID)
{
	m_mapCurvesTable.Insert(lpszID,pObj);
}

bool CResEngine::GetRes(CBufferPtr & dataBuffer, LPCTSTR lpszRelative)
{
    CDuiString strTemp(lpszRelative);

    if(strTemp.empty())
    {
        return false;
    }

    size_t iIndex = strTemp.find(_T('#'));

    if(iIndex == strTemp.npos)
    {
        // 没有 "PackageName#"
        // 可能是绝对路径，也可能是从DefaultPackage中读取资源
        if(::PathFileExists(lpszRelative))
        {
            // 是一个绝对路径
            CDuiFileOperation::readFile(lpszRelative, dataBuffer);
            return true;
        }

        CDuiString strDefault(m_strDefaultSkinPath);
        strDefault.append(lpszRelative);

        if(::PathFileExists(strDefault.c_str()))
        {
            // 来自默认皮肤包的资源
            CDuiFileOperation::readFile(strDefault.c_str(), dataBuffer);
            return true;
        }

        return false;
    }

    // 提取PackageName
    CDuiString strComponent = strTemp.substr(0, iIndex);
    CDuiString strResPath(lpszRelative);
    CDuiString strPlaceholder = CDuiStringOperation::format(_T("%s#"), strComponent.c_str());
    // 从文件系统中查询
    StringMap::iterator iter = m_mapSkinPackage.find(strComponent.c_str());

    if(iter != m_mapSkinPackage.end())
    {
        CDuiString strPrefixPath = CDuiStringOperation::format(_T("%s\\"), (iter->second).c_str());
        CDuiStringOperation::replace(strResPath, strPlaceholder.c_str(), strPrefixPath.c_str());

        if(::PathFileExists(strResPath.c_str()))
        {
            // 是一个绝对路径
            CDuiFileOperation::readFile(strResPath.c_str(), dataBuffer);
            return true;
        }

        return false;
    }

    // 从RDB资源中查询
    PSkinRDB pSkinRDB = static_cast<PSkinRDB>(m_mapRDB.Find(strComponent.c_str()));

    if(pSkinRDB == NULL)
    {
        // 没有对应的RDB包，查询失败
        return false;
    }

    CDuiStringOperation::replace(strResPath, strPlaceholder.c_str(), _T(""));

    if(CDuiZipOperation::GetItemFromZip((HZIP)pSkinRDB->hZip, strResPath.c_str(), dataBuffer))
    {
        // 从RDB中获取成功
        return true;
    }

    return false;
}

TemplateObject *CResEngine::XmlToTemplate(pugi::xml_node & node, TemplateObject *pParent)
{
    TemplateObject *pChildTemplateObj = new TemplateObject;
    pChildTemplateObj->SetType(node.name());

    for(pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute())
    {
        LPCTSTR pstrKey = attr.name();
        LPCTSTR pstrValue = attr.value();
        pChildTemplateObj->SetAttribute(pstrKey, pstrValue);
    }

    for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
    {
        this->XmlToTemplate(child, pChildTemplateObj);
    }

    if(pParent != NULL)
    {
        pParent->InsertChild(pChildTemplateObj, -1);
        return pParent;
    }
    else
    {
        return pChildTemplateObj;
    }
}

FontObject *CResEngine::XmlToFontObj(pugi::xml_node & font)
{
    LPCTSTR pstrName = NULL;
    LPCTSTR pstrFace = NULL;
    int nSize = 0;
    bool bBold = false;
    bool bItalic = false;
    bool bUnderline = false;
    bool bStrikeout = false;
    bool bDefault = false;

    for(pugi::xml_attribute attr = font.first_attribute(); attr; attr = attr.next_attribute())
    {
        LPCTSTR pstrKey = attr.name();
        LPCTSTR pstrValue = attr.value();

        if(_tcsicmp(pstrKey, _T("name")) == 0)
        {
            pstrName = pstrValue;
        }
        else if(_tcsicmp(pstrKey, _T("face")) == 0)
        {
            pstrFace = pstrValue;
        }
        else if(_tcsicmp(pstrKey, _T("size")) == 0)
        {
            nSize = ::_ttoi(pstrValue);
        }
        else if(_tcsicmp(pstrKey, _T("bold")) == 0)
        {
            bBold = _tcsicmp(pstrValue, _T("true")) == 0;
        }
        else if(_tcsicmp(pstrKey, _T("italic")) == 0)
        {
            bItalic = _tcsicmp(pstrValue, _T("true")) == 0;
        }
        else if(_tcsicmp(pstrKey, _T("underline")) == 0)
        {
            bUnderline = _tcsicmp(pstrValue, _T("true")) == 0;
        }
        else if(_tcsicmp(pstrKey, _T("strikeout")) == 0)
        {
            bStrikeout = _tcsicmp(pstrValue, _T("true")) == 0;
        }
        else if(_tcsicmp(pstrKey, _T("default")) == 0)
        {
            bDefault = _tcsicmp(pstrValue, _T("true")) == 0;
        }
    }

    if(pstrName != NULL &&  pstrFace != NULL
            && pstrName[0] != 0 && pstrFace[0] != NULL)
    {
        FontObject *pFont = new FontObject;
        pFont->strFontID = pstrName;
        pFont->strFaceName = pstrFace;
        pFont->nSize = nSize;
        pFont->bBold = bBold;
        pFont->bItalic = bItalic;
        pFont->bUnderline = bUnderline;
        pFont->bStrikeout = bStrikeout;

        if(bDefault)
        {
            m_pDefaultFont = pFont;
        }

        return pFont;
    }

    return NULL;
}

int CResEngine::TemplateToI18N(TemplateObject *pI18NTemplate)
{
    int nCount = pI18NTemplate->GetChildCount();
    int i = 0;

    for(; i < nCount; ++i)
    {
        TemplateObject *pElement = pI18NTemplate->GetChild(i);
        LPCTSTR pstrID = pElement->GetAttribute(_T("id"));
        LPCTSTR pstrText = pElement->GetAttribute(_T("text"));

        if(pstrID && pstrText)
        {
            m_mapI18NCached[pstrID] = pstrText;
        }
    }

    return i;
}

TemplateObject *CResEngine::XmlToStyleObj(pugi::xml_node & style)
{
    LPCTSTR pstrName = style.name();
    TemplateObject *pStyleObj = new TemplateObject;
    pStyleObj->SetType(pstrName);

    for(pugi::xml_attribute attr = style.first_attribute(); attr; attr = attr.next_attribute())
    {
        LPCTSTR pstrKey = attr.name();
        LPCTSTR pstrValue = attr.value();
        pStyleObj->SetAttribute(pstrKey, pstrValue);
    }

    return pStyleObj;
}

void CResEngine::OnIdle()
{
    
}

void CResEngine::GCImageData()
{
	int iCount = m_mapImageCache.GetSize();

	if(iCount == 0)
	{
		return;
	}

	TImageData *data;

	for(int i = 0; i < m_mapImageCache.GetSize(); i++)
	{
		if(LPCTSTR key = m_mapImageCache.GetAt(i))
		{
			data = static_cast<TImageData *>(m_mapImageCache.Find(key, false));

			if(data->nCounter<100)
			{
				FreeImageData(data);
				m_mapImageCache.Remove(key);
				break;
			}
		}
	}
}

void CResEngine::RemoveCahcedI18N()
{
    // 释放I18N描述表
    TemplateMap::iterator iter = m_mapI18N.begin();
    TemplateMap::iterator end = m_mapI18N.end();

    while(iter != end)
    {
        delete iter->second;
        ++iter;
    }
}

void CResEngine::RemoveCachedStyle()
{
    int iCount = m_mapStyle.GetSize();

    if(iCount == 0)
    {
        return;
    }

    TemplateObject *pStyleObj = NULL;

    for(int i = 0; i < iCount; i++)
    {
        if(LPCTSTR key = m_mapStyle.GetAt(i))
        {
            pStyleObj = static_cast<TemplateObject *>(m_mapStyle.Find(key, false));

            if(pStyleObj)
            {
                delete pStyleObj;
            }
        }
    }
}

void CResEngine::RemoveCurveObj()
{
	int nNber = m_mapCurvesTable.GetSize();
	for (int i=0;i<nNber;++i)
	{
		LPCTSTR pstrKey = m_mapCurvesTable.GetAt(i);
		CCurveObject* pObj = static_cast<CCurveObject*>(m_mapCurvesTable.Find(pstrKey,false));
		delete pObj;
	}

	m_mapCurvesTable.RemoveAll();
}

}