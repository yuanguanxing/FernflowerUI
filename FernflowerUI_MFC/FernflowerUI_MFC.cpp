
// FernflowerUI_MFC.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "FernflowerUI_MFC.h"
#include "MainFrm.h"

#include "FernflowerUI_MFCDoc.h"
#include "FernflowerUI_MFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFernflowerUIMFCApp

BEGIN_MESSAGE_MAP(CFernflowerUIMFCApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CFernflowerUIMFCApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CFernflowerUIMFCApp::OpenFile)
END_MESSAGE_MAP()

bool AccessJar(HWND hwnd);
bool DownloadJar();

// CFernflowerUIMFCApp 构造

CFernflowerUIMFCApp::CFernflowerUIMFCApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("FernflowerUIMFC.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CFernflowerUIMFCApp 对象

CFernflowerUIMFCApp theApp;


// CFernflowerUIMFCApp 初始化

BOOL CFernflowerUIMFCApp::InitInstance()
{
	CWinAppEx::InitInstance();


	EnableTaskbarInteraction(FALSE);
	CoInitialize(nullptr);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFernflowerUIMFCDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CFernflowerUIMFCView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();
	AfxGetMainWnd()->SetWindowText(L"FernFlowerUI");
	AccessJar(AfxGetMainWnd()->m_hWnd);
	return TRUE;
}

// CFernflowerUIMFCApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// 用于运行对话框的应用程序命令
void CFernflowerUIMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CFernflowerUIMFCApp 自定义加载/保存方法

void CFernflowerUIMFCApp::OpenFile()
{
	if (!AccessJar(AfxGetMainWnd()->m_hWnd))
	{
		return;
	}
	CFileDialog OpenDlg(true, nullptr, nullptr, OFN_EXPLORER | OFN_FORCESHOWHIDDEN, L"Jar文件(*.jar);Zip文件(*.zip)|*.jar;*.zip||", m_pMainWnd);
	if (OpenDlg.DoModal()==IDOK)
	{
		static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutput()->OutputLog(OpenDlg.GetPathName().GetBuffer());
	}
	else
	{
		return;
	}
}


bool AccessJar(HWND hwnd)
{
	char * java_home;
	size_t tmp;
	errno_t er = _dupenv_s(&java_home, &tmp, "JAVA_HOME");
	if ( er || (java_home==nullptr) )
	{
		if (MessageBoxA(hwnd,"发现未安装Java虚拟机,本程序需要Java虚拟机才能正常使用,是否打开下载链接?","警告",MB_ICONWARNING)==IDOK)
		{
			ShellExecuteA(NULL, "open", "http://www.oracle.com/technetwork/java/javase/overview/index.html", NULL, NULL, SW_SHOW);
			return false;
		}
	}
	free(java_home);
	char * userprofile;
	size_t len;
	if (_dupenv_s(&userprofile, &len, "USERPROFILE"))
	{
		return false;
	}
	std::string saccess = userprofile;
	free(userprofile);
	saccess += "\\AppData\\Local\\FernFlowerUI";
	if (_access(saccess.c_str(), 0) == -1)
	{
		_mkdir(saccess.c_str());
		if (MessageBoxA(hwnd, "检测到是第一次使用，是否要下载fernflower.jar?", "", MB_OKCANCEL) == IDOK)
			return DownloadJar();
		else
		{
			return false;
		}
	}
	else if (_access((saccess + "\\fernflower.jar").c_str(), 0) == -1)
	{
		if (MessageBoxA(hwnd, "找不到%USERPROFILE%\\AppData\\Local\\FernFlowerUI\\fernflower.jar,是否要下载?", "", MB_OKCANCEL) == IDOK)
			return DownloadJar();
		else
		{
			return false;
		}
	}
	else
	{
		HANDLE hDecomp = CreateFileA((saccess + "\\fernflower.jar").c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if ((hDecomp == INVALID_HANDLE_VALUE) || (GetFileSize(hDecomp, nullptr) < 248683))
		{
			CloseHandle(hDecomp);
			if (MessageBoxA(hwnd, "找不到%USERPROFILE%\\AppData\\Local\\FernFlowerUI\\fernflower.jar,是否要下载?", "", MB_OKCANCEL) == IDOK)
				return DownloadJar();
			else
			{
				return false;
			}
		}
		CloseHandle(hDecomp);
	}
	return true;
}

const int MAXBLOCKSIZE = 1024;
char Buffer[MAXBLOCKSIZE];
unsigned long Number = 1;
FILE* stream;
HINTERNET hSession,handle;

bool DownloadJar()
{
	hSession = InternetOpenA("FernFlowerUI2.1", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
	try
	{
		if (hSession)
		{
			handle = InternetOpenUrlA(hSession, "https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar",
				nullptr, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle)
			{
				char * filestr;
				size_t length;
				errno_t err = _dupenv_s(&filestr, &length, "USERPROFILE");
				if (err)
				{
					MessageBoxA(nullptr, "无法打开%USERPROFILE%", "错误", MB_ICONERROR);
					return false;
				}
				std::string filename = filestr;
				free(filestr);
				filename += "\\AppData\\Local\\FernFlowerUI\\fernflower.jar";
				if (!fopen_s(&stream, filename.c_str(), "wb"))
				{
					CWaitDlg wait(AfxGetMainWnd(), []() ->bool 
					{
						InternetReadFile(handle, Buffer, MAXBLOCKSIZE - 1, &Number);		
						fwrite(Buffer, sizeof(char), Number, stream);
						bool Downloaded = !(Number > 0);
						return Downloaded;
					});
					wait.DoModal();
					/*while (Number > 0)
					{
						InternetReadFile(handle, Buffer, MAXBLOCKSIZE - 1, &Number);
						fwrite(Buffer, sizeof(char), Number, stream);
					}*/
					fclose(stream);
				}
				InternetCloseHandle(handle);
				handle = nullptr;
			}
			else
			{
				InternetCloseHandle(hSession);
				hSession = nullptr;
				throw std::bad_exception();
			}
			InternetCloseHandle(hSession);
			hSession = nullptr;
		}
		else
		{
			throw std::bad_exception();
		}
	}
	catch (const std::exception&)
	{
		MessageBox(nullptr, L"出现未知错误,下载失败!\n尝试手动下载,链接:https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar", L"下载失败", MB_ICONERROR);
		ShellExecuteA(NULL, "open", "https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar", NULL, NULL, SW_SHOW);
	}
	return true;
}

void CFernflowerUIMFCApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CFernflowerUIMFCApp::LoadCustomState()
{
}

void CFernflowerUIMFCApp::SaveCustomState()
{
}

// CFernflowerUIMFCApp 消息处理程序



