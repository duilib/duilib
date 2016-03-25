#ifndef __UIGIFCONTROL_H__
#define __UIGIFCONTROL_H__

#pragma once

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

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetBkImage(LPCTSTR pStrImage);	// 只能设置Gif图片

		virtual void DoEvent(TEventUI& event);
		virtual void PaintBkImage(HDC hDC);	// 各种矩形计算可能不完善
		virtual void SetVisible(bool bVisible = true );

		void Start();
		void Stop();
		
		CGifHandler* LoadGif( STRINGorID bitmap, LPCTSTR type=NULL, DWORD mask= 0 );

	protected:
		CGifHandler* m_pGifHandler;
		UINT m_iCurrentFrame;
		bool bGif;
	};

	class CGifHandler
	{
	public:
		CGifHandler();
		virtual ~CGifHandler();

		int GetFrameCount();
		void AddFrameInfo(TImageInfo* pFrameInfo);
		TImageInfo* GetNextFrameInfo();
		TImageInfo* GetCurrentFrameInfo();
		TImageInfo* GetFrameInfoAt(int index);
		bool IsAnimation(void);

	private:
		CStdPtrArray  ImageInfos;
		int nCurrentFrame;
		int nFrameCount;
		bool bAnimation;
		TImageInfo* pFirstFrame;
	};
}

#endif // __UIGIFCONTROL_H__