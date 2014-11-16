#pragma once
//#include <ExDisp.h>

namespace DuiLib
{
	class CFlashEventHandler
	{
	public:
		CFlashEventHandler() {}
		virtual ~CFlashEventHandler() {}

		virtual ULONG STDMETHODCALLTYPE Release( void ) { return S_OK;}
		virtual ULONG STDMETHODCALLTYPE AddRef( void ) { return S_OK;}

		virtual HRESULT OnReadyStateChange ( long newState )
		{
			return S_OK;
		}

		virtual HRESULT OnProgress (long percentDone )
		{
			return S_OK;
		}

		virtual HRESULT FSCommand ( LPCTSTR command, LPCTSTR args )
		{
			return S_OK;
		}
		
		virtual HRESULT FlashCall ( LPCTSTR request,ShockwaveFlashObjects::IShockwaveFlash* m_pFlash )
		{
			return S_OK;
		}
	};
}