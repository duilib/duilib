#include "StdAfx.h"
#include "UISliderTabLayout.h"

namespace DuiLib
{
//////////////////////////////////////////////////////////////////////////
// CSliderTabLayoutUI
	UI_IMPLEMENT_DYNCREATE(CSliderTabLayoutUI);

CSliderTabLayoutUI::CSliderTabLayoutUI() :
    m_bIsVertical(false),
    m_bAnimating(false),
    m_iCurFrame(0)
{
}

LPCTSTR CSliderTabLayoutUI::GetClass() const
{
    return _T("SliderTabLayout");
}

LPVOID CSliderTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("SliderTabLayout")) == 0)
    {
        return static_cast<CSliderTabLayoutUI *>(this);
    }

    return CTabLayoutUI::GetInterface(pstrName);
}

bool CSliderTabLayoutUI::SelectItem(int iIndex)
{
    if(iIndex < 0 || iIndex >= m_items.GetSize())
    {
        return false;
    }

    if(iIndex == m_iCurSel)
    {
        return true;
    }

    if(iIndex > m_iCurSel)
    {
        m_nPositiveDirection = -1;
    }

    if(iIndex < m_iCurSel)
    {
        m_nPositiveDirection = 1;
    }

    m_bAnimating = true;
    m_iCurFrame = 0;
    RECT rcInset = GetInset();
    m_rcCurPos = GetPos();
    m_rcCurPos.left += rcInset.left;
    m_rcCurPos.top += rcInset.top;
    m_rcCurPos.right -= rcInset.right;
    m_rcCurPos.bottom -= rcInset.bottom;
    m_rcNextPos = m_rcCurPos;

    if(!m_bIsVertical)    //横向
    {
        m_rcNextPos.left = m_rcCurPos.left - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
        m_rcNextPos.right = m_rcCurPos.right - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
    }
    else
    {
        m_rcNextPos.top = m_rcCurPos.top - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
        m_rcNextPos.bottom = m_rcCurPos.bottom - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
    }

    int iOldSel = m_iCurSel;
    m_iCurSel = iIndex;

    for(int it = 0; it < m_items.GetSize(); it++)
    {
        if(it == iIndex)
        {
            m_pNextPage = GetItemAt(it);
            m_pNextPage->SetPos(m_rcNextPos);
            m_pNextPage->SetVisible(true);
        }
        else if(it == iOldSel)
        {
            m_pCurPage = GetItemAt(it);
            m_pCurPage->SetVisible(true);
        }
        else
        {
            GetItemAt(it)->SetVisible(false);
        }
    }

    m_pManager->SetTimer(this, TIMER_ANIMATION_ID, ANIMATION_ELLAPSE);

    if(m_pManager != NULL)
    {
        //m_pManager->SetNextTabControl();
        m_pManager->SendNotify(this, UINOTIFY_TAB_SELECTED, m_iCurSel, iOldSel);
    }

    return true;
}

void CSliderTabLayoutUI::EventHandler(TEventUI & event)
{
    if(event.dwType == UIEVENT_TIMER)
    {
        OnTimer(event.wParam);
		return;
    }

	CTabLayoutUI::EventHandler(event);
}

void CSliderTabLayoutUI::OnTimer(int nTimerID)
{
    NeedParentUpdate();
}

void CSliderTabLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc,bNeedInvalidate);
    RECT rcInset = GetInset();
    rc.left += rcInset.left;
    rc.top += rcInset.top;
    rc.right -= rcInset.right;
    rc.bottom -= rcInset.bottom;

    if(m_bAnimating)
    {
        int iStepLen = 0;

        if(!m_bIsVertical)    //横向
        {
            iStepLen = (rc.right - rc.left) * m_nPositiveDirection / ANIMATION_FRAME_COUNT;

            if(m_iCurFrame != ANIMATION_FRAME_COUNT)
            {
                //当前窗体位置
                ::OffsetRect(&m_rcCurPos, iStepLen, 0);
                m_pCurPage->SetPos(m_rcCurPos,false);
                //下一个窗体位置
                ::OffsetRect(&m_rcNextPos, iStepLen, 0);
                m_pNextPage->SetPos(m_rcNextPos,false);
            }
            else
            {
                m_pCurPage->SetVisible(false);
                ::OffsetRect(&m_rcCurPos, iStepLen, 0);
                m_pCurPage->SetPos(m_rcCurPos);
                m_pNextPage->SetPos(rc);
            }
        }
        else //竖向
        {
            iStepLen = (rc.bottom - rc.top) * m_nPositiveDirection / ANIMATION_FRAME_COUNT;

            if(m_iCurFrame != ANIMATION_FRAME_COUNT)
            {
                //当前窗体位置
                ::OffsetRect(&m_rcCurPos, 0, iStepLen);
                m_pCurPage->SetPos(m_rcCurPos,false);
                //下一个窗体位置
                ::OffsetRect(&m_rcNextPos, 0, iStepLen);
                m_pNextPage->SetPos(m_rcNextPos,false);
            }
            else
            {
                m_pCurPage->SetVisible(false);
                ::OffsetRect(&m_rcCurPos, 0, iStepLen);
                m_pCurPage->SetPos(m_rcCurPos);
                m_pNextPage->SetPos(rc);
            }
        }

        if(m_iCurFrame == ANIMATION_FRAME_COUNT)
        {
            m_iCurFrame = 0;
            m_bAnimating = false;
            m_pManager->KillTimer(this, TIMER_ANIMATION_ID);
        }

        m_iCurFrame ++;
    }
    else
    {
        for(int it = 0; it < GetCount(); it++)
        {
            CControlUI *pControl = GetItemAt(it);

            if(!pControl->IsVisible())
            {
                continue;
            }

            if(pControl->IsFloat())
            {
                SetFloatPos(it);
                continue;
            }

            if(it != GetCurSel())
            {
                continue;
            }

            RECT rcPadding = pControl->GetPadding();
            rc.left += rcPadding.left;
            rc.top += rcPadding.top;
            rc.right -= rcPadding.right;
            rc.bottom -= rcPadding.bottom;
            SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
            SIZE sz = pControl->EstimateSize(szAvailable);

            if(sz.cx == 0)
            {
                sz.cx = MAX(0, szAvailable.cx);
            }

            if(sz.cx < pControl->GetMinWidth())
            {
                sz.cx = pControl->GetMinWidth();
            }

            if(sz.cx > pControl->GetMaxWidth())
            {
                sz.cx = pControl->GetMaxWidth();
            }

            if(sz.cy == 0)
            {
                sz.cy = MAX(0, szAvailable.cy);
            }

            if(sz.cy < pControl->GetMinHeight())
            {
                sz.cy = pControl->GetMinHeight();
            }

            if(sz.cy > pControl->GetMaxHeight())
            {
                sz.cy = pControl->GetMaxHeight();
            }

            RECT rcCtrl = {rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
            pControl->SetPos(rcCtrl,false);
        }
    }
}

void CSliderTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    // pstrValue = "vertical" or "horizontal"
    if(_tcscmp(pstrName, _T("direction")) == 0 && _tcscmp(pstrValue, _T("vertical")) == 0)
    {
        m_bIsVertical = true;
    }

    return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
}
} // namespace DuiLib