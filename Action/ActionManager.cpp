#include "stdafx.h"
#include "ActionManager.h"
#include <set>

namespace DuiLib
{

CActionManager::CActionManager(void)
{
}

CActionManager::~CActionManager(void)
{
}

CActionManager* CActionManager::s_pInstance =NULL;

CActionManager* CActionManager::GetInstance()
{
	if ( s_pInstance == NULL)
	{
		s_pInstance = new CActionManager;
	}
	return s_pInstance;
}

void CActionManager::ReleaseInstance()
{
	if ( s_pInstance != NULL)
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}

void CActionManager::AddAction(CAction* pAction, CControlUI* pView, bool bStopOther /*= true*/)
{
	if ( bStopOther )
		RemoveActionByTarget(pView,true);
	AddActionPair(pAction,pView);
	if ( pView && pAction)
	{
		pAction->startWithTarget(pView);
	}
}

void CActionManager::AddAction(CAction* pAction, CWindowUI* pWindow, bool bStopOther /*= true*/)
{
	if ( bStopOther )
		RemoveActionByTarget(pWindow,true);
	AddActionPair(pAction,pWindow);
}

void CActionManager::AddActionPair(CAction* pAction, CObjectUI* pTarget)
{
	if ( pAction == NULL || pTarget ==NULL )
		return ;

	bool  bFindTarget = 0;
	CAction*  pFindAction = NULL;
	TargetToActionElementMap::iterator pActionElement = g_mapTargetToActionElement.find(pTarget);
	TargetToActionElementMap::iterator end = g_mapTargetToActionElement.end();
	if ( pActionElement != end )
	{
		while (pActionElement!= end)
		{
			pFindAction = pActionElement->second;
			if ( pFindAction  == pTarget)
			{
				bFindTarget = true;
				break;
			}
			++pActionElement;
		}
	}

	if ( !bFindTarget )
	{
		g_mapTargetToActionElement.insert(TargetToActionElementMap::value_type(pTarget,pAction));
	}
}

void CActionManager::RemoveAction(CAction* pAction)
{

}

void CActionManager::RemoveActionByTarget(CObjectUI* pTarget, bool bStop /*= false*/)
{
	if ( pTarget && g_mapTargetToActionElement.size() != 0 )
	{
		CAction* pAction = NULL;
		TargetToActionElementMap::iterator pActionElement = g_mapTargetToActionElement.find(pTarget);
		TargetToActionElementMap::iterator end = g_mapTargetToActionElement.end();
		while (pActionElement != end)
		{
			pAction = pActionElement->second;
			if ( pAction)
			{
				if ( bStop && pAction->GetState() != CAction::ActionState_Finished && pAction->GetState() != CAction::ActionState_Stop)
				{
					pAction->SetState(CAction::ActionState_Stop);
				}
			}
			++pActionElement;
		}
	}
}

void CActionManager::RemoveAllActions()
{
	if ( g_mapTargetToActionElement.size() != 0 )
	{
		CAction* pAction = NULL;
		std::set<CAction*> setActions;

		{
			ActionToObjectElementMap::iterator iter = g_mapActionToTargetElement.begin();
			ActionToObjectElementMap::iterator end = g_mapActionToTargetElement.end();
			while (iter != end)
			{
				pAction = iter->first;
				setActions.insert(pAction);
				++iter;
			}
		}

		g_mapActionToTargetElement.clear();
		g_mapTargetToActionElement.clear();

		{
			std::set<CAction*>::iterator iter = setActions.begin();
			std::set<CAction*>::iterator end = setActions.end();
			while (iter != end )
			{
				pAction = *iter;
				if ( pAction)
				{
					delete pAction;
				}
				++iter;
			}
		}
		setActions.clear();
	}
}

bool CActionManager::IsAction(CAction* pAction)
{
	return false;
}

}
