
// FernflowerUI_MFCView.h: CFernflowerUIMFCView 类的接口
//

#pragma once
#include "FernflowerUI_MFCDoc.h"
#include "ViewTree.h"


class CFernflowerUIMFCView : public CView
{
protected: // 仅从序列化创建
	CFernflowerUIMFCView();
	DECLARE_DYNCREATE(CFernflowerUIMFCView)

// 特性
public:
	CFernflowerUIMFCDoc* GetDocument() const;
	void SetViewText(const CStringW & Contact);
	friend class CFernflowerUIMFCApp;

// 操作
public:
	int FinishHighLight;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	//Java关键字(用作语法高亮)
	static const CStringW KeyWord[50];
	//Java运算符(用作语法高亮)
	static const CStringW Operators[24];
	static const CStringW Strings[2];

// 实现
public:
	virtual ~CFernflowerUIMFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CRichEditCtrl m_wndEdit;

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // FernflowerUI_MFCView.cpp 中的调试版本
inline CFernflowerUIMFCDoc* CFernflowerUIMFCView::GetDocument() const
   { return reinterpret_cast<CFernflowerUIMFCDoc*>(m_pDocument); }
#endif

