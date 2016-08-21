#ifndef UIIPAddress_h__
#define UIIPAddress_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	UIIPAddress.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-08 11:40:49
// 说明			:	IPAddress控件，类CEditUI包装系统IPAddress控件
/////////////////////////////////////////////////////////////*/
#pragma once

namespace DuiLib
{

class UILIB_API CIPAddressUI
	: public CLabelUI
{
	friend class CIPAddressWnd;
public:
	CIPAddressUI(void);
	virtual ~CIPAddressUI(void);
	UI_DECLARE_DYNCREATE();
	virtual bool IsClientArea();

	UINT GetControlFlags() const;

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void EventHandler(TEventUI& event);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

public:	// 标准Win32控件接口
	// Determines if all fields in the IP address control are blank.
	BOOL IsBlank() const;

	// Clears the contents of the IP address control.
	void ClearAddress();

	// Retrieves the address values for all four fields in the IP address control.
	int GetAddress(_Out_ BYTE& nField0, _Out_ BYTE& nField1, _Out_ BYTE& nField2, _Out_ BYTE& nField3) const;
	int GetAddress(_Out_ DWORD& dwAddress) const;

	// Sets the address values for all four fields in the IP address control.
	void SetAddress(_In_ DWORD dwAddress);
	void SetAddress(_In_ BYTE nField0, _In_ BYTE nField1, _In_ BYTE nField2, _In_ BYTE nField3);

	// Sets the keyboard focus to the specified field in the IP address control.
	void SetFieldFocus(_In_ WORD nField);

	// Sets the valid range for the specified field in the IP address control.
	void SetFieldRange(_In_ int nField, _In_ BYTE nLower, _In_ BYTE nUpper);

public:	// 辅助函数
	LPCTSTR GetIPAddress();

private:
	CIPAddressWnd* m_pWindow;
	CDuiString m_strIPAddress;
	UINT m_uButtonState;
	BYTE m_nField0;
	BYTE m_nField1;
	BYTE m_nField2;
	BYTE m_nField3;
};

}

#endif // UIIPAddress_h__
