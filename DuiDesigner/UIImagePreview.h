#pragma once
#include <atlimage.h>

// CUIImagePreview

class CUIImagePreview : public CStatic
{
	DECLARE_DYNAMIC(CUIImagePreview)

public:
	CUIImagePreview();
	virtual ~CUIImagePreview();

private:
	CImage m_imgPreview;

public:
	void SetPreviewImage(LPCTSTR pstrImage);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};