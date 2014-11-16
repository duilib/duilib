#include "StdAfx.h"
#include "UIGifControl.h"


struct TGifInfo
{
	unsigned char* pData;
	int w;
	int h;
	int delay;
};

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
		int *comp, int req_comp);

	extern TGifInfo* gif_load_from_memory(unsigned char const *buffer, int len, int *n, int *comp, int req_comp);
	extern void stbi_image_free(void *retval_from_stbi_load);

};

namespace DuiLib
{
	CGifControlUI::CGifControlUI()
		: m_pGifHandler(NULL)
		, m_iCurrentFrame(0)
		, bGif(false)
	{

	}

	CGifControlUI::~CGifControlUI()
	{
		if (m_pGifHandler)
			delete m_pGifHandler;
	}


	LPCTSTR CGifControlUI::GetClass() const
	{
		return _T("GifControlUI");
	}

	LPVOID CGifControlUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("GifControl")) == 0 ) return static_cast<CGifControlUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void CGifControlUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CGifControlUI::SetBkImage( LPCTSTR pStrImage )
	{
		if (m_sBkImage==pStrImage)
			return;

		if (m_pGifHandler)
		{
			this->GetManager()->KillTimer(this,GIFCONTROL_TIMERID);
			delete m_pGifHandler;
			m_pGifHandler=NULL;
		}

		m_sBkImage=pStrImage;
		bGif=false;
		CDuiString strExt=m_sBkImage.Right(4);
		if (strExt.CompareNoCase(_T(".gif"))==0)
		{	// 扩展名是一张Gif图片
			m_pGifHandler=LoadGif(STRINGorID(m_sBkImage));
			if (m_pGifHandler)
			{
				bGif=m_pGifHandler->IsAnimation();	// 单帧Gif，使用普通绘图
				if (bGif==false)
				{
					delete m_pGifHandler;
					m_pGifHandler=NULL;
				}
			}
		}
		
		Invalidate();
	}

	CGifHandler* CGifControlUI::LoadGif( STRINGorID bitmap, LPCTSTR type/*=NULL*/, DWORD mask/*= 0 */ )
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do
		{
			if( type == NULL ) {
				CDuiString sFile = CPaintManagerUI::GetResourcePath();
				if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
					sFile += bitmap.m_lpstr;
					HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
						FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile == INVALID_HANDLE_VALUE ) break;
					dwSize = ::GetFileSize(hFile, NULL);
					if( dwSize == 0 ) break;

					DWORD dwRead = 0;
					pData = new BYTE[ dwSize ];
					::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
					::CloseHandle( hFile );

					if( dwRead != dwSize ) {
						delete[] pData;
						pData = NULL;
						break;
					}
				}
				else {
					sFile += CPaintManagerUI::GetResourceZip();
					HZIP hz = NULL;
					if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
					else hz = OpenZip((void*)sFile.GetData(), 0, 2);
					if( hz == NULL ) break;
					ZIPENTRY ze; 
					int i; 
					if( FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0 ) break;
					dwSize = ze.unc_size;
					if( dwSize == 0 ) break;
					pData = new BYTE[ dwSize ];
					int res = UnzipItem(hz, i, pData, dwSize, 3);
					if( res != 0x00000000 && res != 0x00000600) {
						delete[] pData;
						pData = NULL;
						if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
						break;
					}
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
				}
			}
			else {
				HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), bitmap.m_lpstr, type);
				if( hResource == NULL ) break;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal == NULL ) {
					FreeResource(hResource);
					break;
				}

				dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
				::FreeResource(hResource);
			}
		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) break;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		if (!pData)
		{
			//::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
			return NULL;
		}

		LPBYTE pImage = NULL;
		int x,y,n;
		int count = 0;
		int delay = 0;

		CGifHandler* pGifHandler = new CGifHandler();

		TGifInfo* pGifInfos = (TGifInfo*)gif_load_from_memory(pData, dwSize, &count, &n, 4);
		delete[] pData;
		pData = NULL;
		for (int i = 0; i <count; i++ )
		{
			pImage = pGifInfos[i].pData;
			x = pGifInfos[i].w;
			y = pGifInfos[i].h;
			delay = pGifInfos[i].delay;

			if( !pImage ) 
			{
				stbi_image_free(pImage);
				continue;
			}
			BITMAPINFO bmi;
			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = x;
			bmi.bmiHeader.biHeight = -y;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = x * y * 4;

			bool bAlphaChannel = false;
			LPBYTE pDest = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
			if( !hBitmap ) 
			{
				stbi_image_free(pImage);
				continue;
			}

			for( int i = 0; i < x * y; i++ ) 
			{
				pDest[i*4 + 3] = pImage[i*4 + 3];
				if( pDest[i*4 + 3] < 255 )
				{
					pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
					pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
					pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
					bAlphaChannel = true;
				}
				else
				{
					pDest[i*4] = pImage[i*4 + 2];
					pDest[i*4 + 1] = pImage[i*4 + 1];
					pDest[i*4 + 2] = pImage[i*4]; 
				}

				if( *(DWORD*)(&pDest[i*4]) == mask ) {
					pDest[i*4] = (BYTE)0;
					pDest[i*4 + 1] = (BYTE)0;
					pDest[i*4 + 2] = (BYTE)0; 
					pDest[i*4 + 3] = (BYTE)0;
					bAlphaChannel = true;
				}
			}//end for

			TImageInfo* data = new TImageInfo;
			data->hBitmap = hBitmap;
			data->nX = x;
			data->nY = y;
			data->delay = delay;
			data->alphaChannel = bAlphaChannel;
			pGifHandler->AddFrameInfo(data);

			stbi_image_free(pImage);
			pImage = NULL;

		}//end for
		stbi_image_free(pGifInfos);
		return pGifHandler;
	}

	void CGifControlUI::PaintBkImage( HDC hDC )
	{
		if(bGif==false) // 不是Gif动画，使用普通绘图模式
		{
			CControlUI::PaintBkImage(hDC);
			return;
		}

		if (m_pGifHandler==NULL)
			return ;
		TImageInfo* pCurrentFrame=m_pGifHandler->GetNextFrameInfo();

		if( !pCurrentFrame )
			return;

		CDuiString sImageResType;
		RECT rcItem = m_rcItem;
		RECT rcBmpPart = {0};
		RECT rcCorner = {0};
		DWORD dwMask = 0;
		BYTE bFade = 0xFF;
		bool bHole = false;
		bool bTiledX = false;
		bool bTiledY = false;

		if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
			rcBmpPart.right = pCurrentFrame->nX;
			rcBmpPart.bottom = pCurrentFrame->nY;
		}
		if (rcBmpPart.right > pCurrentFrame->nX) rcBmpPart.right = pCurrentFrame->nX;
		if (rcBmpPart.bottom > pCurrentFrame->nY) rcBmpPart.bottom = pCurrentFrame->nY;

		CRenderEngine::DrawImage(hDC, pCurrentFrame->hBitmap, rcItem, m_rcPaint, rcBmpPart, rcCorner, pCurrentFrame->alphaChannel, bFade, bHole, bTiledX, bTiledY);

		TImageInfo* pNextFrame=m_pGifHandler->GetFrameInfoAt(m_iCurrentFrame++);
		if (m_pGifHandler->IsAnimation() && pNextFrame)	// 如果帧数不大于1，不启动动画
		{
			this->GetManager()->SetTimer(this,GIFCONTROL_TIMERID,pNextFrame->delay ==0 ? 100 : pNextFrame->delay);
		}
	}

	void CGifControlUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_TIMER && event.wParam == GIFCONTROL_TIMERID )
		{
			m_iCurrentFrame++;
			this->Invalidate();
		}
		else
			CControlUI::DoEvent(event);
	}

	void CGifControlUI::SetVisible( bool bVisible /*= true */ )
	{
		if (m_bVisible==bVisible)
			return;
		m_bVisible=bVisible;
		if (m_bVisible==false)
			this->GetManager()->KillTimer(this,GIFCONTROL_TIMERID);
		else
		{
			if ( m_pGifHandler == NULL)
				return;
			TImageInfo* pNextImage=m_pGifHandler->GetNextFrameInfo();
			if (pNextImage==NULL)
				return;
			this->GetManager()->SetTimer(this,GIFCONTROL_TIMERID,pNextImage->delay ==0 ? 100 : pNextImage->delay);
		}
	}

	void CGifControlUI::Start()
	{
		if (m_pGifHandler && m_pGifHandler->IsAnimation())
			this->GetManager()->SetTimer(this,GIFCONTROL_TIMERID,1);
	}

	void CGifControlUI::Stop()
	{
		this->GetManager()->KillTimer(this,GIFCONTROL_TIMERID);
	}

	//////////////////////////////////////////////////////////////////////////
	// CGifHandler

	CGifHandler::CGifHandler()
		: nCurrentFrame(0)
		, nFrameCount(0)
		, bAnimation(false)
		, pFirstFrame(NULL)
	{
	}

	CGifHandler::~CGifHandler()
	{
		for(int i=0; i<ImageInfos.GetSize(); i++)
		{
			CRenderEngine::FreeImage((TImageInfo *)ImageInfos.GetAt(i));
		}
	}

	int CGifHandler::GetFrameCount()
	{
		return nFrameCount;
	}

	void CGifHandler::AddFrameInfo(TImageInfo* pFrameInfo)
	{
		if (pFrameInfo)
		{
			if (nFrameCount==0)
				pFirstFrame=pFrameInfo;

			ImageInfos.Add(pFrameInfo);
			nFrameCount++;

			if (nFrameCount>1)
				bAnimation=true;
		}
	}

	TImageInfo* CGifHandler::GetNextFrameInfo()
	{
		if (bAnimation)
		{
			int n = nCurrentFrame++;
			if (nCurrentFrame >= nFrameCount)
			{
				nCurrentFrame = 0;
			}

			return (TImageInfo *)ImageInfos.GetAt(n);
		}
		else
			return pFirstFrame;

		return NULL;
	}

	TImageInfo* CGifHandler::GetCurrentFrameInfo()
	{
		if (bAnimation)
			return (TImageInfo *)ImageInfos.GetAt(nCurrentFrame);
		else
			return pFirstFrame;
	}

	TImageInfo* CGifHandler::GetFrameInfoAt(int index)
	{
		if (bAnimation)
		{
			if(index>=0 && index <=nFrameCount)
			{	
				return (TImageInfo *)ImageInfos.GetAt(index);
			}
		}
		else
			return pFirstFrame;
		return NULL;
	}

	bool CGifHandler::IsAnimation( void )
	{
		return bAnimation;
	}
}