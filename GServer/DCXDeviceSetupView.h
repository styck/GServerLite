#if !defined(AFX_DCXDEVICESETUPVIEW_H__115BE9A9_7B57_11D1_A3B5_6663AF000000__INCLUDED_)
#define AFX_DCXDEVICESETUPVIEW_H__115BE9A9_7B57_11D1_A3B5_6663AF000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DCXDeviceSetupView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDCXDeviceSetupView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include  "CorTekTBCtrl.h"

class CGServerDoc;

class CDCXDeviceSetupView : public CFormView
{
protected:
	CDCXDeviceSetupView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDCXDeviceSetupView)

// Form Data

public:
	//{{AFX_DATA(CDCXDeviceSetupView)
	enum { IDD = IDD_NETWORK };
	//}}AFX_DATA

// Attributes
public:
  CCorTekTBCtrl   m_tbCtrlModule;
  DWORD           m_dwCurModuleSel;

// Operations
public:
  CGServerDoc* GetDocument(void);

  BOOL          OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCXDeviceSetupView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

  BOOL    OnEraseBkgnd( CDC* pDC );
// Implementation
protected:
	virtual ~CDCXDeviceSetupView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDCXDeviceSetupView)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG  // debug version in CopyNetSrvView.cpp
inline CGServerDoc* CDCXDeviceSetupView::GetDocument(void)
   { return (CGServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCXDEVICESETUPVIEW_H__115BE9A9_7B57_11D1_A3B5_6663AF000000__INCLUDED_)
