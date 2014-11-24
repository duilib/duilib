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
	// 增加资源描述
	virtual void InitResouceDir(LPCTSTR lpszPath,bool bIsDefault = false);
	virtual void FreeResource(LPCTSTR lpszComponentName);
	// 获取指定模块的基路径
	// 参数为null返回默认资源基路径
	LPCTSTR GetResouceDir(LPCTSTR lpszModuleName = NULL);

	// 设置当前显示语言，暂未实现运行时变更通知
	virtual void SetLanguage(LANGID wLangID);
	// 获取当前显示语言
	LANGID GetLanguage();

	// 资源相关API

	// 获取一个窗口xml
	TemplateObject* GetWindowTemplate(LPCTSTR lpszWindowTemplateName);
	// 获取一个布局xml
	TemplateObject* GetViewTemplate(LPCTSTR lpszViewTemplateName);
	// 使用xml实例化控件
	CControlUI *CreateControlFromTemplate(TemplateObject *pTemplate,CWindowUI* pManager,CControlUI* pParent=NULL);
	// 获取图片资源，优先从缓存中查找
	TImageData* GetImage(LPCTSTR lpszImagePath,DWORD dwMask,bool bCached = true );
	// 释放缓存的图片资源
	void FreeImage(LPCTSTR lpszImagePath);
	// 增加一个字体
	void AddFont(FontObject *pFont);
	// 移除所有字体
	void RemoveAllFont();
	// 通过名字查询字体
	FontObject*	GetFont(LPCTSTR lpszFontName);
	// 获取当前语言字符串
	LPCTSTR GetI18N(LPCTSTR lpszName);
	// 将组件相对路径转换为绝对路径，不包含#分隔符的直接返回
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
	// TODO 暂不支持 TemplateMap m_mapTemplates;	// 可复用样式
	
private:
	LANGID					m_DefaultLangID;	// 当前显示语言，默认为用户首选语言
	CDuiString				m_strDefaultResourcePath;
	CDuiString				m_strDefaultFont;	// 默认字体
	// 已加载资源的缓存
	FontPoolVector		m_vecFontPool;		// 已加载的字体样式缓存
	StringMap				m_mapI18NCached;		// 多国语字符串缓存
	CStdStringPtrMap	m_mapImageHash; // 已缓存的图片资源
private:
	CResourceManager(void);
	~CResourceManager(void);
	static CResourceManager *m_pInstace;
	static CUIEngine* m_pEngine;
};

#endif // ResourceManager_h__
