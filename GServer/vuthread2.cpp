//////////////////////////////////////////////////////////////////////
// GServer project.
// Copyright CorTek Software, Inc. 1997-1998. All rights reserved.
//
// VUthread.cpp : implementation file
//

#include "stdafx.h"
#include  "mmsystem.h"
#include "GServer.h"
#include "VUthread2.h"
#include "gserverdoc.h"
#include	"VUMeterArray.h"
#include "DCXReadRegistry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// #define HIGHRESOLUTIONTIMER   // Use MultiMedia timers if defined
#define CYCLECOUNT


// #define BOGUS_DATA         // Define to send fake vu data

#ifdef BOGUS_DATA

	UINT m_nNumMixers;
	HMIXER m_hMixer;
	MIXERCAPS m_mxcaps;

	CString m_strWaveLineName, m_strMeterControlName;
	LONG m_lMinimum, m_lMaximum;
	DWORD m_dwMeterControlID;

	BOOL amdUninitialize();
	BOOL amdInitialize();
	BOOL amdGetWaveMeterControl();
	BOOL amdGetWaveMeterValue(LONG &lVal);

#endif


////////////////////////////////////////////
//
// Delay by milliseconds.
// 
// The value for timeGetTime() wraps around to 0 every 2^32 milliseconds (49.71 days)
// That is why the abs() call is there to check the difference
//
// The default precision can be 5 ms or more for windows NT, for window 95 it is 1 ms.
// The precision is set in the GDCXNetwork.cpp file in the StartAsServer() function
//

#ifdef HIGHRESOLUTIONTIMER
void    CTekSleep(DWORD dwDelay )
{
  DWORD  dwStartTime;

  dwStartTime = timeGetTime();
  while(abs( (timeGetTime() - dwStartTime) < dwDelay));
};

#endif

#ifdef CYCLECOUNT

#define PPRO_NOP_MSEC_COUNTER  90000    // NOT USED

void CTekSleep(DWORD dwBasedelay,DWORD dwDelay)
{
DWORD dwTickCountDelay;

while(dwDelay -- > 0 )
{
  dwTickCountDelay = dwBasedelay;
  while(dwTickCountDelay > 0)
  {
    dwTickCountDelay --;
    __asm{NOP};
  }
}
}

#else

   DWORD     dwTickCountDelay;
  // *************************************************************
  // SPECIAL DELAY LOOP ....... !!!!!
  // IT IS CALCULATED FOR Intel PPro 200 Mhz
  // This Loop gives us a resolution of 0.001 sec / 100000 (!!!!!)
  // NOTE the loop has been adjusted to faster than 1 msec

  while(dwDelay -- > 0 )
  {
    dwTickCountDelay = PPRO_NOP_MSEC_COUNTER;
    while(dwTickCountDelay > 0)
    {
      dwTickCountDelay --;
      __asm{NOP};
    }
  }
};

////////////////////////////////////////////
//
// Use High precision counter to calculate
// 1 ms delay
//
//
void    CTekSleep(DWORD dwDelay )
{
    LARGE_INTEGER goal, start, current;

    QueryPerformanceCounter(&start);
    goal.QuadPart = m_liPeriod.QuadPart * .001 * dwDelay; // Goal in counts for 1ms

    do
    {
      QueryPerformanceCounter(&current)
    }while(abs(current.QuadPart - start.QuadPart) <  goal.QuadPart)
};

#endif


/////////////////////////////////////////////////////////////////////////////

UINT ThreadProc(LPVOID pParam)
{
	// TODO: Add your specialized code here and/or call the base class

	
int		iAddr;
char	chClip1[8];
char	chPeakVU[5];
char	chBufferVUType[128];
char  chBuffer1[sizeof(DCXPORT_WRITE_INPUT )];

DCXPORT_WRITE_INPUT   *RBuffer=(DCXPORT_WRITE_INPUT *)chBuffer1; //->arChar;
ULONG   ulIO;
int     iVUtoRead;
VU_READ *pVUData;		// Data structure for our VU data

CGServerDoc*	m_pDoc = (CGServerDoc*)pParam;

#ifdef BOGUS_DATA
WORD wFlag;     //  TEST TEST

	amdInitialize();
	// get the Control ID and the names
	amdGetWaveMeterControl();

#endif

	// Delay to let the rest of the program catch up when starting with 
	// the gserv file on the command line.

     CTekSleep(m_pDoc->m_dwBasedelay,500);



	if(m_pDoc != NULL)
	{

		/////////////////////////////////////////////////////////////////////////
		// Loop until we get an EventKill from someone
		// Read the VU's for each module and place in its associated array

    while(TRUE)
		{

// Look for the EventKill to exit the thread

			if(WaitForSingleObject(m_pDoc->m_hEventKillVUThread,0) == WAIT_OBJECT_0)
					break;

#ifdef BOGUS_DATA
              wFlag++;    // Toggle fake vu data, low or high
#endif

// Build the VU meter read command based on user selections 

			// Set a loop to go through all VU ...

			iVUtoRead = m_pDoc->m_VUMetersArray.GetFirstReadIdx();

			while(iVUtoRead >= 0)
			{
				// Get a pointer to this VU data

				pVUData = m_pDoc->m_VUMetersArray.GetDataPtr(iVUtoRead);


				// Set the module address

				iAddr = pVUData->wAddr;
				if(pVUData->cLock > 0)	// Use this to determine if we should read this
				{

            //ZeroMemory(chBuffer1, sizeof(DCXPORT_WRITE_INPUT ));
						wsprintf(chBufferVUType, "*%03dXVU?\n", iAddr);			// ALL VU DATA
//            TRACE0 (chBufferVUType);
#ifndef BOGUS_DATA
  					m_pDoc->m_pDCXDevice->Write(iAddr, chBufferVUType, &ulIO, TRUE);
#endif

            // Delay between write and read as selected above by the #define
            // This is the delay for the VU reads

//            CTekSleep(m_pDoc->m_dwBasedelay,m_pDoc->m_dwVudelay);

						// Read ALL the VU data
            // If we read an ACK from the control data then ignore it and
            // find the real data

              lstrcpy(chBuffer1, chBufferVUType);   // Need this to get device address
#ifndef BOGUS_DATA
						  m_pDoc->m_pDCXDevice->Read(chBuffer1, sizeof(DCXPORT_WRITE_INPUT ), &ulIO);
#endif           

          ////////////////////////////////////////////////////
					// Now parse the data and display it ...
					// "!0000,0000,0000,0000,0000,0000,0000,0000,/000"  ... this parsing needs
					// to be done better .. maybe
					//--------------------------------------
#ifdef BOGUS_DATA

		LONG lval;
		amdGetWaveMeterValue(lval); // get value from soundcard mixer
		wsprintf(chBuffer1,"!%d.4,%d.4,%d.4,%d.4,%d.4,%d.4,%d.4,%d.4,/123",lval, lval, lval, lval, lval, lval, lval, lval);
		TRACE0(chBuffer1);
#ifdef NOTUSED
          if(wFlag & 1)
          {
              lstrcpy(chBuffer1, "!0000,0000,0000,0000,0000,0000,0000,0000,/123");
//              TRACE0 (chBuffer1);
          }
          else
          {

              lstrcpy(chBuffer1, "!4000,2000,3000,2500,2000,1500,1000,0500,/123");
//              TRACE0 (chBuffer1);
          }

#endif

#endif

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU0 data

										chPeakVU[0] = chBuffer1[1];
										chPeakVU[1] = chBuffer1[2];
										chPeakVU[2] = chBuffer1[3];
										chPeakVU[3] = chBuffer1[4];
										chPeakVU[4] = 0;


#ifndef BOGUS_DATA
										pVUData->wVUValue[0] = atoi(chPeakVU);
#endif

										if(pVUData->wVUValue[0] < 0 || pVUData->wVUValue[0] > MAX_VU_VALUE)
											pVUData->wVUValue[0] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU1 data
                    
                    chPeakVU[0] = chBuffer1[6];
										chPeakVU[1] = chBuffer1[7];
										chPeakVU[2] = chBuffer1[8];
										chPeakVU[3] = chBuffer1[9];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[1] = atoi(chPeakVU);
#endif

										if(pVUData->wVUValue[1] < 0 || pVUData->wVUValue[1] > MAX_VU_VALUE)
											pVUData->wVUValue[1] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU2 data
                    
                    chPeakVU[0] = chBuffer1[11];
										chPeakVU[1] = chBuffer1[12];
										chPeakVU[2] = chBuffer1[13];
										chPeakVU[3] = chBuffer1[14];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[2] = atoi(chPeakVU);
#endif

										if(pVUData->wVUValue[2] < 0 || pVUData->wVUValue[2] > MAX_VU_VALUE)
											pVUData->wVUValue[2] = 0;

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU3 data
                    
                    chPeakVU[0] = chBuffer1[16];
										chPeakVU[1] = chBuffer1[17];
										chPeakVU[2] = chBuffer1[18];
										chPeakVU[3] = chBuffer1[19];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[3] = atoi(chPeakVU);
#endif

										if(pVUData->wVUValue[3] < 0 || pVUData->wVUValue[3] > MAX_VU_VALUE)
											pVUData->wVUValue[3] = 0;



        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU4 data
                    
                    chPeakVU[0] = chBuffer1[21];
										chPeakVU[1] = chBuffer1[22];
										chPeakVU[2] = chBuffer1[23];
										chPeakVU[3] = chBuffer1[24];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[4] = atoi(chPeakVU);
#endif
										if(pVUData->wVUValue[4] < 0 || pVUData->wVUValue[4] > MAX_VU_VALUE)
											pVUData->wVUValue[4] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU5 data
                    
                    chPeakVU[0] = chBuffer1[26];
										chPeakVU[1] = chBuffer1[27];
										chPeakVU[2] = chBuffer1[28];
										chPeakVU[3] = chBuffer1[29];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[5] = atoi(chPeakVU);
#endif
										if(pVUData->wVUValue[5] < 0 || pVUData->wVUValue[5] > MAX_VU_VALUE)
											pVUData->wVUValue[5] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU6 data
                    
                    chPeakVU[0] = chBuffer1[31];
										chPeakVU[1] = chBuffer1[32];
										chPeakVU[2] = chBuffer1[33];
										chPeakVU[3] = chBuffer1[34];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[6] = atoi(chPeakVU);
#endif

										if(pVUData->wVUValue[6] < 0 || pVUData->wVUValue[6] > MAX_VU_VALUE)
											pVUData->wVUValue[6] = 0;

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU7 data
                    
                    chPeakVU[0] = chBuffer1[36];
										chPeakVU[1] = chBuffer1[37];
										chPeakVU[2] = chBuffer1[38];
										chPeakVU[3] = chBuffer1[39];
										chPeakVU[4] = 0;

#ifndef BOGUS_DATA
										pVUData->wVUValue[7] = atoi(chPeakVU);
#endif
										if(pVUData->wVUValue[7] < 0 || pVUData->wVUValue[7] > MAX_VU_VALUE)
											pVUData->wVUValue[7] = 0;


#ifdef BOGUS_DATA

 										pVUData->wVUValue[0] = pVUData->wVUValue[0]+250;
										pVUData->wVUValue[1]=pVUData->wVUValue[0];
										pVUData->wVUValue[2]=pVUData->wVUValue[1];
										pVUData->wVUValue[3]=pVUData->wVUValue[2];
										pVUData->wVUValue[4]=pVUData->wVUValue[3];
										pVUData->wVUValue[5]=pVUData->wVUValue[4];
										pVUData->wVUValue[6]=pVUData->wVUValue[5];
#endif

        ///////////////////////////////////////////////////////////////////////////////
				// Set clip values


										chClip1[0] = chBuffer1[42];
										chClip1[1] = chBuffer1[43];
										chClip1[2] = chBuffer1[44];
										chClip1[3] = 0;

										pVUData->wPeakClipValue = atoi(chClip1);

										if(pVUData->wPeakClipValue < 0 || pVUData->wPeakClipValue > MAX_VU_VALUE)
											pVUData->wPeakClipValue = 0;


					// Send the vu data to the clients
					// only if we didn't get and ACK character from the read
					// since the client is sending control data and we are not
					// locking our resource at this time because of speed

//					if( chBuffer1[1] != '$' )
	          m_pDoc->m_pdcxNetwork->BroadcastMsgType(pVUData, sizeof(VU_READ), DCX_VU_DATA, NULL, 0);

				}

				// Get the index to the next VU meter data

					iVUtoRead = m_pDoc->m_VUMetersArray.GetNextReadIdx(iVUtoRead);

			}

		} // END WHILE

	}


#ifdef BOGUS_DATA	
	amdUninitialize();
#endif

	SetEvent(m_pDoc->m_hEventVUThreadKilled);		// Tell the GServerDOC destructor that we have died

	return 0;
}


#ifdef BOGUS_DATA

BOOL amdInitialize()
{
	// get the number of mixer devices present in the system
	m_nNumMixers = ::mixerGetNumDevs();

	m_hMixer = NULL;
	::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0)
	{
		if (::mixerOpen(&m_hMixer,
						0,
						0,
						NULL,
						MIXER_OBJECTF_MIXER)
			!= MMSYSERR_NOERROR)
			return FALSE;

		if (::mixerGetDevCaps((UINT)m_hMixer, &m_mxcaps, sizeof(MIXERCAPS))
			!= MMSYSERR_NOERROR)
			return FALSE;
	}

	return TRUE;
}

BOOL amdUninitialize()
{
	BOOL bSucc = TRUE;

	if (m_hMixer != NULL)
	{
		bSucc = ::mixerClose(m_hMixer) == MMSYSERR_NOERROR;
		m_hMixer = NULL;
	}

	return bSucc;
}

BOOL amdGetWaveMeterControl()
{
	m_strWaveLineName.Empty();
	m_strMeterControlName.Empty();

	if (m_hMixer == NULL)
		return FALSE;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	if (::mixerGetLineInfo((HMIXEROBJ)m_hMixer,
		                   &mxl,
			    		   MIXER_OBJECTF_HMIXER |
				    	   MIXER_GETLINEINFOF_COMPONENTTYPE)
		!= MMSYSERR_NOERROR)
		return FALSE;

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_PEAKMETER;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (::mixerGetLineControls((HMIXEROBJ)m_hMixer,
		                       &mxlc,
			    			   MIXER_OBJECTF_HMIXER |
				    		   MIXER_GETLINECONTROLSF_ONEBYTYPE)
	    != MMSYSERR_NOERROR)
		return FALSE;

	// record dwControlID
	m_strWaveLineName = mxl.szName;
	m_strMeterControlName = mxc.szName;
	m_lMinimum = mxc.Bounds.lMinimum;
	m_lMaximum = mxc.Bounds.lMaximum;
	m_dwMeterControlID = mxc.dwControlID;

	return TRUE;
}

BOOL amdGetWaveMeterValue(LONG &lVal)
{
	if (m_hMixer == NULL ||
		m_strWaveLineName.IsEmpty() || m_strMeterControlName.IsEmpty())
		return FALSE;

	MIXERCONTROLDETAILS_SIGNED mxcdMeter;
    MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = m_dwMeterControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_SIGNED);
    mxcd.paDetails = &mxcdMeter;
	if (::mixerGetControlDetails((HMIXEROBJ)m_hMixer,
		                         &mxcd,
		                         MIXER_OBJECTF_HMIXER |
				    			 MIXER_GETCONTROLDETAILSF_VALUE)
		!= MMSYSERR_NOERROR)
		return FALSE;
	
	lVal = mxcdMeter.lValue;

	return TRUE;
}
#endif

