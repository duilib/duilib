#include "stdafx.h"
#include "ImageObject.h"


ImageObject::ImageObject(void)
	: m_nAlpha(255)
	, m_dwMask(0)
	, m_bHole(false)
	, m_bXTiled(false)
	, m_bYTiled(false)
{
}

ImageObject::~ImageObject(void)
{
	if ( m_pImageData == NULL)
		return;

	if ( m_pImageData->nRefCount == -1 )
	{
		if (m_pImageData->hBitmap)
		{
			::DeleteObject(m_pImageData->hBitmap) ; 
		}
		delete m_pImageData ;
	}
	else
		CResourceManager::GetInstance()->FreeImage(m_strFilePath.c_str());
}

void ImageObject::SetImagePath(LPCTSTR lpszImagePath)
{
	m_strFilePath = lpszImagePath;
}

LPCTSTR ImageObject::GetImagePath(void) const
{
	return m_strFilePath.c_str();
}

ImageObject* ImageObject::CreateFromString(LPCTSTR lpszString)
{
	// 2¡¢file='aaa.jpg' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
	// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
	ImageObject* pImageObj = new ImageObject;
	StringMap attributeMap;
	if ( !CDuiStringOperation::parseAttributeString(lpszString,attributeMap))
	{
		pImageObj->SetImagePath(lpszString);
		return pImageObj;
	}

	LPCTSTR lpszVaule =NULL;
	
	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("file"));
	if ( lpszVaule != NULL)
		pImageObj->SetImagePath(lpszVaule);
	 
	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("dest"));
	if ( lpszVaule != NULL)
	{
		CDuiRect rc;
		CDuiCodeOperation::StringToRect(lpszVaule,&rc);
		pImageObj->SetDest(rc);
	}

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("source"));
	if ( lpszVaule != NULL)
	{
		CDuiRect rc;
		CDuiCodeOperation::StringToRect(lpszVaule,&rc);
		pImageObj->SetSource(rc);
	}

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("corner"));
	if ( lpszVaule != NULL)
	{
		CDuiRect rc;
		CDuiCodeOperation::StringToRect(lpszVaule,&rc);
		pImageObj->Set9Gird(rc);
	}

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("mask"));
	if ( lpszVaule != NULL)
		pImageObj->SetMaskColor(CDuiCodeOperation::StringToColor(lpszVaule));

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("fade"));
	if ( lpszVaule != NULL)
		pImageObj->SetAlpha(CDuiCodeOperation::StringToInt(lpszVaule));

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("hole"));
	if ( lpszVaule != NULL && _tcsicmp(lpszVaule,_T("true")) ==0 )
		pImageObj->SetHole(true);

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("xtiled"));
	if ( lpszVaule != NULL && _tcsicmp(lpszVaule,_T("true")) ==0 )
		pImageObj->SetXTiled(true);

	lpszVaule = CDuiStringOperation::FindAttrubuteKey(attributeMap,_T("ytiled"));
	if ( lpszVaule != NULL && _tcsicmp(lpszVaule,_T("true")) ==0 )
		pImageObj->SetYTiled(true);

	return pImageObj;
}

void ImageObject::SetAlpha(int iAlpha)
{
	m_nAlpha = iAlpha;
}

int ImageObject::GetAlpha(void) const
{
	return m_nAlpha;
}

void ImageObject::SetHole(bool bHole)
{
	m_bHole = bHole;
}

bool ImageObject::GetHole(void) const
{
	return m_bHole;
}

void ImageObject::SetXTiled(bool bXTiled)
{
	m_bXTiled = bXTiled;
}

void ImageObject::SetYTiled(bool bYTiled)
{
	m_bYTiled = bYTiled;
}

bool ImageObject::GetXTiled(void) const
{
	return m_bXTiled;
}

bool ImageObject::GetYTiled(void) const
{
	return m_bYTiled;
}

void ImageObject::SetMaskColor(DWORD dwMask)
{
	m_dwMask = dwMask;
}

DWORD ImageObject::GetMaskColor(void) const
{
	return m_dwMask;
}

void ImageObject::SetDest(RECT &rc)
{
	m_rcDest = rc;
}

RECT ImageObject::GetDest(void) const
{
	return m_rcDest;
}

void ImageObject::SetSource(RECT &rc)
{
	m_rcSource = rc;
}

RECT ImageObject::GetSource(void) const
{
	return m_rcSource;
}

void ImageObject::Set9Gird(RECT &rc)
{
	m_rc9Grid = rc;
}

RECT ImageObject::Get9Gird(void) const
{
	return m_rc9Grid;
}

void ImageObject::Init()
{
	m_pImageData = CResourceManager::GetInstance()->GetImage(m_strFilePath.c_str(),m_dwMask);
}

HBITMAP ImageObject::GetHBitmap()
{
	if ( m_pImageData != NULL)
		return m_pImageData->hBitmap;
	else
	{
		if ( m_strFilePath.empty() )
		{
			ASSERT(false);
			return NULL;
		}

		m_pImageData = CResourceManager::GetInstance()->GetImage(m_strFilePath.c_str(),m_dwMask);
		if ( m_pImageData == NULL)
		{
			m_strFilePath.clear();
			ASSERT(false);
			return NULL;
		}
		else
			return m_pImageData->hBitmap;
	}
		
	return NULL;
}

int ImageObject::GetImageWidth()
{
	if ( m_pImageData != NULL)
		return m_pImageData->nX;
	return 0;
}

int ImageObject::GetImageHeight()
{
	if ( m_pImageData != NULL)
		return m_pImageData->nY;
	return 0;
}

bool ImageObject::IsAlphaImage()
{
	if ( m_pImageData != NULL)
		return m_pImageData->alphaChannel;
	return false;
}


