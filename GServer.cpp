//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// GServer.cpp : Defines the class behaviors for the application.
//
//

#include "stdafx.h"
#include "GServer.h"

#include "MainFrm.h"
#include "GServerDoc.h"
#include "GServerView.h"
#include "winver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGServerApp

BEGIN_MESSAGE_MAP(CGServerApp, CWinApp)
	//{{AFX_MSG_MAP(CGServerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGServerApp construction

CGServerApp::CGServerApp()
{
	free((void*)m_pszHelpFilePath);
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGServerApp object

CGServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGServerApp initialization

BOOL CGServerApp::InitInstance()
{
	m_pszHelpFilePath = _tcsdup(_T("..\\VACS Server.HLP"));
	//m_pszHelpFilePath = _tcsdup(_T("d:\\Work\\VACS Server.HLP"));
	

\
	{
\
		CCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\

\
	}                           
	if (!AfxSocketInit(&m_wsadata))
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("CorTek\\GSERVER\\1.00"));

	LoadStdProfileSettings(7);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGServerDoc),				// Doc
		RUNTIME_CLASS(CMainFrame),				// main SDI frame window
		RUNTIME_CLASS(CGServerView));			// View

	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
  BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

static  HFONT   hFontDlg;
static  LPSTR   lpVersion;
static  DWORD   dwVerInfoSize;
static  DWORD   dwVerHnd;
static  UINT    uVersionLen;
static  WORD    wRootLen;
static  BOOL    bRetCode;

static  char    szFullPath[256];
static  char    szResult[256];
static  char    szGetName[256];

static  int     i,iDlg;

  HINSTANCE hInstance = AfxGetInstanceHandle();  
  GetModuleFileName(hInstance, szFullPath, sizeof(szFullPath));
  dwVerInfoSize = GetFileVersionInfoSize(szFullPath , &dwVerHnd);

  if(dwVerInfoSize)   // If we can get the version info then process it
  {
    LPSTR lpstrVffInfo;
    HANDLE  hMem;

        hFontDlg = CreateFont(14,0, 0, 0,
                               0, 0, 0, 0,
                               0, 0, 0, 0,
                               VARIABLE_PITCH | FF_SWISS,"");


    hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
    lpstrVffInfo = (char *)GlobalLock(hMem);

    bRetCode=GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

		// Handle the language gracefully
    // (Under both Windows NT and Windows 95, you can get the language information in 
    // the FileVersionInfo of User.exe by calling GetFileVersionInfo, and then 
    // VerQueryValue (on \\VarFileInfo\\Translation") on the VersionInfo block of 
    // the operating system's User.exe.)
    // Implement the above later ??????????????????????????????????

		if (LANG_JAPANESE == PRIMARYLANGID(GetUserDefaultLangID())) // User can change this is control panel!!
			lstrcpy(szGetName, "\\StringFileInfo\\041104E4\\");
		else
			lstrcpy(szGetName, "\\StringFileInfo\\040904B0\\");

    wRootLen = lstrlen(szGetName);

#ifdef TEST
    VS_FIXEDFILEINFO *lpVersion;
#endif
    
    // Walk through the dialog items that we want to replace
    for(i = IDT_COMPANY_NAME; i <= IDT_TRADEMARKS; i++)
    {
      GetDlgItemText(i, szResult, sizeof(szResult));
      szGetName[wRootLen] = (char)0;
      lstrcat(szGetName, szResult);

      uVersionLen = 0;
      lpVersion = NULL;

      bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
                                szGetName,
                                (LPVOID *)&lpVersion,
                                (PUINT)&uVersionLen);



       if(bRetCode && uVersionLen && lpVersion)
       {
          lstrcpy(szResult, lpVersion);
          SetDlgItemText(i,szResult);    // Replace dialog item text with version info
          SendDlgItemMessage(i, WM_SETFONT, (UINT)hFontDlg, TRUE);

#ifdef NOTUSEDYET

        hFontCopyright = CreateFont(iPointSize,
                                   0, 0, 0,
                                   FW_BOLD,
                                   0, 0, 0, 0,
                                   0, 0, 0, 0,
                                   "Arial");

          SendDlgItemMessage(hdlg, 
                       IDD_VERLAST, 
                       WM_SETFONT, 
                       (WPARAM)hFontCopyright,
                       0L);


#endif
       }
    }

        GlobalUnlock(hMem);
        GlobalFree(hMem);
  }

	CenterWindow();
	return TRUE;
}    

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CGServerApp::OnAppAbout()
{

  CAboutDlg aboutDlg;

	aboutDlg.DoModal();

}

/////////////////////////////////////////////////////////////////////////////
// CGServerApp commands

BOOL CGServerApp::PreTranslateMessage(MSG* pMsg)
{
	return CWinApp::PreTranslateMessage(pMsg);
}

////////////////////////////////////////////////////////////////////////////
// MEMBER OVERWRITE:  ExitInstance ...
//
//  Basicaly clean up ...
//
int CGServerApp::ExitInstance() 
{


	
return CWinApp::ExitInstance();
}



void CGServerApp::OnHelpContents() 
{
	CWinApp::WinHelp(0,HELP_CONTENTS);	
}


