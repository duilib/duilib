#include "StdAfx.h"
#include "UIGifControl.h"


namespace DuiLib
{
	UI_IMPLEMENT_DYNCREATE(CGifControlUI);
CGifControlUI::CGifControlUI()
    : m_pGifImage(NULL)
    , m_pPropertyItem(NULL)
    , m_pStream(NULL)
    , m_hGlobal(NULL)
    , m_nCurrentFrame(0)
    , m_nFrameCount(0)
{
}

CGifControlUI::~CGifControlUI()
{
    FreeImage();
}


LPCTSTR CGifControlUI::GetClass() const
{
    return _T("GifControlUI");
}

LPVOID CGifControlUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("GifControl")) == 0)
    {
        return static_cast<CGifControlUI *>(this);
    }

    return CControlUI::GetInterface(pstrName);
}

void CGifControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    CControlUI::SetAttribute(pstrName, pstrValue);
}

void CGifControlUI::SetBkImage(LPCTSTR pStrImage)
{
    if(_tcsicmp(m_sBkImage.c_str(), pStrImage) == 0)
    {
        return;
    }

    if(m_pGifImage)
    {
        this->GetManager()->KillTimer(this, GIFCONTROL_TIMERID);
        FreeImage();
    }

    m_sBkImage = pStrImage;
    CDuiString strExt = m_sBkImage.substr(m_sBkImage.size() - 4);

    if(CDuiStringOperation::compareNoCase(strExt.c_str(), _T(".gif")) == 0)
    {
        // 扩展名是一张Gif图片
        LoadGif(StringOrID(m_sBkImage.c_str()));
    }

    Invalidate();
}

void CGifControlUI::LoadGif(StringOrID bitmap, LPCTSTR type/*=NULL*/, DWORD mask/*= 0 */)
{
    DWORD dwSize = 0;
	CBufferPtr m_dataBuffer;

	GetResEngine()->GetRes(m_dataBuffer,bitmap.m_lpstr);
    
	/*while (!m_dataBuffer.IsValid())
    {
        CDuiString sFile = CUIEngine::GetInstance()->GetResourcePath();

        if(true)
        {
            sFile += bitmap.m_lpstr;
            HANDLE hFile = ::CreateFile(sFile.c_str(),
                                        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, NULL);

            if(hFile == INVALID_HANDLE_VALUE)
            {
                break;
            }

            dwSize = ::GetFileSize(hFile, NULL);

            if(dwSize == 0)
            {
                break;
            }

            DWORD dwRead = 0;
			m_dataBuffer.Realloc(dwSize,true);
            ::ReadFile(hFile, m_dataBuffer, dwSize, &dwRead, NULL);
            ::CloseHandle(hFile);

            if(dwRead != dwSize)
            {
                m_dataBuffer.Free();
                break;
            }
        }
    }*/

    while(!m_dataBuffer.IsValid())
    {
        //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                                    FILE_ATTRIBUTE_NORMAL, NULL);

        if(hFile == INVALID_HANDLE_VALUE)
        {
            break;
        }

        dwSize = ::GetFileSize(hFile, NULL);

        if(dwSize == 0)
        {
            break;
        }

        DWORD dwRead = 0;
       m_dataBuffer.Realloc(dwSize,true);
        ::ReadFile(hFile, m_dataBuffer, dwSize, &dwRead, NULL);
        ::CloseHandle(hFile);

        if(dwRead != dwSize)
        {
            m_dataBuffer.Free();
        }

        break;
    }

    if(!m_dataBuffer.IsValid())
    {
        //::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
        return;
    }

    {
		dwSize = m_dataBuffer.Size();
        m_hGlobal  =  ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
        void   *pMemData  =  ::GlobalLock(m_hGlobal);
        memcpy(pMemData, m_dataBuffer, dwSize);
        ::GlobalUnlock(m_hGlobal);

        do
        {
            if(::CreateStreamOnHGlobal(m_hGlobal, TRUE,  & m_pStream)  !=  S_OK)
            {
                break;
            }

            m_pGifImage = Gdiplus::Image::FromStream(m_pStream);
        }
        while(false);
    }

    if(m_pGifImage == NULL)
    {
        return;
    }

    UINT nCount = m_pGifImage->GetFrameDimensionsCount();
    GUID *pDimensionIDs =   new GUID[ nCount ];
    m_pGifImage->GetFrameDimensionsList(pDimensionIDs, nCount);
    m_nFrameCount   =   m_pGifImage->GetFrameCount(&pDimensionIDs[0]);
    int nSize       =   m_pGifImage->GetPropertyItemSize(PropertyTagFrameDelay);
    m_pPropertyItem = (Gdiplus::PropertyItem *) malloc(nSize);
    m_pGifImage->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
    delete  pDimensionIDs;
    pDimensionIDs = NULL;
    long lPause = ((long *) m_pPropertyItem->value)[m_nCurrentFrame] * 10;

    if(lPause == 0)
    {
        lPause = 100;
    }

    m_pManager->SetTimer(this, GIFCONTROL_TIMERID, lPause);
    return;
}

void CGifControlUI::PaintBkImage(HDC hDC)
{
    if(m_pGifImage == NULL)
    {
        return ;
    }

    GUID pageGuid = Gdiplus::FrameDimensionTime;
    Gdiplus::Graphics graphics(hDC);
    graphics.DrawImage(m_pGifImage, m_rcItem.left, m_rcItem.top);
    m_pGifImage->SelectActiveFrame(&pageGuid, m_nCurrentFrame);
}

void CGifControlUI::EventHandler(TEventUI & event)
{
    if(event.dwType == UIEVENT_TIMER && event.wParam == GIFCONTROL_TIMERID)
    {
        m_pManager->KillTimer(this, GIFCONTROL_TIMERID);
        m_nCurrentFrame++;

        if(m_nCurrentFrame == m_nFrameCount)
        {
            m_nCurrentFrame = 0;
        }

        this->Invalidate();
        long lPause = ((long *) m_pPropertyItem->value)[m_nCurrentFrame] * 10;

        if(lPause == 0)
        {
            lPause = 100;
        }

        m_pManager->SetTimer(this, GIFCONTROL_TIMERID, lPause);
    }
    else
    {
        CControlUI::EventHandler(event);
    }
}

void CGifControlUI::SetVisible(bool bVisible /*= true */)
{
    if(m_bVisible == bVisible)
    {
        return;
    }

    m_bVisible = bVisible;

    if(m_bVisible == false)
    {
        this->GetManager()->KillTimer(this, GIFCONTROL_TIMERID);
    }
    else
    {
        if(m_pGifImage == NULL)
        {
            return;
        }

        long lPause = ((long *) m_pPropertyItem->value)[m_nCurrentFrame] * 10;

        if(lPause == 0)
        {
            lPause = 100;
        }

        m_pManager->SetTimer(this, GIFCONTROL_TIMERID, lPause);
    }
}

void CGifControlUI::Start()
{
    if(m_pGifImage && m_nFrameCount > 0)
    {
        this->GetManager()->SetTimer(this, GIFCONTROL_TIMERID, 1);
    }
}

void CGifControlUI::Stop()
{
    this->GetManager()->KillTimer(this, GIFCONTROL_TIMERID);
}

void CGifControlUI::FreeImage()
{
    if(m_pGifImage)
    {
        delete m_pGifImage;
        m_pGifImage = NULL;
    }

    if(m_pPropertyItem != NULL)
    {
        free(m_pPropertyItem);
        m_pPropertyItem = NULL;
    }

    m_nFrameCount       =   0;
    m_nCurrentFrame =   0;
    ::GlobalFree(m_hGlobal);
}

}