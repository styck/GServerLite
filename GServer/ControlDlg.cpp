// ControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gserver.h"

#include "DCXBinTable.h"		// cuz we are using these routines
#include "Module.h"
#include "GserverDoc.h"
#include "DCXDevice.h"
#include "ControlDlg.h"
#include "DCXLoadReg0Dialog.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	VU_UPDATE_TIME	70		// update time for VU on server


extern void    CTekSleep(DWORD m_dwBasedelay,DWORD dwDelay);	// see vuthread2.cpp

/////////////////////////////////////////////////////////////////////////////
// CControlDlg dialog


CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CControlDlg)
	m_iDelay = 0;
	m_iModuleAddr = 0;
	//}}AFX_DATA_INIT
  m_iCurValue = -1;
  m_iControl  = -1;

// Timers have not been started yet

  m_uiTimerID = 0;
  m_uiAutoScrollTimerID = 0;

// AutoScroll variables

	m_iCurrentValue=0xFFFFFFFF;		// current value of 
	m_iDirection=1;

    m_ucVUTypeReq = 0xFF;	// No VU of this   

// Initialize conversion table for step to dBu

	for(int i=0;i<4096;i++)
		m_dStepTodBu[i] = 20*log10( (double)i/(double)730);


}


void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlDlg)
	DDX_Control(pDX, IDC_VUMETER2_RDOUT, m_VU2_RdOut);
	DDX_Control(pDX, IDC_VUMETER1_RDOUT, m_VU1_RdOut);
	DDX_Control(pDX, IDC_VUMETER_2, m_VU2);
	DDX_Control(pDX, IDC_VUMETER_1, m_VU1);
	DDX_Control(pDX, IDC_VU_START, m_VuStartButton);
	DDX_Control(pDX, IDC_READVUDATA, m_ReadVuData);
	DDX_Control(pDX, IDC_SHOWCONTROLDATA, m_ShowControlData);
	DDX_Control(pDX, IDC_CTRLS_LIST, m_CControlListBox);
	DDX_Control(pDX, IDC_CTRLS_SLIDER, m_CtrlSlider);
	DDX_Control(pDX, IDC_CTRLS_RDOUT, m_CtrlRdOut);
	DDX_Text(pDX, IDC_CTRLS_DELAY, m_iDelay);
	DDV_MinMaxLong(pDX, m_iDelay, 1, 5000);
	DDX_Text(pDX, IDC_DCXCTRLS_MODULE, m_iModuleAddr);
	DDV_MinMaxInt(pDX, m_iModuleAddr, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialog)
	//{{AFX_MSG_MAP(CControlDlg)
	ON_LBN_SELCHANGE(IDC_CTRLS_LIST, OnSelchangeCtrlsList)
	ON_BN_CLICKED(IDC_DCXCTRS_SETADDR, OnDcxctrsSetaddr)
	ON_BN_CLICKED(IDC_DCXCTRLS_RESTBUS, OnDcxctrlsRestbus)
	ON_BN_CLICKED(IDC_CTRLS_ACTIVATEAUTO, OnCtrlsActivateauto)
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_WRITE_REGISTER_ZERO, OnWriteRegisterZero)
	ON_BN_CLICKED(IDC_SHOWCONTROLDATA, OnShowcontroldata)
	ON_BN_CLICKED(IDC_OSC_ON, OnOscOn)
	ON_BN_CLICKED(IDC_OSC_OFF, OnOscOff)
	ON_BN_CLICKED(IDC_READVUDATA, OnReadvudata)
	ON_BN_CLICKED(IDC_CLIP_RESET, OnClipReset)
	ON_BN_CLICKED(IDC_VU_1, OnVu1)
	ON_BN_CLICKED(IDC_VU_3, OnVu3)
	ON_BN_CLICKED(IDC_VU_5, OnVu5)
	ON_BN_CLICKED(IDC_VU_7, OnVu7)
	ON_BN_CLICKED(IDC_VU_START, OnVuStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlDlg message handlers


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnSelchangeCtrlsList() 
//          
// Descr. : User clicked on a control in the control list box.  Get its 
//			description and default register zero values so the sliders ranges
//			can be set to their range and defaults.
//          
// Return : void
//-----------------------------------------------------------------------------

void CControlDlg::OnSelchangeCtrlsList() 
{
DCX_CTRL_DESC CtrlDesc;

// Get the control index that was clicked on 

	m_iControl = m_CControlListBox.GetCurSel();

// Get the controls description

	if(m_pDoc->m_dcxBinTable.GetControlDesc(m_iControl, &CtrlDesc))
	{
		m_iCurValue = -1;
//		m_CtrlName.SetWindowText(CtrlDesc.szName);
		m_CtrlSlider.SetRange(0, CtrlDesc.iNumEntr - 1, TRUE);

	// Initialize control to its REG0 position
	// and send the value out to the DCX via the HandleSliderMove

		m_CtrlSlider.SetPos(CtrlDesc.iREG0);
		HandleSliderMove(CtrlDesc.iREG0);

	}	
}



void CControlDlg::OnDcxctrsSetaddr() 
{

  	if(m_pDoc)
  {
		UpdateData(TRUE);
//		m_pDoc->m_dcxBinTable.m_iChanAddrOffset = 	m_iModuleAddr;
  }
}

void CControlDlg::OnDcxctrlsRestbus() 
{
	m_pDoc->m_pDCXDevice->ResetBus();
	
}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnCtrlsActivateauto() 
//          
// Descr. : User clicked on the Activate button so get the current control 
//			selection from the list box and start the auto scroll timer, if 
//			the timer was already started then kill the timer.
//          
// Return : void
//-----------------------------------------------------------------------------


void CControlDlg::OnCtrlsActivateauto() 
{

	
  UpdateData(TRUE);	// Get the value

	m_iControl = m_CControlListBox.GetCurSel();

	if(m_iControl > 0)
	{
  
	  // If timer not started yet then set timer for the autoscroll id

		if(m_uiAutoScrollTimerID == 0)
		{
			// TIMER START
				m_uiAutoScrollTimerID = SetTimer(TIMER_AUTO_SCROLL, m_iDelay, NULL);
 		}
		else
		{
			KillTimer(m_uiAutoScrollTimerID);
			m_uiAutoScrollTimerID = 0;
		}	
	}
	else
	{
		MessageBox("Select a Control from the Control List before activiating",NULL,MB_OK);
	}
	
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : FillCtrlListBox()
//          
// Descr. : Fill the controls list box with the controls from the dcx.bin file
//          
// Return : void
//-----------------------------------------------------------------------------

BOOL CControlDlg::FillCtrlListBox()
{
	BOOL bRet=TRUE;
	DCX_CTRL_DESC CtrlDesc;
	int           iCounter, iCtrlNum;
	char          szBuffer[255];

	m_CControlListBox.ResetContent();

	iCtrlNum = m_pDoc->m_dcxBinTable.GetControlsCount();	// Get the number of controls

	if(iCtrlNum > 0)
	{
		for(iCounter = 0; iCounter < iCtrlNum; iCounter ++)
		{
			if(m_pDoc->m_dcxBinTable.GetControlDesc(iCounter, &CtrlDesc))	// Get control description
			{
				wsprintf(szBuffer, "#%03d  %s, %d - %03d", iCounter, CtrlDesc.szName, 
										 CtrlDesc.iNumPots, CtrlDesc.iNumEntr);
				m_CControlListBox.AddString(szBuffer);		
			}
		}
	}
	else
	{
		MessageBox("You must open a gserv configuration before testing",NULL, MB_OK);
		bRet = FALSE;
	}
	return bRet;
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : SetPointerToDoc(CGServerDoc *pDoc)
//          
// Descr. : Lets our dialog box have access to the document objects.
//          
// Return : void
//-----------------------------------------------------------------------------

void CControlDlg::SetPointerToDoc(CGServerDoc *pDoc)
{
	m_pDoc = pDoc;

}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnInitDialog
//          
// Descr. : Initialize the default values of the dialog box and fill in the 
//			controls list box.
//          
// Return : BOOL
//-----------------------------------------------------------------------------
BOOL CControlDlg::OnInitDialog() 
{
	BOOL bRet;
	CDialog::OnInitDialog();

	// Setup default values and update the dialog box

	m_ShowControlData.SetCheck(m_pDoc->m_pdcxNetwork->iShowControlData);
	m_ReadVuData.SetCheck(m_pDoc->m_pdcxNetwork->iReadVUData);

	m_iDelay=10;		// 10ms default delay
	m_iModuleAddr=33;	// Default starting module is 33 for DCX
	UpdateData(FALSE);
 
	// Fill in the list of controls
	// If success then returns TRUE, else FALSE
	bRet = FillCtrlListBox();
	// should check bRet and close dialog if false 

	return TRUE;
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : HandleSliderMove(int iPos)
//          
// Descr. : Handle moving the slider and outputting the proper control data to
//			the DCX.
//          
// Return : void
//-----------------------------------------------------------------------------

void CControlDlg::HandleSliderMove(int iPos)
{
	CModule							cm_Module;		// Converts module number to DCX address

	DCX_CTRLDATA  ctrlData;
	int           iCount;
	ULONG         ulWrite;

	char    chBuffer[64];	// throw away buffer to read the response from DCX
	ULONG   ulIO;					// returned error code from read

	if(m_pDoc)
	{
		if(m_iControl >=0 )
		{
			if(m_pDoc->m_dcxBinTable.RemapControlData(cm_Module.GetModuleNumber(m_iModuleAddr), m_iControl, iPos, &ctrlData))
			{
				// If we are in SERVER mode then we want to write data to DCX

					m_CtrlRdOut.SetWindowText(ctrlData.szRdOut);
//					m_pDoc->DisplayGeneralMessage("$ Msg Data Begin $");

					/////////////////////////////////////////////////////////////////////////////
					// Loop thru all the pots for this control and write the XXW 

					for(iCount = 0; iCount < ctrlData.iPotCount; iCount++)    
					{	
                  if( ! m_pDoc->m_pDCXDevice->Write(ctrlData.arPotData[iCount].iAddr,
						ctrlData.arPotData[iCount].szData, &ulWrite, FALSE) )
					{
							m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);	// Display what we wrote
							break;
					}


					//	CTekSleep(m_lRWDelay);		// Delay 1 ms so that we can read our response
						 CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwCtrldelay);
							
						// Read the response which should be !ccggp\n  We do nothing with this
						// cc - target chip, gg - group on the DCX board the chip is in
						// p - the target pot

						lstrcpy(chBuffer,ctrlData.arPotData[iCount].szData); // must pass the address in string
						m_pDoc->m_pDCXDevice->Read(chBuffer, 64, &ulIO);

						m_pDoc->DisplayGeneralMessage(ctrlData.arPotData[iCount].szData);
					}
//					m_pDoc->DisplayGeneralMessage("$ Msg Data End $");

//					{
						m_CtrlRdOut.SetWindowText(ctrlData.szRdOut);				// Display the readout value
//						m_pDoc->DisplayGeneralMessage("$ Direct Data Begin $");

					/////////////////////////////////////////////////////////////////////////////
					// Loop thru all the pots for this control and write the XXW
						
						for(iCount = 0; iCount < ctrlData.iPotCount; iCount++)    
						{
							if( ! m_pDoc->m_pDCXDevice->Write(ctrlData.arPotData[iCount].iAddr,
								ctrlData.arPotData[iCount].szData, &ulWrite,FALSE))
							{
								m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);
								break;
							}

//							CTekSleep(m_lRWDelay);		// Delay 1 ms so that we can read our response
						 CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwCtrldelay);
							
							// Read the response which should be !ccggp\n. We do nothing with this
							// cc - target chip, gg - group on the DCX board the chip is in
							// p - the target pot

							lstrcpy(chBuffer,ctrlData.arPotData[iCount].szData); // must pass the address in string
							m_pDoc->m_pDCXDevice->Read(chBuffer, 64, &ulIO);

							m_pDoc->DisplayGeneralMessage(ctrlData.arPotData[iCount].szData);	// Display what was written out
						}
//						m_pDoc->DisplayGeneralMessage("$ Direct Data End $");
//					}
			}
			else
				m_pDoc->DisplayGeneralMessage(IDS_DCX_CONTROLREMAP_ERROR);
		}
		else
			m_pDoc->DisplayGeneralMessage(IDS_DCX_CONTROLSEL_INVALID);
      
	}

}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//          
// Descr. : Handle the clicking on the slider.  Get the slider position and
//			send it to the HandleSliderMove routine.
//          
// Return : void
//-----------------------------------------------------------------------------


void CControlDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	int   iCurPos;

	if(pScrollBar == (CScrollBar*)&m_CtrlSlider)
  {
		iCurPos = m_CtrlSlider.GetPos();

		if(m_iCurValue != iCurPos)
		{
			m_iCurValue = iCurPos;
			HandleSliderMove(iCurPos);  
		}
  }
	else
		CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}



//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnTimer(UINT nIDEvent) 
//          
// Descr. : Handle the auto slider timer event.  Uses the controls table to 
//			determine range of currently selected control and move the slider
//			up and down through the range, reversing direction at the 
//			min and max points
//          
// Return : void
//-----------------------------------------------------------------------------


void CControlDlg::OnTimer(UINT nIDEvent) 
{
BOOL    bErr = FALSE;
char    chPeakVU[5];
char    chAverageVU[5];
int     iVU, iVUtoRead, iPeak, iPCV;

VU_READ *pVUData;

																			  
	if(nIDEvent == TIMER_VU)	// VU timer	 144
	{

	// Set a loop to go through all VU 
	
		iVUtoRead = m_pDoc->m_VUMetersArray.GetFirstReadIdx();

//////////////////////////////////////////
// Here we are just looping through the
// pre, post, comp, and gate Vu meters of
// a given module, not all modules

		for(iVUtoRead = 0; iVUtoRead < 4; iVUtoRead++)
		{

		// Get point to first VU data

			pVUData = m_pDoc->m_VUMetersArray.GetDataPtr(iVUtoRead);


// If the current meter we are on is being displayed then update the progress bar
// and labels. This depends on the button pressed in the control panel

			// Scale the dBu reading to 4096 values

				iVU =   (int)((m_dStepTodBu[pVUData->wVUValue[m_ucVUTypeReq]]+43.2871)*4096)/58.27;
				iPeak = (int)((m_dStepTodBu[pVUData->wPeakClipValue]+43.2871)*4096)/58.27;

			// Display the progress bar

				m_VU1.SetPos(iPeak);
				m_VU2.SetPos(iVU);

			// Convert floating point to text

//				gcvt(m_dStepTodBu[pVUData->wVUValue[m_ucVUTypeReq]],2,chAverageVU);
//				gcvt(m_dStepTodBu[pVUData->wPeakClipValue],2,chPeakVU);

				itoa(pVUData->wVUValue[m_ucVUTypeReq],chAverageVU,10);
				itoa(pVUData->wPeakClipValue,chPeakVU,10);

			// Display the dBu value

				m_VU1_RdOut.SetWindowText(chPeakVU);
				m_VU2_RdOut.SetWindowText(chAverageVU);
  
	/////////////////////////////////////////////
	// Update Clip check boxes always, no matter
	// what vu is selected

				iPCV = pVUData->wPeakClipValue;

			switch(m_ucVUTypeReq)
			{
					case 1:
						if(iPCV> 1)
							CheckDlgButton(IDC_CLIP1, 1 );
						else
							CheckDlgButton(IDC_CLIP1, 0 );
					break;
			
					case 2:
						if(iPCV> 1)
							CheckDlgButton(IDC_CLIP3, 1 );
						else
							CheckDlgButton(IDC_CLIP3, 0 );
					break;						
			
					case 3:
						if(iPCV> 1)
							CheckDlgButton(IDC_CLIP5, 1 );
						else
							CheckDlgButton(IDC_CLIP5, 0 );
							
					break;
			
					case 4:
						if(iPCV> 1)
							CheckDlgButton(IDC_CLIP7, 1 );
						else
							CheckDlgButton(IDC_CLIP7, 0 );
						
					break;

						default:
						bErr = TRUE;
						break;

			}

		}

	} else if (nIDEvent == TIMER_AUTO_SCROLL) 		// 145
	{

/////////////////////////////////////////////////////////////////
// Handle AutoScroll Timer event here
/////////////////////////////////////////////////////////////////

		DCX_CTRL_DESC CtrlDesc;

		if(m_pDoc->m_dcxBinTable.GetControlDesc(m_iControl, &CtrlDesc))
		{
			if(m_iDirection > 0)	// Move control UP
			{
				if( (m_iCurValue + m_iDirection) < CtrlDesc.iNumEntr)
					m_iCurValue += m_iDirection;
				else
					m_iDirection = -1;	// At top, reverse

			}
			else									// Move control DOWN
			{
				if((m_iCurValue + m_iDirection) >= 0)
					m_iCurValue += m_iDirection;
				else
					m_iDirection = 1;	// At bottom, reverse
			}

			// Update the slider position

			m_CtrlSlider.SetPos(m_iCurValue);
			HandleSliderMove(m_iCurValue);  
		}

	}

	CDialog::OnTimer(nIDEvent);
}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnOK()
//          
// Descr. : User clicked on DONE (really OK) Clean up the timer and exit
//          
// Return : void
//-----------------------------------------------------------------------------

void CControlDlg::OnOK() 
{
	// TODO: Add extra validation here

	// If the autoscroll timer is going then kill it.

	if(m_uiAutoScrollTimerID)
	{
		KillTimer(m_uiAutoScrollTimerID);
		m_uiAutoScrollTimerID=0;
	}

	
	CDialog::OnOK();
}

///////////////////////////////////////////////
// User clicked on the Set Zero Register
// button so bring up the dialog to do it.

void CControlDlg::OnWriteRegisterZero() 
{

	DCXLoadReg0Dialog dlg;				// Create the object
	dlg.m_pDoc = m_pDoc;				// Make sure it knows about the document functions
	int ret =  dlg.DoModal();			// do it

}


//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnOscOn() 
//          
// Descr. : User clicked on a Oscillator On button. Find the oscillator level 
//			control and move the slider to the -15 db level 
//          
// Return : void
//-----------------------------------------------------------------------------

void CControlDlg::OnOscOn() 
{
DCX_CTRL_DESC CtrlDesc;
int iSaveModuleAddr;

	iSaveModuleAddr = m_iModuleAddr;

	m_iModuleAddr = FindCueModule();

	for(int i=0; i < m_CControlListBox.GetCount(); i++)
	{
		// Loop through all the controls to find oscillator level

		if(m_pDoc->m_dcxBinTable.GetControlDesc(i, &CtrlDesc))
		{
			// See if its the oscillator level

			if(strcmp("OSCILLATOR LEVEL", CtrlDesc.szName) == 0)
			{
				m_iCurValue = -1;
				m_CtrlSlider.SetRange(0, CtrlDesc.iNumEntr - 1, TRUE);

				// We found the oscillator level so set the current selection
				// to this index and set the member variable
				m_iControl=i;
				m_CControlListBox.SetCurSel(m_iControl);

				// Move slider to -15 (Max entry minus 6 gets us there for current table)
				m_CtrlSlider.SetPos(CtrlDesc.iNumEntr-6);
				// and send the value out to the DCX via the HandleSliderMove
				HandleSliderMove(CtrlDesc.iNumEntr-6);

			}
		}

	}
	m_iModuleAddr = iSaveModuleAddr;
}
	
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : OnOscOff() 
//          
// Descr. : User clicked on a Oscillator OFF button. Find the oscillator level 
//			control and move the slider to the register zero value 
//          
// Return : void
//-----------------------------------------------------------------------------


void CControlDlg::OnOscOff() 
{
DCX_CTRL_DESC CtrlDesc;
int iSaveModuleAddr;

	iSaveModuleAddr = m_iModuleAddr;

	m_iModuleAddr = FindCueModule();

	for(int i=0; i < m_CControlListBox.GetCount(); i++)
	{
		// Loop through all the controls to find oscillator level

		if(m_pDoc->m_dcxBinTable.GetControlDesc(i, &CtrlDesc))
		{
			// See if its the oscillator level

			if(strcmp("OSCILLATOR LEVEL", CtrlDesc.szName) == 0)
			{
				m_iCurValue = -1;
				m_CtrlSlider.SetRange(0, CtrlDesc.iNumEntr - 1, TRUE);

				// We found the oscillator level so set the current selection
				// to this index and set the member variable
				m_iControl=i;
				m_CControlListBox.SetCurSel(m_iControl);

				// Move slider Register zero value
				m_CtrlSlider.SetPos(CtrlDesc.iREG0);
				// and send the value out to the DCX via the HandleSliderMove
				HandleSliderMove(CtrlDesc.iREG0);

			}
		}

	}
	m_iModuleAddr = iSaveModuleAddr;
	
}

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
// Name   : FindCueModule() 
//          
// Descr. : Used above in OnOscOff() to find the cue module to set the oscillator
//          
// Return : void
//-----------------------------------------------------------------------------

int CControlDlg::FindCueModule()
{

 CModule   cModule;

  for(int iIdx = 0; iIdx < DCX_DEVMAP_MAXSIZE; iIdx ++)
  {
	
	// Find the CueModule

    switch(m_pDoc->m_dcxdevMap.GetModuleType(iIdx))
      {
      case  DCX_DEVMAP_MODULE_NA:
        break;
      case  DCX_DEVMAP_MODULE_INPUT:
        break;
      case  DCX_DEVMAP_MODULE_AUX:
        break;
      case  DCX_DEVMAP_MODULE_MATRIX:
        break;
      case  DCX_DEVMAP_MODULE_CUE:
		  return cModule.GetModuleAddress(iIdx);
        break;
      case  DCX_DEVMAP_MODULE_MASTER:
        break;
      default:
        break;
      }
	}
	return 0;	// Cuemodule not found
}

///////////////////////////////////////////////////////////
// Check Boxes : let everyone else know the status of these

void CControlDlg::OnShowcontroldata() 
{
	m_pDoc->m_pdcxNetwork->iShowControlData= m_ShowControlData.GetCheck();
}

void CControlDlg::OnReadvudata() 
{
	m_pDoc->m_pdcxNetwork->iReadVUData= m_ReadVuData.GetCheck();	
}


void CControlDlg::OnClipReset() 
{
	// Clear all the peak clip indicators
	// by unchecking all the buttons

				CheckDlgButton(IDC_CLIP1, 0 );
				CheckDlgButton(IDC_CLIP2, 0 );
				CheckDlgButton(IDC_CLIP3, 0 );
				CheckDlgButton(IDC_CLIP4, 0 );
				CheckDlgButton(IDC_CLIP5, 0 );
				CheckDlgButton(IDC_CLIP6, 0 );
				CheckDlgButton(IDC_CLIP7, 0 );
				CheckDlgButton(IDC_CLIP8, 0 );

	
}


void CControlDlg::OnVu1() 
{
	m_ucVUTypeReq = 1;
};

void CControlDlg::OnVu3()
{
	m_ucVUTypeReq = 2;	
};

void CControlDlg::OnVu5() 
{
	m_ucVUTypeReq = 3;	
};

void CControlDlg::OnVu7() 
{
	m_ucVUTypeReq = 4;	
};



void CControlDlg::OnVuStart() 
{
	if(m_pDoc->m_VUthread != NULL)		// Make sure the VU reading thread is started before
	{																// we start the timer to display them.

	//	UpdateData(TRUE);

		if(m_uiTimerID == 0)
		{
				// TIMER START

				m_uiTimerID = SetTimer(TIMER_VU, VU_UPDATE_TIME, NULL);
 
				if(m_uiTimerID)
				{
					m_VuStartButton.SetWindowText("Stop");
				}
		}
		else
		{
			KillTimer(m_uiTimerID);
			m_uiTimerID = 0;
			m_VuStartButton.SetWindowText("Start");
		}	

	}
	else		// SERVER mode starts the thread to read VU data
	{
		MessageBox( "Must be in SERVER mode to read VU", NULL, MB_OK ); 
	}

}
