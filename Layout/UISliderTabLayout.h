#ifndef __SLIDERTABLAYOUT_H__
#define __SLIDERTABLAYOUT_H__

#pragma once


namespace DuiLib
{

	class UILIB_API CSliderTabLayoutUI
		: public CTabLayoutUI
	{
	public:
		CSliderTabLayoutUI();
		UI_DECLARE_DYNCREATE();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void EventHandler(TEventUI& event);
		void OnTimer( int nTimerID );

		void SetPos(RECT rc, bool bNeedInvalidate = true);

		bool SelectItem( int iIndex );

		void OnSliderStep();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		int         m_iCurFrame;
		bool        m_bIsVertical;
		int         m_nPositiveDirection;
		bool        m_bAnimating;

		RECT        m_rcCurPos;
		RECT        m_rcNextPos;

		CControlUI* m_pCurPage;			// 保存当前显示的页面
		CControlUI* m_pNextPage;		// 保存下一页面

		enum
		{
			TIMER_ANIMATION_ID = 1,
			ANIMATION_ELLAPSE = 10,
			ANIMATION_FRAME_COUNT = 15
		};
	};

} // namespace DuiLib

#endif // __UISlider_H__