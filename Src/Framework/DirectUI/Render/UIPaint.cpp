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

}

void CUIPaint::DrawControl(IUIRender *pRender,CControlUI* pControl,LPCRECT rcDst)
{
	CDuiRect rcControl = pControl->GetPosition();
	DWORD dwColor = pControl->GetColorProperty(UIProperty_Back_Color1);
	if ( dwColor != 0 )
	{
		pRender->DrawColor(&rcControl,dwColor);
	}

	ImageObject* pImage = pControl->GetImageProperty(UIProperty_Back_Image);
	if ( pImage != NULL )
	{
		pRender->DrawImage(pImage,rcDst);
	}

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
