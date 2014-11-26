#include "stdafx.h"
#include "UIUtils.h"
#include <algorithm>
#include <xfunctional>
#include <locale>

/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiPoint::CDuiPoint()
{
	x = y = 0;
}

CDuiPoint::CDuiPoint(const POINT& src)
{
	x = src.x;
	y = src.y;
}

CDuiPoint::CDuiPoint(int _x, int _y)
{
	x = _x;
	y = _y;
}

CDuiPoint::CDuiPoint(LPARAM lParam)
{
	x = GET_X_LPARAM(lParam);
	y = GET_Y_LPARAM(lParam);
}

void CDuiPoint::SetPoint( int X, int Y )
{
	this->x = x;
	this->y	= y;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiSize::CDuiSize()
{
	cx = cy = 0;
}

CDuiSize::CDuiSize(const SIZE& src)
{
	cx = src.cx;
	cy = src.cy;
}

CDuiSize::CDuiSize(const RECT rc)
{
	cx = rc.right - rc.left;
	cy = rc.bottom - rc.top;
}

CDuiSize::CDuiSize(int _cx, int _cy)
{
	cx = _cx;
	cy = _cy;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiRect::CDuiRect()
{
	left = top = right = bottom = 0;
}

CDuiRect::CDuiRect(const RECT& src)
{
	left = src.left;
	top = src.top;
	right = src.right;
	bottom = src.bottom;
}

CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
{
	left = iLeft;
	top = iTop;
	right = iRight;
	bottom = iBottom;
}

CDuiRect::CDuiRect(LPCRECT lpRect)
{
	if ( lpRect != NULL)
	{
		left = lpRect->left;
		top = lpRect->top;
		right = lpRect->right;
		bottom = lpRect->bottom;
	}
	else
	{
		left = top = right = bottom = 0;
	}
}

int CDuiRect::GetWidth() const
{
	return right - left;
}

int CDuiRect::GetHeight() const
{
	return bottom - top;
}

void CDuiRect::Empty()
{
	left = top = right = bottom = 0;
}

bool CDuiRect::IsNull() const
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0); 
}

void CDuiRect::Join(const RECT& rc)
{
	if( rc.left < left )
		left = rc.left;
	if( rc.top < top )
		top = rc.top;
	if( rc.right > right )
		right = rc.right;
	if( rc.bottom > bottom )
		bottom = rc.bottom;
}

void CDuiRect::ResetOffset()
{
	::OffsetRect(this, -left, -top);
}

void CDuiRect::Normalize()
{
	if( left > right )
	{
		int iTemp = left;
		left = right;
		right = iTemp;
	}
	if( top > bottom ) 
	{ 
		int iTemp = top; 
		top = bottom;
		bottom = iTemp; 
	}
}

void CDuiRect::Offset(int cx, int cy)
{
	::OffsetRect(this, cx, cy);
}

void CDuiRect::Inflate(int cx, int cy)
{
	::InflateRect(this, cx, cy);
}

void CDuiRect::Deflate(int cx, int cy)
{
	::InflateRect(this, -cx, -cy);
}

void CDuiRect::Union(CDuiRect& rc)
{
	::UnionRect(this, this, &rc);
}

bool CDuiRect::PtInRect( POINT pt )
{
	return ::PtInRect(this,pt) ? true : false ;
}

CDuiPoint CDuiRect::CenterPoint( void )
{
	return CDuiPoint((left+right)/2, (top+bottom)/2);
}

bool CDuiRect::IntersectRect(LPCRECT lpRect1,LPCRECT lpRect2)
{
	return ::IntersectRect(this,lpRect1,lpRect2) == 0 ? false : true ;
}

bool CDuiRect::IsRectEmpty()
{
	return ::IsRectEmpty(this) == 0 ? false : true ;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 )
		m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
{
	for(int i=0; i<src.GetSize(); i++)
		Add(src.GetAt(i));
}

CStdPtrArray::~CStdPtrArray()
{
	if( m_ppVoid != NULL )
		free(m_ppVoid);
}

void CStdPtrArray::Empty()
{
	if( m_ppVoid != NULL )
		free(m_ppVoid);
	m_ppVoid = NULL;
	m_nCount = m_nAllocated = 0;
}

void CStdPtrArray::Resize(int iSize)
{
	Empty();
	m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
	::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
	m_nAllocated = iSize;
	m_nCount = iSize;
}

bool CStdPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CStdPtrArray::Add(LPVOID pData)
{
	if( ++m_nCount >= m_nAllocated)
	{
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 )
			nAllocated = 8;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL )
		{
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else
		{
			--m_nCount;
			return false;
		}
	}
	m_ppVoid[m_nCount - 1] = pData;
	return true;
}

bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
{
	if( iIndex == m_nCount )
		return Add(pData);
	if( iIndex < 0 || iIndex > m_nCount )
		return false;
	if( ++m_nCount >= m_nAllocated)
	{
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 )
			nAllocated = 11;
		LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
		if( ppVoid != NULL )
		{
			m_nAllocated = nAllocated;
			m_ppVoid = ppVoid;
		}
		else
		{
			--m_nCount;
			return false;
		}
	}
	memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
{
	if( iIndex < 0 || iIndex >= m_nCount )
		return false;
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::Remove(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount )
		return false;
	if( iIndex < --m_nCount )
		::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return true;
}

int CStdPtrArray::Find(LPVOID pData) const
{
	for( int i = 0; i < m_nCount; i++ )
	{
		if( m_ppVoid[i] == pData )
			return i;
	}
	return -1;
}

int CStdPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CStdPtrArray::GetData()
{
	return m_ppVoid;
}

LPVOID CStdPtrArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount )
		return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CStdPtrArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_ppVoid[iIndex];
}

void CStdPtrArray::Sort(int (*sortfunc)(LPVOID p1,LPVOID p2))
{
	for(int i=0;i<m_nCount;i++)
	{
		if( i < m_nCount - 1)
		{
			int iRet = (*sortfunc)(m_ppVoid[i],m_ppVoid[i+1]);
			if( i > 0 )
			{
				void* p =m_ppVoid[i];
				m_ppVoid[i] = m_ppVoid[i+1];
				m_ppVoid[i+1] = p;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
	m_nCount(0), 
	m_iElementSize(iElementSize), 
	m_nAllocated(iPreallocSize)
{
	ASSERT(iElementSize>0);
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 )
		m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
}

CStdValArray::~CStdValArray()
{
	if( m_pVoid != NULL )
		free(m_pVoid);
}

void CStdValArray::Empty()
{   
	m_nCount = 0;  // NOTE: We keep the memory in place
}

bool CStdValArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CStdValArray::Add(LPCVOID pData)
{
	if( ++m_nCount >= m_nAllocated)
	{
		int nAllocated = m_nAllocated * 2;
		if( nAllocated == 0 ) nAllocated = 11;
		LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
		if( pVoid != NULL )
		{
			m_nAllocated = nAllocated;
			m_pVoid = pVoid;
		}
		else
		{
			--m_nCount;
			return false;
		}
	}
	::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
	return true;
}

bool CStdValArray::Remove(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount )
		return false;
	if( iIndex < --m_nCount )
		::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
	return true;
}

int CStdValArray::GetSize() const
{
	return m_nCount;
}

LPVOID CStdValArray::GetData()
{
	return static_cast<LPVOID>(m_pVoid);
}

LPVOID CStdValArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount )
		return NULL;
	return m_pVoid + (iIndex * m_iElementSize);
}

LPVOID CStdValArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_pVoid + (iIndex * m_iElementSize);
}

//////////////////////////////////////////////////////////////////////////
// 

static UINT HashKey(LPCTSTR Key)
{
	UINT i = 0;
	SIZE_T len = _tcslen(Key);
	while( len-- > 0 ) i = (i << 5) + i + Key[len];
	return i;
}

static UINT HashKey(const CDuiString& Key)
{
	return HashKey((LPCTSTR)Key.data());
};

CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
{
	if( nSize < 16 ) nSize = 16;
	m_nBuckets = nSize;
	m_aT = new TITEM*[nSize];
	memset(m_aT, 0, nSize * sizeof(TITEM*));
}

CStdStringPtrMap::~CStdStringPtrMap()
{
	if( m_aT )
	{
		int len = m_nBuckets;
		while( len-- )
		{
			TITEM* pItem = m_aT[len];
			while( pItem )
			{
				TITEM* pKill = pItem;
				pItem = pItem->pNext;
				delete pKill;
			}
		}
		delete [] m_aT;
		m_aT = NULL;
	}
}

void CStdStringPtrMap::RemoveAll()
{
	this->Resize(m_nBuckets);
}

void CStdStringPtrMap::Resize(int nSize)
{
	if( m_aT )
	{
		int len = m_nBuckets;
		while( len-- )
		{
			TITEM* pItem = m_aT[len];
			while( pItem )
			{
				TITEM* pKill = pItem;
				pItem = pItem->pNext;
				delete pKill;
			}
		}
		delete [] m_aT;
		m_aT = NULL;
	}

	if( nSize < 0 )
		nSize = 0;
	if( nSize > 0 )
	{
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	} 
	m_nBuckets = nSize;
	m_nCount = 0;
}

LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
{
	if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

	UINT slot = HashKey(key) % m_nBuckets;
	for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext )
	{
		if( pItem->Key == key )
		{
			if (optimize && pItem != m_aT[slot])
			{
				if (pItem->pNext)
				{
					pItem->pNext->pPrev = pItem->pPrev;
				}
				pItem->pPrev->pNext = pItem->pNext;
				pItem->pPrev = NULL;
				pItem->pNext = m_aT[slot];
				pItem->pNext->pPrev = pItem;
				//将item移动至链条头部
				m_aT[slot] = pItem;
			}
			return pItem->Data;
		}        
	}

	return NULL;
}

bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
{
	if( m_nBuckets == 0 )
		return false;
	if( Find(key) )
		return false;

	// Add first in bucket
	UINT slot = HashKey(key) % m_nBuckets;
	TITEM* pItem = new TITEM;
	pItem->Key = key;
	pItem->Data = pData;
	pItem->pPrev = NULL;
	pItem->pNext = m_aT[slot];
	if (pItem->pNext)
		pItem->pNext->pPrev = pItem;
	m_aT[slot] = pItem;
	m_nCount++;
	return true;
}

LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
{
	if( m_nBuckets == 0 ) return pData;

	if (GetSize()>0)
	{
		UINT slot = HashKey(key) % m_nBuckets;
		// Modify existing item
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext )
		{
			if( pItem->Key == key )
			{
				LPVOID pOldData = pItem->Data;
				pItem->Data = pData;
				return pOldData;
			}
		}
	}

	Insert(key, pData);
	return NULL;
}

bool CStdStringPtrMap::Remove(LPCTSTR key)
{
	if( m_nBuckets == 0 || GetSize() == 0 )
		return false;

	UINT slot = HashKey(key) % m_nBuckets;
	TITEM** ppItem = &m_aT[slot];
	while( *ppItem )
	{
		if( (*ppItem)->Key == key )
		{
			TITEM* pKill = *ppItem;
			*ppItem = (*ppItem)->pNext;
			if (*ppItem)
				(*ppItem)->pPrev = pKill->pPrev;
			delete pKill;
			m_nCount--;
			return true;
		}
		ppItem = &((*ppItem)->pNext);
	}

	return false;
}

int CStdStringPtrMap::GetSize() const
{
#if 0//def _DEBUG
	int nCount = 0;
	int len = m_nBuckets;
	while( len-- )
	{
		for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext )
			nCount++;
	}
	ASSERT(m_nCount==nCount);
#endif
	return m_nCount;
}

LPCTSTR CStdStringPtrMap::GetAt(int iIndex) const
{
	if( m_nBuckets == 0 || GetSize() == 0 )
		return false;

	int pos = 0;
	int len = m_nBuckets;
	while( len-- )
	{
		for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext )
		{
			if( pos++ == iIndex )
			{
				return pItem->Key.data();
			}
		}
	}

	return NULL;
}

LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
{
	return GetAt(nIndex);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiWaitCursor::CDuiWaitCursor()
{
	m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}

CDuiWaitCursor::~CDuiWaitCursor()
{
	::SetCursor(m_hOrigCursor);
}


StringOrID::StringOrID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
{

}

StringOrID::StringOrID(LPCTSTR lpString) : m_lpstr(lpString)
{

}


CDuiStringW CDuiCharsetConvert::MbcsToUnicode(LPCSTR lpcszString)
{
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	UINT nLength=strlen(lpcszString);
	int nChar = ::MultiByteToWideChar(codepage, 0, lpcszString, nLength, NULL, 0);

	wchar_t* pUnicode = new wchar_t[nChar + 1];
	ZeroMemory(pUnicode,(nChar + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(codepage, 0, lpcszString, nLength, (LPWSTR)pUnicode, nChar);
	CDuiStringW wString = (wchar_t*)pUnicode;
	delete [] pUnicode;

	return wString;
}

CDuiStringA CDuiCharsetConvert::MbcsToUTF8(LPCSTR lpcszString)
{
	return CDuiCharsetConvert::UnicodeToUTF8(CDuiCharsetConvert::MbcsToUnicode(lpcszString).c_str());
}

CDuiStringA CDuiCharsetConvert::UnicodeToMbcs(LPCWSTR lpwcszWString)
{
	UINT nLength=wcslen(lpwcszWString);
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
	int nChar = ::WideCharToMultiByte(codepage, 0, lpwcszWString, nLength, NULL, 0, NULL, NULL);

	char* pElementText = new char[nChar + 1];
	ZeroMemory(pElementText,(nChar+1)* sizeof(char) );
	::WideCharToMultiByte(codepage, 0, lpwcszWString, nLength, pElementText, nChar, NULL, NULL);
	CDuiStringA strReturn(pElementText);
	delete [] pElementText;
	return strReturn;
}

CDuiStringA CDuiCharsetConvert::UnicodeToUTF8(LPCWSTR lpwcszWString)
{
	UINT nLength=wcslen(lpwcszWString);
	int nChar = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, nLength, NULL, 0, NULL, NULL);
	char* pElementText = new char[nChar + 1];
	ZeroMemory(pElementText, (nChar + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, nLength, pElementText, nChar, NULL, NULL);
	CDuiStringA strReturn(pElementText);
	delete [] pElementText;
	return strReturn;
}

CDuiStringW CDuiCharsetConvert::UTF8ToUnicode(LPCSTR lpcszString)
{
	UINT nLength=strlen(lpcszString);
	int nChar = ::MultiByteToWideChar(CP_UTF8, 0, lpcszString, nLength, NULL, 0);
	wchar_t* pUnicode = new wchar_t[nChar + 1];
	ZeroMemory(pUnicode,(nChar + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, lpcszString, nLength, (LPWSTR)pUnicode, nChar);
	CDuiStringW wstrReturn(pUnicode);
	delete [] pUnicode;
	return wstrReturn;
}

CDuiStringA CDuiCharsetConvert::UTF8ToMbcs(LPCSTR lpcszString)
{
	return CDuiCharsetConvert::UnicodeToMbcs(CDuiCharsetConvert::UTF8ToUnicode(lpcszString).c_str());
}

#ifdef _UNICODE
// TCHAR Unicode

CDuiString CDuiCharsetConvert::UTF8ToTCHAR(LPCSTR lpcszString)
{
	return UTF8ToUnicode(lpcszString);
}

#else
// TCHAR Mbcs

CDuiString CDuiCharsetConvert::UTF8ToTCHAR(LPCSTR lpcszString)
{
	return UTF8ToMbcs(lpcszString);
}

#endif // _UNICODE

//////////////////////////////////////////////////////////////////////////
//

int CDuiStringOperation::compareNoCase(LPCTSTR left,LPCTSTR right)
{
	return _tcscmp(left,right);
}

int CDuiStringOperation::replace(CDuiString& content,LPCTSTR lpszOld,LPCTSTR lpszNew,bool bIsLoop /*= true*/)
{
	int i =0;
	int nOldLength = _tcsclen(lpszOld);

	while (true)  
	{  
		size_t pos = content.find(lpszOld);  
		if (pos != std::wstring::npos)  
		{  
			TCHAR pBuf[1]={_T('\0')};  
			content.replace(pos, nOldLength, pBuf, 0);  
			content.insert(pos, lpszNew);
			++i;
			if ( !bIsLoop )
				break;
		}  
		else  
		{  
			break;  
		}  
	}  

	return i; 
}

void CDuiStringOperation::lower(CDuiString &str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}

void CDuiStringOperation::upper(CDuiString &str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper); 
}

bool CDuiStringOperation::parseAttributeString(LPCTSTR lpszAttributeString,StringMap& attributeMap)
{
	VecString vec;
	splite(lpszAttributeString,_T(" "),vec);
	size_t count = vec.size();
	if ( count == 1 )
		return false;
	
	for ( size_t i = 0 ;i < count ; ++i)
	{
		VecString attribute;
		splite(vec[i].c_str(),_T("="),attribute);
		if ( attribute.size() == 2 )
		{
			attributeMap[attribute[0]] =  attribute[1];
			trim(attributeMap[attribute[0]]);
		}
	}

	return true;
}

void CDuiStringOperation::splite(LPCTSTR lpszContent,LPCTSTR lpszDelim,VecString& vec)
{
	CDuiString s(lpszContent);
	CDuiString delim(lpszDelim);

	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		vec.push_back(s.substr(last,index-last));
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		vec.push_back(s.substr(last,index-last));
	}
}

void CDuiStringOperation::trim(CDuiString&content)
{
	content.erase(0, content.find_first_not_of(_T("' \n\r\t")));  
	content.erase(content.find_last_not_of(_T("' \n\r\t")) + 1); 
}

LPCTSTR CDuiStringOperation::FindAttrubuteKey(AttributeMap& attributeMap,LPCTSTR lpszkey)
{
	AttributeMap::iterator iter = attributeMap.find(lpszkey);
	if (iter != attributeMap.end())
	{
		return iter->second.c_str();
	}

	return NULL;
}

CDuiString CDuiStringOperation::format(LPCTSTR lpszFormat, ...)
{
	CDuiString sFormat = lpszFormat;
	// Do ordinary printf replacements
	// NOTE: Documented max-length of wvsprintf() is 1024
	TCHAR szBuffer[1025] = { 0 };
	va_list argList;
	va_start(argList, lpszFormat);
	int iRet = ::wvsprintf(szBuffer, sFormat.c_str(), argList);
	va_end(argList);
	szBuffer[iRet] = _T('\0');
	return szBuffer;
}

//////////////////////////////////////////////////////////////////////////
// 

bool CDuiFileOperation::readFile(LPCTSTR lpszFilePath,CBufferPtr& buffer)
{
	HANDLE hFile=::CreateFile(lpszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile!=INVALID_HANDLE_VALUE && hFile!=NULL)
	{
		DWORD dwFileSize=::GetFileSize(hFile,NULL);

		buffer.Realloc(dwFileSize+1,true);
		
		DWORD dwRead=0;
		if (::ReadFile(hFile,buffer,dwFileSize,&dwRead,NULL) && dwRead==buffer.Size()-1)
		{
			buffer[dwRead]='\0';
			::CloseHandle(hFile);
			hFile=NULL;
			return true;
		}
		else
		{
			::CloseHandle(hFile);
			hFile=NULL;
			return false;
		}
	}
	return false;
}

ULONG CDuiCodeOperation::HexStringToULong(LPCWSTR lpszValue, int nSize /*= -1*/)
{
	LPCWSTR pchValue = lpszValue;
	ULONG ulValue = 0;
	while (*pchValue && nSize != 0)
	{
		ulValue <<= 4;

		if ('a' <= *pchValue && L'f' >= *pchValue)
			ulValue |= (*pchValue - L'a' + 10);
		else if ('A' <= *pchValue && L'F' >= *pchValue)
			ulValue |= (*pchValue - L'A' + 10);
		else if ('0' <= *pchValue && L'9' >= *pchValue)
			ulValue |= (*pchValue - L'0');
		else
			return 0;

		++ pchValue;
		-- nSize;
	}

	return ulValue;
}

COLORREF CDuiCodeOperation::HexStringToColor(LPCWSTR lpszValue)
{
	if(wcslen(lpszValue)>6)
	{
		return RGB(
			HexStringToULong(lpszValue + 2, 2),
			HexStringToULong(lpszValue + 4, 2),
			HexStringToULong(lpszValue + 6, 2)
			)| (HexStringToULong(lpszValue , 2)<<24);
	}
	else
	{
		return RGB(
			HexStringToULong(lpszValue, 2),
			HexStringToULong(lpszValue + 2, 2),
			HexStringToULong(lpszValue + 4, 2)
			)|0xFF000000;
	}
}

COLORREF CDuiCodeOperation::StringToColor(const LPCTSTR lpszColorStringstr)
{
	if(lpszColorStringstr == NULL)
		return 0;

	CDuiString str2=lpszColorStringstr;
	CDuiStringOperation::lower(str2);
	int r=0,g=0,b=0,a=255;
	int nSegs=0;
	if(str2[0] == L'#')
	{
		nSegs=swscanf(str2.c_str(),L"#%02x%02x%02x%02x",&a,&r,&g,&b);
	}

	return RGB(r,g,b)|(a<<24);
}

void CDuiCodeOperation::StringToRect(LPCTSTR lpszRectStr,LONG *l1,LONG *l2,LONG *l3,LONG *l4)
{
	LPTSTR pstr = NULL;
	*l1 = _tcstol(lpszRectStr, &pstr, 10);		ASSERT(pstr);
	*l2 = _tcstol(pstr + 1, &pstr, 10);			ASSERT(pstr);
	*l3 = _tcstol(pstr + 1, &pstr, 10);			ASSERT(pstr);
	*l4 = _tcstol(pstr + 1, &pstr, 10);			ASSERT(pstr);
}

void CDuiCodeOperation::StringToRect(LPCTSTR lpszRectStr,LPRECT pRect)
{
	LPTSTR pstr = NULL;
	pRect->left = _tcstol(lpszRectStr, &pstr, 10);		ASSERT(pstr);
	pRect->top = _tcstol(pstr + 1, &pstr, 10);			ASSERT(pstr);
	pRect->right = _tcstol(pstr + 1, &pstr, 10);		ASSERT(pstr);
	pRect->bottom = _tcstol(pstr + 1, &pstr, 10);	ASSERT(pstr);
}

void CDuiCodeOperation::StringToSize(LPCTSTR lpszSizeStr,LPSIZE pSize)
{
	LPTSTR pstr = NULL;
	pSize->cx = _tcstol(lpszSizeStr, &pstr, 10);	ASSERT(pstr);    
	pSize->cy = _tcstol(pstr + 1, &pstr, 10);			 ASSERT(pstr);     
}

int CDuiCodeOperation::StringToInt(LPCTSTR lpszNumber)
{
	
	return _ttoi(lpszNumber);
}

__int64 CDuiCodeOperation::StringToInt64(LPCTSTR lpszNumber)
{
	return _ttoi64(lpszNumber);
}

int CDuiCodeOperation::MbcsStrToInt(LPCSTR lpszNumber)
{
	return atoi(lpszNumber);
}

int CDuiCodeOperation::UnicodeStrToInt(LPCWSTR lpszNumber)
{
	return _wtoi(lpszNumber);
}
