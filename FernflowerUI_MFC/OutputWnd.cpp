
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

std::mutex mut;
bool Finished;
HHOOK MainMessage;
CWnd * TaskList;
int ButtonID;

CProgressDlg::CProgressDlg(CWnd * MainWnd) : CDialogEx(IDD_PROGRESSBAR,MainWnd),DoProcessInit(true),isExited(STILL_ACTIVE),NewFileSize(0),DecomplieProcess(INVALID_HANDLE_VALUE)
{
}

LRESULT CProgressDlg::OnInitDialog(WPARAM wParam,LPARAM lParam)
{
	LRESULT bRet = HandleInitDialog(wParam, lParam);
	m_wndProgressBar = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS1));
	m_wndProgressBar->SetRange32(0, INT_MAX / 2);
	m_wndProgressBar->GetRange(iLow, iHigh);
	m_wndProgressBar->SetPos(iLow);
	m_wndProgressBar->SetStep(1);
	Pre = 0;
	//MainMessage = SetWindowsHookEx(WH_MOUSE_LL, ShowMainWnd, AfxGetApp()->m_hInstance, 0);
	return bRet;
}

void CProgressDlg::OnShowWindow(BOOL bShow,UINT nStatus)
{
//	this->ModifyStyle(WS_POPUP, WS_CHILD);
//	this->SetParent(AfxGetMainWnd());
	this->CDialogEx::OnShowWindow(bShow, nStatus);
//	AfxGetMainWnd()->ModifyStyleEx(true, WS_EX_TOOLWINDOW);
	PostMessage(WM_COMMAND, ID_DOPROGRESS);
	/*CWnd * Tray = FindWindow(L"Shell_TrayWnd", nullptr);
	CWnd * ReBar, *Tasks;
	if (Tray->GetSafeHwnd())
	{
		ReBar = Tray->FindWindowExW(Tray->GetSafeHwnd(), nullptr, L"ReBarWindow32", nullptr);
	}
	else
	{
		return;
	}
	if (ReBar->GetSafeHwnd())
	{
		Tasks = ReBar->FindWindowExW(ReBar->GetSafeHwnd(), nullptr, L"MSTaskSwWClass", nullptr);
	}
	else
	{
		return;
	}
	if (Tasks->GetSafeHwnd())
	{
		TaskList = Tasks->FindWindowExW(Tasks->GetSafeHwnd(), nullptr, L"MSTaskListWClass", nullptr);
	}
	else
	{
		return;
	}
	if (TaskList->GetSafeHwnd())
	{
		for (size_t i = 0; i <= TaskList->SendMessage(TB_BUTTONCOUNT,0,0); i++)
		{
			DWORD ProID;
			SIZE_T size;
			wchar_t * WindowName = new wchar_t[MAX_PATH];
			memset(WindowName, 0, MAX_PATH);
			GetWindowThreadProcessId(TaskList->GetSafeHwnd(), &ProID);
			HANDLE hPro = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, ProID);
			LPVOID Buf = VirtualAllocEx(hPro, nullptr, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			int len = TaskList->SendMessage(TB_GETBUTTONTEXT, i, (LPARAM)Buf);
			if (len!=-1)
			{
				ReadProcessMemory(hPro, Buf, WindowName, MAX_PATH, &size);
			}
			else
			{
				delete WindowName;
				break;
			}
			VirtualFreeEx(hPro, Buf, MAX_PATH, MEM_DECOMMIT);
			CStringW str;
			AfxGetMainWnd()->GetWindowTextW(str);
			if (str==WindowName)
			{
				ButtonID = i;
				delete WindowName;
				break;
			}
			delete WindowName;
		}
	}
	*/
}

void CProgressDlg::DoProgress()
{
	if (DoProcessInit)
	{
		if (!static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->CanUseProgress)
		{
			m_wndProgressBar->SetMarquee(true, 5);
		}
		DecomplieProcess = OpenProcess(PROCESS_ALL_ACCESS, false, static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->pi.dwProcessId);
		if (DecomplieProcess == INVALID_HANDLE_VALUE)
		{
			MessageBox(L"出现未知错误，无法显示进度条", L"错误", MB_ICONERROR);
			static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->CanUseProgress = false;
		}
		AfxGetMainWnd()->EnableWindow(true);
		mut.unlock();
		DoProcessInit = false;
	}
	if (isExited == STILL_ACTIVE)
	{
		if(DecomplieProcess!=INVALID_HANDLE_VALUE)
			GetExitCodeProcess(DecomplieProcess, &isExited);
		if (static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->CanUseProgress)
		{
			NewFileSize = GetFileSize(static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->hfile_new, nullptr);
			int Pos = int(double(NewFileSize) / double(static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->ChosenFileSize)*(iHigh - iLow)) + iLow;
			if (Pos!=Pre)
			{
				if (Pos == (Pre + 1))
					m_wndProgressBar->StepIt();
				else
					m_wndProgressBar->SetPos(Pos);
				Pre = m_wndProgressBar->GetPos();
				static_cast<CMainFrame*>(AfxGetMainWnd())->m_pTaskBar->SetProgressValue(AfxGetMainWnd()->GetSafeHwnd(), Pos, INT_MAX / 2);
			}
			Sleep(40);
		}
		else
		{
			Sleep(40);
		}
		PostMessage(WM_COMMAND, ID_DOPROGRESS);
		return;
	}
	CloseHandle(DecomplieProcess);
	if (isExited != 0)
	{
		std::ostringstream sout;
		sout << "Java虚拟机异常退出，退出码为" << isExited;
		MessageBoxA(AfxGetMainWnd()->m_hWnd, sout.str().c_str(), "错误", MB_ICONERROR);
	}
	//UnhookWindowsHookEx(MainMessage);
	static_cast<CMainFrame*>(AfxGetMainWnd())->m_pTaskBar->SetProgressValue(AfxGetMainWnd()->GetSafeHwnd(), 0, 0);
	EndDialog(true);
//	AfxGetMainWnd()->ModifyStyleEx(WS_EX_TOOLWINDOW, true);
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

LRESULT CALLBACK ShowMainWnd(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_SYSCOMMAND)
	{
		if(wParam==SC_RESTORE)
			AfxGetMainWnd()->SetForegroundWindow();
	}
	return CallNextHookEx(MainMessage, nCode, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
	ON_MESSAGE(WM_INITDIALOG,&CProgressDlg::OnInitDialog)
	ON_COMMAND(ID_DOPROGRESS,&CProgressDlg::DoProgress)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出选项卡窗口\n");
		return -1;      // 未能创建
	}

	// 创建输出窗格: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡: 
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);

	// 使用一些虚拟文本填写输出选项卡(无需复杂数据)
	FillBuildWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndOutputBuild.AddString(_T("反编译输出正显示在此处。"));
	m_wndOutputBuild.AddString(_T("输出正显示在列表视图的行中"));
	m_wndOutputBuild.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::OutputLog(const std::wstring & Log)
{
	std::ifstream fin;
	for (int i = m_wndOutputBuild.GetCount(); i >= 0; i--)
	{
		m_wndOutputBuild.DeleteString(i);
	}
	szOutput = Log;
	szFile = Log;
	for (size_t i = szOutput.length(); szOutput[i]!='\\' ; i--)
	{
		szOutput[i] = L'\0';
	}
	command = L"/c java -jar %USERPROFILE%\\AppData\\Local\\FernFlowerUI\\fernflower.jar";
	CStringW ss;
	ss += szOutput.c_str();
	ss += L"decomplie";
	int iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		ss,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	char * tmp = new char[iTextLen + 1];
	memset(tmp, 0, sizeof(char)*(iTextLen + 1));
	WideCharToMultiByte(CP_ACP, 0, ss, -1, tmp, iTextLen, nullptr, nullptr);
	_mkdir(tmp);
	delete tmp;
	ss += L"\\DecomplieLog.txt";
	command += L"  ";
	command += Log.c_str();
	command += L"  ";
	command += szOutput.c_str();
	command += L"decomplie\\";
	command += L"  >";
	command += ss;
	{
		CanUseProgress = false;
		SHFILEINFOW FileInfo = { 0 };
		SHGetFileInfoW(static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->szFile.c_str(),
			0, &FileInfo, sizeof(FileInfo), SHGFI_DISPLAYNAME);
		CStringW newfile = static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->szOutput.c_str();
		newfile += L"decomplie\\";
		newfile += FileInfo.szDisplayName;
		HANDLE hfile_old = CreateFileW(static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->szFile.c_str(),
			GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (hfile_old == INVALID_HANDLE_VALUE)
		{
			MessageBox(L"出现未知错误，无法显示进度条", L"错误", MB_ICONERROR);
			CanUseProgress = false;
		}
		else
		{
			CanUseProgress = true;
			ChosenFileSize = GetFileSize(hfile_old, nullptr);
		}
		CloseHandle(hfile_old);
		hfile_new = CreateFileW(newfile, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (hfile_new == INVALID_HANDLE_VALUE)
		{
			MessageBox(L"出现未知错误，无法显示进度条", L"错误", MB_ICONERROR);
			CanUseProgress = false;
		}
		STARTUPINFOW startinfo = { 0 };
		startinfo.cb = sizeof(STARTUPINFOW);
		startinfo.dwFlags = STARTF_USESHOWWINDOW;
		startinfo.lpTitle = L"Decomplie Thread";
		startinfo.wShowWindow = SW_HIDE;
		const wchar_t * com = static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->command;
		bool b = CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", const_cast<wchar_t*>(com), nullptr, nullptr, false,
			CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE|IDLE_PRIORITY_CLASS,
			nullptr, nullptr, &startinfo, &pi);
		char * buf;
		size_t len;
		if (_dupenv_s(&buf,&len,"USERPROFILE"))
		{
			AfxGetMainWnd()->MessageBox(L"搜索路径%USERPROFILE%失败!", L"错误", MB_ICONERROR);
			return;
		}
		while (true)
		{
			if (ChosenFileSize<=1000000)
			{
				break;
			}
			CStringW str;
			str += "/c jps >";
			str += buf;
			str += "\\AppData\\Local\\FernFlowerUI\\ProcessIDCache.txt";
			STARTUPINFOW start = { 0 };
			start.cb = sizeof(STARTUPINFOW);
			start.dwFlags = STARTF_USESHOWWINDOW;
			start.wShowWindow = SW_HIDE;
			PROCESS_INFORMATION jps;
			bool bn = CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", str.GetBuffer(), nullptr, nullptr, false,
				CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE | IDLE_PRIORITY_CLASS,
				nullptr, nullptr, &start, &jps);
			if (!bn)
			{
				AfxGetMainWnd()->MessageBox(L"设置进程相关性失败,CPU占用可能会过高!", L"警告", MB_ICONWARNING);
				break;
			}
			DWORD exit=STILL_ACTIVE;
			while (exit==STILL_ACTIVE)
			{
				GetExitCodeProcess(jps.hProcess, &exit);
				Sleep(250);
			}
			std::string ProcessFile = buf;
			ProcessFile += "\\AppData\\Local\\FernFlowerUI\\ProcessIDCache.txt";
			fin.open(ProcessFile);
			DWORD JavaProcessID;
			try
			{
				std::string tmp;
				std::istringstream sin;
				while ((std::getline(fin, tmp)) && (tmp.find("fernflower.jar") == std::string::npos))
				{
					continue;
				}
				sin.str(tmp);
				sin >> JavaProcessID;
			}
			catch (const std::exception & ex)
			{
				MessageBoxA(AfxGetMainWnd()->m_hWnd, ex.what(), "发生异常", MB_ICONERROR);
			}
			HANDLE JavaProcess = OpenProcess(PROCESS_ALL_ACCESS, false, JavaProcessID);
			if (JavaProcess == INVALID_HANDLE_VALUE || !SetProcessAffinityMask(JavaProcess, 0x1))
			{
				AfxGetMainWnd()->MessageBox(L"设置进程相关性失败,CPU占用可能会过高!", L"警告", MB_ICONWARNING);
			}
			CloseHandle(JavaProcess);
			break;
		}
		free(buf);
		fin.close();
		if (!b)
		{
			AfxGetMainWnd()->MessageBox(L"创建线程失败!", L"错误", MB_ICONERROR);
			CloseHandle(hfile_new);
			return;
		}
		else
		{
			AfxGetMainWnd()->SetWindowText(L"FernFlowerUI -正在反编译，请耐心等待");
			/*int iLow = static_cast<CMainFrame*>(AfxGetMainWnd())->Low;
			int iHigh = static_cast<CMainFrame*>(AfxGetMainWnd())->High;
			DWORD isExited=STILL_ACTIVE,NewFileSize=0;
			static_cast<CMainFrame*>(AfxGetMainWnd())->m_hProgressBar.ShowWindow(SW_SHOW);
			if (!CanUseProgress)
			{
				static_cast<CMainFrame*>(AfxGetMainWnd())->m_hProgressBar.SetMarquee(true, 5);
			}
			while (isExited==STILL_ACTIVE)
			{
				GetExitCodeProcess(pi.hProcess, &isExited);
				if (CanUseProgress)
				{
					NewFileSize = GetFileSize(hfile_new, nullptr);
					int Pos = int(double(NewFileSize) / double(ChosenFileSize)*(iHigh - iLow)) + iLow;
					static_cast<CMainFrame*>(AfxGetMainWnd())->m_hProgressBar.SetPos(Pos);
					Sleep(250);
				}
				else
				{
					Sleep(250);
				}
			}
			if (isExited!=0)
			{
				std::ostringstream sout;
				sout << "Java虚拟机异常退出，退出码为" << isExited;
				MessageBoxA(AfxGetMainWnd()->m_hWnd, sout.str().c_str(), "错误", MB_ICONERROR);
			}
			static_cast<CMainFrame*>(AfxGetMainWnd())->m_hProgressBar.ShowWindow(SW_HIDE);
			*/
			CProgressDlg ProgressDlg(AfxGetMainWnd());
			std::future<bool>scan = std::async(std::launch::async,&ScanLog,ss ,AfxGetMainWnd());
			mut.lock();
			ProgressDlg.DoModal();
			CloseHandle(hfile_new);
			mut.lock();
			CWaitDlg WaitDlg(AfxGetMainWnd(), []() {return Finished; });
			WaitDlg.DoModal();
			if (!scan.get())
			{
				if (AfxGetMainWnd()->MessageBox(L"打开日志失败,是否使用电脑默认文本编辑器打开?",L"错误",MB_ICONERROR)==IDOK)
				{
					ShellExecute(AfxGetMainWnd()->m_hWnd, L"open", ss, nullptr, nullptr, SW_SHOW);
				}
			}
			mut.unlock();
		}
		AfxGetMainWnd()->SetWindowText(L"FernFlowerUI");
		//HANDLE hLogtxt = CreateFile(ss, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	}
}

bool ScanLog(const CStringW & filename, CWnd * MainWnd)
{
	Sleep(10);
	Finished = false;
	mut.lock();
	mut.unlock();
	std::string contact;
	COutputWnd & out = *(dynamic_cast<CMainFrame*>(MainWnd)->GetOutput());
	CStringW addstr;
	std::ifstream fin;
	fin.open(filename, std::ios_base::in);
	HANDLE hLog = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if (hLog == INVALID_HANDLE_VALUE)
	{
		out.FillBuildWindow();
		Finished = true;
		return false;
	}
	if (!fin)
	{
		out.FillBuildWindow();
		Finished = true;
		return false;
	}
	else
	{
		try
		{
			while (mut.try_lock())
			{
				fin.clear();
				if (std::getline(fin, contact))
				{
					addstr = contact.c_str();
					out.m_wndOutputBuild.AddString(addstr);
				}
				mut.unlock();
				Sleep(10);
			}
			while (std::getline(fin, contact))
			{
				addstr = contact.c_str();
				out.m_wndOutputBuild.AddString(addstr);
			}
		}
		catch (const std::exception& ex)
		{
			std::string err = "打开日志时发生异常:";
			err += ex.what();
			if (MessageBoxA(MainWnd->m_hWnd, err.c_str(), "错误", MB_ICONERROR) == IDOK)
			{
			}
			out.FillBuildWindow();
			Finished = true;
			return false;
		}
		fin.close();
		CloseHandle(hLog);
	}
	Finished = true;
	return true;
}


void COutputWnd::UpdateFonts()
{
	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("复制输出"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("清除输出"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

CWaitDlg::CWaitDlg(CWnd * MainWnd,bool(*c)()):Condit(c),CDialogEx(IDD_PROGRESSBAR,MainWnd)
{
}

LRESULT CWaitDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT bRet = HandleInitDialog(wParam, lParam);
	m_wndProgressBar = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS1));
	m_wndProgressBar->SetRange(0, 100);
	m_wndProgressBar->SetStep(1);
	m_wndProgressBar->SetMarquee(true, 5);
	return bRet;
}

void CWaitDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	PostMessage(WM_COMMAND, ID_DOPROGRESS);
}

void CWaitDlg::DoProgress()
{
	if (!Condit())
	{
		PostMessage(WM_COMMAND, ID_DOPROGRESS);
		return;
	}
	EndDialog(true);
}

void CWaitDlg::DoDataExchange(CDataExchange * pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWaitDlg, CDialogEx)
	ON_MESSAGE(WM_INITDIALOG, &CWaitDlg::OnInitDialog)
	ON_COMMAND(ID_DOPROGRESS, &CWaitDlg::DoProgress)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()