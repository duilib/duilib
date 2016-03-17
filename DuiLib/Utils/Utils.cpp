#include "stdafx.h"
#include "Utils.h"

namespace DuiLib
{
    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    STRINGorID::STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
    {
    }

    STRINGorID::STRINGorID(unsigned int nID) : m_lpstr(MAKEINTRESOURCE(nID))
    {
    }

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

    CDuiChar::CDuiChar() : m_cValue(0)
    {
    }

    CDuiChar::CDuiChar(char cValue)
    {
        m_cValue = cValue;
    }

    CDuiChar::CDuiChar(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_cValue = 0;
        else m_cValue = (char)_ttol(pstrValue);
    }

    CDuiString CDuiChar::ToString()
    {
        TCHAR tBuffer[16] = {0};
        long lValue = m_cValue;
        _ltot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiByte::CDuiByte() : m_byValue(0)
    {
    }

    CDuiByte::CDuiByte(byte byValue)
    {
        m_byValue = byValue;
    }

    CDuiByte::CDuiByte(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_byValue = 0;
        else m_byValue = (byte)_ttol(pstrValue);
    }

    CDuiString CDuiByte::ToString()
    {
        TCHAR tBuffer[16] = {0};
        unsigned long ulValue = m_byValue;
        _ultot(ulValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiTChar::CDuiTChar() : m_tValue(_T(' '))
    {
    }

    CDuiTChar::CDuiTChar(TCHAR tValue)
    {
        m_tValue = tValue;
    }

    CDuiTChar::CDuiTChar(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_tValue = _T(' ');
        else m_tValue = pstrValue[0];
    }

    CDuiString CDuiTChar::ToString()
    {
        return m_tValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiWord::CDuiWord() : m_wValue(0)
    {
    }

    CDuiWord::CDuiWord(WORD wValue)
    {
        m_wValue = wValue;
    }

    CDuiWord::CDuiWord(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_wValue = 0;
        else m_wValue = (WORD)_ttol(pstrValue);
    }

    CDuiString CDuiWord::ToString()
    {
        TCHAR tBuffer[16] = {0};
        unsigned long lValue = m_wValue;
        _ultot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiDWord::CDuiDWord() : m_dwValue(0)
    {
    }

    CDuiDWord::CDuiDWord(DWORD dwValue)
    {
        m_dwValue = dwValue;
    }

    CDuiDWord::CDuiDWord(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_dwValue = 0;
        else m_dwValue = (DWORD)_ttoi64(pstrValue);
    }

    CDuiString CDuiDWord::ToString()
    {
        TCHAR tBuffer[32] = {0};
        unsigned long lValue = m_dwValue;
        _ultot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiInt::CDuiInt() : m_iValue(0)
    {
    }

    CDuiInt::CDuiInt(int iValue)
    {
        m_iValue = iValue;
    }

    CDuiInt::CDuiInt(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_iValue = 0;
        else m_iValue = (int)_ttoi(pstrValue);
    }

    CDuiString CDuiInt::ToString()
    {
        TCHAR tBuffer[32] = {0};
        int lValue = m_iValue;
        _itot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiUInt::CDuiUInt() : m_uValue(0)
    {
    }

    CDuiUInt::CDuiUInt(unsigned int uValue)
    {
        m_uValue = uValue;
    }

    CDuiUInt::CDuiUInt(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_uValue = 0;
        else m_uValue = (unsigned int)_ttoi64(pstrValue);
    }

    CDuiString CDuiUInt::ToString()
    {
        TCHAR tBuffer[32] = {0};
        unsigned int lValue = m_uValue;
        _ui64tot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiLong::CDuiLong() : m_lValue(0)
    {
    }

    CDuiLong::CDuiLong(long lValue)
    {
        m_lValue = lValue; 
    }

    CDuiLong::CDuiLong(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_lValue = 0;
        else m_lValue = (long)_ttol(pstrValue);
    }

    CDuiString CDuiLong::ToString()
    {
        TCHAR tBuffer[32] = {0};
        long lValue = m_lValue;
        _ltot(lValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiULong::CDuiULong() : m_ulValue(0)
    {
    }

    CDuiULong::CDuiULong(unsigned long ulValue)
    {
        m_ulValue = ulValue;
    }

    CDuiULong::CDuiULong(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_ulValue = 0;
        else m_ulValue = (unsigned long)_ttoi64(pstrValue);
    }

    CDuiString CDuiULong::ToString()
    {
        TCHAR tBuffer[32] = {0};
        _ultot(m_ulValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiInt64::CDuiInt64() : m_iiValue(0)
    {
    }
    
    CDuiInt64::CDuiInt64(__int64 iiValue)
    {
        m_iiValue = iiValue;
    }

    CDuiInt64::CDuiInt64(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) m_iiValue = 0;
        else m_iiValue = _ttoi64(pstrValue);
    }

    CDuiString CDuiInt64::ToString()
    {
        TCHAR tBuffer[32] = {0};
        _i64tot(m_iiValue, tBuffer, 10);
        return tBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CDuiBool::CDuiBool() : m_bValue(false)
    {
    }

    CDuiBool::CDuiBool(bool bValue)
    {
        m_bValue = bValue;
    }

    CDuiBool::CDuiBool(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL) m_bValue = false;
        else m_bValue = (_tcsicmp(pstrValue, _T("true")) == 0);
    }

    CDuiString CDuiBool::ToString()
    {
        if (m_bValue) return _T("true");
        else return _T("false");
    }

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

	CDuiPoint::CDuiPoint(long _x, long _y)
	{
		x = _x;
		y = _y;
	}

	CDuiPoint::CDuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}

    CDuiPoint::CDuiPoint(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) x = y = 0;
        LPTSTR pstr = NULL;
        x = y = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        y = _tcstol(pstr + 1, &pstr, 10);      ASSERT(pstr);
    }

    CDuiString CDuiPoint::ToString()
    {
        CDuiString sPoint;
        sPoint.SmallFormat(_T("%ld,%ld"), x, y);
        return sPoint;
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

	CDuiSize::CDuiSize(long _cx, long _cy)
	{
		cx = _cx;
		cy = _cy;
	}

    CDuiSize::CDuiSize(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) cx = cy = 0;
        LPTSTR pstr = NULL;
        cx = cy = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        cy = _tcstol(pstr + 1, &pstr, 10);       ASSERT(pstr);
    }

    CDuiString CDuiSize::ToString()
    {
        CDuiString sSize;
        sSize.SmallFormat(_T("%ld,%ld"), cx, cy);
        return sSize;
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

	CDuiRect::CDuiRect(long iLeft, long iTop, long iRight, long iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

    CDuiRect::CDuiRect(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) left = top = right = bottom = 0;
        LPTSTR pstr = NULL;
        left = top = right = bottom = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        top = bottom = _tcstol(pstr + 1, &pstr, 10);                 ASSERT(pstr);
        right = _tcstol(pstr + 1, &pstr, 10);                        ASSERT(pstr);
        bottom = _tcstol(pstr + 1, &pstr, 10);                       ASSERT(pstr);
    }

    CDuiString CDuiRect::ToString()
    {
        CDuiString sRect;
        sRect.SmallFormat(_T("%ld,%ld,%ld,%ld"), left, top, right, bottom);
        return sRect;
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
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void CDuiRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void CDuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
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


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiPtrArray::CDuiPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
	{
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	CDuiPtrArray::CDuiPtrArray(const CDuiPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	CDuiPtrArray::~CDuiPtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void CDuiPtrArray::Empty()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
	}

	void CDuiPtrArray::Resize(int iSize)
	{
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CDuiPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CDuiPtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool CDuiPtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		if( iIndex == m_nCount ) return Add(pData);
		if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CDuiPtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		m_ppVoid[iIndex] = pData;
		return true;
	}

    bool CDuiPtrArray::Remove(int iIndex, int iCount)
    {
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + iCount, (m_nCount - iIndex - iCount) * sizeof(LPVOID));
        m_nCount -= iCount;
        return true;
    }

	int CDuiPtrArray::Find(LPVOID pData) const
	{
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int CDuiPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CDuiPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CDuiPtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID CDuiPtrArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_ppVoid[iIndex];
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiValArray::CDuiValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CDuiValArray::~CDuiValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CDuiValArray::Empty()
	{   
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CDuiValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CDuiValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CDuiValArray::Remove(int iIndex, int iCount)
	{
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + (iIndex + iCount) * m_iElementSize, (m_nCount - iIndex - iCount) * m_iElementSize);
        m_nCount -= iCount;
		return true;
	}

	int CDuiValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CDuiValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CDuiValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID CDuiValArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiString::CDuiString() : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = _T('\0');
	}

	CDuiString::CDuiString(const TCHAR ch) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = _T('\0');
	}

	CDuiString::CDuiString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
	{      
		ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
		m_szBuffer[0] = _T('\0');
		Assign(lpsz, nLen);
	}

	CDuiString::CDuiString(const CDuiString& src) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = _T('\0');
		Assign(src.m_pstr);
	}

	CDuiString::~CDuiString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

    CDuiString CDuiString::ToString()
    {
        return m_pstr;
    }

	int CDuiString::GetLength() const
	{ 
		return (int) _tcslen(m_pstr); 
	}

	CDuiString::operator LPCTSTR() const 
	{ 
		return m_pstr; 
	}

	void CDuiString::Append(LPCTSTR pstr)
	{
		int nNewLength = GetLength() + (int) _tcslen(pstr);
		if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
			if( m_pstr == m_szBuffer ) {
				m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
				_tcscpy(m_pstr, m_szBuffer);
				_tcscat(m_pstr, pstr);
			}
			else {
				m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
				_tcscat(m_pstr, pstr);
			}
		}
		else {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
			_tcscat(m_szBuffer, pstr);
		}
	}

	void CDuiString::Assign(LPCTSTR pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = _T("");
		cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
		if( cchMax < MAX_LOCAL_STRING_LEN ) {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
		}
		else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
			if( m_pstr == m_szBuffer ) m_pstr = NULL;
			m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
		}
		_tcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = _T('\0');
	}

	bool CDuiString::IsEmpty() const 
	{ 
		return m_pstr[0] == _T('\0'); 
	}

	void CDuiString::Empty() 
	{ 
		if( m_pstr != m_szBuffer ) free(m_pstr);
		m_pstr = m_szBuffer;
		m_szBuffer[0] = _T('\0'); 
	}

	LPCTSTR CDuiString::GetData() const
	{
		return m_pstr;
	}

	TCHAR CDuiString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	TCHAR CDuiString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const CDuiString& CDuiString::operator=(const CDuiString& src)
	{      
		Assign(src);
		return *this;
	}

	const CDuiString& CDuiString::operator=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

#ifdef _UNICODE

	const CDuiString& CDuiString::operator=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Assign(pwstr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Append(pwstr);
		}
		
		return *this;
	}

#else

	const CDuiString& CDuiString::operator=(LPCWSTR lpwStr)
	{      
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Assign(pstr);
		}
		else
		{
			Empty();
		}
		
		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr)
	{
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Append(pstr);
		}
		
		return *this;
	}

#endif // _UNICODE

	const CDuiString& CDuiString::operator=(const TCHAR ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = _T('\0');
		return *this;
	}

	CDuiString CDuiString::operator+(const CDuiString& src) const
	{
		CDuiString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CDuiString CDuiString::operator+(LPCTSTR lpStr) const
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			CDuiString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CDuiString& CDuiString::operator+=(const CDuiString& src)
	{      
		Append(src);
		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Append(lpStr);
		}
		
		return *this;
	}

	const CDuiString& CDuiString::operator+=(const TCHAR ch)
	{      
		TCHAR str[] = { ch, _T('\0') };
		Append(str);
		return *this;
	}

	bool CDuiString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
	bool CDuiString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
	bool CDuiString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
	bool CDuiString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
	bool CDuiString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
	bool CDuiString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

	void CDuiString::SetAt(int nIndex, TCHAR ch)
	{
		ASSERT(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int CDuiString::Compare(LPCTSTR lpsz) const 
	{ 
		return _tcscmp(m_pstr, lpsz); 
	}

	int CDuiString::CompareNoCase(LPCTSTR lpsz) const 
	{ 
		return _tcsicmp(m_pstr, lpsz); 
	}

	void CDuiString::MakeUpper() 
	{ 
		_tcsupr(m_pstr); 
	}

	void CDuiString::MakeLower() 
	{ 
		_tcslwr(m_pstr); 
	}

	CDuiString CDuiString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return CDuiString(m_pstr, iLength);
	}

	CDuiString CDuiString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return CDuiString();
		return CDuiString(m_pstr + iPos, iLength);
	}

	CDuiString CDuiString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return CDuiString(m_pstr + iPos, iLength);
	}

	int CDuiString::Find(TCHAR ch, int iPos /*= 0*/) const
	{
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		LPCTSTR p = _tcschr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
	{
		ASSERT(!::IsBadStringPtr(pstrSub,-1));
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::ReverseFind(TCHAR ch) const
	{
		LPCTSTR p = _tcsrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
	{
		CDuiString sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) _tcslen(pstrFrom);
		int cchTo = (int) _tcslen(pstrTo);
		while( iPos >= 0 ) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	int CDuiString::Format(LPCTSTR pstrFormat, ...)
	{
		LPTSTR szSprintf = NULL;
		va_list argList;
        int nLen;
		va_start(argList, pstrFormat);
        nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
        szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
        ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
		int iRet = _vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
        free(szSprintf);
		return iRet;
	}

	int CDuiString::SmallFormat(LPCTSTR pstrFormat, ...)
	{
		CDuiString sFormat = pstrFormat;
		TCHAR szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);
		int iRet = ::wvsprintf(szBuffer, sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
    struct TITEM
    {
        CDuiString Key;
        LPVOID Data;
        struct TITEM* pPrev;
        struct TITEM* pNext;
    };

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const CDuiString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

	CDuiStringPtrMap::CDuiStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	CDuiStringPtrMap::~CDuiStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CDuiStringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void CDuiStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID CDuiStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
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

	bool CDuiStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

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

	LPVOID CDuiStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CDuiStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
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

	int CDuiStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CDuiStringPtrMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CDuiStringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}

} // namespace DuiLib