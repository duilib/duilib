#include "stdafx.h"
#include "ImageObject.h"


ImageObject::ImageObject(void)
	: m_nWidth(0)
	, m_nHeight(0)
	, m_nAlpha(255)
{
}

ImageObject::~ImageObject(void)
{
}

void ImageObject::SetImagePath(LPCTSTR lpszImagePath)
{
	m_strFilePath = lpszImagePath;
}

LPCTSTR ImageObject::GetImagePath(void)
{
	return m_strFilePath.c_str();
}

void ImageObject::Release()
{

}
