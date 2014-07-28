#include "StdAfx.h"
#include "UICommandHistory.h"
#include "LayoutManager.h"
#include "UIUtil.h"

//////////////////////////////////////////////////////////////////////////
//CUICommandElement

CUICommandElement::CUICommandElement()
	: m_pElementXml(NULL)
{
}

CUICommandElement::CUICommandElement(CArray<CControlUI*,CControlUI*>& arrSelected, BOOL bModify)
	: m_pElementXml(NULL)
{
	TiXmlNode* pNode;
	m_pElementXml = new TiXmlElement("UIHistory");
	for(int i=0; i<arrSelected.GetSize(); i++)
	{
		CLayoutManager::SaveProperties(arrSelected[i], m_pElementXml, !bModify);

		if(i == 0)
			pNode = m_pElementXml->FirstChild();
		else
			pNode = pNode->NextSibling();

		CControlUI* pControl = arrSelected[i];
		ASSERT(pNode && pControl);
		pNode->ToElement()->SetAttribute("myname", StringConvertor::WideToUtf8(pControl->GetName()));

		CControlUI* pParent = arrSelected[i]->GetParent();
		ASSERT(pNode && pParent);
		pNode->ToElement()->SetAttribute("parentname", StringConvertor::WideToUtf8(pParent->GetName()));
	}
}

CUICommandElement::CUICommandElement(const CUICommandElement& copy)
{
	m_pElementXml = new TiXmlElement(*copy.m_pElementXml);
}

CUICommandElement::~CUICommandElement()
{
	if(m_pElementXml)
		delete m_pElementXml;
}

//////////////////////////////////////////////////////////////////////////
//CUICommandNode

CUICommandNode::CUICommandNode(ActionType type)
	: m_ActionType(type), m_pBefore(NULL), m_pAfter(NULL), m_pAllSelected(NULL)
	, m_pControl(NULL)
{

}

CUICommandNode::CUICommandNode(CUICommandElement* pBefore, CUICommandElement* pAfter, ActionType type)
	: m_ActionType(type), m_pAllSelected(NULL), m_pControl(NULL)
{
	m_pBefore = pBefore ? new CUICommandElement(*pBefore) : NULL;
	m_pAfter = pAfter ? new CUICommandElement(*pAfter) : NULL;

	switch(type)
	{
	case actionAdd:
		break;
	case actionModify:
		RemoveSameProperties(m_pBefore->m_pElementXml, m_pAfter->m_pElementXml);
		break;
	case actionDelete:
		break;
	}
}

CUICommandNode::~CUICommandNode()
{
	if(m_pBefore)
		delete m_pBefore;
	if(m_pAfter)
		delete m_pAfter;
}

void CUICommandNode::Begin(CArray<CControlUI*,CControlUI*>& arrSelected)
{
	ASSERT(m_pBefore == NULL && m_pAfter == NULL);
	ASSERT(m_pAllSelected == NULL);

	m_pAllSelected = &arrSelected;
	switch(m_ActionType)
	{
	case actionAdd:
		m_pBefore = NULL;
		break;
	case actionModify:
		m_pBefore = new CUICommandElement(arrSelected, TRUE);
		break;
	case actionDelete:
		m_pBefore = new CUICommandElement(arrSelected, FALSE);
		break;
	}
}

void CUICommandNode::Begin(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue)
{
	ASSERT(m_pBefore == NULL && m_pAfter == NULL);
	ASSERT(m_pControl == NULL);

	m_pControl = pControl;
	m_pBefore = new CUICommandElement();
	TiXmlElement* pElement = new TiXmlElement("UIHistory");
	CString strClass = m_pControl->GetClass();
	strClass = strClass.Mid(0, strClass.GetLength() - 2);
	TiXmlElement* pNode = new TiXmlElement(StringConvertor::WideToUtf8(strClass.GetBuffer()));
	pNode->SetAttribute("myname", StringConvertor::WideToUtf8(m_pControl->GetName()));
	pNode->SetAttribute(StringConvertor::WideToUtf8(pstrName), StringConvertor::WideToUtf8(pstrValue));
	pElement->InsertEndChild(*pNode);
	m_pBefore->m_pElementXml = pElement;

	delete pNode;
}

void CUICommandNode::End()
{
	ASSERT(m_pAfter == NULL);
	ASSERT(m_pAllSelected);

	switch(m_ActionType)
	{
	case actionAdd:
		m_pAfter = new CUICommandElement(*m_pAllSelected, FALSE);
		break;
	case actionModify:
		m_pAfter = new CUICommandElement(*m_pAllSelected, TRUE);
		RemoveSameProperties(m_pBefore->m_pElementXml, m_pAfter->m_pElementXml);
		break;
	case actionDelete:
		m_pAfter = NULL;
		break;
	}

	m_pAllSelected = NULL;
}

void CUICommandNode::End(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue)
{
	ASSERT(m_pAfter == NULL);
	ASSERT(m_pControl);
	if(m_pControl != pControl)
		return;

	m_pAfter = new CUICommandElement();
	TiXmlElement* pElement = new TiXmlElement("UIHistory");
	CString strClass = m_pControl->GetClass();
	strClass = strClass.Mid(0, strClass.GetLength() - 2);
	TiXmlElement* pNode = new TiXmlElement(StringConvertor::WideToUtf8(strClass.GetBuffer()));
	pNode->SetAttribute("myname", StringConvertor::WideToUtf8(m_pControl->GetName()));
	pNode->SetAttribute(StringConvertor::WideToUtf8(pstrName), StringConvertor::WideToUtf8(pstrValue));
	pElement->InsertEndChild(*pNode);
	m_pAfter->m_pElementXml = pElement;

	delete pNode;
	m_pControl = NULL;
}

BOOL CUICommandNode::RemoveSameProperties(TiXmlNode* pBeforeElem, TiXmlNode* pAfterElem)
{
	ASSERT(pBeforeElem && pAfterElem);
	if(m_ActionType != actionModify)
		return FALSE;

	TiXmlNode* pBeforeNode = pBeforeElem->FirstChild();
	TiXmlNode* pAfterNode = pAfterElem->FirstChild();
	while(pBeforeNode)
	{
		int nType = pBeforeNode->Type();
		switch(nType)
		{
		case TiXmlNode::ELEMENT:
			RemoveSameProperty(pBeforeNode, pAfterNode);
			break;
		case TiXmlNode::TEXT:
			break;
		}

		pBeforeNode = pBeforeNode->NextSibling();
		pAfterNode = pAfterNode->NextSibling();
	}

	return TRUE;
}

void CUICommandNode::RemoveSameProperty(TiXmlNode* pBeforeElem, TiXmlNode* pAfterElem)
{
	TiXmlAttribute* pBeforeAttrib = pBeforeElem->ToElement()->FirstAttribute();
	ASSERT(pBeforeAttrib);
	TiXmlAttribute* pAfterAttrib = pAfterElem->ToElement()->FirstAttribute();
	ASSERT(pAfterAttrib);

	while(pBeforeAttrib)
	{
		TiXmlAttribute* pBeforeAttribNext = pBeforeAttrib->Next();
		ASSERT(pBeforeAttribNext);
		if (pBeforeAttribNext)
			return;
		TiXmlAttribute* pAfterAttribNext = pAfterAttrib->Next();
		ASSERT(pAfterAttribNext);
		if(strcmp(pBeforeAttrib->Name(), "myname") == 0)
		{
			CStringA strBeforeName = pBeforeAttrib->Value();
			CStringA strAfterName = pAfterAttrib->Value();
			if(strBeforeName != strAfterName)
			{
				pBeforeAttrib->SetValue(strAfterName);
				pAfterAttrib->SetValue(strBeforeName);
			}
		}
		else if(strcmp(pBeforeAttrib->Value(), pAfterAttrib->Value()) == 0)
		{
			pBeforeElem->ToElement()->RemoveAttribute(pBeforeAttrib->Name());
			pAfterElem->ToElement()->RemoveAttribute(pAfterAttrib->Name());
		}
		pBeforeAttrib = pBeforeAttribNext;
		pAfterAttrib = pAfterAttribNext;
	}
}

//////////////////////////////////////////////////////////////////////////
//CUICommandHistory

CUICommandHistory::CUICommandHistory(void)
	: m_pNode(NULL), m_nCommandIndex(0)
{
}

CUICommandHistory::~CUICommandHistory(void)
{
	while(!m_lstCommandNodes.IsEmpty())
		delete m_lstCommandNodes.RemoveHead();
}

void CUICommandHistory::Begin(CArray<CControlUI*,CControlUI*>& arrSelected, ActionType type)
{
	ASSERT(m_pNode == NULL);

	m_pNode = new CUICommandNode(type);
	m_pNode->Begin(arrSelected);
}

void CUICommandHistory::Begin(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue)
{
	ASSERT(m_pNode == NULL);

	m_pNode = new CUICommandNode(actionModify);
	m_pNode->Begin(pControl, pstrName, pstrValue);
}

void CUICommandHistory::End()
{
	ASSERT(m_pNode);

	m_pNode->End();
	AddUICommand(m_pNode);
	m_pNode = NULL;
}

void CUICommandHistory::End(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue)
{
	ASSERT(m_pNode);

	m_pNode->End(pControl, pstrName, pstrValue);
	AddUICommand(m_pNode);
	m_pNode = NULL;
}

BOOL CUICommandHistory::AddUICommand(CUICommandNode* pNode)
{
	switch(pNode->m_ActionType)
	{
	case actionAdd:
		if(pNode->m_pAfter == NULL)
			return FALSE;
		break;
	case actionModify:
		if(pNode->m_pBefore==NULL || pNode->m_pAfter==NULL)
			return FALSE;
		break;
	case actionDelete:
		if(pNode->m_pBefore == NULL)
			return FALSE;
		break;
	}

	int count = m_lstCommandNodes.GetCount();
	while(m_nCommandIndex < count--)
		delete m_lstCommandNodes.RemoveTail();
	if(m_lstCommandNodes.GetCount() >= UI_COMMAND_HISTORY)
		delete m_lstCommandNodes.RemoveHead();
	m_lstCommandNodes.AddTail(pNode);
	m_nCommandIndex = m_lstCommandNodes.GetCount();

	return TRUE;
}

void CUICommandHistory::Undo()
{
	UICommandAction(cmdUndo);
}

void CUICommandHistory::Redo()
{
	UICommandAction(cmdRedo);
}

void CUICommandHistory::UICommandAction(CommandType type)
{
	CUICommandNode* pOldNode;
	CUICommandNode* pNewNode;

	if(type == cmdRedo)
	{
		if(!CanRedo())
			return;

		pOldNode = m_lstCommandNodes.GetAt(m_lstCommandNodes.FindIndex(m_nCommandIndex));
		pNewNode = new CUICommandNode(pOldNode->m_pBefore, pOldNode->m_pAfter, pOldNode->m_ActionType);
		m_nCommandIndex++;
	}
	else
	{
		if(!CanUndo())
			return;

		m_nCommandIndex--;
		pOldNode = m_lstCommandNodes.GetAt(m_lstCommandNodes.FindIndex(m_nCommandIndex));
		ActionType action;
		switch(pOldNode->m_ActionType)
		{
		case actionAdd:
			action = actionDelete;
			break;
		case actionModify:
			action = actionModify;
			break;
		case actionDelete:
			action = actionAdd;
			break;
		}
		pNewNode = new CUICommandNode(pOldNode->m_pAfter, pOldNode->m_pBefore, action);
	}

	switch(pNewNode->m_ActionType)
	{
	case actionAdd:
		ActionAdd(pNewNode->m_pAfter);
		break;
	case actionModify:
		ActionModify(pNewNode->m_pAfter);
		break;
	case actionDelete:
		ActionDelete(pNewNode->m_pBefore);
		break;
	}

	delete pNewNode;
}

BOOL CUICommandHistory::CanUndo()
{
	return m_nCommandIndex != 0;
}

BOOL CUICommandHistory::CanRedo()
{
	return m_nCommandIndex != m_lstCommandNodes.GetCount();
}

void CUICommandHistory::ActionAdd(CUICommandElement* pAfter)
{
	g_pMainFrame->GetActiveUIView()->ClearUITracker();
	SetUIAction(pAfter->m_pElementXml, UIAdd);
}

void CUICommandHistory::ActionModify(CUICommandElement* pAfter)
{
	SetUIAction(pAfter->m_pElementXml, UIModify);
}

void CUICommandHistory::ActionDelete(CUICommandElement* pBefore)
{
	SetUIAction(pBefore->m_pElementXml, UIDelete);
}

void CUICommandHistory::SetUIAction(TiXmlNode* pElement, UIACTIONPROC Proc)
{
	ASSERT(pElement);

	TiXmlNode* pNode = pElement->FirstChild();
	while(pNode)
	{
		int nType = pNode->Type();
		switch(nType)
		{
		case TiXmlNode::ELEMENT:
			Proc(pNode);
			break;
		case TiXmlNode::TEXT:
			break;
		}
		pNode = pNode->NextSibling();
	}
}

void CALLBACK CUICommandHistory::UIAdd(TiXmlNode* pNode)
{
	TiXmlElement* pElement = pNode->ToElement();
	CStringA strParentName = pElement->Attribute("parentname");
	pElement->RemoveAttribute("parentname");
	if(strParentName.IsEmpty())
		return;

	CUIDesignerView* pUIView = g_pMainFrame->GetActiveUIView();
	CPaintManagerUI* pManager = pUIView->GetPaintManager();
	CControlUI* pParentControl = pManager->FindControl(StringConvertor::Utf8ToWide(strParentName));
	if(pParentControl == NULL)
		return;

	TiXmlDocument xmlDoc;
	TiXmlDeclaration Declaration("1.0","utf-8","yes");
	xmlDoc.InsertEndChild(Declaration);
	TiXmlElement* pRootElem = new TiXmlElement("UIAdd");
	pRootElem->InsertEndChild(*pElement);
	xmlDoc.InsertEndChild(*pRootElem);
	TiXmlPrinter printer;
	xmlDoc.Accept(&printer);
	delete pRootElem;

	CDialogBuilder builder;
	CControlUI* pRoot=builder.Create(StringConvertor::Utf8ToWide(printer.CStr()), (UINT)0, NULL, pManager);
 	if(pRoot)
		pUIView->RedoUI(pRoot, pParentControl);
}

void CALLBACK CUICommandHistory::UIModify(TiXmlNode* pNode)
{
	TiXmlElement* pElement = pNode->ToElement();
	CStringA strName = pElement->Attribute("myname");
	pElement->RemoveAttribute("myname");
	if(strName.IsEmpty())
		return;

	CPaintManagerUI* pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
	CControlUI* pControl = pManager->FindControl(StringConvertor::Utf8ToWide(strName));
	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	if(pControl == NULL)
		return;

	while(pAttrib)
	{
		if(strcmp(pAttrib->Name(), "name") == 0)
		{
			pManager->ReapObjects(pControl);
			g_pClassView->RenameUITreeItem(pControl, StringConvertor::Utf8ToWide(pAttrib->Value()));
			pControl->SetAttribute(StringConvertor::Utf8ToWide(pAttrib->Name())
				, StringConvertor::Utf8ToWide(pAttrib->Value()));
			pManager->InitControls(pControl);
		}
		else
			pControl->SetAttribute(StringConvertor::Utf8ToWide(pAttrib->Name())
				, StringConvertor::Utf8ToWide(pAttrib->Value()));

		pAttrib = pAttrib->Next();
	}
	CControlUI* pParent = pControl->GetParent();
	if(pParent == NULL)
		pParent = pControl;
	pParent->SetPos(pParent->GetPos());
}

void CALLBACK CUICommandHistory::UIDelete(TiXmlNode* pNode)
{
	TiXmlElement* pElement = pNode->ToElement();
	CStringA strName = pElement->Attribute("myname");
	if(strName.IsEmpty())
		return;

	CUIDesignerView* pUIView = g_pMainFrame->GetActiveUIView();
	CPaintManagerUI* pManager = pUIView->GetPaintManager();
	CControlUI* pControl = pManager->FindControl(StringConvertor::Utf8ToWide(strName));
	if(pControl == NULL)
		return;
	CControlUI* pParent=pControl->GetParent();
	HTREEITEM hDelete=(HTREEITEM)(((ExtendedAttributes*)pControl->GetTag())->hItem);
	g_pClassView->RemoveUITreeItem(hDelete);
 	pUIView->DeleteUI(pControl);
	if(pParent)
		pParent->NeedUpdate();
}