//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// DCXLoadReg0Dialog.cpp : implementation file
//
// Load and write the register zero values
//

#include "stdafx.h"
#include "DCXLoadReg0Dialog.h"
#include "Module.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DCXLoadReg0Dialog dialog


DCXLoadReg0Dialog::DCXLoadReg0Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(DCXLoadReg0Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DCXLoadReg0Dialog)
	m_nAddress = 0;
	m_strFile = _T("");
	m_strList1 = _T("");
	m_strList2 = _T("");
	m_bAllModulesCheck = FALSE;
	//}}AFX_DATA_INIT

}


void DCXLoadReg0Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DCXLoadReg0Dialog)
	DDX_Text(pDX, IDC_ADDRESS, m_nAddress);
	DDV_MinMaxUInt(pDX, m_nAddress, 33, 999);
	DDX_Text(pDX, IDC_FILENAME, m_strFile);
	DDX_LBString(pDX, IDC_LIST1, m_strList1);
	DDX_LBString(pDX, IDC_LIST2, m_strList2);
	DDX_Check(pDX, IDC_ALL_MODULES, m_bAllModulesCheck);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(DCXLoadReg0Dialog, CDialog)
	//{{AFX_MSG_MAP(DCXLoadReg0Dialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	ON_BN_CLICKED(ID_DOWNLOAD, OnDownload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DCXLoadReg0Dialog message handlers

void DCXLoadReg0Dialog::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	
	CWnd::UpdateData(TRUE); // move data from dialog to variables

	// open a get file dialog box
	CFileDialog dlg(TRUE, "wrz", "*.wrz;*.rzd", NULL,"DCX Reg0 Files (*.wrz;*.rzd)|*.wrz;*.rzd|All Files (*.*)|*.*||");
	if (dlg.DoModal() == IDOK)	
	{
		m_strFile = dlg.GetPathName(); //put filename in dialog box variable

		CWnd::UpdateData(FALSE); // update the dialog box data to show filename

		CStdioFile reg0file; //create a text file object

		// open the file read-only
		VERIFY(reg0file.Open(dlg.GetPathName(), CFile::modeRead));

		CListBox* pLB1 = (CListBox*) GetDlgItem(IDC_LIST1); //get a pointer to the dialog list1 box
		pLB1->ResetContent(); // clear the list box
		CListBox* pLB2 = (CListBox*) GetDlgItem(IDC_LIST2); //get a pointer to the dialog list2 box
		pLB2->ResetContent(); // clear the list box


		CString strRegline; //create a string to hold a line of text from file
		int ichip = -1; // create an int to hold the chip value
						// ichip is set to -1 to handle the case
						// where chip numbers are absent from the
						// DCX register file.
		int iwiper[4];	// create an int array to hold the wiper values

		dcxreg0strings.RemoveAll();  // clear elements from the CString Array

		while (1) // process all the lines in the file
		{
			
			if (reg0file.ReadString(strRegline) == FALSE) //read a line of text from file
			break;
			pLB1->InsertString(-1,strRegline); //copy string to list box
						
			// check this string to see if it's a "//" comment line
			// or if it begins with "DCX"
			if ( _strnicmp(strRegline,"//",2) != 0  &&  _strnicmp(strRegline,"DCX",3) != 0 ) 
			{
				
				// Parse this line of text and break out the chip
				// and four wiper values
				ParseReg0Line(&strRegline, &ichip, &iwiper[0]);

				// Now that we have the chip and wiper values,
				// convert them to four DCX command strings.
				char chBuff[32];
				CString buff;
				int ipot;

				for (ipot =0; ipot<4; ipot++) 
				{
					wsprintf(chBuff,"XXD%02d%d0%03d\n",ichip,ipot,iwiper[ipot]);
					pLB1->InsertString(-1,chBuff); //copy string to list1 box
					
					wsprintf(buff.GetBuffer(100),"XXD%02d%d0%03d\n",ichip,ipot,iwiper[ipot]);
					buff.ReleaseBuffer(); // free the CString conversion buffer
					dcxreg0strings.Add(buff); // put the string in the CString Array
									
				}
			} else
			{
				if(_strnicmp(strRegline,"//DCX SUB",9) == 0)
						m_iReg0FileType=DCX_DEVMAP_MODULE_AUX;

				if(_strnicmp(strRegline,"//DCX CUE",9) == 0)
						m_iReg0FileType=DCX_DEVMAP_MODULE_CUE;

				if(_strnicmp(strRegline,"//DCX INPUT",11) == 0)
						m_iReg0FileType=DCX_DEVMAP_MODULE_INPUT;

				if(_strnicmp(strRegline,"//DCX MATRIX",12) == 0)
						m_iReg0FileType=DCX_DEVMAP_MODULE_MATRIX;

				if(_strnicmp(strRegline,"//DCX MASTER",12) == 0)
						m_iReg0FileType=DCX_DEVMAP_MODULE_MASTER;

			}
		}

		CWnd::UpdateData(FALSE); // update the dialog box
		reg0file.Close(); //close the file

	}
}

void DCXLoadReg0Dialog::OnClose() 
{
	// TODO: Add your control notification handler code here
	
	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////
//
// Must calculate the address since they are not linear	then write the data
//

void DCXLoadReg0Dialog::OnDownload() 
{
CModule							cm_Module;		// Converts module number to DCX address

		UpdateData(TRUE);

	if(m_bAllModulesCheck)
	{
		for(int iCount = 0; iCount < DCX_DEVMAP_MAXSIZE; iCount++)
		{
			if(m_pDoc->m_dcxdevMap.GetModuleType(iCount) == m_iReg0FileType)
			{

				m_nAddress = cm_Module.GetModuleAddress(iCount+1);

				UpdateData(FALSE);
				WriteRegZero();
			}
		}
	}
	else
	{
		WriteRegZero();
	}
}

BOOL DCXLoadReg0Dialog::OnInitDialog() 
{
	m_nAddress = DCX_START_ADDRESS;  //initalize DCX address in dialog box to 33
	return CDialog::OnInitDialog();
}


/*	class function: DCXLoadReg0Dialog::ParseReg0Line
	This function parses a line of text from a DCX Register 0 file,
	and breaks it into integers representing the chip number and
	four pot wiper values.  The text string in placed in a CString
	variable and passed to this class function.  The chip and wiper
	variables are returned by the function.

	Note:	If the chip value isn't specified in the text line,
			then the existing value of ichip will be incremented by one.
	
	Usage:	In your calling routine declare storage 
			for the variables:
			 CString strRegline; //create a string
			 int ichip= -1; // create an int to hold the chip value
							// ichip is set to -1 to handle the case
							// where chip numbers are absent from the
							// DCX register file.
			 int iwiper[4]; // create an int array to hold the wiper values
			Load a line of text into the CString strRegline.
			Then call the parser:
			 ParseReg0Line(&strRegline, &ichip, &iwiper[0]);
*/
void DCXLoadReg0Dialog::ParseReg0Line(CString * strRegline, int * ichip, int * iwiper )
{
				
				BOOL CFLAG=FALSE;
				// check for chip number, i.e. leading "C" as in C1, C2, ...
				if ((strRegline->GetAt(0) == 'C') || (strRegline->GetAt(0) == 'c')) 
				{
					CFLAG=TRUE;
				}
				// setup token handling
				char seps[] = " ,\t\n";
				char* token;

				// get the first token
				token = strtok( strRegline->GetBuffer(100), seps); // get pointer to first token

				// handle chip numbers
				if (CFLAG)
				{
					// chip number is present
					*ichip = atoi(token+1); // skip over the "C" character
											// and get the chip value
					iwiper[0] = atoi(strtok(NULL,seps)); //get wiper value from next token
				}
				else 
				{
					// chip number is missing
					*ichip = *ichip + 1; // increment the chip number
					iwiper[0] = atoi(token); // get the wiper value from the current token
				}

				iwiper[1] = atoi(strtok(NULL,seps)); //get wiper value from next token
				iwiper[2] = atoi(strtok(NULL,seps)); //get wiper value from next token
				iwiper[3] = atoi(strtok(NULL,seps)); //get wiper value from next token
				strRegline->ReleaseBuffer(); // free the CString conversion buffer

}


/////////////////////////////////////////////////////////////
//
//
// This function writes the register zero file to the 
// module address specified in the member variable

void DCXLoadReg0Dialog::WriteRegZero()
{

	CListBox* pLB2 = (CListBox*) GetDlgItem(IDC_LIST2); //get a pointer to the dialog list2 box
	pLB2->ResetContent(); // clear the list box

	CWnd::UpdateData(TRUE); // move data from dialog to variables
	if (m_nAddress <DCX_START_ADDRESS || m_nAddress > 999) // check the address limits
	{
		return;
	}

	char    chBuffer[64];	// throw away buffer to read the response from DCX
	char		addbuff[64];
	ULONG		ulWrite;			// needed by call to dcx write

	for (int icount =0; icount < dcxreg0strings.GetSize(); icount++) 
	{
		wsprintf(addbuff,"*%03d",	m_nAddress); // convert dcx address to string

		// combine address and commands and move to listbox2

		pLB2->InsertString(-1,(const char*)(addbuff + dcxreg0strings.GetAt(icount))); //copy string to list2 box

		// call the DCX download routine, you may need to get the pDoc handle.
		// The type cast below may need changing, or a call to .GetBuffer and
		// .ReleaseBuffer may be needed to handle the CString conversion.

		strcat(addbuff,(const char*)dcxreg0strings.GetAt(icount));


		// Set Xicor Data register to value

		if( ! m_pDoc->m_pDCXDevice->Write(addbuff, &ulWrite, FALSE))
    {
			m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_WRITE);
    }

			Sleep(30);		// Delay 30 ms so that we can read our response
				
			// Read the response which should be !ccggp\n  
			// cc - chip target 00 to 99
			// gg - group on the DCX board the chip is in
			// p - target pot

			lstrcpy(chBuffer,addbuff); // must pass the address in string
			if( ! m_pDoc->m_pDCXDevice->Read(chBuffer, 64, &ulWrite))
			{
				m_pDoc->DisplayGeneralMessage(DCXDEV_ERROR_READ);
			}
			else
			{
				chBuffer[7]='\0';													// NULL terminate return string
				m_pDoc->DisplayGeneralMessage(chBuffer);	// Display results
			}
	}

	CWnd::UpdateData(FALSE); // update the dialog box


}
