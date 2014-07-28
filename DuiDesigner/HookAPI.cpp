#include "StdAfx.h"
#include "HookAPI.h"
#include <Dbghelp.h>

bool CHookAPI::m_bCreateFileEnabled=false;
TCHAR CHookAPI::m_sSkinDir[MAX_PATH]={0};
pfnCreateFile CHookAPI::CreateFileAPI=NULL;

bool CHookAPI::m_bInvalidateEnabled=false;
HOOKSTRUCT CHookAPI::m_InvalidateHookInfo={0};

bool CHookAPI::m_bGetImageExEnabled=false;
HOOKSTRUCT CHookAPI::m_GetImageExHookInfo={0};

CHookAPI::CHookAPI(void)
{
#ifdef _DEBUG
	CreateFileAPI=(pfnCreateFile)HookAPI(_T("KERNEL32.dll"),LPCSTR("CreateFileW"),(FARPROC)Hook_CreateFile,GetModuleHandle(_T("Duilib_ud.dll")));
	EnableCreateFile(true);

	HookAPI(_T("Duilib_ud.dll"),LPCSTR("?Invalidate@CPaintManagerUI@DuiLib@@QAEXAAUtagRECT@@@Z"),(FARPROC)Hook_Invalidate,m_InvalidateHookInfo);
	EnableInvalidate(true);

	HookAPI(_T("Duilib_ud.dll"),LPCSTR("?GetImageEx@CPaintManagerUI@DuiLib@@QAEPAUtagTImageInfo@2@PB_W0K@Z"),(FARPROC)Hook_GetImageEx,m_GetImageExHookInfo);
	EnableGetImageEx(true);
#else
	CreateFileAPI=(pfnCreateFile)HookAPI(_T("KERNEL32.dll"),LPCSTR("CreateFileW"),(FARPROC)Hook_CreateFile,GetModuleHandle(_T("Duilib_u.dll")));
	EnableCreateFile(true);

	HookAPI(_T("Duilib_u.dll"),LPCSTR("?Invalidate@CPaintManagerUI@DuiLib@@QAEXAAUtagRECT@@@Z"),(FARPROC)Hook_Invalidate,m_InvalidateHookInfo);
	EnableInvalidate(true);

	HookAPI(_T("Duilib_u.dll"),LPCSTR("?GetImageEx@CPaintManagerUI@DuiLib@@QAEPAUtagTImageInfo@2@PB_W0K@Z"),(FARPROC)Hook_GetImageEx,m_GetImageExHookInfo);
	EnableGetImageEx(true);
#endif
}

CHookAPI::~CHookAPI(void)
{
}

FARPROC CHookAPI::HookAPI(LPCTSTR pstrDllName,LPCSTR pstrFuncName,FARPROC pfnNewFunc,HMODULE hModCaller)
{
	if(hModCaller==NULL)
		return NULL;

	ULONG size;
	//获取指向PE文件中的Import中IMAGE_DIRECTORY_DESCRIPTOR数组的指针
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc=(PIMAGE_IMPORT_DESCRIPTOR)
		ImageDirectoryEntryToData(hModCaller,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&size);
	if(pImportDesc==NULL)
		return NULL;
	HMODULE hModule=LoadLibrary(pstrDllName);
	//纪录函数地址
	FARPROC pfnOriginFunc=GetProcAddress(hModule,pstrFuncName);

	//查找记录,看看有没有我们想要的DLL
	USES_CONVERSION;
	char* pstrDest=W2A(pstrDllName);
	for(;pImportDesc->Name;pImportDesc++)
	{
		LPSTR pszDllName=(LPSTR)((PBYTE)hModCaller+pImportDesc->Name);
		if(lstrcmpiA(pszDllName,pstrDest)==0)
			break;
	}
	if(pImportDesc->Name==NULL)
	{
		return NULL;
	}
	//寻找我们想要的函数
	PIMAGE_THUNK_DATA pThunk=(PIMAGE_THUNK_DATA)((PBYTE)hModCaller+pImportDesc->FirstThunk);
	for(;pThunk->u1.Function;pThunk++)
	{
		//ppfn记录了与IAT表相应的地址
		PROC*ppfn=(PROC*)&pThunk->u1.Function ;
		if(*ppfn==pfnOriginFunc)
		{
			DWORD dwOldProtect;
			//修改内存包含属性
			VirtualProtect(ppfn, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			WriteProcessMemory(GetCurrentProcess(),ppfn,&(pfnNewFunc),sizeof(pfnNewFunc),NULL);
			return pfnOriginFunc;
		}
	}

	return NULL;
}

BOOL CHookAPI::HookAPI(LPCTSTR pstrDllName,LPCSTR pstrFuncName,FARPROC pfnNewFunc,HOOKSTRUCT& HookInfo)
{
	HMODULE hModule=LoadLibrary(pstrDllName);
	//纪录函数地址
	HookInfo.pfnFuncAddr=GetProcAddress(hModule,pstrFuncName);
	FreeLibrary(hModule);
	if(HookInfo.pfnFuncAddr==NULL)
		return FALSE;
	//备份原函数的前5个字节，一般的WIN32 API以__stdcall声明的API理论上都可以这样进行HOOK
	memcpy(HookInfo.OldCode,HookInfo.pfnFuncAddr,5);
	HookInfo.NewCode[0]=0xe9;//构造JMP
	DWORD dwJmpAddr=(DWORD)pfnNewFunc-(DWORD)HookInfo.pfnFuncAddr-5;//计算JMP地址
	memcpy(&HookInfo.NewCode[1],&dwJmpAddr,4);
	EnableHook(HookInfo,TRUE);//开始进行HOOK

	return TRUE;
}

void CHookAPI::EnableHook(HOOKSTRUCT& HookInfo,BOOL bEnable)
{
	if(bEnable)
		WriteProcessMemory((HANDLE)-1,HookInfo.pfnFuncAddr,HookInfo.NewCode,5,0);//替换函数地址
	else
		WriteProcessMemory((HANDLE)-1,HookInfo.pfnFuncAddr,HookInfo.OldCode,5,0);//还原函数地址
}

HANDLE WINAPI CHookAPI::Hook_CreateFile(
							  LPCTSTR lpFileName, 
							  DWORD dwDesiredAccess, 
							  DWORD dwShareMode, 
							  LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
							  DWORD dwCreationDisposition, 
							  DWORD dwFlagsAndAttributes, 
							  HANDLE hTemplateFile
							  )
{
	TCHAR sFullPath[MAX_PATH];
	if(m_bCreateFileEnabled)
	{
		if(*(lpFileName+1)!=':')//relative path
		{
			_tcscpy_s(sFullPath,m_sSkinDir);
			_tcscat_s(sFullPath,lpFileName);
			lpFileName=sFullPath;
		}
	}

	return CreateFileAPI(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition
		,dwFlagsAndAttributes,hTemplateFile);
}

__declspec(naked) void WINAPI CHookAPI::Hook_Invalidate(RECT& rcItem)
{
	_asm
	{
		push ebp;
		mov ebp,esp;
		pushad;
		push ecx;//push this pointer
	}
	EnableHook(m_InvalidateHookInfo,FALSE);

	if(m_bInvalidateEnabled)
	{
	::OffsetRect(&rcItem,FORM_OFFSET_X,FORM_OFFSET_Y);
	::InflateRect(&rcItem,TRACKER_HANDLE_SIZE,TRACKER_HANDLE_SIZE);
	}
	_asm pop ecx;//pop this pointer
	((pfnInvalidate)m_InvalidateHookInfo.pfnFuncAddr)(rcItem);

	EnableHook(m_InvalidateHookInfo,TRUE);
	_asm
	{
		popad;
		pop ebp;
		retn 0x04;
	}
}

__declspec(naked) TImageInfo* WINAPI CHookAPI::Hook_GetImageEx(LPCTSTR pstrBitmap, LPCTSTR pstrType, DWORD mask)
{
	_asm
	{
		push ebp;
		mov ebp,esp;
		sub esp,0x208;//full path
		push ebx;
		push esi;
		push edi;
		push ecx;//save this pointer
	}
	EnableHook(m_GetImageExHookInfo,FALSE);

	if(m_bGetImageExEnabled)
	{
		if(_tcslen(pstrBitmap)>=2&&*(pstrBitmap+1)!=':')//relative path
		{
			_asm
			{
				push offset CHookAPI::m_sSkinDir;
				push 0x104;
				lea eax,[ebp-0x208];
				push eax;
				call dword ptr[_tcscpy_s];
				add esp,0x0C;
				mov eax,dword ptr [ebp+8];
				push eax;
				push 0x104;
				lea ecx,[ebp-0x208];
				push ecx;
				call dword ptr[_tcscat_s];
				add esp,0x0C;
				lea eax,[ebp-0x208];
				mov dword ptr[ebp+8],eax;
			}
		}
	}

	_asm
	{
		pop ecx;//load this pointer
		push mask;
		push pstrType;
		push pstrBitmap;
		call m_GetImageExHookInfo.pfnFuncAddr;
		push eax;//save return value
	}

	EnableHook(m_GetImageExHookInfo,TRUE);
	_asm
	{
		pop eax;//load return value
		pop edi;
		pop esi;
		pop ebx;
		add esp,0x208;
		pop ebp;
		retn 0x0C;
	}
}