
// testQMPluginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testQMPlugin.h"
#include "testQMPluginDlg.h"
#include "afxdialogex.h"


#include "Cdmsoft.h"
#include"Room.h"
#include"test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CtestQMPluginDlg dialog



CtestQMPluginDlg::CtestQMPluginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTQMPLUGIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestQMPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestQMPluginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CtestQMPluginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OpenConsole, &CtestQMPluginDlg::OnBnClickedOpenconsole)
END_MESSAGE_MAP()


// CtestQMPluginDlg message handlers

Cdmsoft dm;
BOOL CtestQMPluginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	::CoInitialize(NULL);
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(OLESTR("dm.dmsoft"), &clsid);
	dm.CreateDispatch(clsid);
	dm.Reg(L"dieofai3e4c4149f6970cd69b4fc3af7ac85de4", L"0001");

	test::OpenConsole();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestQMPluginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestQMPluginDlg::OnPaint()
{


	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}


}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestQMPluginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestQMPluginDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here


	dm.RightClick();

	
	test::getMonsterOverlay(dm, gandalfr::CRectangle());
//	gandalfr::CMonster::m_vecCMon.push_back(gandalfr::CMonster(gandalfr::CRectangle()));
//	CDialogEx::OnCancel();
}


void CtestQMPluginDlg::OnBnClickedOpenconsole()
{
	// TODO: Add your control notification handler code here
	test::OpenConsole();

}
