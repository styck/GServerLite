//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// MainFrm.cpp : implementation of the CMainFrame class
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"

#include "MainFrm.h"

#include "GServerDoc.h"
#include "DCXDeviceSetupView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
//	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder) 
//	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	
}

CMainFrame::~CMainFrame()
{
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnCreate(LPCREATESTRUCT lpCreateStruct)
//
//
//
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
CString   cs;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Load the standard windows toolbar

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// Create the status bar

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
  //----------------------------------------------------------------------

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
  //----------------------------------------------------------------

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
  
  if(cs.LoadString(IDS_TBNAME_GENERAL))
    m_wndToolBar.SetWindowText(cs);

	DockControlBar(&m_wndToolBar);

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


////////////////////////////////////////////////////////////////////////////////////////
// This function is called from the menubar when the help information is needed.
// It launches the Microsoft HTML Help executable -- hh.exe
// The VACS HTML help file -- VACWS1.chm is the file opened by the executable.
// In debug, the help files MUST be in the source code directory.
// In release, the help files MUST be in the same directory as the executable.
// Added 1/4/99,........ELC
void CMainFrame::OnLaunchHTMLHelp()
{
	STARTUPINFO p_startupInfo;										// Struct needed for startup information
	PROCESS_INFORMATION p_processInformation;						// Struct for information on process execution
	char chCurrentDirectory[128];									// The current dirctory of the GServer executable
	char chWindowsDirectory[128];									// The windows directory,..usually c:\winnt
	char chAppPathAndFileName[100];									// The help file name with its leading path
	char chCommandLine[50];											// The command line for hh.exe to run
	char chFullCommandLine[150];									// The full command line includeing application
																	// and help file
	int nResult;													
	char chAppFileName[100] = "\\hh.exe";							// Microsoft HTML Help file executable
	
	GetStartupInfo(&p_startupInfo);									// Get the startup info needed for creating a process
	DWORD dwResult = GetCurrentDirectory(128,chCurrentDirectory);	// Get the current process directory of GServer exe
	UINT  uiResult = GetWindowsDirectory(chWindowsDirectory,128);	// Get the windows directory

	nResult = sprintf(chAppPathAndFileName,"%s%s",chWindowsDirectory,chAppFileName);// Appand the path to the HTML file exe
	LPCTSTR pstrAppPathAndFileName = (LPCTSTR)chAppPathAndFileName;	// Cast to pointer

	nResult = sprintf(chCommandLine,"%s%s",chCurrentDirectory,"\\VACWS1.chm");	// Append current directory to help file
	nResult = sprintf(chFullCommandLine,"%s %s",chAppPathAndFileName,chCommandLine); // Merge into one big command line
	LPTSTR pstrFullCommandLine = (LPTSTR)chFullCommandLine;			// cast to a pointer


	// Create the process
	int nSuccess = CreateProcess(
		pstrAppPathAndFileName,					// pointer to name of executable module
		pstrFullCommandLine,					// pointer to command line string
		NULL,									// pointer to process security attributes
		NULL,									// pointer to thread security attributes
		FALSE,									// handle inheritance flag
		NORMAL_PRIORITY_CLASS,					// creation flag
		NULL,									// pointer to new environment block
		NULL,									// pointer to current directory name
		&p_startupInfo,							// pointer to STARTUPINFO struct
		&p_processInformation);					// pointer to PROCESS_INFORMATION struct


	if(!nSuccess)			// If the process creation was not successful, post a message box iunforming the end user
	{						// that the hh.exe file is in the wrong directory.
		CString strError;
		strError.Format("The file 'hh.exe' is not in the %s directory.\n",chWindowsDirectory);
		strError += "Do a search on your computer for 'hh.exe' and copy the file into the Windows directory.";
		MessageBox(strError,"HTML Help File location error",MB_OK|MB_ICONINFORMATION); 
	}
}
