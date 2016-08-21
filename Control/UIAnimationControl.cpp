#include "stdafx.h"
#include "UIAnimationControl.h"

namespace DuiLib
{
	UI_IMPLEMENT_DYNCREATE(CAnimationControlUI);
CAnimationControlUI::CAnimationControlUI(void)
	: m_nFrameCount(0)
	, m_bVerticalMode(false)
	, m_nCurrentFrame(-1)
	, m_dwInterval(150)
	, m_pAnimationImage(NULL)
	, m_pImageSeqAnimation(NULL)
{
	m_pImageSeqAnimation = new ImageSequenceAnimation;
}

CAnimationControlUI::~CAnimationControlUI(void)
{
	delete m_pImageSeqAnimation;
	m_pImageSeqAnimation = NULL;
}

LPCTSTR CAnimationControlUI::GetClass() const
{
	return _T("AnimationControl");
}

LPVOID CAnimationControlUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("AnimationControl")) == 0 )
		return static_cast<CAnimationControlUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CAnimationControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if ( _tcsicmp(pstrName,_T("framecount")) ==0 )
	{
		m_nFrameCount = CDuiCodeOperation::StringToInt(pstrValue);
	}
	else if ( _tcsicmp(pstrName,_T("bkimage")) == 0 )
	{
		m_sBkImage = pstrValue;
		//m_pAnimationImage = CUIEngine::GetInstance()->AddImage(pstrValue);
	}
	else if ( _tcsicmp(pstrName,_T("interval")) == 0 )
	{
		m_dwInterval =  CDuiCodeOperation::StringToInt(pstrValue);
	}
	else
		CControlUI::SetAttribute(pstrName,pstrValue);
}

void CAnimationControlUI::EventHandler(TEventUI& event)
{
	if(event.dwType == UIEVENT_TIMER && event.wParam == AnimationControl_TimerID)
	{
		m_nCurrentFrame++;

		if(m_nCurrentFrame == m_nFrameCount)
		{
			m_nCurrentFrame = 0;
		}

		this->Invalidate();
	}
	else
	{
		CControlUI::EventHandler(event);
	}
}

void CAnimationControlUI::PaintBkImage(HDC hDC)
{
	// 根据当前帧和图片排布模式计算当前帧的矩形坐标，然后输出到hDC
	RECT rc = {0};

	if( m_bVerticalMode )
	{	// 图片时垂直拼接的
		rc.right = m_pAnimationImage->nX;

		rc.top = m_pAnimationImage->nY / m_nFrameCount * m_nCurrentFrame;
		rc.bottom = m_pAnimationImage->nY / m_nFrameCount * (m_nCurrentFrame+1);
	}
	else
	{	// 图片是水平拼接的
		rc.bottom = m_pAnimationImage->nY;
		if ( m_nCurrentFrame == 0)
			rc.left = 0;
		else
			rc.left = m_pAnimationImage->nX / m_nFrameCount * m_nCurrentFrame;
		rc.right = m_pAnimationImage->nX / m_nFrameCount * (m_nCurrentFrame + 1 );
	}

	if( !m_sBkImage.empty() ) 
	{
		CDuiString m_sImageModify;
		m_sImageModify = CDuiStringOperation::format(_T("source='%d,%d,%d,%d'"), rc.left, rc.top, rc.right, rc.bottom);

		if( !DrawImage(hDC, (LPCTSTR)m_sBkImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) )
			m_sBkImage.clear();
		else 
			return;
	}
}

void CAnimationControlUI::SetVisible(bool bVisible /*= true */)
{
	if(m_bVisible == bVisible)
	{
		return;
	}

	m_bVisible = bVisible;

	if(m_bVisible == false)
	{
		this->Stop();
	}
	else
	{
		this->Start();
	}
}

void CAnimationControlUI::SetInternVisible(bool bVisible /* = true */)
{
	if(m_bInternVisible == bVisible)
	{
		return;
	}

	m_bInternVisible = bVisible;

	if(m_bInternVisible == false)
	{
		this->Stop();
	}
	else
	{
		this->Start();
	}
}

void CAnimationControlUI::Start()
{
	m_pImageSeqAnimation->SetParam(m_dwInterval,m_nFrameCount,true);
	m_pImageSeqAnimation->Resume();
	//m_pManager->SetTimer(this, AnimationControl_TimerID,m_dwInterval);
}

void CAnimationControlUI::Stop()
{
	m_pImageSeqAnimation->Paused();
	//m_pManager->KillTimer(this, AnimationControl_TimerID);
}

void CAnimationControlUI::ShowNextFrame()
{
	m_nCurrentFrame++;

	if(m_nCurrentFrame == m_nFrameCount)
	{
		m_nCurrentFrame = 0;
	}

	this->Invalidate();
}

void CAnimationControlUI::Init()
{
	if ( m_pAnimationImage != NULL)
	{
		if ( m_cXY.cx == 0 )
		{
			if ( m_bVerticalMode )
				SetFixedWidth(m_pAnimationImage->nX);
			else
				SetFixedWidth(m_pAnimationImage->nX / m_nFrameCount);
		}
		if ( m_cXY.cy == 0 )
		{
			if ( m_bVerticalMode )
				SetFixedHeight(m_pAnimationImage->nY / m_nFrameCount);
			else
				SetFixedHeight(m_pAnimationImage->nY);
		}
		this->Start();
	}
}

}