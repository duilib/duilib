#ifndef TemplateObject_h__
#define TemplateObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	TemplateObject.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-12 11:30:32
// 说明			:	键值对模板
/////////////////////////////////////////////////////////////*/
#pragma once

class TemplateObject
{
public:
	TemplateObject(void);
	~TemplateObject(void);

	void SetType(LPCTSTR lpszType);
	LPCTSTR GetType(void);

	void SetAttribute(LPCTSTR lpszKey,LPCTSTR lpszValue);
	LPCTSTR GetAttribute(LPCTSTR lpszKey);

	int GetChildCount(void);
	TemplateObject* GetChild(int nIndex);

	void InsertChild(TemplateObject* pChild,int nAtIndex=-1);	// nAtIndex为-1添加到队尾

	AttributeMap m_mapAttribute;	// Xml的Attribute
private:
	CDuiString m_strType;					// 控件类型，就是XML标签名
	CStdPtrArray m_arrayChild;			// 子控件解析后的 TemplateObject*
};

#endif // TemplateObject_h__
