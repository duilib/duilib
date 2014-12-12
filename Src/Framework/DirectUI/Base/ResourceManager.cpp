#include "stdafx.h"
#include "ResourceManager.h"
#include "TinyXml/tinyxml.h"
#include "Base/TemplateObject.h"
#include "Base/UIEngine.h"
#include "Control/ControlUI.h"

CResourceManager * CResourceManager::m_pInstace = NULL;
CUIEngine* CResourceManager::m_pEngine = NULL;  

CResourceManager::CResourceManager(void)
	: m_pDefaultFontObj(NULL)
{
	m_DefaultLangID = GetUserDefaultUILanguage();
	m_pDefaultFontObj = new FontObject;
}


CResourceManager::~CResourceManager(void)
{
	TemplateMap::iterator iter = m_mapLayouts.begin();
	TemplateMap::iterator end = m_mapLayouts.end();
	while (iter != end)
	{
		delete iter->second;
		++iter;
	}

	// 这句话是否执行不影响析构资源清理，跑到这里来的时候已经全都释放了
	RemoveAllCachedImage();
	RemoveAllFont();
}

CResourceManager * CResourceManager::GetInstance()
{
	if ( m_pInstace == NULL)
	{
		m_pInstace = new CResourceManager;
		m_pEngine = CUIEngine::GetInstance();
	}
	return m_pInstace;
}

void CResourceManager::ReleaseInstance()
{
	if ( m_pInstace != NULL)
	{
		delete m_pInstace;
		m_pInstace = NULL;
	}
}

LANGID CResourceManager::GetLanguage()
{
	return m_DefaultLangID;
}

void CResourceManager::InitResouceDir(LPCTSTR lpszPath,bool bIsDefault /*= false*/)
{
	// 1.检查目录合法性，必须存在一个 Resource.xml
	CString strResourceFilePath(lpszPath);
	strResourceFilePath+= _T("\\Resource.xml");
	if ( !::PathFileExists(strResourceFilePath) )
	{
		// TODO 可能需要输出警告
		return;
	}
	
	// 2.解析Resouce.xml
	// component作为key，文件路径作为value，插入m_mapComponent
	// LayoutFiles中，type是Window/View的保存到m_mapLayouts,是Font的直接插入m_vecFontPool
	// LanguageFiles中，按当前显示语言，操作系统加载，插入m_mapI18NPool
	// TODO 暂不支持 Template中，直接插入m_mapTemplates
	TiXmlDocument doc;
	if ( !doc.LoadFile(CDuiCharsetConvert::UnicodeToMbcs(strResourceFilePath).c_str(),TIXML_ENCODING_UTF8))
	{
		return;
	}

	TiXmlElement *pRootElement = doc.RootElement();
	LPCSTR pComponent = pRootElement->Attribute("component");
	if ( pComponent == NULL)
		return;

	CDuiString strComponentName = CDuiCharsetConvert::MbcsToUnicode(pComponent);
	m_mapComponent[strComponentName] = lpszPath;

	TiXmlElement *pLayouts = pRootElement->FirstChildElement("Layouts");
	if ( pLayouts!=NULL)
		parseLayouts(pLayouts, strComponentName.c_str());

	TiXmlElement *pFonts =  pRootElement->FirstChildElement("Fonts");
	if ( pFonts!=NULL)
		parseFonts(pFonts,strComponentName.c_str());

	TiXmlElement *pLanguages =  pRootElement->FirstChildElement("Languages");
	if ( pLanguages!=NULL)
		parseLanguages(pLanguages,strComponentName.c_str());

	// 3.是否为根资源路径
	if ( bIsDefault )
	{
		m_strDefaultResourcePath = lpszPath;
		// TODO 可能需要发送一个默认资源路径变更通知
	}
}

TemplateObject* CResourceManager::GetWindowTemplate(LPCTSTR lpszWindowTemplateName)
{
	TemplateMap::iterator iter;
	iter = m_mapLayouts.find(lpszWindowTemplateName);
	if ( iter != m_mapLayouts.end() )
	{
		return iter->second;
	}

	return NULL;
}

TemplateObject* CResourceManager::GetViewTemplate(LPCTSTR lpszViewTemplateName)
{
	return NULL;
}

void CResourceManager::FreeResource(LPCTSTR lpszComponentName)
{
	
}

bool CResourceManager::GetAbsolutePath(CDuiString& strFullPath,LPCTSTR lpszComponent,LPCTSTR lpszRelativePath)
{
	StringMap::iterator iter = m_mapComponent.find(lpszComponent);
	if ( iter != m_mapComponent.end())
	{
		strFullPath = lpszRelativePath;
		CDuiStringOperation::replace(strFullPath,_T("#"),_T("\\"));
		CDuiStringOperation::replace(strFullPath,lpszComponent,(iter->second).c_str());
		return true;
	}
	return false;
}

bool CResourceManager::GetAbsolutePath(CDuiString& strFullPath,LPCTSTR lpszRelativePath)
{
	CDuiString strTemp(lpszRelativePath);
	if ( strTemp.empty())
		return false;

	size_t iIndex = strTemp.find(_T('#'));
	if ( iIndex != strTemp.npos)
	{
		CDuiString strComponent = strTemp.substr(0,iIndex);
		StringMap::iterator iter = m_mapComponent.find(strComponent.c_str());
		if ( iter != m_mapComponent.end())
		{
			strFullPath = lpszRelativePath;
			CDuiStringOperation::replace(strFullPath,_T("#"),_T("\\"));
			CDuiStringOperation::replace(strFullPath,strComponent.c_str(),(iter->second).c_str(),false);
			return true;
		}
	}
	strFullPath = lpszRelativePath;
	return false;
}

void CResourceManager::parseLayouts(TiXmlElement * pLayouts, LPCTSTR lpszComponentName)
{
	TiXmlElement *pElement = pLayouts->FirstChildElement("File");
	do 
	{
		if ( pElement == NULL)
			break;

		do 
		{
			CDuiString strPath;
			CDuiString strName;

			LPCSTR pcstrAttributeVaule = NULL;

			pcstrAttributeVaule = pElement->Attribute("name");
			if ( pcstrAttributeVaule != NULL)
				strName = CDuiCharsetConvert::UTF8ToUnicode(pcstrAttributeVaule);
			else
				break;

			pcstrAttributeVaule = pElement->Attribute("path");
			if ( pcstrAttributeVaule != NULL)
				strPath = CDuiCharsetConvert::UTF8ToUnicode(pcstrAttributeVaule);
			else
				break;

			CDuiString strAbsolutePath;
			GetAbsolutePath(strAbsolutePath,lpszComponentName,strPath.c_str());
			if ( ::PathFileExists(strAbsolutePath.c_str()))
			{

				TemplateObject* pTemplate = this->parseXmlFile(strAbsolutePath.c_str());
				if ( pTemplate )
				{
					m_mapLayouts[strName] = pTemplate;
				}
			}
			
		} while (false);

		pElement = pElement->NextSiblingElement();
	} while (pElement != NULL);
}

void CResourceManager::parseFonts(TiXmlElement * pLayouts, LPCTSTR lpszComponentName)
{
	TiXmlElement *pElement = pLayouts->FirstChildElement("Font");
	do 
	{
		if ( pElement == NULL)
			break;
		LPCSTR lpcstr = pElement->Attribute("name");
		CDuiString strName;
		if (lpcstr)
		{
			strName = CDuiCharsetConvert::UTF8ToUnicode(lpcstr);
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("face");
		CDuiString strFace;
		if (lpcstr)
		{
			strFace = CDuiCharsetConvert::UTF8ToUnicode(lpcstr);
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("size");
		int nSize = 0;
		if (lpcstr)
		{
			nSize = CDuiCodeOperation::MbcsStrToInt(lpcstr);
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("bold");
		bool bBold = false;
		if (lpcstr)
		{
			bBold = CDuiCharsetConvert::UTF8ToUnicode(lpcstr) == CDuiString(L"true");
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("italic");
		bool bItalic = false;
		if (lpcstr)
		{
			bItalic = CDuiCharsetConvert::UTF8ToUnicode(lpcstr) == CDuiString(L"true");
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("underline");
		bool bUnderline = false;
		if (lpcstr)
		{
			bUnderline = CDuiCharsetConvert::UTF8ToUnicode(lpcstr) == CDuiString(L"true");
			lpcstr = nullptr;
		}

		lpcstr = pElement->Attribute("strikeout");
		bool bStrikeout = false;
		if (lpcstr)
		{
			bStrikeout = CDuiCharsetConvert::UTF8ToUnicode(lpcstr) == CDuiString(L"true");
			lpcstr = nullptr;
		}

		FontObject* pfo = new FontObject;
		pfo->m_bBold = bBold;
		pfo->m_bItalic = bItalic;
		pfo->m_bUnderline = bUnderline;
		pfo->m_nSize = nSize;
		pfo->m_bStrikeout = bStrikeout;
		pfo->m_IndexName = strName;
		pfo->m_FaceName = strFace;
		this->m_vecFontPool.push_back(pfo);

		pElement = pElement->NextSiblingElement();
	} while (pElement != NULL);
}

void CResourceManager::parseLanguages(TiXmlElement * pLayouts, LPCTSTR lpszComponentName)
{
	TiXmlElement *pElement = pLayouts->FirstChildElement("File");
	do 
	{
		if ( pElement == NULL)
			break;

		do 
		{
			LANGID wLangID = 0;
			CDuiString strPath;

			LPCSTR pcstrAttributeVaule = NULL;

			pcstrAttributeVaule = pElement->Attribute("langid");
			if ( pcstrAttributeVaule != NULL)
				wLangID = CDuiCodeOperation::MbcsStrToInt(pcstrAttributeVaule);
			else
				break;

			if ( wLangID != m_DefaultLangID)
				break;

			pcstrAttributeVaule = pElement->Attribute("path");
			if ( pcstrAttributeVaule != NULL)
				strPath = CDuiCharsetConvert::UTF8ToUnicode(pcstrAttributeVaule);
			else
				break;

			CDuiString strAbsolutePath;
			GetAbsolutePath(strAbsolutePath,lpszComponentName,strPath.c_str());
			if ( ::PathFileExists(strAbsolutePath.c_str()))
			{
				this->LoadI18NString(strAbsolutePath.c_str());
			}

		} while (false);

		pElement = pElement->NextSiblingElement();
	} while (pElement != NULL);
}

CControlUI * CResourceManager::CreateControlFromTemplate(TemplateObject *pTemplate,CWindowUI* pManager,CControlUI* pParent/*=NULL*/)
{
	CControlUI* pReturnControl = NULL;
	pReturnControl = m_pEngine->CreateControl(pTemplate->GetType());
	if ( pReturnControl != NULL )
	{
		// 控件创建成功，设置管理
		pReturnControl->SetManager(pManager,pParent);

		// 设置控件属性
		AttributeMap::iterator iter = pTemplate->m_mapAttribute.begin();
		AttributeMap::iterator end = pTemplate->m_mapAttribute.end();
		while (iter != end )
		{
			pReturnControl->SetAttribute(iter->first.c_str(),iter->second.c_str());
			++iter;
		}

		// 提供IContainter接口的布局控件才继续解析并创建子控件
		IContainerUI *pContainer = static_cast<IContainerUI*>(pReturnControl->GetInterface(_T("IContainer")));
		int nCount = pTemplate->GetChildCount();
		if ( pContainer != NULL )
		{
			// 有子控件
			for (int i=0;i<nCount;++i )
			{
				CControlUI *pChildControl = this->CreateControlFromTemplate(pTemplate->GetChild(i),pManager,pReturnControl);
				if ( pChildControl != NULL)
					pContainer->Add(pChildControl);
			}

			pReturnControl->SetManager(pManager,pParent);
		}
	}

	return pReturnControl;
}

TemplateObject* CResourceManager::parseXmlFile(LPCTSTR lpszFilePath)
{	
	TiXmlDocument doc;
	if ( !doc.LoadFile(CDuiCharsetConvert::UnicodeToMbcs(lpszFilePath).c_str(),TIXML_ENCODING_UNKNOWN))
	{
		ASSERT(false);
		return NULL;
	}

	TiXmlElement *pRootElement = doc.RootElement();
	return XmlToTemplate(pRootElement,NULL);
}

TemplateObject* CResourceManager::XmlToTemplate(TiXmlElement *pElement,TemplateObject* pParent)
{
	TemplateObject *pChildTemplateObj = new TemplateObject;
	pChildTemplateObj->SetType(CDuiCharsetConvert::MbcsToUnicode(pElement->Value()).c_str());

	TiXmlAttribute * pAttribute = pElement->FirstAttribute();
	while (pAttribute != NULL)
	{
		
		pChildTemplateObj->SetAttribute(
			CDuiCharsetConvert::UTF8ToUnicode(pAttribute->Name()).c_str(),
			CDuiCharsetConvert::UTF8ToUnicode(pAttribute->Value()).c_str()
			);
		pAttribute = pAttribute->Next();
	}

	TiXmlElement *pChildElement = pElement->FirstChildElement();
	while (pChildElement != NULL)
	{
		this->XmlToTemplate(pChildElement,pChildTemplateObj);

		pChildElement = pChildElement->NextSiblingElement();
	}

	if ( pParent == NULL)
	{
		return pChildTemplateObj;
	}
	else
	{
		pParent->InsertChild(pChildTemplateObj,-1);
	}

	return NULL;
}

LPCTSTR CResourceManager::GetI18N(LPCTSTR lpszName)
{
	StringMap::iterator iter = m_mapI18NCached.find(lpszName);
	if ( iter != m_mapI18NCached.end())
	{
		return iter->second.c_str();
	}

	return NULL;
}

void CResourceManager::SetLanguage(LANGID wLangID)
{
	m_DefaultLangID = wLangID;
}

void CResourceManager::LoadI18NString(LPCTSTR lpszFilePath)
{
	TiXmlDocument doc;
	if ( !doc.LoadFile(CDuiCharsetConvert::UnicodeToMbcs(lpszFilePath).c_str(),TIXML_ENCODING_UTF8))
	{
		return;
	}

	TiXmlElement *pRootElement = doc.RootElement();
	TiXmlElement *pStringElemet = pRootElement->FirstChildElement("String");

	do 
	{
		if ( pStringElemet == NULL)
			break;

		LPCSTR pName = pStringElemet->Attribute("name");
		if ( pName != NULL)
		{
			CDuiString strName = CDuiCharsetConvert::UTF8ToUnicode(pName);

			LPCSTR pVaule = pStringElemet->Value();
			if ( pVaule != NULL)
			{
				m_mapI18NCached[strName] = CDuiCharsetConvert::UTF8ToUnicode(pVaule);
			}
		}

		pStringElemet = pStringElemet->NextSiblingElement();
	} while (pStringElemet != NULL);
}

TImageData* CResourceManager::GetImage(LPCTSTR lpszImagePath,DWORD dwMask,bool bCached /*= true */)
{
	TImageData* pImageData = static_cast<TImageData*>(m_mapImageHash.Find(lpszImagePath));
	if( !pImageData )
	{
		pImageData = LoadImage(lpszImagePath, dwMask);
		if ( bCached )
		{
			if( !m_mapImageHash.Insert(lpszImagePath, pImageData) )
			{
				::DeleteObject(pImageData->hBitmap);
				delete pImageData;
				pImageData = NULL;
			}
		}
	}

	if ( pImageData != NULL )
	{
		if ( bCached == true )
			++pImageData->nRefCount;
		else
			pImageData->nRefCount = -1;
	}

	return pImageData;
}

TImageData* CResourceManager::LoadImage(LPCTSTR lpszFilePath,DWORD dwMask)
{
	LPBYTE pData = NULL;
	DWORD dwSize = 0;

	CDuiString strFullPath;
	if ( !GetAbsolutePath(strFullPath,lpszFilePath) ) // 返回false说明没找到 # 分隔符
	{
		// 
		CDuiString strDefaultDir = m_strDefaultResourcePath;
		strDefaultDir.append(_T("\\"));
		strDefaultDir.append(lpszFilePath);
		if ( ::PathFileExists(strDefaultDir.c_str()) )	// 拼接出的字符串不是文件路径
			strFullPath = strDefaultDir;
	}
	

	do 
	{
		HANDLE hFile = ::CreateFile(strFullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
			FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
			break;
		dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 )
			break;

		DWORD dwRead = 0;
		pData = new BYTE[ dwSize ];
		::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize )
		{
			delete[] pData;
			pData = NULL;
			break;
		}

	} while (0);

	while (!pData)
	{
		//读不到图片, 则直接去读取 lpszFilePath 指向的路径
		HANDLE hFile = ::CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
			FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
			break;
		dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 )
			break;

		DWORD dwRead = 0;
		pData = new BYTE[ dwSize ];
		::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize )
		{
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
	pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
	delete[] pData;
	if( !pImage )
	{
		return NULL;
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
		return NULL;
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

		if( *(DWORD*)(&pDest[i*4]) == dwMask )
		{
			pDest[i*4] = (BYTE)0;
			pDest[i*4 + 1] = (BYTE)0;
			pDest[i*4 + 2] = (BYTE)0; 
			pDest[i*4 + 3] = (BYTE)0;
			bAlphaChannel = true;
		}
	}

	stbi_image_free(pImage);

	TImageData* data = new TImageData;
	data->hBitmap = hBitmap;
	data->nX = x;
	data->nY = y;
	data->delay=0;
	data->alphaChannel = bAlphaChannel;
	data->nRefCount = 0;
	if( !data )
		return NULL;
	data->dwMask = dwMask;

	return data;
}

void CResourceManager::FreeImageData(TImageData* pData)
{
	if (pData->hBitmap)
	{
		::DeleteObject(pData->hBitmap) ; 
	}
	delete pData ;
}

void CResourceManager::FreeImage(LPCTSTR lpszImagePath)
{
	TImageData* pData = static_cast<TImageData*>(m_mapImageHash.Find(lpszImagePath));
	if( pData !=NULL )
	{
		--pData->nRefCount;
		if ( pData->nRefCount == 0)
		{
			FreeImageData(pData);
			m_mapImageHash.Remove(lpszImagePath);
		}
	}
}

void CResourceManager::RemoveAllCachedImage()
{
	int iCount = m_mapImageHash.GetSize();
	if ( iCount == 0)
		return;

	TImageData* data;
	for( int i = 0; i< iCount; i++ )
	{
		if(LPCTSTR key = m_mapImageHash.GetAt(i)) 
		{
			data = static_cast<TImageData*>(m_mapImageHash.Find(key, false));
			if (data)
				FreeImageData(data);
		}
	}
	//m_mapImageHash.RemoveAll();
}

FontObject*	CResourceManager::GetFont(LPCTSTR lpszFontName)
{
	ASSERT(lpszFontName);
	if (m_vecFontPool.size() == 0)
	{
		return m_pDefaultFontObj;
	}

	FontPoolVector::iterator it = m_vecFontPool.begin();
	for (; it != m_vecFontPool.end(); ++it)
	{
		if ((*it)->m_IndexName == lpszFontName)
		{
			return (*it);
		}
	}
	
	return m_pDefaultFontObj;
}

void CResourceManager::RemoveAllFont()
{
	if (m_vecFontPool.size() == 0)
	{
		return;
	}

	FontPoolVector::iterator it = m_vecFontPool.begin();
	for (; it != m_vecFontPool.end(); it++)
	{
		if ((*it))
		{
			delete (*it);
		}
	}

	m_vecFontPool.swap(FontPoolVector());
}

LPCTSTR CResourceManager::GetResouceDir(LPCTSTR lpszModuleName /*= NULL*/)
{
	if ( lpszModuleName == NULL)
		return m_strDefaultResourcePath.c_str();

	StringMap::iterator iter = m_mapComponent.find(lpszModuleName);
	if ( iter != m_mapComponent.end())
		return iter->second.c_str();

	return NULL;
}

void CResourceManager::SetDefaultFont(LPCTSTR lpszFaceName,int nSize /*= 12*/, bool bBold /*= false*/, bool bUnderline/*= false*/, bool bItalic/*= false */,bool bStrikeout/*= false*/)
{
	if ( m_pDefaultFontObj != NULL )
	{
		delete m_pDefaultFontObj;
		m_pDefaultFontObj = new FontObject;
	}

	m_pDefaultFontObj->m_FaceName = lpszFaceName;
	m_pDefaultFontObj->m_nSize = nSize;
	m_pDefaultFontObj->m_bBold = bBold;
	m_pDefaultFontObj->m_bUnderline = bUnderline;
	m_pDefaultFontObj->m_bItalic = bItalic;
	m_pDefaultFontObj->m_bStrikeout = bStrikeout;
}

FontObject* CResourceManager::GetDefaultFont(void)
{
	return m_pDefaultFontObj;
}
