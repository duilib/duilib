#include "StdAfx.h"
#include "UILabel.h"

// UMU: DO NOT use "using namespace" in .h file
#ifdef _USE_GDIPLUS
using namespace Gdiplus;
#endif

namespace DuiLib
{
	Color ARGB2Color(DWORD dwColor)
	{
		return Color(HIBYTE((dwColor)>>16), GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
	}

	CLabelUI::CLabelUI() : 
		m_pWideText(0),
		m_uTextStyle(DT_VCENTER|DT_SINGLELINE), 
		m_dwTextColor(0), 
		m_dwDisabledTextColor(0),
		m_iFont(-1),
		m_bShowHtml(false),
        m_bNeedEstimateSize(true),
		m_EnableEffect(false),
		m_bEnableLuminous(false),
		m_fLuminousFuzzy(3),
		m_gdiplusToken(0),
		m_dwTextColor1(-1),
		m_dwTextShadowColorA(0xff000000),
		m_dwTextShadowColorB(-1),
		m_GradientAngle(0),
		m_EnabledStroke(false),
		m_dwStrokeColor(0),
		m_EnabledShadow(false),
		m_GradientLength(0)
	{
		m_ShadowOffset.X		= 0.0f;
		m_ShadowOffset.Y		= 0.0f;
		m_ShadowOffset.Width	= 0.0f;
		m_ShadowOffset.Height	= 0.0f;

        m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
        m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

#ifdef _USE_GDIPLUS
        GdiplusStartup( &m_gdiplusToken,&m_gdiplusStartupInput, NULL);
#endif
	}

	CLabelUI::~CLabelUI()
	{
#ifdef _UNICODE
		if( m_pWideText && m_pWideText != m_sText.GetData()) delete[] m_pWideText;
#else
		if( m_pWideText ) delete[] m_pWideText;
#endif

#ifdef _USE_GDIPLUS
		GdiplusShutdown( m_gdiplusToken );
#endif
	}

	LPCTSTR CLabelUI::GetClass() const
	{
		return DUI_CTR_LABEL;
	}

	LPVOID CLabelUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_LABEL) == 0 ) return static_cast<CLabelUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

    void CLabelUI::SetFixedWidth(int cx)
    {
        m_bNeedEstimateSize = true;
        CControlUI::SetFixedWidth(cx);
    }

    void CLabelUI::SetFixedHeight(int cy)
    {
        m_bNeedEstimateSize = true;
        CControlUI::SetFixedHeight(cy);
    }

	void CLabelUI::SetText(LPCTSTR pstrText)
	{
		CControlUI::SetText(pstrText);
        m_bNeedEstimateSize = true;
		if( m_EnableEffect) {
#ifdef _UNICODE
			m_pWideText = (LPWSTR)m_sText.GetData();
#else 
			int iLen = _tcslen(pstrText);
			if (m_pWideText) delete[] m_pWideText;
			m_pWideText = new WCHAR[iLen + 1];
			::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)m_pWideText, iLen);
#endif
		}
	}

	void CLabelUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
        m_bNeedEstimateSize = true;
		Invalidate();
	}

	UINT CLabelUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	bool CLabelUI::IsMultiLine()
	{
		return (m_uTextStyle & DT_SINGLELINE) == 0;
	}

	void CLabelUI::SetMultiLine(bool bMultiLine)
	{
		if (bMultiLine)	{
            m_uTextStyle  &= ~DT_SINGLELINE;
            m_uTextStyle |= DT_WORDBREAK;
        }
		else m_uTextStyle |= DT_SINGLELINE;
        m_bNeedEstimateSize = true;
	}

	void CLabelUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CLabelUI::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void CLabelUI::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CLabelUI::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	void CLabelUI::SetFont(int index)
	{
		m_iFont = index;
        m_bNeedEstimateSize = true;
		Invalidate();
	}

	int CLabelUI::GetFont() const
	{
		return m_iFont;
	}

	RECT CLabelUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CLabelUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
        m_bNeedEstimateSize = true;
		Invalidate();
	}

	bool CLabelUI::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void CLabelUI::SetShowHtml(bool bShowHtml)
	{
		if( m_bShowHtml == bShowHtml ) return;

		m_bShowHtml = bShowHtml;
        m_bNeedEstimateSize = true;
		Invalidate();
	}

	SIZE CLabelUI::EstimateSize(SIZE szAvailable)
	{
        if (m_cxyFixed.cx > 0 && m_cxyFixed.cy > 0) return m_cxyFixed;

        if ((m_uTextStyle & DT_SINGLELINE) == 0 && 
            (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
            m_bNeedEstimateSize = true;
        }

        if (m_bNeedEstimateSize) {
            m_bNeedEstimateSize = false;
            m_szAvailableLast = szAvailable;
            m_cxyFixedLast = m_cxyFixed;
            if ((m_uTextStyle & DT_SINGLELINE) != 0) {
                if (m_cxyFixedLast.cy == 0) {
                    m_cxyFixedLast.cy = m_pManager->GetFontInfo(m_iFont)->tm.tmHeight + 8;
                    m_cxyFixedLast.cy += m_rcTextPadding.top + m_rcTextPadding.bottom;
                }
                if (m_cxyFixedLast.cx == 0) {
                    RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
                    if( m_bShowHtml ) {
                        int nLinks = 0;
                        CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                    }
                    else {
                        CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                    }
                    m_cxyFixedLast.cx = rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right;
                }
            }
            else {
                if( m_cxyFixedLast.cx == 0 ) {
                    m_cxyFixedLast.cx = szAvailable.cx;
                }
                RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
                rcText.left += m_rcTextPadding.left;
                rcText.right -= m_rcTextPadding.right;
                if( m_bShowHtml ) {
                    int nLinks = 0;
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                m_cxyFixedLast.cy = rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom;
            }
        }
        return m_cxyFixedLast;
	}

	void CLabelUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
			return;
		}
		CControlUI::DoEvent(event);
	}

	void CLabelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if (_tcscmp(pstrName, _T("valign")) == 0)
		{
		    if (_tcsstr(pstrValue, _T("top")) != NULL) {
		        m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
		        m_uTextStyle |= DT_TOP;
		    }
		    if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
		        m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
		        m_uTextStyle |= DT_VCENTER;
		    }
		    if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
		        m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
		        m_uTextStyle |= DT_BOTTOM;
		    }
		}
		else if( _tcscmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("disabledtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("multiline")) == 0 ) SetMultiLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("enabledeffect")) == 0 ) SetEnabledEffect(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("enabledluminous")) == 0 ) SetEnabledLuminous(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("luminousfuzzy")) == 0 ) SetLuminousFuzzy((float)_tstof(pstrValue));
		else if( _tcscmp(pstrName, _T("gradientangle")) == 0 ) SetGradientAngle(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("enabledstroke")) == 0 ) SetEnabledStroke(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("enabledshadow")) == 0 ) SetEnabledShadow(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("gradientlength")) == 0 ) SetGradientLength(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("shadowoffset")) == 0 ){
			LPTSTR pstr = NULL;
			int offsetx = _tcstol(pstrValue, &pstr, 10);	ASSERT(pstr);    
			int offsety = _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);
			SetShadowOffset(offsetx,offsety);
		}
		else if( _tcscmp(pstrName, _T("textcolor1")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor1(clrColor);
		}
		else if( _tcscmp(pstrName, _T("textshadowcolora")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextShadowColorA(clrColor);
		}
		else if( _tcscmp(pstrName, _T("textshadowcolorb")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextShadowColorB(clrColor);
		}
		else if( _tcscmp(pstrName, _T("strokecolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetStrokeColor(clrColor);
		}
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CLabelUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if(!GetEnabledEffect())
		{
			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			if( IsEnabled() ) {
				if( m_bShowHtml )
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
					NULL, NULL, nLinks, m_iFont, m_uTextStyle);
				else
					CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
					m_iFont, m_uTextStyle);
			}
			else {
				if( m_bShowHtml )
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
					NULL, NULL, nLinks, m_iFont, m_uTextStyle);
				else
					CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
					m_iFont, m_uTextStyle);
			}
		}
		else
		{
#ifdef _USE_GDIPLUS
			Font	nFont(hDC,m_pManager->GetFont(GetFont()));
			Graphics nGraphics(hDC);
			nGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

			StringFormat format;
			StringAlignment sa = StringAlignment::StringAlignmentNear;
			if ((m_uTextStyle & DT_VCENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
			else if( (m_uTextStyle & DT_BOTTOM) != 0) sa = StringAlignment::StringAlignmentFar;
			format.SetLineAlignment((StringAlignment)sa);
			sa = StringAlignment::StringAlignmentNear;
			if ((m_uTextStyle & DT_CENTER) != 0) sa = StringAlignment::StringAlignmentCenter;
			else if( (m_uTextStyle & DT_RIGHT) != 0) sa = StringAlignment::StringAlignmentFar;
			format.SetAlignment((StringAlignment)sa);

			RectF nRc((float)rc.left,(float)rc.top,(float)rc.right-rc.left,(float)rc.bottom-rc.top);
			RectF nShadowRc = nRc;
			nShadowRc.X += m_ShadowOffset.X;
			nShadowRc.Y += m_ShadowOffset.Y;

			int nGradientLength	= GetGradientLength();

			if(nGradientLength == 0)
				nGradientLength = (rc.bottom-rc.top);

			LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0),Point(0,nGradientLength),ARGB2Color(GetTextShadowColorA()),ARGB2Color(GetTextShadowColorB() == -1?GetTextShadowColorA():GetTextShadowColorB()));
			LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0),Point(0,nGradientLength),ARGB2Color(GetTextColor()),ARGB2Color(GetTextColor1() == -1?GetTextColor():GetTextColor1()));

			if (GetEnabledLuminous())
			{
				// from http://bbs.csdn.net/topics/390346428
				int iFuzzyWidth = (int)(nRc.Width/GetLuminousFuzzy());
				if (iFuzzyWidth < 1) iFuzzyWidth = 1;
				int iFuzzyHeight = (int)(nRc.Height/GetLuminousFuzzy());
				if (iFuzzyHeight < 1) iFuzzyHeight = 1;
				RectF nTextRc(0.0f, 0.0f, nRc.Width, nRc.Height);

				Bitmap Bit1((INT)nRc.Width, (INT)nRc.Height);
				Graphics g1(&Bit1);
				g1.SetSmoothingMode(SmoothingModeAntiAlias);
				g1.SetTextRenderingHint(TextRenderingHintAntiAlias);
				g1.SetCompositingQuality(CompositingQualityAssumeLinear);
				Bitmap Bit2(iFuzzyWidth, iFuzzyHeight);
				Graphics g2(&Bit2);
				g2.SetInterpolationMode(InterpolationModeHighQualityBicubic);
				g2.SetPixelOffsetMode(PixelOffsetModeNone);

				FontFamily ftFamily;
				nFont.GetFamily(&ftFamily);
				int iLen = wcslen(m_pWideText);
				g1.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushB);

				g2.DrawImage(&Bit1, 0, 0, (int)iFuzzyWidth, (int)iFuzzyHeight);
				g1.Clear(Color(0));
				g1.DrawImage(&Bit2, (int)m_ShadowOffset.X, (int)m_ShadowOffset.Y, (int)nRc.Width, (int)nRc.Height);
				g1.SetTextRenderingHint(TextRenderingHintAntiAlias);

				nGraphics.DrawImage(&Bit1, nRc.X, nRc.Y);
			}
			
			if(GetEnabledStroke() && GetStrokeColor() > 0)
			{
				LinearGradientBrush nLineGrBrushStroke(Point(GetGradientAngle(),0),Point(0,rc.bottom-rc.top+2),ARGB2Color(GetStrokeColor()),ARGB2Color(GetStrokeColor()));
#ifdef _UNICODE
				nRc.Offset(-1,0);
				nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(2,0);
				nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(-1,-1);
				nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(0,2);
				nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(0,-1);
#else
				int iLen = wcslen(m_pWideText);
				nRc.Offset(-1,0);
				nGraphics.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(2,0);
				nGraphics.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(-1,-1);
				nGraphics.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(0,2);
				nGraphics.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushStroke);
				nRc.Offset(0,-1);
#endif	
			}
#ifdef _UNICODE
			if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
				nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nShadowRc,&format,&nLineGrBrushA);

			nGraphics.DrawString(m_sText,m_sText.GetLength(),&nFont,nRc,&format,&nLineGrBrushB);
#else
			int iLen = wcslen(m_pWideText);
			if(GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
				nGraphics.DrawString(m_pWideText,iLen,&nFont,nShadowRc,&format,&nLineGrBrushA);

			nGraphics.DrawString(m_pWideText,iLen,&nFont,nRc,&format,&nLineGrBrushB);
#endif
#endif
		}
	}

	void CLabelUI::SetShadowOffset( int _offset,int _angle )
	{
		if(_angle > 180 || _angle < -180) return;

		RECT rc = m_rcItem;
		if(_angle >= 0 && _angle <= 180) rc.top -= _offset;
		else if(_angle > -180 && _angle < 0) rc.top += _offset;

		if(_angle > -90 && _angle <= 90) rc.left -= _offset;
		else if( _angle > 90 || _angle < -90) rc.left += _offset;

		m_ShadowOffset.X = (float)rc.top;
		m_ShadowOffset.Y = (float)rc.left;
		Invalidate();
	}

	RectF CLabelUI::GetShadowOffset()
	{
		return m_ShadowOffset;
	}

	void CLabelUI::SetEnabledEffect( bool _EnabledEffect )
	{
		m_EnableEffect = _EnabledEffect;
		if (m_EnableEffect) {
#ifdef _UNICODE
			m_pWideText = (LPWSTR)m_sText.GetData();
#else 
			int iLen = m_sText.GetLength();
			if (m_pWideText) delete[] m_pWideText;
			m_pWideText = new WCHAR[iLen + 1];
			::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, m_sText.GetData(), -1, (LPWSTR)m_pWideText, iLen);
#endif
		}
		Invalidate();
	}

	bool CLabelUI::GetEnabledEffect()
	{
		return m_EnableEffect;
	}

	void CLabelUI::SetEnabledLuminous(bool bEnableLuminous)
	{
		m_bEnableLuminous = bEnableLuminous;
		Invalidate();
	}

	bool CLabelUI::GetEnabledLuminous()
	{
		return m_bEnableLuminous;
	}

	void CLabelUI::SetLuminousFuzzy(float fFuzzy)
	{
		if (fFuzzy < 0.0001f) return;
		m_fLuminousFuzzy = fFuzzy;
		Invalidate();
	}

	float CLabelUI::GetLuminousFuzzy()
	{
		return m_fLuminousFuzzy;
	}

	void CLabelUI::SetTextColor1( DWORD _TextColor1 )
	{
		m_dwTextColor1 = _TextColor1;
		Invalidate();
	}

	DWORD CLabelUI::GetTextColor1()
	{
		return m_dwTextColor1;
	}

	void CLabelUI::SetTextShadowColorA( DWORD _TextShadowColorA )
	{
		m_dwTextShadowColorA = _TextShadowColorA;
		Invalidate();
	}

	DWORD CLabelUI::GetTextShadowColorA()
	{
		return m_dwTextShadowColorA;
	}

	void CLabelUI::SetTextShadowColorB( DWORD _TextShadowColorB )
	{
		m_dwTextShadowColorB	= _TextShadowColorB;
		Invalidate();
	}

	DWORD CLabelUI::GetTextShadowColorB()
	{
		return m_dwTextShadowColorB;
	}

	void CLabelUI::SetGradientAngle( int _SetGradientAngle )
	{
		m_GradientAngle	= _SetGradientAngle;
		Invalidate();
	}

	int CLabelUI::GetGradientAngle()
	{
		return m_GradientAngle;
	}

	void CLabelUI::SetEnabledStroke( bool _EnabledStroke )
	{
		m_EnabledStroke = _EnabledStroke;
		Invalidate();
	}

	bool CLabelUI::GetEnabledStroke()
	{
		return m_EnabledStroke;
	}

	void CLabelUI::SetStrokeColor( DWORD _StrokeColor )
	{
		m_dwStrokeColor = _StrokeColor;
		Invalidate();
	}

	DWORD CLabelUI::GetStrokeColor()
	{
		return m_dwStrokeColor;
	}

	void CLabelUI::SetEnabledShadow( bool _EnabledShadowe )
	{
		m_EnabledShadow = _EnabledShadowe;
		Invalidate();
	}

	bool CLabelUI::GetEnabledShadow()
	{
		return m_EnabledShadow;
	}

	void CLabelUI::SetGradientLength( int _GradientLength )
	{
		m_GradientLength	= _GradientLength;
		Invalidate();
	}

	int CLabelUI::GetGradientLength()
	{
		return m_GradientLength;
	}
}
