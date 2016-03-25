#ifndef __UIWEBBROWSER_H__
#define __UIWEBBROWSER_H__

#pragma once

#include "Utils/WebBrowserEventHandler.h"
#include <ExDisp.h>

namespace DuiLib
{
	class DUILIB_API CWebBrowserUI
		: public CActiveXUI
		, public IDocHostUIHandler
		, public IServiceProvider
		, public IOleCommandTarget
		, public IDispatch
		, public ITranslateAccelerator
	{
	public:
		/// 构造函数
		CWebBrowserUI();
		virtual ~CWebBrowserUI();

		void SetHomePage(LPCTSTR lpszUrl);
		LPCTSTR GetHomePage();

		void SetAutoNavigation(bool bAuto = TRUE);
		bool IsAutoNavigation();

		void SetWebBrowserEventHandler(CWebBrowserEventHandler* pEventHandler);
		void Navigate2(LPCTSTR lpszUrl);
		void Refresh();
		void Refresh2(int Level);
		void GoBack();
		void GoForward();
		void NavigateHomePage();
		void NavigateUrl(LPCTSTR lpszUrl);
		virtual bool DoCreateControl();
		IWebBrowser2* GetWebBrowser2(void);
		IDispatch*		   GetHtmlWindow();
		static DISPID FindId(IDispatch *pObj, LPOLESTR pName);
		static HRESULT InvokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs);
		static HRESULT GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);
		static HRESULT SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);

	protected:
		IWebBrowser2*			m_pWebBrowser2; //浏览器指针
		IHTMLWindow2*		_pHtmlWnd2;
		LONG m_dwRef;
		DWORD m_dwCookie;
		virtual void ReleaseControl();
		HRESULT RegisterEventHandler(BOOL inAdvise);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		CDuiString m_sHomePage;	// 默认页面
		bool m_bAutoNavi;	// 是否启动时打开默认页面
		CWebBrowserEventHandler* m_pWebBrowserEventHandler;	//浏览器事件处理

		// DWebBrowserEvents2
		void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel );
		void NavigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel);
		void NavigateComplete2(IDispatch *pDisp,VARIANT *&url);
		void DocumentComplete(IDispatch *pDisp,VARIANT *&url); 
		void ProgressChange(LONG nProgress, LONG nProgressMax);
		void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
		void CommandStateChange(long Command,VARIANT_BOOL Enable);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );

		// IUnknown
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

		// IDispatch
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
		virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

		// IDocHostUIHandler
		STDMETHOD(ShowContextMenu)(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit);
		STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo);
		STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc);
		STDMETHOD(HideUI)();
		STDMETHOD(UpdateUI)();
		STDMETHOD(EnableModeless)(BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
		STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
		STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow);
		STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID);	//浏览器消息过滤
		STDMETHOD(GetOptionKeyPath)(LPOLESTR* pchKey, DWORD dwReserved);
		STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget);
		STDMETHOD(GetExternal)(IDispatch** ppDispatch);
		STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
		STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet);

		// IServiceProvider
		STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void** ppvObject);

		// IOleCommandTarget
		virtual HRESULT STDMETHODCALLTYPE QueryStatus( __RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds, __RPC__inout_ecount_full(cCmds ) OLECMD prgCmds[ ], __RPC__inout_opt OLECMDTEXT *pCmdText);
		virtual HRESULT STDMETHODCALLTYPE Exec( __RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut );

		// IDownloadManager
		STDMETHOD(Download)( 
			/* [in] */ IMoniker *pmk,
			/* [in] */ IBindCtx *pbc,
			/* [in] */ DWORD dwBindVerb,
			/* [in] */ LONG grfBINDF,
			/* [in] */ BINDINFO *pBindInfo,
			/* [in] */ LPCOLESTR pszHeaders,
			/* [in] */ LPCOLESTR pszRedir,
			/* [in] */ UINT uiCP);

		// ITranslateAccelerator
		// Duilib消息分发给WebBrowser
		virtual LRESULT TranslateAccelerator( MSG *pMsg );
	};
} // namespace DuiLib
#endif // __UIWEBBROWSER_H__