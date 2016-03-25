#ifndef TemplateObject_h__
#define TemplateObject_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	TemplateObject.h
// ������		: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-12 11:30:32
// ˵��			:	��ֵ��ģ��
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

	void InsertChild(TemplateObject* pChild,int nAtIndex=-1);	// nAtIndexΪ-1��ӵ���β

	AttributeMap m_mapAttribute;	// Xml��Attribute
private:
	CDuiString m_strType;					// �ؼ����ͣ�����XML��ǩ��
	CStdPtrArray m_arrayChild;			// �ӿؼ�������� TemplateObject*
};

#endif // TemplateObject_h__
