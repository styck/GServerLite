//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// CorTekTBCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GServer.h"
#include "CorTekTBCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCorTekTBCtrl

CCorTekTBCtrl::CCorTekTBCtrl()
{
}

CCorTekTBCtrl::~CCorTekTBCtrl()
{
}


BEGIN_MESSAGE_MAP(CCorTekTBCtrl, CToolBarCtrl)
	//{{AFX_MSG_MAP(CCorTekTBCtrl)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorTekTBCtrl message handlers

/////////////////////////////////////////////////////////////////////
//  MEMBER FUNCTION: OnEraseBkgnd(CDC* pDC)  
//
//
//
BOOL CCorTekTBCtrl::OnEraseBkgnd(CDC* pDC) 
{
RECT    r;
CBrush  cbrush;

	// TODO: Add your message handler code here and/or call default
	//return CToolBarCtrl::OnEraseBkgnd(pDC);

// Erase background of device setup

	GetClientRect(&r);
	pDC->SelectStockObject(WHITE_BRUSH);
	pDC->PatBlt(0,0,r.right, 1, PATCOPY);
	pDC->PatBlt(0,0, 1, r.bottom, PATCOPY);

	cbrush.CreateSolidBrush(RGB(128, 128, 128));
	pDC->FrameRect(CRect(1, 1, r.right - 1, r.bottom - 1), &cbrush);

	pDC->SelectStockObject(BLACK_BRUSH);
	pDC->PatBlt(1,r.bottom - 1, r.right, 1, PATCOPY);
	pDC->PatBlt(r.right - 1, 1, 1, r.bottom, PATCOPY);


return TRUE;

}


