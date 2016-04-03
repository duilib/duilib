#include "StdAfx.h"

namespace DuiLib {

extern int g_imageRectStyle;

CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
{

}

CControlUI* CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback* pCallback, 
                                   CPaintManagerUI* pManager, CControlUI* pParent)
{
	//资源ID为0-65535，两个字节；字符串指针为4个字节
	//字符串以<开头认为是XML字符串，否则认为是XML文件

    if( HIWORD(xml.m_lpstr) != NULL ) {
        if( *(xml.m_lpstr) == _T('<') ) {
            if( !m_xml.Load(xml.m_lpstr) ) return NULL;
        }
        else {
            if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
        }
    }
    else {
        HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
        if( hResource == NULL ) return NULL;
        HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
        if( hGlobal == NULL ) {
            FreeResource(hResource);
            return NULL;
        }

        m_pCallback = pCallback;
        if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) )) return NULL;
        ::FreeResource(hResource);
        m_pstrtype = type;
    }

    return Create(pCallback, pManager, pParent);
}

CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
{
    m_pCallback = pCallback;
    CMarkupNode root = m_xml.GetRoot();
    if( !root.IsValid() ) return NULL;

    if( pManager ) {
        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;
        for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
            pstrClass = node.GetName();
            if( _tcsicmp(pstrClass, _T("Image")) == 0 ) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pImageName = NULL;
                LPCTSTR pImageResType = NULL;
                DWORD mask = 0;
				bool shared = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcsicmp(pstrName, _T("name")) == 0 ) {
                        pImageName = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("restype")) == 0 ) {
                        pImageResType = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("mask")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        mask = _tcstoul(pstrValue, &pstr, 16);
                    }
					else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
						shared = (_tcsicmp(pstrValue, _T("true")) == 0);
					}
                }
                if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask, shared);
            }
            else if( _tcsicmp(pstrClass, _T("Font")) == 0 ) {
                nAttributes = node.GetAttributeCount();
				int id = -1;
                LPCTSTR pFontName = NULL;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;
				bool shared = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
					if( _tcsicmp(pstrName, _T("id")) == 0 ) {
						id = _tcstol(pstrValue, &pstr, 10);
					}
                    else if( _tcsicmp(pstrName, _T("name")) == 0 ) {
                        pFontName = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("size")) == 0 ) {
                        size = _tcstol(pstrValue, &pstr, 10);
                    }
                    else if( _tcsicmp(pstrName, _T("bold")) == 0 ) {
                        bold = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("underline")) == 0 ) {
                        underline = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("italic")) == 0 ) {
                        italic = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("default")) == 0 ) {
                        defaultfont = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
					else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
						shared = (_tcsicmp(pstrValue, _T("true")) == 0);
					}
                }
                if( id >= 0 && pFontName ) {
                    pManager->AddFont(id, pFontName, size, bold, underline, italic, shared);
                    if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic, shared);
                }
            }
            else if( _tcsicmp(pstrClass, _T("Default")) == 0 || _tcsicmp(pstrClass, _T("Style")) == 0 ) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pStyleName = NULL;
                LPCTSTR pImageRectStyle = NULL;
                bool shared = false;
                /* 先检查是否存在name属性 */
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if (_tcsicmp(pstrName, _T("name")) == 0) {
                        pStyleName = pstrValue;
                    }
                    else if (pStyleName && _tcsicmp(pstrName, _T("shared")) == 0) {
                        shared = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if (_tcsicmp(pstrName, _T("imagerectstyle")) == 0) {
                        pImageRectStyle = pstrValue;
                    }
                }
                /* imagerectstyle只有在没有name属性的,Default或Style标签内定义才有效 */
                if (!pStyleName && pImageRectStyle && _tcsicmp(pImageRectStyle, _T("l,t,w,h")) == 0) {
                    g_imageRectStyle = STYLE_RECT_LTWH;
                }

                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcsicmp(pstrName, _T("name")) == 0 || _tcsicmp(pstrName, _T("shared")) == 0 ) {
                        /* 已经处理，跳过 */
                    }
                    else {
                        /*
                        if( _tcsicmp(pstrName, _T("value")) == 0 ||
                              _tcsicmp(pstrName, _T("vscrollbarstyle")) == 0 ||
                               _tcsicmp(pstrName, _T("hscrollbarstyle")) == 0 ||
                                _tcsicmp(pstrName, _T("horizattr")) == 0 ||
                                 _tcsicmp(pstrName, _T("dotlineattr")) == 0 ||
                                  _tcsicmp(pstrName, _T("folderattr")) == 0 ||
                                   _tcsicmp(pstrName, _T("checkboxattr")) == 0 ||
                                    _tcsicmp(pstrName, _T("itemattr")) == 0 ) {
                        */

                        /* 允许width,height等全部属性不作为value属性的值,而是像folderattr一样进行单独定义
                           (当风格定义太长时，可在单独属性定义后添加换行符来定义多行风格标签)
                           <Style name="test" value="width=&quot;60&quot;" height="60"
                                              folderattr="......"
                           />
                        */
                        if (pStyleName) {
                            pManager->AddDefaultAttributeList(pStyleName, pstrName, pstrValue, shared);
                        }
                    }
                }

            }
			else if( _tcsicmp(pstrClass, _T("MultiLanguage")) == 0 ) {
				nAttributes = node.GetAttributeCount();
				int id = -1;
				LPCTSTR pMultiLanguage = NULL;
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);
					if( _tcsicmp(pstrName, _T("id")) == 0 ) {
						id = _tcstol(pstrValue, &pstr, 10);
					}
					else if( _tcsicmp(pstrName, _T("value")) == 0 ) {
						pMultiLanguage = pstrValue;
					}
				}
				if (id >= 0 && pMultiLanguage ) {
					pManager->AddMultiLanguageString(id, pMultiLanguage);
				}
			}
        }

        pstrClass = root.GetName();
        if( _tcsicmp(pstrClass, _T("Window")) == 0 ) {
            if( pManager->GetPaintWindow() ) {
                int nAttributes = root.GetAttributeCount();
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    pManager->SetWindowAttribute(pstrName, pstrValue);
                }
            }
        }
    }
    return _Parse(&root, pParent, pManager);
}

CMarkup* CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

#define DEFAULT_STYLE_NAME_SEPARATOR    _T(':')    /* 控件的style属性中的风格名和参数列表的分隔符 */

template<typename CControlNode>
static void CDialogBuilder::ProcessAttributes(CPaintManagerUI* pManager, LPCTSTR pstrClass, CMarkupNode* pNode, CControlNode* pControlNode)
{
    // 若有控件默认配置先初始化默认属性
    if( pManager ) {
        pControlNode->SetManager(pManager, NULL, false);
        CDuiStringPtrMap* pDefautAttrList = pManager->GetDefaultAttributeList(pstrClass);
        pControlNode->SetAttributeList(pDefautAttrList);
    }

    // 解析所有属性并覆盖默认属性
    if (pNode->HasAttributes()) {
        // Set ordinary attributes
        int nAttributes = pNode->GetAttributeCount();
        for( int i = 0; i < nAttributes; i++ ) {
            LPCTSTR pstrName = pNode->GetAttributeName(i);
            LPCTSTR pstrValue = pNode->GetAttributeValue(i);

            // 应用风格模板
            if( _tcsicmp(pstrName, _T("style")) == 0 ) {
                if( pManager ) {
                    CDuiString sStyleStr(pstrValue);
                    int iPos = sStyleStr.Find(DEFAULT_STYLE_NAME_SEPARATOR);
                    bool styleHasFmt = (iPos >= 0);
                    LPCTSTR pstrStyleName = sStyleStr.Mid(0, iPos).GetData();
                    CDuiStringPtrMap* pStyleAttrList = pManager->GetDefaultAttributeList(pstrStyleName);
                    if( pStyleAttrList ) {
                        if( styleHasFmt ) {
                            CDuiString sStyleArgumentList(sStyleStr.Mid(iPos + 1));
                            pControlNode->SetAttributeList(pStyleAttrList, &sStyleArgumentList);
                        }
                        else {
                            pControlNode->SetAttributeList(pStyleAttrList);
                        }
                    }
                }
            }
            else {
                pControlNode->SetAttribute(pstrName, pstrValue);
            }
        }
    }
}

CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
{
    IContainerUI* pContainer = NULL;
    CControlUI* pReturn = NULL;
    for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
        LPCTSTR pstrClass = node.GetName();
        if( _tcsicmp(pstrClass, _T("Image")) == 0 || _tcsicmp(pstrClass, _T("Font")) == 0 \
            || _tcsicmp(pstrClass, _T("Default")) == 0 || _tcsicmp(pstrClass, _T("Style")) == 0 \
                || _tcsicmp(pstrClass, _T("MultiLanguage")) == 0 ) continue;

        CControlUI* pControl = NULL;
        if( _tcsicmp(pstrClass, _T("Include")) == 0 ) {
            if( !node.HasAttributes() ) continue;
            int count = 1;
            LPTSTR pstr = NULL;
            TCHAR szValue[500] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
                count = _tcstol(szValue, &pstr, 10);
            cchLen = lengthof(szValue) - 1;
            if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
            for ( int i = 0; i < count; i++ ) {
                CDialogBuilder builder;
                if( m_pstrtype != NULL ) { // 使用资源dll，从资源中读取
                    WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
                    pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
                }
                else {
                    pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
                }
            }
            continue;
        }
		//树控件XML解析
		else if( _tcsicmp(pstrClass, _T("TreeNode")) == 0 ) {
			CTreeNodeUI* pParentTreeNode	= static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
			CTreeNodeUI* pTreeNode			= new CTreeNodeUI();
			if( pParentTreeNode ){
				if( !pParentTreeNode->Add(pTreeNode) ){
					delete pTreeNode;
					continue;
				}
			}

			ProcessAttributes(pManager, pstrClass, &node, pTreeNode);

			//检索子节点及附加控件
			if( node.HasChildren() ){
				CControlUI* pSubControl = _Parse(&node, pTreeNode, pManager);
				if( pSubControl && _tcsicmp(pSubControl->GetClass(),_T("TreeNodeUI")) != 0 )
				{
					// 					pSubControl->SetFixedWidth(30);
					// 					CHorizontalLayoutUI* pHorz = pTreeNode->GetTreeNodeHoriznotal();
					// 					pHorz->Add(new CEditUI());
					// 					continue;
				}
			}

			if( !pParentTreeNode ){
				CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));
				ASSERT(pTreeView);
				if( pTreeView == NULL ) return NULL;
				if( !pTreeView->Add(pTreeNode) ) {
					delete pTreeNode;
					continue;
				}
			}
			continue;
		}
        else {
#ifdef _DEBUG
			DUITRACE(_T("Create Control: %s"), pstrClass);
#endif
            SIZE_T cchLen = _tcslen(pstrClass);
            switch( cchLen ) {
            case 4:
                if( _tcsicmp(pstrClass, DUI_CTR_EDIT) == 0 )                  pControl = new CEditUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LIST) == 0 )             pControl = new CListUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TEXT) == 0 )             pControl = new CTextUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TREE) == 0 )             pControl = new CTreeViewUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_HBOX) == 0 )             pControl = new CHorizontalLayoutUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_VBOX) == 0 )             pControl = new CVerticalLayoutUI;
                break;
            case 5:
                if( _tcsicmp(pstrClass, DUI_CTR_COMBO) == 0 )                 pControl = new CComboUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LABEL) == 0 )            pControl = new CLabelUI;
				//else if( _tcsicmp(pstrClass, DUI_CTR_FLASH) == 0 )           pControl = new CFlashUI;
                break;
            case 6:
                if( _tcsicmp(pstrClass, DUI_CTR_BUTTON) == 0 )                pControl = new CButtonUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_OPTION) == 0 )           pControl = new COptionUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_SLIDER) == 0 )           pControl = new CSliderUI;
                break;
            case 7:
                if( _tcsicmp(pstrClass, DUI_CTR_CONTROL) == 0 )               pControl = new CControlUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_ACTIVEX) == 0 )          pControl = new CActiveXUI;
				else if (_tcscmp(pstrClass, DUI_CTR_GIFANIM) == 0)            pControl = new CGifAnimUI;
                break;
            case 8:
                if( _tcsicmp(pstrClass, DUI_CTR_PROGRESS) == 0 )              pControl = new CProgressUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_RICHEDIT) == 0 )         pControl = new CRichEditUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_CHECKBOX) == 0 )		  pControl = new CCheckBoxUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_COMBOBOX) == 0 )		  pControl = new CComboUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_DATETIME) == 0 )		  pControl = new CDateTimeUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TREEVIEW) == 0 )         pControl = new CTreeViewUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TREENODE) == 0 )		  pControl = new CTreeNodeUI;
                break;
            case 9:
                if( _tcsicmp(pstrClass, DUI_CTR_CONTAINER) == 0 )             pControl = new CContainerUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TABLAYOUT) == 0 )        pControl = new CTabLayoutUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_SCROLLBAR) == 0 )        pControl = new CScrollBarUI; 
                break;
            case 10:
                if( _tcsicmp(pstrClass, DUI_CTR_LISTHEADER) == 0 )            pControl = new CListHeaderUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TILELAYOUT) == 0 )       pControl = new CTileLayoutUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_WEBBROWSER) == 0 )       pControl = new CWebBrowserUI;
                break;
			case 11:
				if (_tcsicmp(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)			  pControl = new CChildLayoutUI;
				break;
            case 14:
                if( _tcsicmp(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0 )        pControl = new CVerticalLayoutUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0 )   pControl = new CListHeaderItemUI;
                break;
            case 15:
                if( _tcsicmp(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0 )       pControl = new CListTextElementUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LISTHBOXELEMENT) == 0 )  pControl = new CListHBoxElementUI;
                break;
            case 16:
                if( _tcsicmp(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0 )      pControl = new CHorizontalLayoutUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0 ) pControl = new CListLabelElementUI;
                break;
            case 20:
                if( _tcsicmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0 )  pControl = new CListContainerElementUI;
                break;
            }
            // User-supplied control factory
            if( pControl == NULL ) {
                CDuiPtrArray* pPlugins = CPaintManagerUI::GetPlugins();
                LPCREATECONTROL lpCreateControl = NULL;
                for( int i = 0; i < pPlugins->GetSize(); ++i ) {
                    lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
                    if( lpCreateControl != NULL ) {
                        pControl = lpCreateControl(pstrClass);
                        if( pControl != NULL ) break;
                    }
                }
            }
            if( pControl == NULL && m_pCallback != NULL ) {
                pControl = m_pCallback->CreateControl(pstrClass);
            }
        }

#ifndef _DEBUG
        ASSERT(pControl);
#endif // _DEBUG
			if( pControl == NULL )
			{
#ifdef _DEBUG
				DUITRACE(_T("Unknow Control:%s"),pstrClass);
#endif
				continue;
			}

        // Add children
        if( node.HasChildren() ) {
            _Parse(&node, pControl, pManager);
        }
        TCHAR szValue[256] = { 0 };
        int cchLen = lengthof(szValue) - 1;
        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
        if( pParent != NULL ) {
            LPCTSTR lpValue = szValue;
            if( node.GetAttributeValue(_T("cover"), szValue, cchLen) && _tcscmp(lpValue, _T("true")) == 0 ) {
                pParent->SetCover(pControl);
            }
            else {
                CTreeNodeUI* pContainerNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(DUI_CTR_TREENODE));
                if(pContainerNode)
                    pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
                else
                {
                    if( pContainer == NULL ) pContainer = static_cast<IContainerUI*>(pParent->GetInterface(DUI_CTR_ICONTAINER));
                    ASSERT(pContainer);
                    if( pContainer == NULL ) return NULL;
                    if( !pContainer->Add(pControl) ) {
                        pControl->Delete();
                        continue;
                    }
                }
            }
        }

        ProcessAttributes(pManager, pstrClass, &node, pControl);

        if( pManager ) {
            pControl->SetManager(NULL, NULL, false);
        }
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

} // namespace DuiLib
