#include "stdafx.h"
#include "ChildLayoutUI.h"
#include "Base/TemplateObject.h"
UI_IMPLEMENT_DYNCREATE(CChildLayoutUI);

CChildLayoutUI::CChildLayoutUI(void)
	: m_pRootControl(NULL)
{
}

CChildLayoutUI::~CChildLayoutUI(void)
{
}

LPCTSTR CChildLayoutUI::GetClass() const
{
	return _T("ChildLayout");
}

LPVOID CChildLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ChildLayout")) == 0 )
		return static_cast<CChildLayoutUI*>(this);
	return
		CContainerUI::GetInterface(pstrName);
}

bool CChildLayoutUI::Notify(TNotifyUI *pMsg)
{
	INotifyUI *pNotifyFilter = NULL;
	int iCount = m_arrayNotifyFilters.GetSize();
	for ( int i=0; i<iCount;++i)
	{
		pNotifyFilter = static_cast<INotifyUI*>(m_arrayNotifyFilters.GetAt(i));
		if ( pNotifyFilter && pNotifyFilter->Notify(pMsg))
			return true;
	}
	return false;
}

void CChildLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("xmlfile")) == 0 )
		SetChildLayoutXML(lpszValue);
	else
		CContainerUI::SetAttribute(lpszName,lpszValue);
}

void CChildLayoutUI::SetChildLayoutXML(LPCTSTR lpFilePath)
{
	m_strXmlPath = lpFilePath;
}

LPCTSTR CChildLayoutUI::GetChildLayoutXML()
{
	return m_strXmlPath.c_str();
}

void CChildLayoutUI::AddNotify(INotifyUI *pNotify)
{
	if ( m_arrayNotifyFilters.Find(pNotify) == -1)
	{
		m_arrayNotifyFilters.Add(pNotify);
	}
}

void CChildLayoutUI::RemoveNotify(INotifyUI *pNotify)
{
	int nIndex = m_arrayNotifyFilters.Find(pNotify);
	if ( nIndex != -1 )
	{
		m_arrayNotifyFilters.Remove(nIndex);
	}
}

void CChildLayoutUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	if ( m_strXmlPath.empty())
	{
		CContainerUI::SetManager(pManager,pParent);
		return;
	}

	TemplateObject* pLayoutTemplate;
	CResourceManager * pResourceManager = CResourceManager::GetInstance();
	// 布局属性名以#开头的，说明是布局ID，否则是xml的相对/绝对路径
	size_t nIndex = m_strXmlPath.find(_T("#"));
	if ( nIndex != m_strXmlPath.npos )
	{
		// 是布局模板ID
		CDuiString strLayoutName = m_strXmlPath.substr(nIndex+1);
		pLayoutTemplate = pResourceManager->GetWindowTemplate(strLayoutName.c_str());
		if ( pLayoutTemplate != NULL )
		{
			AttributeMap::iterator iter = pLayoutTemplate->m_mapAttribute.begin();
			AttributeMap::iterator end= pLayoutTemplate->m_mapAttribute.end();
			while (iter != end )
			{
				this->SetAttribute(iter->first.c_str(),iter->second.c_str());
				++iter;
			}

			TemplateObject* pControl = pLayoutTemplate->GetChild(0);
			m_pRootControl = pResourceManager->CreateControlFromTemplate(pControl,pManager,pParent);
		}
	}
	else
	{
		// 是相对或者绝对路径
#pragma TODO(子页面相对/绝对路径方式未实现)
	}


}

