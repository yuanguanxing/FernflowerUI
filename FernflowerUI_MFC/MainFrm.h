
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "OutputWnd.h"
#include "FileView.h"
#include "ClassView.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
// 特性
public:
	friend class COutputWnd;
	friend class CProgressDlg;
	friend class CShellTreeView;
	friend class CFernflowerUIMFCApp;
	friend void UnZipJar(CWnd * MainWnd, const CStringW & JarPath);
	ITaskbarList4 * m_pTaskBar;
// 操作
public:
	COutputWnd * GetOutput()
	{
		return &m_wndOutput;
	}
	CMFCStatusBar * GetStatusBar()
	{
		return &m_wndStatusBar;
	}
	typedef    void    (WINAPI    *PROCSWITCHTOTHISWINDOW)    (HWND, BOOL);
	PROCSWITCHTOTHISWINDOW    SwitchToThisWindow;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COutputWnd        m_wndOutput;
	CClassView        m_wndClassView;
	CFileView         m_wndFileView;
	int Low;
	int High;
	bool              EnableRecovery;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnDropFiles(HDROP DropFile);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnEnableRecovery();
	afx_msg void OnUpdateEnableRecovery(CCmdUI *pCmdUI);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

