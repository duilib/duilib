#include "stdafx.h"
#include "ImageObject.h"

namespace DuiLib
{

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
}

}
