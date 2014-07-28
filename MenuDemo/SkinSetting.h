#pragma once
#include "resource.h"

namespace DuiLib {

class CSkinSetting
{
public:
	CSkinSetting(LPCTSTR lpSkinFile=_T("101"),LPCTSTR lpSkinFolder=_T(""),UILIB_RESOURCETYPE iResType=UILIB_RESOURCE,LPCTSTR lpResID=MAKEINTRESOURCE(101),LPCTSTR lpZipFileName=_T("xx.zip"));
	~CSkinSetting(void);
public:
	CDuiString				m_SkinFile;
	CDuiString				m_SkinFolder;
	UILIB_RESOURCETYPE		m_ResourceType;
	LPCTSTR					m_ResourceID;
	CDuiString				m_ZIPFileName;
public:
	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual UILIB_RESOURCETYPE GetResourceType() const;

	virtual LPCTSTR GetResourceID() const;

	CDuiString GetZIPFileName() const;
};

} // namespace DuiLib
