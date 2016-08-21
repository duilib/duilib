#ifndef __UIGIFCONTROL_H__
#define __UIGIFCONTROL_H__

#pragma once
#include <GdiPlus.h>
namespace DuiLib
{
	class CGifHandler;
	class UILIB_API CGifControlUI : public CControlUI
	{
		enum
		{ 
			GIFCONTROL_TIMERID = 10,
		};
	public:
		CGifControlUI();
		~CGifControlUI();
		UI_DECLARE_DYNCREATE();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetBkImage(LPCTSTR pStrImage);	// 只能设置Gif图片

		virtual void EventHandler(TEventUI& event);
		virtual void PaintBkImage(HDC hDC);	// 各种矩形计算可能不完善
		virtual void SetVisible(bool bVisible = true );

		void Start();
		void Stop();
		
		void LoadGif( StringOrID bitmap, LPCTSTR type=NULL, DWORD mask= 0 );
	private:
		void FreeImage();

	protected:
		Gdiplus::Image	*m_pGifImage;
		Gdiplus::PropertyItem*	m_pPropertyItem;
		HGLOBAL m_hGlobal;
		IStream *m_pStream;
		int m_nCurrentFrame;
		int m_nFrameCount;
	};

}

#endif // __UIGIFCONTROL_H__