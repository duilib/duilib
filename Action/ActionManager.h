#ifndef ActionManager_h__
#define ActionManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ActionManager.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-15 11:50:58
// 说明			:	动画管理器
/////////////////////////////////////////////////////////////*/
#pragma once

namespace DuiLib
{

class UILIB_API CActionManager
{
private:
	CActionManager(void);
	virtual ~CActionManager(void);
	static CActionManager* s_pInstance;

public:
	static CActionManager* GetInstance();
	static void ReleaseInstance();

	void AddAction(CAction* pAction, CControlUI* pView, bool bStopOther = true);
	void AddAction(CAction* pAction, CWindowUI* pWindow, bool bStopOther = true);
	void AddActionPair(CAction* pAction, CObjectUI* pTarget);

	void RemoveAction(CAction* pAction);
	void RemoveActionByTarget(CObjectUI* pTarget, bool bStop = false);
	void RemoveAllActions();

	bool IsAction(CAction* pAction);

private:
	typedef std::multimap<CObjectUI*,CAction*> TargetToActionElementMap;
	typedef std::multimap<CAction*,CObjectUI*> ActionToObjectElementMap;
	TargetToActionElementMap g_mapTargetToActionElement;
	ActionToObjectElementMap g_mapActionToTargetElement;
};

}

#endif // ActionManager_h__
