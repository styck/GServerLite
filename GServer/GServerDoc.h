//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// GServerDoc.h : interface of the CGServerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GSERVERDOC_H__266E64BF_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
#define AFX_GSERVERDOC_H__266E64BF_6482_11D1_A3B1_C4D3CD000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include  "DCXBinTable.h"		// Need to include these to define the object pointers below
#include  "GDCXNetwork.h"
#include  "DCXDevice.h"
#include  "DCXDeviceMap.h"
#include	"VUMeterArray.h"
#include  "VUthread2.h"

// class   CDCXServerMonitorView;

class CGServerDoc : public CDocument
{

#define     GSERVER_ID_STRING  "GSERV"
#define     GSERVER_VERSION    0x00010001

protected: // create from serialization only
	CGServerDoc();
	DECLARE_DYNCREATE(CGServerDoc)

private:
  CString         m_csGServerDocID;  
  DWORD           m_dwVersion;

//  CDCXServerMonitorView *m_pServerMonitorView;

// Attributes
public:
  CDCXDeviceMap   m_dcxdevMap;				// Create member objects
  CDCXBinTable    m_dcxBinTable;

  CGDCXNetwork    *m_pdcxNetwork;
  CDCXDevice      *m_pDCXDevice;
	CWinThread*			m_VUthread;
	CVUMeterArray		m_VUMetersArray;	// Holds all VU data that needs to be read 

	char m_SocketVULocks[MAX_ASYNC_SONNECTIONS][MAX_VU_READ_DATA];	// Keep tracks of locks for each connection

	HANDLE	m_hEventKillVUThread;				// Handle for the event to kill the VU thread
	HANDLE	m_hEventVUThreadKilled;			// Handle for event that thread has died


  BOOL LoadBinTable(CString  &csFName)
  {
    if(m_dcxBinTable.ReadDCXTableFile( (LPSTR)LPCSTR(csFName)) == 0)
    {
      return TRUE;
    }
    else
      return FALSE;
  }

// Operations
public:
  // Views and other windows handling functions
  //--------------------------------------------
//  void          SetServerMonitorView(CDCXServerMonitorView  *psrvmonview);

  BOOL          InitGServerDoc(void);
  
  BOOL          StartServer(LPCTSTR lpcs, UINT iPort);

  BOOL          ShutDownNetwork(void);
  
  void          HandleNetConnectionClose(void);
  BOOL          HandleServerMessage(CString &cs);
  BOOL          HandleClientMessage(CString &cs);
  BOOL          DisplayGeneralMessage(LPSTR lps);
  BOOL          DisplayGeneralMessage(UINT uiMsgID);
  BOOL          DisplayNetErrorMessage(LPSTR lps);
  BOOL          DisplayNetErrorMessage(UINT uiMsgID);
  void          DisplayTime(MMTIME  mmt);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	LARGE_INTEGER m_liPeriod;
	double m_dCpuSpeed;
	DWORD m_dwCtrldelay;
	DWORD m_dwVudelay;
	DWORD m_dwBasedelay;
	void StartVUthread();
	virtual ~CGServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGServerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GSERVERDOC_H__266E64BF_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
