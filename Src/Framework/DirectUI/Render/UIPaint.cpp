#include "stdafx.h"
#include "UIPaint.h"


CUIPaint* CUIPaint::m_pInstance = NULL;

CUIPaint::CUIPaint(void)
{
}


CUIPaint::~CUIPaint(void)
{
}

CUIPaint* CUIPaint::GetInstance()
{
	if ( m_pInstance == NULL )
	{
		m_pInstance = new CUIPaint;
	}
	return m_pInstance;
}

void CUIPaint::ReleaseInstance()
{
	if ( m_pInstance != NULL )
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CUIPaint::DrawButton(IUIRender *pRender,CButtonUI *pButton,LPCRECT rcDst)
{
	DWORD dwTextColor = 0;
	DWORD dwBackColor = 0;
	DWORD dwBorderColor = 0;
	
	do 
	{
		// »­±³¾°Í¼Æ¬
		if ( pButton->CheckState(UISTATE_Disabled))
		{
			// ½ûÓÃ×´Ì¬£¬»­½ûÓÃÍ¼Æ¬
			int i=0;
			break;
		}
		if ( pButton->CheckState( UISTATE_Pushed) )
		{
			// ½¹µã×´Ì¬£¬»­½¹µã±³¾°ÔÙ»­½¹µãÇ°¾°
			int i=0;
			break;
		}
		if ( pButton->CheckState(UISTATE_Focused))
		{
			int i=0;
			break;
		}
		if ( pButton->CheckState(UISTATE_Hover))
		{
			int i=0;
			break;
		}
		if ( pButton->CheckState(UISTATE_Normal))
		{
			int i=0;
			break;
		}

	} while (false);
}

void CUIPaint::DrawBackColor(IUIRender *pRender,DWORD dwBackColor,DWORD dwBackColor2,DWORD dwBackColor3,LPCRECT pPaint,LPCRECT pOverlap,LPCRECT pControl)
{
	DWORD m_dwBackColor = dwBackColor;
	DWORD m_dwBackColor2 = dwBackColor2;
	DWORD m_dwBackColor3 =dwBackColor3;
	if( m_dwBackColor != 0 )
	{
		if( m_dwBackColor2 != 0 )
		{
			if( m_dwBackColor3 != 0 )
			{
				RECT rc = *pControl;
				rc.bottom = (rc.bottom + rc.top) / 2;
				pRender->DrawGradient(&rc,m_dwBackColor,m_dwBackColor2,true,8);

				rc.top = rc.bottom;
				rc.bottom = pControl->bottom;
				pRender->DrawGradient(&rc,m_dwBackColor2,m_dwBackColor2,true,8);
			}
			else
				pRender->DrawGradient(pControl,m_dwBackColor,m_dwBackColor2,true,16);
		}
		else
			if( m_dwBackColor >= 0xFF000000 )
				pRender->DrawColor(pOverlap,m_dwBackColor);
				//CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor));
			else
				pRender->DrawColor(pControl,m_dwBackColor);
				//CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor));
	}
}

void CUIPaint::DrawControl(IUIRender *pRender,CControlUI* pControl,LPCRECT rcUpdate)
{
	// »­±³¾°É«
	CDuiRect rcControl(pControl->GetPosition());
	CDuiRect rcOverlop;
	rcOverlop.IntersectRect(&rcControl,rcUpdate);

	this->DrawBackColor(pRender,
		pControl->GetColorProperty(UIProperty_Back_Color1),
		pControl->GetColorProperty(UIProperty_Back_Color2),
		pControl->GetColorProperty(UIProperty_Back_Color3),
		rcUpdate,&rcOverlop,&rcControl);

	ImageObject* pImage = NULL;
	LPCTSTR lpszString = NULL;

	// »­±³¾°Í¼
	pImage = pControl->GetImageProperty(UIProperty_Back_Image);
	if ( pImage != NULL )
	{
		pRender->DrawImage(pImage,&rcControl,rcUpdate);
	}

	// »­Ç°¾°Í¼
	pImage = pControl->GetImageProperty(UIProperty_Fore_Image);
	if ( pImage != NULL )
	{
		pRender->DrawImage(pImage,&rcControl,rcUpdate);
	}

	// »­ÎÄ×Ö
	lpszString = pControl->GetPropertyForState(UIProperty_Text_String);
	if ( lpszString != NULL )
	{
		//pRender->DrawText(&rcControl,lpszString);
	}
	
	// »­±ß¿ò
	if ( pControl->GetIntProperty(UIProperty_Border_Wdith) != 0 )
	{
		// ÐèÒª»­±ß¿ò
		do 
		{
			DWORD dwBorderColor = pControl->GetColorProperty(UIProperty_Border_Color);
			if ( dwBorderColor == 0 )
				break;

			CDuiRect rcBorder;
			rcBorder.left = 1;
			rcBorder.right = 1;
			rcBorder.top = 1;
			rcBorder.bottom = 1;
			pRender->DrawRectangleBorder(&rcControl,&rcBorder,0xFF0000FF);
		} while (false);
	
	}
}
