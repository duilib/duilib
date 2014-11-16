#pragma once
#include <mshtmhst.h>
#include <ExDisp.h>
#include <ExDispid.h>

namespace DuiLib
{
	class CWebBrowserEventHandler
	{
	public:
		CWebBrowserEventHandler() {}
		virtual ~CWebBrowserEventHandler() {}

		virtual ULONG STDMETHODCALLTYPE Release( void ) { return S_OK;}
		virtual ULONG STDMETHODCALLTYPE AddRef( void ) { return S_OK;}
		virtual void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel ) {}
		virtual void NavigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel) {}
		virtual void NavigateComplete2(IDispatch *pDisp,VARIANT *&url){}
		virtual void DocumentComplete(IDispatch *pDisp,VARIANT *&url){}
		virtual void ProgressChange(LONG nProgress, LONG nProgressMax){}
		virtual void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl){}
		virtual void CommandStateChange(long Command,VARIANT_BOOL Enable){}
		virtual void TitleChange(BSTR bstrTitle){}
		virtual void OnClick(LPCTSTR lpElementID,IHTMLEventObj *pEvtObj){}

		// interface IDocHostUIHandler
		virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(
			/* [in] */ DWORD dwID,
			/* [in] */ POINT __RPC_FAR *ppt,
			/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
			/* [in] */ IDispatch __RPC_FAR *pdispReserved)
		{
			//return E_NOTIMPL;
			//返回 E_NOTIMPL 正常弹出系统右键菜单
			return S_OK;
			//返回S_OK 则可屏蔽系统右键菜单
		}

		virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
			/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
		{
			// 		if (pInfo != NULL)
			// 		{
			// 			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
			// 		}
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE EnableModeless(
			/* [in] */ BOOL fEnable)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
			/* [in] */ LPCRECT prcBorder,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
			/* [in] */ BOOL fRameWindow)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
			/* [in] */ LPMSG lpMsg,
			/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ DWORD nCmdID)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
			/* [out] */ LPOLESTR __RPC_FAR *pchKey,
			/* [in] */ DWORD dw)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
			/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
			/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
		{
			return E_NOTIMPL;//使用默认拖拽
			//return S_OK;//自定义拖拽
		}

		virtual HRESULT STDMETHODCALLTYPE GetExternal(
			/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
			/* [in] */ DWORD dwTranslate,
			/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
			/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
			/* [in] */ IDataObject __RPC_FAR *pDO,
			/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
		{
			return S_OK;
		}

		// 	virtual HRESULT STDMETHODCALLTYPE GetOverrideKeyPath( 
		// 		/* [annotation][out] */ 
		// 		__deref_out  LPOLESTR *pchKey,
		// 		/* [in] */ DWORD dw)
		// 	{
		// 		return E_NOTIMPL;
		// 	}

		// IDownloadManager
		virtual HRESULT STDMETHODCALLTYPE Download( 
			/* [in] */ IMoniker *pmk,
			/* [in] */ IBindCtx *pbc,
			/* [in] */ DWORD dwBindVerb,
			/* [in] */ LONG grfBINDF,
			/* [in] */ BINDINFO *pBindInfo,
			/* [in] */ LPCOLESTR pszHeaders,
			/* [in] */ LPCOLESTR pszRedir,
			/* [in] */ UINT uiCP)
		{
			return E_NOTIMPL;
		}
	};
}
