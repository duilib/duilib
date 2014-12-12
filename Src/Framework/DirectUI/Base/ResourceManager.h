#ifndef ResourceManager_h__
#define ResourceManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ResourceManager.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-11 17:47:53
// 说明			:	全局资源描述管理：布局，语言，字体
/////////////////////////////////////////////////////////////*/
#pragma once
class CUIEngine;
class TemplateObject;
class TiXmlElement;
class CWindowUI;

class DIRECTUI_API CResourceManager
{
public:
	static CResourceManager *GetInstance();
	static void ReleaseInstance();

public:
	virtual void SetLanguage(LANGID wLangID);
	virtual void InitResouceDir(LPCTSTR lpszPath,bool bIsDefault = false);

	virtual void FreeResource(LPCTSTR lpszComponentName);

	LANGID GetLanguage();
	LPCTSTR GetResouceDir(LPCTSTR lpszModuleName = NULL);

	void SetDefaultFont(LPCTSTR lpszFaceName,int nSize = 12, bool bBold = false, bool bUnderline= false, bool bItalic= false ,bool bStrikeout= false);
	FontObject* GetDefaultFont(void);

	TemplateObject* GetWindowTemplate(LPCTSTR lpszWindowTemplateName);
	TemplateObject* GetViewTemplate(LPCTSTR lpszViewTemplateName);

	CControlUI *CreateControlFromTemplate(TemplateObject *pTemplate,CWindowUI* pManager,CControlUI* pParent=NULL);

	TImageData* GetImage(LPCTSTR lpszImagePath,DWORD dwMask,bool bCached = true );
	void FreeImage(LPCTSTR lpszImagePath);

	void AddFont(FontObject *pFont);
	FontObject*	GetFont(LPCTSTR lpszFontName);
	void RemoveAllFont();

	LPCTSTR GetI18N(LPCTSTR lpszName);

	bool GetAbsolutePath(CDuiString& strFullPath,LPCTSTR lpszRelativePath);
private:
	bool GetAbsolutePath(CDuiString& strFullPath,LPCTSTR lpszComponent,LPCTSTR lpszRelativePath);

	void parseLayouts(TiXmlElement * pLayouts, LPCTSTR lpszComponentName);
	void parseFonts(TiXmlElement * pLayouts, LPCTSTR lpszComponentName);
	void parseLanguages(TiXmlElement * pLayouts, LPCTSTR lpszComponentName);

	TemplateObject* parseXmlFile(LPCTSTR lpszFilePath);
	TemplateObject* XmlToTemplate(TiXmlElement *pElement,TemplateObject* pParent);

	void LoadI18NString(LPCTSTR lpszFilePath);
	TImageData* LoadImage(LPCTSTR lpszFilePath,DWORD dwMask);
	void FreeImageData(TImageData* pData);
	void RemoveAllCachedImage();
private:
	// 对Resource.xml的缓存
	StringMap m_mapComponent;		// 组件资源路径
	TemplateMap m_mapLayouts;		// 布局，包含：窗口，视图
	TemplateMap m_mapLanguages;	// 多国语
	// TODO 暂不支持 TemplateMap m_mapTemplates; 可复用样式
	
private:
	LANGID m_DefaultLangID;
	CDuiString m_strDefaultResourcePath;
	FontObject *m_pDefaultFontObj;

	FontPoolVector m_vecFontPool;		// 已加载的字体样式缓存
	StringMap		m_mapI18NCached;		// 多国语字符串缓存

	CStdStringPtrMap m_ImageDataCached; // 已缓存的图片资源
	CStdStringPtrMap	m_mapImageHash; // 已缓存的图片资源
private:
	CResourceManager(void);
	~CResourceManager(void);
	static CResourceManager *m_pInstace;
	static CUIEngine* m_pEngine;
};

#endif // ResourceManager_h__
