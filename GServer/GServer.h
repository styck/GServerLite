// GServer.h : main header file for the GSERVER application
//

#if !defined(AFX_GSERVER_H__266E64B9_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
#define AFX_GSERVER_H__266E64B9_6482_11D1_A3B1_C4D3CD000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


#define DCX_START_ADDRESS	33	// Hardware address of first module

/////////////////////////////////////////////////////////////////////////////
// CGServerApp:
// See GServer.cpp for the implementation of this class
//

class CGServerApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CGServerApp();

  WSADATA   m_wsadata;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGServerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GSERVER_H__266E64B9_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
