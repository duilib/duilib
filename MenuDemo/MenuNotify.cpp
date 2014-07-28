#include "StdAfx.h"
#include "MenuNotify.h"
#include "UIMenu.h"
#include "main_frame.hpp"

CMenuNotify::CMenuNotify(void)
{
}


CMenuNotify::~CMenuNotify(void)
{
}

void CMenuNotify::Notify(TNotifyUI& msg)
{
	if( msg.sType == DUI_MSGTYPE_ITEMCLICK) 
	{
		CMenuElementUI *pMenuElement  = (CMenuElementUI*)msg.pSender;
		CPaintManagerUI *m_pm = pMenuElement->GetManager();
		if (m_pm != NULL)
		{

			CControlUI *pRootControl = m_pm->GetRoot();
			if (pRootControl != NULL)
			{

				CDuiString szMenuName = pRootControl->GetName();

				//if (szMenuName == _T("workstation_menu"))
				//{
					CDuiString szText = pMenuElement->GetText();
					int index = pMenuElement->GetIndex();
					bool bTickStatus = false;
					switch (index)
					{
					case 0:
						PostMessage(pMainFrame->m_hWnd,WM_CLICKABOUT,0,0);
						break;
					}

				//}
			}
		}
	}
}