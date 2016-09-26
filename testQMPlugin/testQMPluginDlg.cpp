
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
	, CSLayerWidth(_T("100"))
	, CSLayerHeight(_T("50"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestQMPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LayerWidth, CSLayerWidth);
	DDX_Text(pDX, IDC_LayerHeight, CSLayerHeight);
}

BEGIN_MESSAGE_MAP(CtestQMPluginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CtestQMPluginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OpenConsole, &CtestQMPluginDlg::OnBnClickedOpenconsole)
	ON_BN_CLICKED(IDC_findMonster, &CtestQMPluginDlg::OnBnClickedfindmonster)
	ON_BN_CLICKED(IDC_BUTTON1, &CtestQMPluginDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtestQMPluginDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtestQMPluginDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CtestQMPluginDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CtestQMPluginDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CtestQMPluginDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CtestQMPluginDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CtestQMPluginDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CtestQMPluginDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDCANCEL, &CtestQMPluginDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON10, &CtestQMPluginDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CtestQMPluginDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CtestQMPluginDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CtestQMPluginDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CtestQMPluginDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CtestQMPluginDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CtestQMPluginDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON17, &CtestQMPluginDlg::OnBnClickedButton17)
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

	test::initialTest();
	::InitializeCriticalSection(&CKeyOp::g_csKeyOp);

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



	UpdateData(TRUE);
//	test::getMonsterOverlay(dm, gandalfr::CRectangle(0,0, ::_ttoi(CSLayerWidth) , ::_ttoi(CSLayerHeight) ));
//	gandalfr::CMonster::m_vecCMon.push_back(gandalfr::CMonster(gandalfr::CRectangle()));
//	CDialogEx::OnCancel();
}


void CtestQMPluginDlg::OnBnClickedOpenconsole()
{
	// TODO: Add your control notification handler code here
	test::OpenConsole();	
}



void CtestQMPluginDlg::OnBnClickedfindmonster()
{
	// TODO: Add your control notification handler code here
//	test::findmonster(dm);
}



void CtestQMPluginDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	test::InitialNeural();

}


void CtestQMPluginDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	test::runInsZone(dm);
}


void CtestQMPluginDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	test::printSetKeyOp();
}


void CtestQMPluginDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	test::printCurNeural();
}


void CtestQMPluginDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here

	test::reset();
}


void CtestQMPluginDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
//	test::printImage(dm);
}


void CtestQMPluginDlg::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
//	test::performanceCustomVSdm(dm);
}


void CtestQMPluginDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	test::PrintRoomState(dm);

}


void CtestQMPluginDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	test::estimateTotalRun(dm);
}


void CtestQMPluginDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	test::exitTheNeuralThread();
	Sleep(2000);
	CDialogEx::OnCancel();
}


void CtestQMPluginDlg::OnBnClickedButton10()
{
	// TODO: Add your control notification handler code here
	test::beginKeyboardThread();
}


void CtestQMPluginDlg::OnBnClickedButton11()
{
	// TODO: Add your control notification handler code here
	Sleep(1000);
	test::beginNeuralThread();
}


void CtestQMPluginDlg::OnBnClickedButton12()
{
	// TODO: Add your control notification handler code here
	test::exitTheNeuralThread();
}


void CtestQMPluginDlg::OnBnClickedButton13()
{
	// TODO: Add your control notification handler code here
	test::pauseNeuralThread();
}


void CtestQMPluginDlg::OnBnClickedButton14()
{
	// TODO: Add your control notification handler code here
	test::restartNeuralThread();
}


void CtestQMPluginDlg::OnBnClickedButton15()
{
	// TODO: Add your control notification handler code here
	test::testGetPlayer(dm);
}


void CtestQMPluginDlg::OnBnClickedButton16()
{
	// TODO: Add your control notification handler code here


	UpdateData(TRUE);
	test::RunToTarget(dm, ::_ttoi(CSLayerWidth), ::_ttoi(CSLayerHeight));
	//test::ExecuteTraiDeviation(dm);
}


void CtestQMPluginDlg::OnBnClickedButton17()
{
	// TODO: Add your control notification handler code here
	test::printRunState();
}
