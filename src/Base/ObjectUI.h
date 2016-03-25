#ifndef DirectUIObject_h__
#define DirectUIObject_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	DirectUIObject.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-08 15:27:41
// ˵��		:	Root Object
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
	virtual bool IsClientArea(void);	// Ϊfalseʱ����Caption���ÿؼ�������Ӧ�ͻ�����Ϣ

private:
	CDuiString m_strName;
};

#endif // DirectUIObject_h__
