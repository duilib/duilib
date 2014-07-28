#pragma once
#include "UIDesignerView.h"
#include "tinyxml.h"
#include "..\DuiLib\UIlib.h"
using DuiLib::CContainerUI;

enum CommandType
{
	cmdUndo,
	cmdRedo,
};
enum ActionType
{
	actionAdd,
	actionDelete,
	actionModify,
};

//////////////////////////////////////////////////////////////////////////
//CUICommandElement

class CUICommandNode;
class CUICommandHistory;

class CUICommandElement
{
	friend CUICommandNode;
	friend CUICommandHistory;

public:
	CUICommandElement();
	CUICommandElement(CArray<CControlUI*,CControlUI*>& arrSelected, BOOL bModify);
	CUICommandElement(const CUICommandElement& copy);

	~CUICommandElement();

protected:
	TiXmlElement* m_pElementXml;
};

//////////////////////////////////////////////////////////////////////////
//CUICommandNode

class CUICommandHistory;

class CUICommandNode
{
	friend CUICommandHistory;

public:
	CUICommandNode(ActionType type);
	CUICommandNode(CUICommandElement* pBefore, CUICommandElement* pAfter, ActionType type);

	~CUICommandNode();

public:
	void Begin(CArray<CControlUI*,CControlUI*>& arrSelected);
	void Begin(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue);
	void End();
	void End(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	BOOL RemoveSameProperties(TiXmlNode* pBeforeElem, TiXmlNode* pAfterElem);
	inline void RemoveSameProperty(TiXmlNode* pBeforeElem, TiXmlNode* pAfterElem);

protected:
	ActionType m_ActionType;
	CUICommandElement* m_pBefore;
	CUICommandElement* m_pAfter;
	CArray<CControlUI*,CControlUI*>* m_pAllSelected;
	CControlUI* m_pControl;
};

//////////////////////////////////////////////////////////////////////////
//CUICommandHistory

typedef void (CALLBACK* UIACTIONPROC)(TiXmlNode*);

class CUICommandHistory
{
public:
	CUICommandHistory(void);
	~CUICommandHistory(void);

public:
	void Begin(CArray<CControlUI*,CControlUI*>& arrSelected, ActionType type);
	void Begin(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue);
	void End();
	void End(CControlUI* pControl, LPCTSTR pstrName, LPCTSTR pstrValue);

	void Undo();
	void Redo();
	BOOL CanUndo();
	BOOL CanRedo();

protected:
	BOOL AddUICommand(CUICommandNode* pNode);
	void UICommandAction(CommandType type);

	void ActionAdd(CUICommandElement* pAfter);
	void ActionModify(CUICommandElement* pAfter);
	void ActionDelete(CUICommandElement* pBefore);
	void SetUIAction(TiXmlNode* pElement, UIACTIONPROC Proc);
	
private:
	static void CALLBACK UIAdd(TiXmlNode* pNode);
	static void CALLBACK UIModify(TiXmlNode* pNode);
	static void CALLBACK UIDelete(TiXmlNode* pNode);

private:
	CUICommandNode* m_pNode;
	int m_nCommandIndex;
	CList<CUICommandNode*, CUICommandNode*> m_lstCommandNodes;
};