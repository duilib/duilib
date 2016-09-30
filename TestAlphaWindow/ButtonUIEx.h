#pragma once

class CButtonUIEx :	public CButtonUI
{
public:
	CButtonUIEx(void);
	virtual ~CButtonUIEx(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void DoEvent(TEventUI& event);

	void OnTimer(const int& nTimerId);

protected:
	int m_nAnimationState;		//动画状态 0-原始状态 1-鼠标进入,正在变大 2-变大过程完成,大个子状态 3-鼠标离开,正在变小
	//动画的最大最小利用原先的maxwidth和minwidth吧
};
