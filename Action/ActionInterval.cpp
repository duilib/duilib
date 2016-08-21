#include "stdafx.h"
#include "ActionInterval.h"

namespace DuiLib
{

CActionInterval::CActionInterval(void)
	: m_dwLastTickCount(0)
	, m_dwInterval(0)
	, m_dwInnerInterval(0)
	, m_dwCurFrame(0)
	, m_dwTotalFrames(0)
	, m_pOwner(NULL)
	, m_bAsc(true)
	, m_bByStep(true)
	, m_bLoop(false)
	, m_bRebound(false)
	, m_bReverse(false)
	, m_bDone(false)
	, m_bPaused(false)
	, m_bRunning(false)
{
}

CActionInterval::~CActionInterval(void)
{
}

void CActionInterval::Start()
{
	Resume();
	m_bRunning = true;
	m_bDone = false;
	gs_arrActionInterval.Add(this);
	if ( gs_uActionIntervalID==0 )
	{
		HWND hWnd= GetUIEngine()->GetPumpWnd();
		UINT_PTR idEvent = (UINT_PTR)GetUIEngine();
		::SetTimer(hWnd,idEvent,10,CActionInterval::UpdateProc);
	}
}

void CActionInterval::Stop()
{
	m_bRunning = false;
	// 将自己从全局队列中删除
	gs_arrActionInterval.Remove(gs_arrActionInterval.Find(this));
	if ( gs_arrActionInterval.GetSize() == 0)
	{
		// 如果队列为空了，则关闭定时器回调
		if ( gs_uActionIntervalID != 0 )
		{
			::KillTimer(GetUIEngine()->GetPumpWnd(),gs_uActionIntervalID);
			gs_uActionIntervalID = 0;
		}
	}
}

void CActionInterval::Resume()
{
	m_bPaused = false;
	m_dwLastTickCount = ::GetTickCount();
}

void CActionInterval::Paused()
{
	m_bPaused = true;
}

void CActionInterval::SetOwner(CAction* pOwner)
{
	m_pOwner = pOwner;
}

void CActionInterval::CallInterval(DWORD dwTick)
{
	if ( m_bPaused)
		return;

	bool bUpdate=false;
	bool v5 = false;
	UINT nFrame =0;

	if ( m_bByStep )
	{
		m_dwInnerInterval+=10;
		if ( m_dwInnerInterval >= m_dwInterval)
		{
			bUpdate=true;
			m_dwInnerInterval =0;
			if ( m_bAsc)
			{
				++m_dwCurFrame;
			}
			else if ( m_dwCurFrame)
			{
				--m_dwCurFrame;
			}
			else
			{
				bUpdate = true;
				m_dwCurFrame =0;
			}
		}
	}
	else
	{
		nFrame = m_dwLastTickCount / m_dwInterval ;
		if ( nFrame >0)
		{
			bUpdate = true;
			if ( m_bAsc )
			{
				m_dwCurFrame += nFrame;
			}
			else if ( m_dwCurFrame)
			{
				m_dwCurFrame -= nFrame;
			}
			else
			{
				v5 = true;
				m_dwCurFrame = 0;
			}
			m_dwLastTickCount = dwTick;
		}
	}

	if ( bUpdate )
	{
		if ( m_dwCurFrame )
		{
			if ( m_dwCurFrame >= m_dwTotalFrames )
			{
				if ( m_bLoop )
				{
					if ( m_bRebound )
					{
						m_bAsc = 0;
						m_dwCurFrame = m_dwTotalFrames;
						m_bDone = 0;
					}
					else if ( m_bReverse )
					{
						m_bAsc = 1;
						m_dwCurFrame = m_dwTotalFrames;
						m_bDone = 1;
					}
					else if ( m_dwCurFrame > m_dwTotalFrames )
					{
						m_bAsc = 1;
						m_dwCurFrame = 0;
						m_bDone = 0;
					}
				}
				else
				{
					m_bAsc = 1;
					m_dwCurFrame = m_dwTotalFrames;
					m_bDone = 1;
				}
			}
		}
		else if ( m_bLoop )
		{
			if ( m_bRebound )
			{
				m_bAsc = 1;
				m_dwCurFrame = 0;
				m_bDone = 0;
			}
			else if ( m_bReverse )
			{
				if ( v5 )
				{
					m_bAsc = 0;
					if ( m_bByStep )
						m_dwCurFrame = m_dwTotalFrames;
					else
						m_dwCurFrame = m_dwTotalFrames - nFrame + 1;
					m_bDone = 0;
				}
			}
			else
			{
				m_bAsc = 0;
				m_dwCurFrame = 0;
				m_bDone = 1;
			}
		}
		else
		{
			m_bAsc = 0;
			m_dwCurFrame = 0;
			m_bDone = 1;
		}
		if ( IsDone() )
			Stop();
		if ( m_pOwner )
			m_pOwner->Update(this);
	}
}

void WINAPI CActionInterval::UpdateProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTimer)
{
	DWORD dwCurrent = ::GetTickCount();
	if ( dwCurrent - s_dwLastTickCount >=10)
	{
		s_dwLastTickCount = dwCurrent;
		int nCount = gs_arrActionInterval.GetSize();
		if ( nCount >0)
		{
			CActionInterval *pActionInterval = NULL;
			for ( int i=nCount-1;i>=0;--i)
			{
				pActionInterval = (CActionInterval*)gs_arrActionInterval.GetAt(i);
				pActionInterval->CallInterval(dwCurrent);
			}
		}
	}
}

int CActionInterval::FadeInterval(CObjectUI* pTarget,CCurveObject* pCurveObj,IActionInterval* pActionInterval,int nStartAlpha,int nEndAlpha)
{
	int nAlpha = nStartAlpha;
	if ( pActionInterval )
	{
		DWORD dwTotalFrame = pActionInterval->GetTotalFrame();
		DWORD dwCurFrame = pActionInterval->GetCurFrame();
		DWORD dwSplan = nEndAlpha - nStartAlpha;
		nAlpha = BezierAlgorithm::BezierValue(pCurveObj,dwCurFrame,nStartAlpha,dwSplan,dwTotalFrame);
	}
	if ( pTarget && pTarget->GetInterface(_T("Control")))
	{
		CControlUI* pView = static_cast<CControlUI*>(pTarget->GetInterface(_T("Control")));
		pView->SetAlpha(nAlpha);
		DUI__Trace(_T("FadeInterval:%d"),nAlpha);
	}
	return nAlpha;
}

RECT CActionInterval::PosInterval(CObjectUI* pView, CCurveObject* pCurveObj, IActionInterval* pInterval, RECT& rcStart, RECT& rcEnd)
{
	CDuiRect rcDest(rcStart);
	if ( pCurveObj)
	{
		DWORD dwTotalFrame = pInterval->GetTotalFrame();
		DWORD dwCurFrame = pInterval->GetCurFrame();
		rcDest = BezierAlgorithm::BezierRect(pCurveObj,dwCurFrame,rcStart,rcEnd,dwTotalFrame);
	}
	CControlUI* pTarget = static_cast<CControlUI*>(pView);
	if ( pTarget)
	{
		// TODO 这里的设置牵扯到动画矩形区的起始计算
		pTarget->SetFixedXY(CDuiSize(rcDest.left,rcDest.top));
		pTarget->SetFixedWidth(rcDest.GetWidth());
		pTarget->SetFixedHeight(rcDest.GetHeight());
		DUI__Trace(_T("PosInterval：%d.%d.%d.%d"),rcDest.left,rcDest.top,rcDest.GetWidth(),rcDest.GetHeight());
	}

	return rcDest;
}

bool CActionInterval::IsLoop()
{
	return m_bLoop;
}

bool CActionInterval::IsRebound()
{
	return m_bRebound;
}

bool CActionInterval::IsReverse()
{
	return m_bReverse;
}

bool CActionInterval::IsDone()
{
	return m_bDone;
}

bool CActionInterval::IsRunning()
{
	return m_bRunning;
}

DWORD CActionInterval::GetInterval()
{
	return m_dwInterval;
}

DWORD CActionInterval::GetCurFrame()
{
	return m_dwCurFrame;
}

DWORD CActionInterval::GetTotalFrame()
{
	return m_dwTotalFrames;
}

void CActionInterval::SetCurFrame(DWORD dwCurFrame,bool bReverse)
{
	m_dwCurFrame = dwCurFrame;
	m_bReverse = bReverse;
	m_bAsc = bReverse == false;
}

void CActionInterval::SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop, bool bRebound , bool bByStep)
{
	m_dwInnerInterval = 0;
	m_dwInterval = uInterval;
	m_dwTotalFrames = uTotalFrame;
	m_bLoop = bLoop;
	m_bRebound = bRebound;
	m_bByStep = bByStep;
}

UINT CActionInterval::gs_uActionIntervalID =0;

DuiLib::CStdPtrArray CActionInterval::gs_arrActionInterval;

DWORD CActionInterval::s_dwLastTickCount;

}