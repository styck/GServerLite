// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__266E64BD_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
#define AFX_MAINFRM_H__266E64BD_6482_11D1_A3B1_C4D3CD000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Splitter window defines

#ifdef NOTUSED
	#define SPLITTER_SIZE1	100	// initial size of the new view
	#define SPLITTER_SIZE2	400	// initial size of the new view
#endif

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

#ifdef NOTUSED
  UINT      m_nCurrentCommView;

  void OnChangeCommView(UINT nCmdID);
  void OnUpdateCommViewUI(CCmdUI* pCmdUI);

  void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);
  void DockControlBarRightOf(CToolBar* Bar,CToolBar* RightOf);
#endif

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
//	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnLaunchHTMLHelp();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
//  CToolBar    m_wndTBViews;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__266E64BD_6482_11D1_A3B1_C4D3CD000000__INCLUDED_)
