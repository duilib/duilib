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
	int m_nAnimationState;		//����״̬ 0-ԭʼ״̬ 1-������,���ڱ�� 2-���������,�����״̬ 3-����뿪,���ڱ�С
	//�����������С����ԭ�ȵ�maxwidth��minwidth��
};
