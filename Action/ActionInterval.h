#ifndef TActionInterval_h__
#define TActionInterval_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	TActionInterval.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-15 14:14:17
// 说明			:	定时器回调处理
/////////////////////////////////////////////////////////////*/
#pragma once

namespace DuiLib
{

class CActionInterval
	: public IActionInterval
{
public:
	CActionInterval(void);
	~CActionInterval(void);

	void Start();
	void Stop();
	void Resume();
	void Paused();

	void SetOwner(CAction* pOwner);
	
	void CallInterval(DWORD dwTick);

public:
	static void WINAPI UpdateProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer);
	static int FadeInterval(CObjectUI* pTarget,CCurveObject* pCurveObj,IActionInterval* pActionInterval,int nStartAlpha,int nEndAlpha);
	static RECT PosInterval(CObjectUI* pView, CCurveObject* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd);

public:	// IActionInterval
	virtual bool IsLoop();
	virtual bool IsRebound();
	virtual bool IsReverse();
	virtual bool IsDone();
	virtual bool IsRunning();

	virtual DWORD GetInterval();
	virtual DWORD GetCurFrame();
	virtual DWORD GetTotalFrame();

	virtual void SetCurFrame(DWORD dwCurFrame,bool bReverse);
	virtual void SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop, bool bRebound , bool bByStep );

private:
	static UINT gs_uActionIntervalID;
	static CStdPtrArray gs_arrActionInterval;
	static DWORD s_dwLastTickCount;

	DWORD m_dwLastTickCount;
	DWORD m_dwInterval;
	DWORD m_dwCurFrame;
	DWORD m_dwTotalFrames;
	DWORD m_dwInnerInterval;

	bool m_bByStep;
	bool m_bAsc;
	bool m_bRunning;
	bool m_bPaused;
	bool m_bDone;
	bool m_bRebound;
	bool m_bReverse;
	bool m_bLoop;

	CAction* m_pOwner;
};

}


#endif // TActionInterval_h__
