#include "stdafx.h"
#include "UIUtil.h"

namespace StringConvertor {

#pragma warning(disable: 4996)

#if defined(USE_OP_LIBC)
#define __malloc(size) ((void*)(op_malloc(size)))
#define __free(ptr) ((void)(op_free(ptr)))
#elif defined(USE_LIBC)
#define __malloc(size) ((void*)(malloc(size)))
#define __free(ptr) ((void)(free(ptr)))
#else
#define __malloc(size) ((void*)(LocalAlloc(LPTR, (size))))
#define __free(ptr) ((void)(LocalFree(ptr)))
#endif

enum FROM
{
    ansi,
    wide
};

template <FROM dir> struct Conversion;
template <> struct Conversion<wide>
{
    typedef LPWSTR  FromT;
    typedef LPCWSTR CFromT;
    typedef LPSTR   ToT;
    enum { TO = ansi };
    static __inline int Convert(UINT cp, CFromT strW, int lenW, ToT strA, int lenA) { return WideCharToMultiByte(cp, 0, strW, lenW, strA, lenA, NULL, NULL); }
    static __inline FromT  Alloc(size_t len) { return StringAllocW(len); }
    static __inline size_t StrLen(CFromT strW) { return wcslen(strW); }
    static __inline size_t BufferSize(size_t len) { return len * sizeof(wchar_t); }
};

template <> struct Conversion<ansi>
{
    typedef LPSTR  FromT;
    typedef LPCSTR CFromT;
    typedef LPWSTR ToT;
    enum { TO = wide };
    static __inline int Convert(UINT cp, CFromT strA, int lenA, ToT strW, int lenW) { return MultiByteToWideChar(cp, 0, strA, lenA, strW, lenW); }
    static __inline FromT  Alloc(size_t len) { return StringAllocA(len); }
    static __inline size_t StrLen(CFromT strA) { return strlen(strA); }
    static __inline size_t BufferSize(size_t len) { return len; }
};

template <FROM from, UINT to> struct Decode
{
    typedef typename Conversion<from>::CFromT CFromT;
    typedef typename Conversion<from>::ToT    ToT;
    enum { OPPOSITE = Conversion<from>::TO };
    ToT out;
    __inline Decode(CFromT in, int in_len = -1): out(NULL)
    {
        if (!in) return;
        int out_len = Conversion<from>::Convert(to, in, in_len, NULL, 0);
        if (out_len)
        {
            ++out_len; //this might be over-allocation
            out = Conversion< (FROM)OPPOSITE >::Alloc(out_len);
            if (out)
            {
                Conversion<from>::Convert(to, in, in_len, out, out_len);
            }
        }
    }
    __inline operator ToT () const { return out; }
};

template <UINT from, UINT to> struct Recode
{
    LPSTR out;
    __inline Recode(LPCSTR in, int len = -1): out(NULL)
    {
        LPWSTR mid = Decode<ansi, from>(in, len);
        if (!mid) return;
        out = Decode<wide, to>(mid);
        StringFreeW(mid);
    }
    __inline operator LPSTR () const { return out; }
};

template <FROM kind> struct Duplicate
{
    typedef typename Conversion<kind>::FromT   FromT;
    typedef typename Conversion<kind>::CFromT CFromT;
    FromT out;
    __inline Duplicate(CFromT in, int in_len = -1): out(NULL)
    {
        if (!in) return;
        size_t len = ((in_len < 0) ? Conversion<kind>::StrLen(in) : (size_t)in_len) + 1;
        out = Conversion<kind>::Alloc(len);
        if (!out) return;
        CopyMemory(out, in, Conversion<kind>::BufferSize(len));
        out[len - 1] = 0;
    }
    __inline operator FromT () const { return out; }
};

LPSTR  WideToAnsi(LPCWSTR str, int len) { return Decode<wide,    CP_ACP >(str, len); }
LPSTR  WideToUtf8(LPCWSTR str, int len) { return Decode<wide,    CP_UTF8>(str, len); }
LPWSTR AnsiToWide(LPCSTR  str, int len) { return Decode<ansi,    CP_ACP >(str, len); }
LPWSTR Utf8ToWide(LPCSTR  str, int len) { return Decode<ansi,    CP_UTF8>(str, len); }
LPSTR  AnsiToUtf8(LPCSTR  str, int len) { return Recode<CP_ACP,  CP_UTF8>(str, len); }
LPSTR  Utf8ToAnsi(LPCSTR  str, int len) { return Recode<CP_UTF8, CP_ACP >(str, len); }
LPWSTR WideStrDup(LPCWSTR str, int len) { return Duplicate<wide>(str, len); }
LPSTR  AnsiStrDup(LPCSTR  str, int len) { return Duplicate<ansi>(str, len); }

//memory routines
LPWSTR StringAllocW(size_t len) { return (LPWSTR)__malloc(len * sizeof(wchar_t)); }
LPSTR  StringAllocA(size_t len) { return (LPSTR)__malloc(len); }
void   StringFreeW(LPWSTR strW) { __free(strW); }
void   StringFreeA(LPSTR strA)  { __free(strA); }

} // namespace StringConvertor