#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString);
		STRINGorID(unsigned int nID);

		LPCTSTR m_lpstr;
	};

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CDuiString;
    class DUILIB_API IDuiBaseType
    {
    public:
        void FromString();
        CDuiString ToString();
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiChar : public IDuiBaseType
    {
    public:
        CDuiChar();
        CDuiChar(char cValue);
        CDuiChar(LPCTSTR pstrValue);
        CDuiString ToString();

        char m_cValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiByte : public IDuiBaseType
    {
    public:
        CDuiByte();
        CDuiByte(byte byValue);
        CDuiByte(LPCTSTR pstrValue);
        CDuiString ToString();

        byte m_byValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiTChar : public IDuiBaseType
    {
    public:
        CDuiTChar();
        CDuiTChar(TCHAR tValue);
        CDuiTChar(LPCTSTR pstrValue);
        CDuiString ToString();

        TCHAR m_tValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiWord : public IDuiBaseType
    {
    public:
        CDuiWord();
        CDuiWord(WORD wValue);
        CDuiWord(LPCTSTR pstrValue);
        CDuiString ToString();

        WORD m_wValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiDWord : public IDuiBaseType
    {
    public:
        CDuiDWord();
        CDuiDWord(DWORD dwValue);
        CDuiDWord(LPCTSTR pstrValue);
        CDuiString ToString();

        DWORD m_dwValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiInt : public IDuiBaseType
    {
    public:
        CDuiInt();
        CDuiInt(int iValue);
        CDuiInt(LPCTSTR pstrValue);
        CDuiString ToString();

        int m_iValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiUInt : public IDuiBaseType
    {
    public:
        CDuiUInt();
        CDuiUInt(unsigned int uValue);
        CDuiUInt(LPCTSTR pstrValue);
        CDuiString ToString();

        unsigned int m_uValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiLong : public IDuiBaseType
    {
    public:
        CDuiLong();
        CDuiLong(long lValue);
        CDuiLong(LPCTSTR pstrValue);
        CDuiString ToString();

        long m_lValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiULong : public IDuiBaseType
    {
    public:
        CDuiULong();
        CDuiULong(unsigned long ulValue);
        CDuiULong(LPCTSTR pstrValue);
        CDuiString ToString();

        unsigned long m_ulValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiInt64 : public IDuiBaseType
    {
    public:
        CDuiInt64();
        CDuiInt64(__int64 iiValue);
        CDuiInt64(LPCTSTR pstrValue);
        CDuiString ToString();

        __int64 m_iiValue;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiBool : public IDuiBaseType
    {
    public:
        CDuiBool();
        CDuiBool(bool bValue);
        CDuiBool(LPCTSTR pstrValue);
        CDuiString ToString();

        bool m_bValue;
    };

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CDuiPoint : public tagPOINT, public IDuiBaseType
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(long x, long y);
		CDuiPoint(LPARAM lParam);
        CDuiPoint(LPCTSTR pstrValue);
        CDuiString ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CDuiSize : public tagSIZE, public IDuiBaseType
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(long cx, long cy);
        CDuiSize(LPCTSTR pstrValue);
        CDuiString ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CDuiRect : public tagRECT, public IDuiBaseType
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(long iLeft, long iTop, long iRight, long iBottom);
        CDuiRect(LPCTSTR pstrValue);
        CDuiString ToString();

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CDuiString : public IDuiBaseType
    {
    public:
        enum { MAX_LOCAL_STRING_LEN = 63 };

        CDuiString();
        CDuiString(const TCHAR ch);
        CDuiString(const CDuiString& src);
        CDuiString(LPCTSTR lpsz, int nLen = -1);
        ~CDuiString();
        CDuiString ToString();

        void Empty();
        int GetLength() const;
        bool IsEmpty() const;
        TCHAR GetAt(int nIndex) const;
        void Append(LPCTSTR pstr);
        void Assign(LPCTSTR pstr, int nLength = -1);
        LPCTSTR GetData() const;

        void SetAt(int nIndex, TCHAR ch);
        operator LPCTSTR() const;

        TCHAR operator[] (int nIndex) const;
        const CDuiString& operator=(const CDuiString& src);
        const CDuiString& operator=(const TCHAR ch);
        const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
        const CDuiString& CDuiString::operator=(LPCSTR lpStr);
        const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
        const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
        const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
        CDuiString operator+(const CDuiString& src) const;
        CDuiString operator+(LPCTSTR pstr) const;
        const CDuiString& operator+=(const CDuiString& src);
        const CDuiString& operator+=(LPCTSTR pstr);
        const CDuiString& operator+=(const TCHAR ch);

        bool operator == (LPCTSTR str) const;
        bool operator != (LPCTSTR str) const;
        bool operator <= (LPCTSTR str) const;
        bool operator <  (LPCTSTR str) const;
        bool operator >= (LPCTSTR str) const;
        bool operator >  (LPCTSTR str) const;

        int Compare(LPCTSTR pstr) const;
        int CompareNoCase(LPCTSTR pstr) const;

        void MakeUpper();
        void MakeLower();

        CDuiString Left(int nLength) const;
        CDuiString Mid(int iPos, int nLength = -1) const;
        CDuiString Right(int nLength) const;

        int Find(TCHAR ch, int iPos = 0) const;
        int Find(LPCTSTR pstr, int iPos = 0) const;
        int ReverseFind(TCHAR ch) const;
        int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

        int __cdecl Format(LPCTSTR pstrFormat, ...);
        int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

    protected:
        LPTSTR m_pstr;
        TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
    };

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CDuiPtrArray
	{
	public:
		CDuiPtrArray(int iPreallocSize = 0);
		CDuiPtrArray(const CDuiPtrArray& src);
		~CDuiPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex, int iCount = 1);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CDuiValArray
	{
	public:
		CDuiValArray(int iElementSize, int iPreallocSize = 0);
		~CDuiValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex,  int iCount = 1);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

    struct TITEM;
	class DUILIB_API CDuiStringPtrMap
	{
	public:
		CDuiStringPtrMap(int nSize = 83);
		~CDuiStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

}// namespace DuiLib

#endif // __UTILS_H__