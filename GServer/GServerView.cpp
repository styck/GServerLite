//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// GServerView.cpp : implementation of the CGServerView class
//
//

#include "stdafx.h"
#include "GServer.h"
#include "Module.h"
#include "ControlDlg.h"	// Controls test dialog
#include "GServerDoc.h"
#include "GServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void WINAPI DDX_IPAddress(CDataExchange* pDX, int nIDC, UINT & value);

// Declare the server type positions in the combo box

enum {ST_CUSTOM, ST_EVENT, ST_SHOWTIME, ST_CABARET}; 

DWORD g_dwEvent60[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 40
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 60
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 8 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA
};


DWORD g_dwEvent40[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 40
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 8 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA, 
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA, 
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA
};



DWORD g_dwShowTime[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 20
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 24
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,
DCX_DEVMAP_MODULE_AUX,  // 6 aux modules
DCX_DEVMAP_MODULE_MASTER,
DCX_DEVMAP_MODULE_CUE,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX,
DCX_DEVMAP_MODULE_MATRIX, // 6
DCX_DEVMAP_MODULE_NA,		
DCX_DEVMAP_MODULE_NA,      // 2
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 20
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA    // 40
};

DWORD g_dwCabaret[DCX_DEVMAP_MAXSIZE] =
{
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,
DCX_DEVMAP_MODULE_INPUT,  // 12
DCX_DEVMAP_MODULE_AUX,    // 13
DCX_DEVMAP_MODULE_AUX,    // 14
DCX_DEVMAP_MODULE_MASTER, // 15
DCX_DEVMAP_MODULE_CUE,    // 16
DCX_DEVMAP_MODULE_MATRIX, // 17
DCX_DEVMAP_MODULE_MATRIX, // 18
DCX_DEVMAP_MODULE_NA,		  // 19
DCX_DEVMAP_MODULE_NA,     // 20
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 40
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,    // 60
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA,
DCX_DEVMAP_MODULE_NA    // 80
};


/////////////////////////////////////////////////////////////////////////////
// CGServerView

IMPLEMENT_DYNCREATE(CGServerView, CFormView)

BEGIN_MESSAGE_MAP(CGServerView, CFormView)
	//{{AFX_MSG_MAP(CGServerView)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHK_NET_SERVER, OnChkNetServer)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_SERVER_COMBO, OnSelchangeServerCombo)
	ON_BN_CLICKED(IDC_TESTCONTROLS, OnTestcontrols)
	ON_BN_CLICKED(IDC_CLEARERRORS, OnClearerrors)
	//}}AFX_MSG_MAP

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)

	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGServerView construction/destruction

CGServerView::CGServerView()
	: CFormView(CGServerView::IDD)
{
	//{{AFX_DATA_INIT(CGServerView)
	m_csTcpAddr = _T("");
	m_iPort = 0;
	m_csNumClients = _T("");
	m_csServerType = _T("");
	//}}AFX_DATA_INIT

  
  m_dwCurModuleSel = 0;   // Default to the first module
  m_iServerType = 0;      // Default to CUSTOM configuration
  m_iServerTypeCurrent = 0;

  m_bServerStart = FALSE;
}

CGServerView::~CGServerView()
{
}

BOOL CGServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGServerView drawing
//
// Update the background screen

void CGServerView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CGServerView printing

BOOL CGServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGServerView diagnostics

#ifdef _DEBUG
void CGServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGServerDoc* CGServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGServerDoc)));
	return (CGServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGServerView message handlers

void CGServerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CGServerDoc   *pDoc = GetDocument();


// Lets Update the data in the view
//---------------------------------

	if(pDoc)
	{
//		m_iPort       = pDoc->m_pdcxNetwork->m_iPort;         // Port number
//		m_csTcpAddr   = pDoc->m_pdcxNetwork->m_csIPAddress;   // IP Address

    // Show the number of clients currently connected

    m_csNumClients.Format(": %d",pDoc->m_pdcxNetwork->m_iConnInUse);

		UpdateData(FALSE);

    // Setup the client connections progress bar

    m_NumClientsProgress.SetRange(0,MAX_ASYNC_SONNECTIONS);   // Only eight clients allowed
    m_NumClientsProgress.SetStep(1);
    m_NumClientsProgress.SetPos(0);

    // indicate the number of client connections

    m_NumClientsProgress.SetPos(pDoc->m_pdcxNetwork->m_iConnInUse);

  }

////////////////////////////////////////////////////////////////////
//Update the Device Map View	
//--------------------------

  int           iIdx;
  int           iBitmap;

#define VU_BITMAP_OFFSET  6   // Offset for a bitmap to a bitmap with VU

	for(iIdx = 0; iIdx < DCX_DEVMAP_MAXSIZE; iIdx ++)
	{

    //--------------------------------------------------------------------
    // check the VU lock flag.  If it is not zero then we are sending
    // VU data for this module.  Set the bitmap offset to display module
    // bitmap indicating VU data is being sent.
    //--------------------------------------------------------------------

		if(pDoc->m_VUMetersArray.m_aVUReadData[iIdx].cLock)
			iBitmap = VU_BITMAP_OFFSET;
		else
			iBitmap = 0;    // No offset

		switch(pDoc->m_dcxdevMap.GetModuleType(iIdx))
		{

		  case  DCX_DEVMAP_MODULE_NA:
			iBitmap += 0;
			break;
		  case  DCX_DEVMAP_MODULE_INPUT:
			iBitmap += 1;
			break;
		  case  DCX_DEVMAP_MODULE_AUX:
			iBitmap += 2;
			break;
		  case  DCX_DEVMAP_MODULE_MATRIX:
			iBitmap += 3;
			break;
		  case  DCX_DEVMAP_MODULE_CUE:
			iBitmap += 4;
			break;
		  case  DCX_DEVMAP_MODULE_MASTER:
			iBitmap += 5;
			break;
	//      case  DCX_DEVMAP_MODULE_COMPUTER:
	//        iBitmap = 6;
	//        break;
			default:
				// handle an error here .....
				iBitmap = 0;
				break;
		}

		m_tbCtrlModule.SendMessage(TB_CHANGEBITMAP, iIdx + 1, iBitmap);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CGServerView message handlers
//
// This creates the device setup toobar so that we can select the different modules

BOOL CGServerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
BOOL      bRet;
RECT      r;
RECT      Rect;

CBitmap   cbmp;
BITMAP    bmpInfo;
int       iIdx;
TBBUTTON  tbb;
CString   csTipText;  
CGServerDoc*  pDoc;


//-------------------------------------------------------
// Create the Window ... first
// and then we can get the Document binded to this View
//-------------------------------------------------------

	bRet	=	CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	pDoc = GetDocument();

	if(bRet)
  {
  
  r.left = r.top = 0;
  r.right = r.bottom = 0;

  iIdx=m_tbCtrlModule.Create(WS_CHILD | WS_VISIBLE | CCS_NODIVIDER | 
                        CCS_NOPARENTALIGN | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE, 
                        r, this, nID);

  // Load the N/A Module to get bitmap size

	cbmp.LoadBitmap(IDB_MODULE_NA);  
	cbmp.GetBitmap(&bmpInfo);

	m_tbCtrlModule.SetButtonSize(CSize(bmpInfo.bmWidth, bmpInfo.bmHeight));
	m_tbCtrlModule.SetBitmapSize(CSize(bmpInfo.bmWidth, bmpInfo.bmHeight));


  // add all the bitmaps for all of the modules

	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_NA);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_INPUT);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_AUX);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_MATRIX);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_CUE);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_MASTER);

  // VU data being sent versions of above
  //
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_NA_VU);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_INPUT_VU);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_AUX_VU);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_MATRIX_VU);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_CUE_VU);
	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_MASTER_VU);


//	m_tbCtrlModule.AddBitmap(1, IDB_MODULE_COMPUTER);	// No computer module for now


  // Setup toolbar variables

  tbb.idCommand = 0;
  tbb.fsState = TBSTATE_ENABLED;
  tbb.fsStyle = TBSTYLE_BUTTON;
  tbb.iString = 0;
  tbb.iBitmap = 0;
  tbb.dwData  = 0;


  //-------------------------------------------------------
  // Build the device bitmap of the system
  //-------------------------------------------------------

  for(iIdx = 0; iIdx < DCX_DEVMAP_MAXSIZE; iIdx ++)
  {
	
    tbb.idCommand = tbb.dwData = iIdx + 1;
    csTipText.Format("#%02d", iIdx + 1);

		// Add second '\0', Very important or will not operate correctly

		int nStringLength = csTipText.GetLength() + 1;
		TCHAR* pString = csTipText.GetBufferSetLength(nStringLength);
		pString[nStringLength] = 0;

    tbb.iString = m_tbCtrlModule.AddStrings((LPSTR)LPCTSTR(csTipText));
		csTipText.ReleaseBuffer();
	
	// Find what bitmap to display

    switch(pDoc->m_dcxdevMap.GetModuleType(iIdx))
      {
      case  DCX_DEVMAP_MODULE_NA:
        tbb.iBitmap = 0;
        break;
      case  DCX_DEVMAP_MODULE_INPUT:
        tbb.iBitmap = 1;
        break;
      case  DCX_DEVMAP_MODULE_AUX:
        tbb.iBitmap = 2;
        break;
      case  DCX_DEVMAP_MODULE_MATRIX:
        tbb.iBitmap = 3;
        break;
      case  DCX_DEVMAP_MODULE_CUE:
        tbb.iBitmap = 4;
        break;
      case  DCX_DEVMAP_MODULE_MASTER:
        tbb.iBitmap = 5;
        break;
//      case  DCX_DEVMAP_MODULE_COMPUTER:
//        tbb.iBitmap = 6;
//        break;
      default:
        // handle an error here .....
        tbb.iBitmap = 0;
        break;
      }

    m_tbCtrlModule.AddButtons(1, &tbb);    
	}
 
  m_tbCtrlModule.SetRows(4, TRUE, &r);	// Limit to only 4 rows
  m_tbCtrlModule.AutoSize();						// Autosize the device setup

  // Position tool bar window and make it on top.

#define MODULE_SETUP_SCREEN_X		20
#define MODULE_SETUP_SCREEN_Y		260			//180


    m_tbCtrlModule.SetWindowPos(&wndTop, 
			MODULE_SETUP_SCREEN_X, MODULE_SETUP_SCREEN_Y, 
			0, 0, 
			SWP_NOSIZE | SWP_SHOWWINDOW);

	m_tbCtrlModule.GetWindowRect(&r);

  }

return bRet;
	
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
//
// Displays ToolTip text when mouse cursor is over a module
//


BOOL CGServerView::OnToolTipText(UINT wParam, NMHDR* pNMHDR, LRESULT* pResult)
{
//ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

// need to handle both ANSI and UNICODE versions of the message
// but for now lets deal with the ANSI code only
//--------------------------------------------------------------
TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
CString       csTipText;
UINT          nID = pNMHDR->idFrom;
BOOL          bRet = TRUE;
CToolTipCtrl* pttctrl;
CModule				cm_Module;		// Converts module number to DCX address

	if( (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND)) ||
			(pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND)) || 
			(pNMHDR->code == TTN_NEEDTEXT && (pTTTA->uFlags & TTF_IDISHWND))  )
  {
		// idFrom is actually the HWND of the tool
		//----------------------------------------
		nID = ::GetDlgCtrlID((HWND)nID);
  }
                
	pttctrl = m_tbCtrlModule.GetToolTips();

	if(pttctrl == NULL)
		return FALSE;

	ASSERT(pttctrl->m_hWnd != (HWND)pNMHDR->hwndFrom || pNMHDR->code == TTN_NEEDTEXT );


  // nID is the module number (1-80)

	if (nID != 0 && pttctrl->m_hWnd == (HWND)pNMHDR->hwndFrom) // will be zero on a separator
	{

		// This is the right way to do it .... but again for now .....
		//------------------------------------------------------------
		//csTipext.LoadString(nID);
		// this is the command id, not the button index
		//---------------------------------------------
		//AfxExtractSubString(strTipText, szFullText, 1, '\n');
		/*

		#ifndef _UNICODE
			if (pNMHDR->code == TTN_NEEDTEXTA)
				lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
			else
				_mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
		#else
			if (pNMHDR->code == TTN_NEEDTEXTA)
				_wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
			else
				lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
		#endif
		*/                       

		//  if(nID == DCX_DEVMAP_COMPUTER_MODULE)
		//    csTipText.Format("Computer Module");
		//  else    


		// The card-cage addresses jump from one cage to another
		// nID is the module number (1-80)
		//
		// To get the address of the module we need to send it the module 
		// number base 0

		csTipText.Format("Module #%02d, Address #%002d", nID, cm_Module.GetModuleAddress(nID-1) );
		lstrcpyn(pTTTA->szText, (LPSTR)LPCTSTR(csTipText), csTipText.GetLength() + 1);

		// bring the tooltip window above other popup windows
		//---------------------------------------------------

		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

	}
	else
		bRet = FALSE;


return bRet;
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnCommand(WPARAM wParam, LPARAM lParam) 
//
//  User clicked on the Toolbar module. Show user what module is selected
//  and allow them to change it.
//
//

BOOL CGServerView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
CGServerDoc*  pDoc = GetDocument();

	if(wParam > 0 && wParam <= DCX_DEVMAP_MAXSIZE)
	{
		CMenu   menu;      
		CMenu   *pmenuPop;   
		POINT   pntCPos;

    if(m_iServerType == ST_CUSTOM)  // only allow editing for CUSTOM server type
    {
      // Someone clicked on one of the modules, find out which one.

		  m_dwCurModuleSel = wParam - 1;	// From base 1 to base 0

		  GetCursorPos(&pntCPos);

			  if(menu.LoadMenu(IDR_MENU_MODULESELECTION))
			  {
					  pmenuPop = menu.GetSubMenu(0);

					  switch(pDoc->m_dcxdevMap.GetModuleType(m_dwCurModuleSel))
					  {
						  case  DCX_DEVMAP_MODULE_NA:
							  pmenuPop->CheckMenuItem(ID_NATYPE, MF_CHECKED);
							  break;
						  case  DCX_DEVMAP_MODULE_INPUT:
							  pmenuPop->CheckMenuItem(ID_INPUTMODULE, MF_CHECKED);
							  break;
						  case  DCX_DEVMAP_MODULE_AUX:
							  pmenuPop->CheckMenuItem(ID_AUXMODULE, MF_CHECKED);
							  break;
						  case  DCX_DEVMAP_MODULE_MATRIX:
							  pmenuPop->CheckMenuItem(ID_MATRIXMODULE, MF_CHECKED);
							  break;
						  case  DCX_DEVMAP_MODULE_CUE:
							  pmenuPop->CheckMenuItem(ID_CUEMODULE, MF_CHECKED);
							  break;
						  case  DCX_DEVMAP_MODULE_MASTER:
							  pmenuPop->CheckMenuItem(ID_MASTERMODULE, MF_CHECKED);
							  break;
						  default:
							  break;
					  }

				  pmenuPop->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
														  pntCPos.x, pntCPos.y, 
														  this);

				  menu.DestroyMenu();
			  }
    } // end if ST_CUSTOM
	}
	else
	{
		BOOL      bCtrlFound = TRUE;
		int       iBitmap;
		int       iType;

	// Select bitmap type depending upon module selection

			switch(wParam)
			{
				case ID_NATYPE:
					iBitmap = 0;
					iType = DCX_DEVMAP_MODULE_NA;
					break;

				case ID_INPUTMODULE:
					iBitmap = 1;
					iType = DCX_DEVMAP_MODULE_INPUT;
					break;

				case ID_AUXMODULE:
					iBitmap = 2;
					iType = DCX_DEVMAP_MODULE_AUX;
					break;

				case ID_MATRIXMODULE:
								iBitmap = 3;
								iType = DCX_DEVMAP_MODULE_MATRIX;
					break;

				case ID_CUEMODULE:


						// Check if there is already a matrix
						// If there is then set the ctrlfound to false
						// so that the type does not get updated
						// and display an informative message

						for(iType =0; iType < DCX_DEVMAP_MAXSIZE; iType++)
						{
							if(pDoc->m_dcxdevMap.GetModuleType(iType) == DCX_DEVMAP_MODULE_CUE)
							{
									bCtrlFound = FALSE;
									MessageBox("Cue module already installed","Device Setup Error",MB_ICONEXCLAMATION);

									break;	// break out of for loop if we already found one.
							}	
						}

						// If still true then set it up

						if(bCtrlFound)
						{
							iBitmap = 4;
							iType = DCX_DEVMAP_MODULE_CUE;
						}

					break;

				case ID_MASTERMODULE:

						// Check if there is already a master
						// If there is then set the ctrlfound to false
						// so that the type does not get updated
						// and display an informative message

						for(iType =0; iType < DCX_DEVMAP_MAXSIZE; iType++)
						{
							if(pDoc->m_dcxdevMap.GetModuleType(iType) == DCX_DEVMAP_MODULE_MASTER)
							{
									MessageBox("Master module already installed","Device Setup Error",MB_ICONEXCLAMATION);
									bCtrlFound = FALSE;
									break;	// break out of for loop if we already found one.
							}	
						}

						// If still true then set it up

						if(bCtrlFound)
						{
							iBitmap = 5;
							iType = DCX_DEVMAP_MODULE_MASTER;
						}

					break;

				default:
					bCtrlFound = FALSE;
					break;
			}

		// Update the Controls ....
		// Delete the old button and Add the new one with the new Image
		//-------------------------------------------------------------

		if(bCtrlFound)
		{
			pDoc->m_dcxdevMap.SetModuleType(m_dwCurModuleSel, iType);
			m_tbCtrlModule.SendMessage(TB_CHANGEBITMAP, m_dwCurModuleSel + 1, iBitmap);
		}

	}
	
	return CFormView::OnCommand(wParam, lParam);
}

//////////////////////////////////////////
// Update the combo box showing the 
// current server type.

void CGServerView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
  m_pCB = (CComboBox*) GetDlgItem(IDC_SERVER_COMBO);

  m_pCB->SetCurSel((int)m_iServerType);
	
	UpdateData(TRUE);	
	
}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: void CGServerView::OnPaint() 
//
//  Update the background of the main window
//

void CGServerView::OnPaint() 
{
CDC       dcMem ;
CBitmap   cBmp;
BITMAP    bmpInfo;
int       iX, iY;
RECT      rClient;


  if(IsIconic())  // If its an icon
  {

  }
  else
  {
    CPaintDC  dc(this); // device context for painting

	  GetClientRect(&rClient);
	  cBmp.LoadBitmap(IDB_BITMAP1);
	  cBmp.GetBitmap(&bmpInfo);

		  CGServerDoc* pDoc = GetDocument();

	  if(dcMem.CreateCompatibleDC(&dc))
    {
		  dcMem.SelectObject(cBmp);


		  for(iX = 0; iX < rClient.right; iX += bmpInfo.bmWidth)
      {
			  for(iY = 0; iY < rClient.bottom; iY += bmpInfo.bmHeight)
			  {
				  dc.BitBlt(iX, iY, bmpInfo.bmWidth, bmpInfo.bmWidth, 
								   &dcMem, 0, 0,  SRCCOPY);
			  }
      }

      // Draw the horizontal bar accross the top of the client 

      dc.FillSolidRect(0,0,rClient.right,rClient.top+30,RGB(173,0,33));

	  }

  }

}


/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnChkNetServer() 
//
//  User clicked on the SERVER button
//  If SERVER button pressed in then start server and VU thread and
//  create the events so that we can shutdown the thread
//
//	If SERVER button is being turned off then shutdown the network
//  and the VU thread
//
void CGServerView::OnChkNetServer() 
{
	CGServerDoc   *pDoc = GetDocument();


	if(pDoc)	
	{
		UpdateData(TRUE);		// Get status of server button


///// A GAMBLE HACK - PRESS BUTTON COMMANDS, UNPRESS, AND THEN REPRESS

		if(m_bServerStart)	// If it is down the start the server
		{
			if(pDoc->StartServer(LPCTSTR(m_csTcpAddr), m_iPort))
			{
				MessageBeep(MB_OK);
				pDoc->m_VUthread = NULL;											// The VU thread hasn't started yet
				pDoc->m_hEventKillVUThread = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
				pDoc->m_hEventVUThreadKilled = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
				pDoc->StartVUthread();	// Start the thread to read the VU meters and update the member display variables

//////////////////////////////////////
// HACK BUTTON UP COMMAND

			// KILL Thread
			// Signel the VU thread to exit

			SetEvent(pDoc->m_hEventKillVUThread);		

			// Wait until thread kills itself
			// Cannot wait INFINITE since thread may have not been started
			// so wait 900 ms

			WaitForSingleObject(pDoc->m_hEventVUThreadKilled,900);  // CRASHES HERE IF SERVER STOPPED AND VUS ARE GOINS

      // Shutdown netork after killing the VU thread

			pDoc->ShutDownNetwork();

      // Close these handles explicitly. These may be closed automatically
      // by the system but we do it here to make sure.

			  CloseHandle(pDoc->m_hEventKillVUThread);
			  CloseHandle(pDoc->m_hEventVUThreadKilled);

			  pDoc->m_hEventKillVUThread = NULL;
			  pDoc->m_hEventVUThreadKilled = NULL;


      // HACK BUTTON DOWN AGAIN

				pDoc->StartServer(LPCTSTR(m_csTcpAddr), m_iPort);
				MessageBeep(MB_OK);
				pDoc->m_VUthread = NULL;											// The VU thread hasn't started yet
				pDoc->m_hEventKillVUThread = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
				pDoc->m_hEventVUThreadKilled = CreateEvent(NULL,FALSE, FALSE, NULL); // auto reset, initially reset
				pDoc->StartVUthread();	// Start the thread to read the VU meters and update the member display variables


			}
			else
			{
					m_bServerStart = FALSE;
					UpdateData(FALSE);
			}
		}
		else
		{

			// KILL Thread
			// Signel the VU thread to exit

			SetEvent(pDoc->m_hEventKillVUThread);		

			// Wait until thread kills itself
			// Cannot wait INFINITE since thread may have not been started
			// so wait 900 ms

			WaitForSingleObject(pDoc->m_hEventVUThreadKilled,900);  // CRASHES HERE IF SERVER STOPPED AND VUS ARE GOINS

      // Shutdown netork after killing the VU thread

			pDoc->ShutDownNetwork();

      // Close these handles explicitly. These may be closed automatically
      // by the system but we do it here to make sure.

			  CloseHandle(pDoc->m_hEventKillVUThread);
			  CloseHandle(pDoc->m_hEventVUThreadKilled);

			  pDoc->m_hEventKillVUThread = NULL;
			  pDoc->m_hEventVUThreadKilled = NULL;

		}
	}
}



BOOL CGServerView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
return TRUE;
	
// 	return CFormView::OnEraseBkgnd(pDC);
}



void CGServerView::DoDataExchange(CDataExchange* pDX) 
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGServerView)
	DDX_Control(pDX, IDC_STATUS_LIST, m_clbStatusList);
	DDX_Control(pDX, IDC_NUM_CONNECTIONS, m_NumClientsProgress);
	DDX_Text(pDX, IDC_TCP_ADDRESS, m_csTcpAddr);
  DDX_Text(pDX, IDC_TCP_PORT, m_iPort);
	DDX_Check(pDX, IDC_CHK_NET_SERVER, m_bServerStart);
	DDX_Text(pDX, IDC_NUM_CLIENTS_TEXT, m_csNumClients);
	DDX_CBString(pDX, IDC_SERVER_COMBO, m_csServerType);
	//}}AFX_DATA_MAP
}

void CGServerView::Serialize(CArchive& ar) 
{

  if(!m_bServerStart)
  {
	  if (ar.IsStoring())
	  {	// storing code

      UpdateData(TRUE);
		  ar << m_iPort;       
		  ar << m_csTcpAddr;                
  //		ar << m_bServerStart;
      ar << m_iServerType;

	  }
	  else
	  {	// loading code

		  ar >> m_iPort;       
		  ar >> m_csTcpAddr;                
  //		ar >> m_bServerStart;
      ar >> m_iServerType;
      UpdateData(FALSE);

      m_iServerTypeCurrent = m_iServerType; // set the current type

	  }
  }
  else
  {
    MessageBox( "Stop the SERVER before changing configurations", "ERROR", MB_OK );  
  }
}

// DDX routine fo IP address translation
// CURRENTLY NOT USED

void WINAPI DDX_IPAddress(CDataExchange* pDX, int nIDC, UINT & value)
{
	// from dialog to class ?
	if( pDX->m_bSaveAndValidate)
	{
		CString Val;
		BOOL bValid = true;

		pDX->m_pDlgWnd->GetDlgItem(nIDC)->GetWindowText(Val);

		for( int i = 0; i < Val.GetLength(); i++)
		{
			// let's check if all entered char in entered
			// IP address are digits
			if(Val[i] == '.')
				continue;

			if(isdigit(Val[i]) == 0)
			{
				bValid = false;
				break;			
			}
		}

		if(bValid)
		{
			value = inet_addr(Val);
			if(value == INADDR_NONE)
			{
				pDX->m_pDlgWnd->MessageBox("The entered IP address is invalid.");
				pDX->PrepareEditCtrl(nIDC);
				pDX->Fail();
			}
		}
		else
		{
			pDX->m_pDlgWnd->MessageBox("IP address can only have digits and dots.");
			pDX->PrepareEditCtrl(nIDC);
			pDX->Fail();
		}
	}
	else
	{
		// if the value is a valid IP address store it in the child control
		in_addr IPaddress;
		memcpy(&IPaddress, &value, 4);
		CString Address = inet_ntoa(IPaddress);
		if(!Address.IsEmpty())
		{
			pDX->m_pDlgWnd->GetDlgItem(nIDC)->SetWindowText(Address);
		}
	}	
}

///////////////////////////////////////////////////
// Someone changed the server type so lets update
// the device map to reflect the changes.
// Only Custom configurations are allowed to 
// change the module layout.

void CGServerView::OnSelchangeServerCombo() 
{
  int iCount;

    CComboBox * m_pCB = (CComboBox*) GetDlgItem(IDC_SERVER_COMBO);
    m_iServerType=m_pCB->GetCurSel();
    m_pCB->GetLBText(m_iServerType,m_csServerType);

	  CGServerDoc   *pDoc = GetDocument();

  if(!m_bServerStart)
  {
	  if(pDoc)	
    {
      m_iServerTypeCurrent = m_iServerType; // set the current type

      switch(m_iServerType)
      {
        case 0:  // custom
          break;
        case 1:  // event60
          // setup default values for an EVENT
          //--------------------------
          for(iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount ++)
            pDoc->m_dcxdevMap.SetModuleType(iCount, g_dwEvent60[iCount]);
        break;

        case 2:  // event40
          // setup default values for an EVENT
          //--------------------------
          for(iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount ++)
            pDoc->m_dcxdevMap.SetModuleType(iCount, g_dwEvent40[iCount]);
        break;

        case 3:  // showtime
          // setup default values for an SHOWTIME
          //--------------------------
          for(iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount ++)
            pDoc->m_dcxdevMap.SetModuleType(iCount, g_dwShowTime[iCount]);
        break;

        case 4: // cabaret
          // setup default values for an CABARET
          //--------------------------
          for(iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount ++)
            pDoc->m_dcxdevMap.SetModuleType(iCount, g_dwCabaret[iCount]);
        break;
      }
      pDoc->UpdateAllViews(NULL);

    }
  }
  else
  {
    m_iServerType=m_pCB->SetCurSel(m_iServerTypeCurrent);
    MessageBox( "Stop the SERVER before changing configurations", "ERROR", MB_OK );  
  }

}



void CGServerView::OnTestcontrols() 
{
	// TODO: Add your control notification handler code here

  CControlDlg TestControlsDlg;
  CGServerDoc   *pDoc = GetDocument();

	TestControlsDlg.SetPointerToDoc(pDoc);
	TestControlsDlg.DoModal();
	
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnClearerrors() 
//          
// Descr. : Clear out the error list box.
//          
// Return : void
//-----------------------------------------------------------------------------

void CGServerView::OnClearerrors() 
{
	m_clbStatusList.ResetContent();	
}
