// GServerView.h : interface of the CGServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GSERVERVIEW_H__266E64C1_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
#define AFX_GSERVERVIEW_H__266E64C1_6482_11D1_A3B1_C4D3CD000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include  "CorTekTBCtrl.h"

class CGServerView : public CFormView
{
protected: // create from serialization only
	CGServerView();
	DECLARE_DYNCREATE(CGServerView)

// Form Data
public:
	//{{AFX_DATA(CGServerView)
	enum { IDD = IDD_DEVICE_SETUP };
	CProgressCtrl	m_NumClientsProgress;
	CString	m_csTcpAddr;
	UINT	m_iPort;
	BOOL	m_bServerStart;
	CString	m_csNumClients;
	CString	m_csServerType;
	//}}AFX_DATA


// Attributes
public:
	CGServerDoc* GetDocument();

	// Module setup variables

  CCorTekTBCtrl   m_tbCtrlModule;
  CComboBox *     m_pCB;

  DWORD m_dwCurModuleSel;
  UINT  m_iServerType;      // Index into server type combo box 

// Operations
public:

  BOOL          OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGServerView)
	afx_msg void OnPaint();
	afx_msg void OnChkNetServer();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelchangeServerCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GServerView.cpp
inline CGServerDoc* CGServerView::GetDocument()
   { return (CGServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GSERVERVIEW_H__266E64C1_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
