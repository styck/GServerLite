#if !defined(AFX_DCXLOADREG0DIALOG_H__77E184B7_A48C_11D1_A022_444553540000__INCLUDED_)
#define AFX_DCXLOADREG0DIALOG_H__77E184B7_A48C_11D1_A022_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DCXLoadReg0Dialog.h : header file
//

#include "resource.h"       // main symbols
#include "GServerDoc.h"			// Added by ClassView
#include "gserver.h"				// Needed for the DCX_ defines

/////////////////////////////////////////////////////////////////////////////
// DCXLoadReg0Dialog dialog

class DCXLoadReg0Dialog : public CDialog
{
// Construction
public:
	CGServerDoc* m_pDoc;
	CStringArray dcxreg0strings; // Declare a CString Array
	void ParseReg0Line(CString* strRegline, int *ichip, int * iwiper);
	DCXLoadReg0Dialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DCXLoadReg0Dialog)
	enum { IDD = IDD_REG0DIALOG };
	UINT	m_nAddress;
	CString	m_strFile;
	CString	m_strList1;
	CString	m_strList2;
	BOOL	m_bAllModulesCheck;
	//}}AFX_DATA

	UINT m_iReg0FileType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DCXLoadReg0Dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DCXLoadReg0Dialog)
	afx_msg void OnBrowse();
	afx_msg void OnClose();
	afx_msg void OnDownload();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void WriteRegZero();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCXLOADREG0DIALOG_H__77E184B7_A48C_11D1_A022_444553540000__INCLUDED_)
