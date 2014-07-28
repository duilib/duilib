
// UIDesignerDoc.h : CUIDesignerDoc 类的接口
//


#pragma once


class CUIDesignerDoc : public CDocument
{
protected: // 仅从序列化创建
	CUIDesignerDoc();
	DECLARE_DYNCREATE(CUIDesignerDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CUIDesignerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnMdiCopyFullPath();
	afx_msg void OnMdiOpenFullPath();

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
};


