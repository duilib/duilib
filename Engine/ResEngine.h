#ifndef ResEngine_h__
#define ResEngine_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ResEngine.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-04-24 14:05:00
// 说明			:	UI资源管理，包括不限于：国际化字符串、图片、字体
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Xml/pugixml.hpp"

namespace DuiLib
{

class UILIB_API CResEngine
	: public IResEngine
	, public IIdleFilter		// ResEngine注册执行GC收集
{
private:
	CResEngine(void);
	~CResEngine(void);

	static CResEngine* s_pResEngine;
public:
	static CResEngine* GetInstance();
	static void ReleaseInstance();

public:
	// IResEngine
	virtual void SetLanguage(LPCTSTR lpszLanguage);
	virtual LPCTSTR GetLanguage();

	virtual void SetDefaultFontID(LPCTSTR lpszDefaultFontID);

	virtual void LoadResFromPath(LPCTSTR lpszResPath,bool bDefaultSkin= false);
	virtual void LoadResFromRDB(LPCTSTR lpszRDBPath,bool bDefaultSkin= false);
	virtual void LoadResFromRes(UINT nResID,LPCTSTR lpszResType,bool bDefaultSkin= false);

	virtual void FreeResPackage(LPCTSTR lpszPackageName);

	virtual LPCTSTR GetResDir(LPCTSTR lpszPackageName);

	virtual TImageData* GetImage(LPCTSTR lpszImagePath,DWORD dwMask,bool bCached = true );
	// 获取图片资源
	TImageData* GetImage(LPCTSTR lpszImageName,bool bCached = true);

	virtual void AddFont(FontObject* pFontObject);
	virtual FontObject* GetFont(LPCTSTR lpszFontID);
	virtual FontObject* GetDefaultFont();

	virtual LPCTSTR GetFont(LPCTSTR pstrFace, int nSize,
		bool bBold=false, bool bUnderline=false, bool bItalic=false,bool bStrikeout=false);
	
	virtual LPCTSTR GetI18NString(LPCTSTR lpszID);

	virtual bool ApplyResTemplate(CControlUI* pControl,LPCTSTR lpszTemplateID);

	virtual void GetHSL(short* H, short* S, short* L);
	// H:0~360, S:0~200, L:0~200 
	virtual void SetHSL(bool bUseHSL, short H, short S, short L);

	virtual DWORD GetDefaultColor(LPCTSTR lpszID);

	virtual CCurveObject* GetCurveObj(LPCTSTR lpszID);

	// 将 "{@IDSTRING}" 中的IDSTRING作为I18N字符串ID查询
	// 通过引用返回真实字符串
	void ProcessString(CDuiString& strText,LPCTSTR lpszText);
public:
	void SetDefaultShadowImage(LPCTSTR lpszShadowString);
	LPCTSTR GetDefaultShadowImage();

	// 设置应用默认阴影边距
	void SetDefaultShadowCorner(LPCRECT lprcCorner);
	RECT GetDefaultShadowCorner();

	// 设置应用默认边框圆角
	void SetDefaultShadowRoundCorner(LPSIZE lpszRoundCorner);
	SIZE GetDefaultShadowRoundCorner();

public:	// 内部方法
	// 布局-》对象辅助方法

	// 使用指定模板初始化传入窗口
	bool CreateWindowWithTemplate(CWindowUI* pWindow,LPCTSTR lpszWindowTemplate);
	// 使用指定模板初始化自定布局或控件
	CControlUI* CreateViewWithTemplate(CControlUI* pParent,LPCTSTR lpszViewTemplate,CControlUI* pNext = NULL);

	TemplateObject* GetTemplate(LPCTSTR lpszWindowTemplateName);

	CControlUI *CreateControlFromTemplate(TemplateObject *pTemplate,CWindowUI* pManager,CControlUI* pParent=NULL);

public:	// 缓存资源管理相关
	// 释放指定图片资源
	void RemoveImageFromCache(LPCTSTR lpszImageName);

	void FreeImageData(TImageData* pData);
	void RemoveAllCachedImage();
	void RemoveAllCachedFont();
	void RemoveAllCachedRDB();
	void RemoveCachedTemplate();
	void RemoveCahcedI18N();
	void RemoveCachedStyle();
	void RemoveCurveObj();

private:	
	// 根据资源URI查找文件并读取
	bool GetRes(CBufferPtr &dataBuffer,LPCTSTR lpszRelative);
	void CrackResXml(pugi::xml_node &root);
	void InitBezierPoint();
	void InsertCurveObj(CCurveObject* pObj,LPCTSTR lpszID);
private:
	TemplateObject* XmlToTemplate(pugi::xml_node& node,TemplateObject* pParent);
	FontObject* XmlToFontObj(pugi::xml_node& font);
	int TemplateToI18N(TemplateObject* pI18NTemplate);
	TemplateObject* XmlToStyleObj(pugi::xml_node& style);

	// GC处理逻辑
	virtual void OnIdle();
	void GCImageData();

private:
	FontObject* m_pDefaultFont;
	CDuiString m_strDefaultLanguage;
	CDuiString m_strDefaultSkinPath;
	IResDecoder * m_pResDecoder;

	DWORD m_dwLastGC;

	short m_H;
	short m_S;
	short m_L;

	CDuiString m_strDefaultShadowImage;
	CDuiRect m_rcDefaultShadowCorner;
	CDuiSize m_szDefaultShadowRoundCorner;

	StringMap m_mapSkinPackage;		// 皮肤包路径列表
	TemplateMap m_mapLayouts;		// 布局，包含：窗口，视图
	CStdStringPtrMap m_mapFonts;		// 字体描述表

	typedef std::multimap<CDuiString,TemplateObject*> StringTemplateMap;
	// ID->Template*
	StringTemplateMap m_mapI18N;	// I18N所有的Template

	// ID->String
	StringMap		m_mapI18NCached;		// 多国语字符串缓存

	// ID->TImageData*
	CStdStringPtrMap	m_mapImageCache; // 已缓存的图片资源

	// ID->PSkinRDB
	CStdStringPtrMap	m_mapRDB;					// 已缓存RDB包资源

	// ID->Style(Template*)
	CStdStringPtrMap m_mapStyle;				// 样式表

	// ID->CCurveObject*
	CStdStringPtrMap m_mapCurvesTable;	// 曲线对象表
};

}
#endif // ResEngine_h__
