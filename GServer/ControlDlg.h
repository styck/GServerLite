#if !defined(AFX_CONTROLDLG_H__24951443_57C5_11D3_AD1C_00A024EC0B59__INCLUDED_)
#define AFX_CONTROLDLG_H__24951443_57C5_11D3_AD1C_00A024EC0B59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlDlg.h : header file
//


#include "Gserverdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CControlDlg dialog

class CControlDlg : public CDialog
{
// Construction
public:
	void SetPointerToDoc(CGServerDoc *pDoc);
	void FillCtrlListBox();
	CControlDlg(CWnd* pParent = NULL);   // standard constructor

  CGServerDoc             *m_pDoc;

    int   m_iCurValue;
	int   m_iControl;
	UINT  m_uiTimerID;
	int m_uiAutoScrollTimerID;


// Dialog Data
	//{{AFX_DATA(CControlDlg)
	enum { IDD = IDD_TESTCONTROLSSETUP };
	CListBox	m_CControlListBox;
	CSliderCtrl	m_CtrlSlider;
	CEdit	m_CtrlRdOut;
	int		m_iDelay;
	int		m_iModuleAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CControlDlg)
	afx_msg void OnSelchangeCtrlsList();
	afx_msg void OnDcxctrsSetaddr();
	afx_msg void OnDcxctrlsRestbus();
	afx_msg void OnCtrlsActivateauto();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
//	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int m_iCurrentValue;	// AutoScroll current value
	int m_iDirection;			// AutoScroll direction

	void HandleSliderMove(int iPos);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLDLG_H__24951443_57C5_11D3_AD1C_00A024EC0B59__INCLUDED_)
