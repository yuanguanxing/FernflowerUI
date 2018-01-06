
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口

class COutputList : public CListBox
{
// 构造
public:
	COutputList();

// 实现
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();
	friend class CFernflowerUIMFCApp;
	friend class CProgressDlg;
	friend bool ScanLog(const CStringW & filename,CWnd * MainWnd);
	void UpdateFonts();

// 特性
protected:
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputBuild;

protected:
	void FillBuildWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

private:
	CStringW command;
	std::wstring szOutput;
	std::wstring szFile;
	bool CanUseProgress;
	DWORD ChosenFileSize;
	HANDLE hfile_new;
	PROCESS_INFORMATION pi;

// 实现
public:
	virtual ~COutputWnd();
	void OutputLog(const std::wstring & Log);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

class CProgressDlg : public CDialogEx
{
public:
	CProgressDlg(CWnd * MainWnd);
	afx_msg LRESULT OnInitDialog(WPARAM, LPARAM);
	void CProgressDlg::OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void DoProgress();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
														// 实现
	CProgressCtrl * m_wndProgressBar;
	int iLow;
	int iHigh;
	int Pre;
	DWORD isExited;
	DWORD NewFileSize;
	HANDLE DecomplieProcess;
	bool DoProcessInit;
protected:
	DECLARE_MESSAGE_MAP()
};
LRESULT CALLBACK ShowMainWnd(int nCode, WPARAM wParam, LPARAM lParam);

class CWaitDlg : public CDialogEx
{
public:
	CWaitDlg(CWnd * MainWnd, bool(*c)());
	afx_msg LRESULT OnInitDialog(WPARAM, LPARAM);
	void CWaitDlg::OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void DoProgress();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
	CProgressCtrl * m_wndProgressBar;
	bool(*Condit) ();
protected:
	DECLARE_MESSAGE_MAP()
};