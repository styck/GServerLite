#if !defined(AFX_CORTEKTBCTRL_H__68075F63_7DE6_11D1_A3B5_6663AF000000__INCLUDED_)
#define AFX_CORTEKTBCTRL_H__68075F63_7DE6_11D1_A3B5_6663AF000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CorTekTBCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCorTekTBCtrl window

class CCorTekTBCtrl : public CToolBarCtrl
{
// Construction
public:
	CCorTekTBCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCorTekTBCtrl)
	//}}AFX_VIRTUAL

    
// Implementation
public:
	virtual ~CCorTekTBCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCorTekTBCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CORTEKTBCTRL_H__68075F63_7DE6_11D1_A3B5_6663AF000000__INCLUDED_)
