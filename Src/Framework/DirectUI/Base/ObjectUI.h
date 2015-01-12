#ifndef DirectUIObject_h__
#define DirectUIObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	DirectUIObject.h
// 创建人	: 	daviyang35@gmail.com
// 创建时间	:	2014-11-08 15:27:41
// 说明		:	Root Object
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API CObjectUI
{
public:
	CObjectUI(void);
	virtual ~CObjectUI(void);

	virtual void SetName(LPCTSTR lpszName);
	virtual LPCTSTR GetName();

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);
	virtual bool IsClientArea(void);	// 为false时，在Caption区该控件区域不响应客户区消息

private:
	CDuiString m_strName;
};

#endif // DirectUIObject_h__
