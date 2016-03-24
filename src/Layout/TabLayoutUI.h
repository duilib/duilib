#ifndef TabLayoutUI_h__
#define TabLayoutUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	TabLayoutUI.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-26 16:49:51
// ˵��			:	
/////////////////////////////////////////////////////////////*/
#pragma once
class CTabLayoutUI
	: public CContainerUI
{
public:
	CTabLayoutUI(void);
	virtual ~CTabLayoutUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	int GetCurSel() const;
	bool SelectItem(int iIndex);
	bool SelectItem(CControlUI* pControl);

	virtual bool Add(CControlUI* pControl);
	virtual bool AddAt(CControlUI* pControl, int iIndex);
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll();

	virtual void SetPosition(LPCRECT rc);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

protected:
	int m_iCurSel;
};

#endif // TabLayoutUI_h__
