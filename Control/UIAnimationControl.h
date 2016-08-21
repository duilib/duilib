#ifndef UIAnimationControl_h__
#define UIAnimationControl_h__
#pragma once

namespace DuiLib
{

class CAnimationControlUI
	: public CControlUI
{
	enum
	{ 
		AnimationControl_TimerID = 11,
	};
public:
	CAnimationControlUI(void);
	virtual ~CAnimationControlUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual void EventHandler(TEventUI& event);
	virtual void PaintBkImage(HDC hDC);	// 各种矩形计算可能不完善
	virtual void SetVisible(bool bVisible = true );
	virtual void SetInternVisible(bool bVisible /* = true */);
	virtual void Init();

	void Start();
	void Stop();

	void ShowNextFrame();

protected:
	UINT m_nFrameCount;
	bool m_bVerticalMode;
	UINT m_nCurrentFrame;
	DWORD m_dwInterval;
	const TImageData *m_pAnimationImage;
	ImageSequenceAnimation* m_pImageSeqAnimation;
};

}

#endif // UIAnimationControl_h__
