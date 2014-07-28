#pragma once
using DuiLib::TImageInfo;

//////////////////////////////////////////////////////////////////////////
//CHookAPI

typedef struct tagHOOKSTRUCT
{
	FARPROC pfnFuncAddr;//用于保存API函数地址
	BYTE    OldCode[5]; //保存原API前5个字节
	BYTE    NewCode[5]; //JMP XXXX其中XXXXJMP的地址
}HOOKSTRUCT;

typedef HANDLE (WINAPI *pfnCreateFile)(
									 LPCTSTR lpFileName, 
									 DWORD dwDesiredAccess, 
									 DWORD dwShareMode, 
									 LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
									 DWORD dwCreationDisposition, 
									 DWORD dwFlagsAndAttributes, 
									 HANDLE hTemplateFile
							   );
typedef void (WINAPI *pfnInvalidate)(RECT& rcItem);
typedef TImageInfo* (WINAPI *pfnGetImageEx)(LPCTSTR pstrBitmap, LPCTSTR pstrType, DWORD mask);

class CHookAPI
{
public:
	CHookAPI(void);
	~CHookAPI(void);

public:
	FARPROC HookAPI(LPCTSTR pstrDllName,LPCSTR pstrFuncName,FARPROC pfnNewFunc,HMODULE hModCaller);
	BOOL HookAPI(LPCTSTR pstrDllName,LPCSTR pstrFuncName,FARPROC pfnNewFunc,HOOKSTRUCT& HookInfo);
	static void EnableHook(HOOKSTRUCT& HookInfo,BOOL bEnable);

public:
	static HANDLE WINAPI Hook_CreateFile(
		LPCTSTR lpFileName, 
		DWORD dwDesiredAccess, 
		DWORD dwShareMode, 
		LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
		DWORD dwCreationDisposition, 
		DWORD dwFlagsAndAttributes, 
		HANDLE hTemplateFile
		);
	static void EnableCreateFile(bool bEnable=true) { m_bCreateFileEnabled=bEnable; }
	static void SetSkinDir(LPCTSTR pstrDir) { _tcscpy_s(m_sSkinDir,pstrDir); }

	static void WINAPI Hook_Invalidate(RECT& rcItem);
	static void EnableInvalidate(bool bEnable=true) { m_bInvalidateEnabled=bEnable; }

	static TImageInfo* WINAPI Hook_GetImageEx(LPCTSTR pstrBitmap, LPCTSTR pstrType, DWORD mask);
	static void EnableGetImageEx(bool bEnable=true) { m_bGetImageExEnabled=bEnable; }

private:
	static bool m_bCreateFileEnabled;
	static TCHAR m_sSkinDir[MAX_PATH];
	static pfnCreateFile CreateFileAPI;

	static bool m_bInvalidateEnabled;
	static HOOKSTRUCT m_InvalidateHookInfo;

	static bool m_bGetImageExEnabled;
	static HOOKSTRUCT m_GetImageExHookInfo;
};