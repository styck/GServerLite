//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// GServerDoc.cpp : implementation of the CGServerDoc class
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GServer.h"

#include "GServerDoc.h"
#include "vuthread2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "DCXServerMonitorView.h"

/////////////////////////////////////////////////////////////////////////////
// CGServerDoc

IMPLEMENT_DYNCREATE(CGServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGServerDoc, CDocument)
	//{{AFX_MSG_MAP(CGServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGServerDoc construction/destruction

CGServerDoc::CGServerDoc()
{
	m_csGServerDocID = GSERVER_ID_STRING;  
	m_dwVersion = GSERVER_VERSION;

//	m_pServerMonitorView = NULL;

	m_pdcxNetwork = new CGDCXNetwork(this);	// Create a pointer to the network class
	m_pDCXDevice = new   CDCXDevice(this);	// Create a pointer to the device class
	m_pDCXDevice->Open();										// Open the DCX device

	m_VUthread = NULL;											// The VU thread hasn't started yet

#ifdef NOTUSED // this is done when the server button is pressed
	m_hEventKillVUThread = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
	m_hEventVUThreadKilled = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
#endif
	
}

CGServerDoc::~CGServerDoc()
{

// Signel the VU thread to exit

	SetEvent(m_hEventKillVUThread);		

// Wait until thread kills itself
// Cannot wait INFINITE since thread may have not been started
// so wait 500 ms

	WaitForSingleObject(m_hEventVUThreadKilled,500);

// Delete the object pointers

	delete  m_pdcxNetwork;
	delete  m_pDCXDevice;		// Thread must be stopped before deleting the device
													// since the thread is accessing the device
}

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnNewDocument()
//
//
//
BOOL CGServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

  // Set the default mapping for the Device
  //---------------------------------------
  m_dcxdevMap.SetDefaults();
  UpdateAllViews(NULL);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CGServerDoc serialization

void CGServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Store this Document Settings
		ar <<  m_csGServerDocID;  
		ar <<  m_dwVersion;
  
		// keep this order consistent with 
		// the load order ... its like PUSH/POP thingy
		//--------------------------------------------
		m_dcxBinTable.Serialize(ar);

//		m_GServerView.Serialize(ar);		// TEST TEST TEST NEED TO SERIALIZE THE GSERVERVIEW TO SAVE SETTINGS
		m_pdcxNetwork->Serialize(ar);
		m_dcxdevMap.Serialize(ar);
	}
	else
	{
		// Load This Document Settings
		ar >>  m_csGServerDocID;  
		ar >>  m_dwVersion;

		m_dcxBinTable.Serialize(ar);
		m_pdcxNetwork->Serialize(ar);
		m_dcxdevMap.Serialize(ar);

		// Init this Object
		//-----------------
		InitGServerDoc();
		UpdateAllViews(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGServerDoc diagnostics

#ifdef _DEBUG
void CGServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}               
#endif //_DEBUG

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: GetServerMonitorView(void)
//
//
#ifdef NOTUSED
void CGServerDoc::SetServerMonitorView(CDCXServerMonitorView  *psrvmonview)
{
//	m_pServerMonitorView = (CDCXServerMonitorView *)psrvmonview;
}
#endif


////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: InitGServerDoc(void)
//
//
BOOL    CGServerDoc::InitGServerDoc(void)
{
BOOL    bRet;

char szBinDirectory[_MAX_PATH];		// 128

//
// Load in the DCX.BIN file
//
// Need to make this look in the current directory where the program is ran
// for the dcx.bin file.

	GetCurrentDirectory(sizeof(szBinDirectory),szBinDirectory);
	strcat(szBinDirectory,"\\dcx.bin");
//	if(m_dcxBinTable.ReadDCXTableFile( (LPSTR)LPCSTR(m_dcxBinTable.GetBinTableFileName())) == 0)
	if(m_dcxBinTable.ReadDCXTableFile(szBinDirectory)	)
 		bRet = TRUE;
	else
		bRet = FALSE;

return bRet;
}

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: StartServer(UINT iPort)
//
//
BOOL    CGServerDoc::StartServer(LPCTSTR lpcs, UINT iPort)
{
BOOL    bRet = TRUE;

	if(m_pdcxNetwork)
  {
		bRet = m_pdcxNetwork->StartAsServer(lpcs, iPort);
//		if(bRet)
//			m_pServerMonitorView->StartMessagePump();
  }
	else
		bRet = FALSE;

	if(bRet)
		DisplayGeneralMessage(IDS_SERVER_STARTED);
	else
		DisplayGeneralMessage(IDS_SERVER_FAILEDTOSTART);

	return bRet;
};

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: ShutDownNetwork(void)
//
//
BOOL    CGServerDoc::ShutDownNetwork(void)
{
BOOL    bRet = TRUE;

	if(m_pdcxNetwork)
  {
//	  m_pServerMonitorView->StopMessagePump();
	  bRet = m_pdcxNetwork->ShutDown();
	  DisplayGeneralMessage(IDS_SERVER_SHUTDOWN);
  }
	else
		bRet = FALSE;

return bRet;
};

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: HandleServerMessage(CString &cs)
//
//
BOOL    CGServerDoc::HandleServerMessage(CString &cs)
{
BOOL    bRet = TRUE;

return bRet;
};

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: HandleClientMessage(CString &cs)
//
//
BOOL    CGServerDoc::HandleClientMessage(CString &cs)
{
BOOL    bRet = TRUE;

return bRet;
};


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: DisplayTime(MMTIME  mmt)
//
//
//
void  CGServerDoc::DisplayTime(MMTIME  mmt)
{
char    szBuffer[32];

	wsprintf(szBuffer, "%10d", mmt.u.ms);
//	m_pServerMonitorView->m_TimeDisplay.SetWindowText(szBuffer);

return;
}

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: DisplayGeneralMessage(LPSTR lps)
//
//
BOOL    CGServerDoc::DisplayGeneralMessage(UINT   uiMsgID)
{
BOOL      bRet = TRUE;
CString   cs;

	if(cs.LoadString(uiMsgID))
    OutputDebugString((LPSTR) LPCTSTR(cs));

return bRet;
};

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: DisplayGeneralMessage(LPSTR lps)
//
//
BOOL    CGServerDoc::DisplayGeneralMessage(LPSTR lps)
{
BOOL      bRet = TRUE;

    OutputDebugString((LPSTR) LPCTSTR(lps));
//	DisplayGeneralMsg(lps);

return bRet;
};


////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: DisplayNetErrorMessage(UINT   uiMsgID)
//
//
BOOL    CGServerDoc::DisplayNetErrorMessage(UINT   uiMsgID)
{
BOOL      bRet = TRUE;
CString   cs;


if(cs.LoadString(uiMsgID))
  DisplayNetErrorMessage((LPSTR) LPCTSTR(cs));

return bRet;
};

////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: DisplayNetErrorMessage(LPSTR lps)
//
//
BOOL    CGServerDoc::DisplayNetErrorMessage(LPSTR lps)
{
BOOL      bRet = TRUE;
CString   cs;

	cs.LoadString(IDS_NET_ERROR_MSG);
	cs = cs + " " + lps;
    OutputDebugString((LPSTR) LPCTSTR(cs));
//	m_pServerMonitorView->DisplayNetErrorMsg((LPSTR) LPCTSTR(cs));

return bRet;
};


////////////////////////////////////////////////////////////////////////////
// MEMBER FUNCTION: HandleNetConnectionClose(void)
//
//
void    CGServerDoc::HandleNetConnectionClose(void)
{
POSITION    pos;
CView       *pView;

pos = GetFirstViewPosition();

	while(pos != NULL)
  {
		pView = GetNextView(pos);
  }                              
}


/////////////////////////////////////////////////////////////////////////////
// CGServerDoc commands



void CGServerDoc::StartVUthread()
{
	if (m_VUthread == NULL)
	{
		// Begin the worker thread.  

		m_VUthread = AfxBeginThread(ThreadProc, this, THREAD_PRIORITY_BELOW_NORMAL);
	} 
}

