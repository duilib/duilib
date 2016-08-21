#include "stdafx.h"
#include "UIChildLayout.h"

namespace DuiLib
{
	UI_IMPLEMENT_DYNCREATE(CChildLayoutUI);

	CChildLayoutUI::CChildLayoutUI()
	{
	}

	void CChildLayoutUI::Init()
	{
		if (!m_strViewName.empty())
		{
			//CDialogBuilder builder;
			//CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(m_pstrXMLFile.c_str(), NULL,this,m_pManager));
			CControlUI* pChildWindow = CResEngine::GetInstance()->CreateViewWithTemplate(this,m_strViewName.c_str());
			if (pChildWindow!=NULL)
			{
				// ´´½¨ÊÓÍ¼Ê§°Ü
				this->Add(pChildWindow);
			}
			else
			{

			}
		}

		CContainerUI::Init();
	}

	void CChildLayoutUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else
			CContainerUI::SetAttribute(pstrName,pstrValue);
	}

	void CChildLayoutUI::SetChildLayoutXML( LPCTSTR pXML )
	{
		m_pstrXMLFile=pXML;
	}

	LPCTSTR CChildLayoutUI::GetChildLayoutXML()
	{
		return m_pstrXMLFile.c_str();
	}

	LPVOID CChildLayoutUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("ChildLayout")) == 0 )
			return static_cast<CChildLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	LPCTSTR CChildLayoutUI::GetClass() const
	{
		return _T("ChildLayoutUI");
	}

	void CChildLayoutUI::SetViewName(LPCTSTR lpszViewName)
	{
		m_strViewName = lpszViewName;
	}

	LPCTSTR CChildLayoutUI::GetViewName()
	{
		return m_strViewName.c_str();
	}

} // namespace DuiLib
