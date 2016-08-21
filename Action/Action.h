#ifndef IAction_h__
#define IAction_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	IAction.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-15 11:59:04
// 说明			:	动画接口
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Core/ObjectUI.h"

namespace DuiLib
{
	class CAction;

class UILIB_API IActionInterval
{
public:
	//是否配置循环
	virtual bool  IsLoop() = 0;
	//是否配置反弹
	virtual bool  IsRebound() = 0;
	//是否逆序
	virtual bool  IsReverse() = 0;
	//是否结束
	virtual bool  IsDone() = 0;
	// 是否运行
	virtual bool IsRunning() =0;
	//每帧间隔
	virtual DWORD GetInterval() = 0;
	//当前帧
	virtual DWORD GetCurFrame() = 0;
	//总帧数
	virtual DWORD GetTotalFrame() = 0;
	// 设置当前帧
	virtual void SetCurFrame(DWORD dwCurFrame,bool bReverse) =0;
	// 设置参数
	virtual void SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop, bool bRebound , bool bByStep ) =0;
};

class CActionInterval;

class UILIB_API IActionListener
{
public:
	virtual void OnActionStep(CAction* pAction, IActionInterval* pActionInterval){};
	virtual void OnActionStateChanged(CAction* pAction){}
};

class UILIB_API CAction
	: public CObjectUI
{
	friend class CActionManager;
public:
	typedef enum
	{
		ActionState_Ready = 0,		//准备状态
		ActionState_Running,			//运行中
		ActionState_Paused,			//暂停
		ActionState_Stop,				//停止（未结束时被析构）
		ActionState_Finished,			//结束状态（会执行析构）
		ActionState_Unknown = -1,
	}ActionState;

public:
	CAction();
	virtual ~CAction();

	void Paused();
	void Resume();

	/*!
	   \brief    获取状态 
	   \note     const 
	   \return   CAction::ActionState 
	 ************************************/
	CAction::ActionState GetState() const;

	/*!
	   \brief    设置监听
	   \param    IActionListener * val 
	   \return   void 
	 ************************************/
	void SetActionListener(IActionListener* pActionListener);

	/*!
	   \brief    动画执行目标 
	   \note     const 
	   \return   BaseObject* 
	 ************************************/
	CObjectUI* ActionTarget() const;

	/*!
	   \brief    设置动画曲线ID
	   \param    LPCTSTR val 
	   \note     
			   curve.liner 
			   curve.quad.in 
			   curve.quad.out 
			   curve.quad.inout 
			   curve.cubic.in 
			   curve.cubic.out 
			   curve.cubic.inout 
			   curve.quart.in 
			   curve.quart.out 
			   curve.quart.inout 
			   curve.quint.in 
			   curve.quint.out 
			   curve.quint.inout 
			   curve.sine.in 
			   curve.sine.out 
			   curve.sine.inout 
			   curve.expo.in 
			   curve.expo.out 
			   curve.expo.inout 
			   curve.circ.in 
			   curve.circ.out 
			   curve.circ.inout 
			   curve.back.in 
			   curve.back.out 
			   curve.back.inout 
	   \return   void 
	 ************************************/
	void SetCurveID(LPCTSTR lpszID);

	/*!
	   \brief    获取动画曲线类
	   \return   CurveObj* 
	 ************************************/
	CCurveObject* GetCurveObj();

	/*!
	   \brief    设置动画参数
	   \param    UINT uInterval 间隔毫秒时间
	   \param    UINT uTotalFrame 总帧数
	   \param    bool bLoop 是否循环
	   \param    bool bRebound 是否反弹
	   \param    bool bByStep true:步进;false:实际时间
	   \return   void 
	 ************************************/
	void SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop = false, bool bRebound = false, bool bByStep = true);

	/*!
	   \brief    设置当前帧
	   \param    DWORD dwCurFrame 
	   \param    bool bReverse 逆序
	   \return   void 
	 ************************************/
	void SetCurFrame(DWORD dwCurFrame, bool bReverse = false);


	DWORD GetCurFrame();
	DWORD GetTotalFrame();

	bool IsLoop() const;

	bool IsReverse() const;

	void FinishedOfRelease(bool val);

	virtual LPCTSTR GetClass() = 0;

protected:

	virtual void startWithTarget(CObjectUI* pActionTarget);

	//非正常结束，恢复初态
	virtual void onStop();

	virtual void onFinished();

	void SetState(CAction::ActionState state);

	void ActionTarget(CObjectUI* pTarget);

	// 动画帧
	virtual void Tick(IActionInterval* pActionInterval);

	/// 计数
	CActionInterval* m_pActionInterval;
private:
	friend class CActionInterval;

	virtual void Update(IActionInterval* pActionInterval);

	/// 状态
	ActionState m_stateAction;
	/// 动画曲线
	CCurveObject* m_pCurveObj;
	/// 动画目标
	CObjectUI* m_pActionTarget;
	/// 监听类
	IActionListener* m_pActionListener;

	bool m_bFinishedOfRelease;
};

}
#endif // IAction_h__