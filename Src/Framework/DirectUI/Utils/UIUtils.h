#ifndef UIUtils_h__
#define UIUtils_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	Utils.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-07 16:54:33
// 说明			:	Win32 API Wrap
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Utils/BufferPtr.h"

// 常用 Buffer 类型的 typedef
typedef CBufferPtrT<char>						CCharBufferPtr;
typedef CBufferPtrT<wchar_t>				CWCharBufferPtr;
typedef CBufferPtrT<unsigned char>		CByteBufferPtr;
typedef CByteBufferPtr							CBufferPtr;

#ifdef _UNICODE
typedef CWCharBufferPtr						CTCharBufferPtr;
#else
typedef CCharBufferPtr							CTCharBufferPtr;
#endif

class StringOrID;
class CDuiPoint;
class CDuiRect;
class CDuiSize;
class CStdPtrArray;
class CStdValArray;
class CStdStringPtrMap;
class CDuiWaitCursor;
class CDuiVariant;
class CDuiCharsetConvert;		// 编码转换
class CDuiStringOperation;		// 字符串处理
class CDuiFileOperation;			// 文件操作

/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API StringOrID
{
public:
	StringOrID(LPCTSTR lpString);
	StringOrID(UINT nID);
	LPCTSTR m_lpstr;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CDuiPoint
	: public tagPOINT
{
public:
	CDuiPoint();
	CDuiPoint(const POINT& src);
	CDuiPoint(int x, int y);
	CDuiPoint(LPARAM lParam);

	void SetPoint(int X, int Y);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CDuiSize
	: public tagSIZE
{
public:
	CDuiSize();
	CDuiSize(const SIZE& src);
	CDuiSize(const RECT rc);
	CDuiSize(int cx, int cy);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CDuiRect
	: public tagRECT
{
public:
	CDuiRect();
	CDuiRect(const RECT& src);
	CDuiRect(LPCRECT lpRect);
	CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

	int GetWidth() const;
	int GetHeight() const;
	void Empty();
	bool IsNull() const;
	bool PtInRect(POINT pt);
	bool IsRectEmpty();
	bool IntersectRect(LPCRECT lpRect1,LPCRECT lpRect2);
	void Join(const RECT& rc);
	void ResetOffset();
	void Normalize();
	void Offset(int cx, int cy);
	void Inflate(int cx, int cy);
	void Deflate(int cx, int cy);
	void Union(CDuiRect& rc);
	CDuiPoint CenterPoint(void);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CStdPtrArray
{
public:
	CStdPtrArray(int iPreallocSize = 0);
	CStdPtrArray(const CStdPtrArray& src);
	~CStdPtrArray();

	void Empty();
	void Resize(int iSize);
	bool IsEmpty() const;
	int Find(LPVOID iIndex) const;
	bool Add(LPVOID pData);
	bool SetAt(int iIndex, LPVOID pData);
	bool InsertAt(int iIndex, LPVOID pData);
	bool Remove(int iIndex);
	int GetSize() const;
	LPVOID* GetData();

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;

	virtual void Sort(int (*sortfunc)(LPVOID p1,LPVOID p2));
protected:
	LPVOID* m_ppVoid;
	int m_nCount;
	int m_nAllocated;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CStdValArray
{
public:
	CStdValArray(int iElementSize, int iPreallocSize = 0);
	~CStdValArray();

	void Empty();
	bool IsEmpty() const;
	bool Add(LPCVOID pData);
	bool Remove(int iIndex);
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

struct TITEM
{
	CDuiString Key;
	LPVOID Data;
	struct TITEM* pPrev;
	struct TITEM* pNext;
};

class DIRECTUI_API CStdStringPtrMap
{
public:
	CStdStringPtrMap(int nSize = 64);
	~CStdStringPtrMap();

	void Resize(int nSize = 64);
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

class DIRECTUI_API CDuiWaitCursor
{
public:
	CDuiWaitCursor();
	~CDuiWaitCursor();

protected:
	HCURSOR m_hOrigCursor;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DIRECTUI_API CDuiVariant
	: public VARIANT
{
public:
	CDuiVariant() 
	{ 
		VariantInit(this); 
	}
	CDuiVariant(int i)
	{
		VariantInit(this);
		this->vt = VT_I4;
		this->intVal = i;
	}
	CDuiVariant(float f)
	{
		VariantInit(this);
		this->vt = VT_R4;
		this->fltVal = f;
	}
	CDuiVariant(LPOLESTR s)
	{
		VariantInit(this);
		this->vt = VT_BSTR;
		this->bstrVal = s;
	}
	CDuiVariant(IDispatch *disp)
	{
		VariantInit(this);
		this->vt = VT_DISPATCH;
		this->pdispVal = disp;
	}

	~CDuiVariant() 
	{ 
		VariantClear(this); 
	}
};


//////////////////////////////////////////////////////////////////////////
// 
class DIRECTUI_API CDuiCharsetConvert
{
public:
	static CDuiStringW	MbcsToUnicode(LPCSTR lpcszString);
	static CDuiStringA	MbcsToUTF8(LPCSTR lpcszString);

	static CDuiStringA	UnicodeToMbcs(LPCWSTR lpwcszWString);
	static CDuiStringA	UnicodeToUTF8(LPCWSTR lpwcszWString);

	static CDuiStringW	UTF8ToUnicode(LPCSTR lpcszString);
	static CDuiStringA	UTF8ToMbcs(LPCSTR lpcszString);

	// 编译语言环境决定转换过程
	static CDuiString UTF8ToTCHAR(LPCSTR lpcszString);
};

//////////////////////////////////////////////////////////////////////////
// 

class DIRECTUI_API CDuiStringOperation
{
public:
	static int compareNoCase(LPCTSTR left,LPCTSTR right);
	static int replace(CDuiString& content,LPCTSTR lpszOld,LPCTSTR lpszNew,bool bIsLoop = true);
	static void lower(CDuiString &str);
	static void upper(CDuiString &str);
	static void trim(CDuiString&content);
	static CDuiString format(LPCTSTR lpszFormat, ...);
	static void splite(LPCTSTR lpszContent,LPCTSTR lpszDelim,VecString& vec);

	// 返回真，说明是Map
	static bool parseAttributeString(LPCTSTR lpszAttributeString,StringMap& attributeMap);
	static LPCTSTR FindAttrubuteKey(AttributeMap& attributeMap,LPCTSTR lpszkey);
};

//////////////////////////////////////////////////////////////////////////
// 
class DIRECTUI_API CDuiFileOperation
{
public:
	static bool readFile(LPCTSTR lpszFilePath,CBufferPtr& buffer);
};

class DIRECTUI_API CDuiCodeOperation
{
public:
	static ULONG HexStringToULong(LPCTSTR lpszValue, int nSize = -1);
	static COLORREF HexStringToColor(const LPCTSTR lpszValue);

	// 将#ARGB格式颜色转换为DWORD值
	static COLORREF StringToColor(const LPCTSTR lpszColorString);

	static void StringToRect(LPCTSTR lpszRectStr,LONG *l1,LONG *l2,LONG *l3,LONG *l4);
	static void StringToRect(LPCTSTR lpszRectStr,LPRECT pRect);
	static void StringToSize(LPCTSTR lpszSizeStr,LPSIZE pSize);

	static int MbcsStrToInt(LPCSTR lpszNumber);
	static int UnicodeStrToInt(LPCWSTR lpszNumber);

	static int StringToInt(LPCTSTR lpszNumber);
	static __int64 StringToInt64(LPCTSTR lpszNumber);
}; 

#endif // UIUtils_h__
