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


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define	RW_DELAY	4


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

#ifdef NOTUSED
void    CTekSleep(DWORD dwDelay )
{
  DWORD  dwStartTime;

  dwStartTime = timeGetTime();
  while(abs( (timeGetTime() - dwStartTime) < dwDelay));
};

#else
////////////////////////////////////////////
//
//
//
#define PPRO_NOP_MSEC_COUNTER     90000 // !!
void    CTekSleep(DWORD dwDelay )
{
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
//            OutputDebugString (chBufferVUType);

  					m_pDoc->m_pDCXDevice->Write(iAddr, chBufferVUType, &ulIO);

						// Delay between write and read as selected by user

            CTekSleep(RW_DELAY);

						// Read ALL the VU data
            // If we read an ACK from the control data then ignore it and
            // find the real data

//            do
//            {
              lstrcpy(chBuffer1, chBufferVUType);
						  m_pDoc->m_pDCXDevice->Read(chBuffer1, sizeof(DCXPORT_WRITE_INPUT ), &ulIO);
              OutputDebugString (chBuffer1);
//            }while(chBuffer1[0] == '!' && chBuffer1[1] == '$');
           

          ////////////////////////////////////////////////////
					// Now parse the data and display it ...
					// "!0000,0000,0000,0000,0000,0000,0000,0000,/000"  ... this parsing needs
					// to be done better .. maybe
					//--------------------------------------
// #define BOGUS_DATA
#ifdef BOGUS_DATA
              lstrcpy(chBuffer1, "!1000,0900,1100,0800,1200,0700,1300,0800,/123");
              OutputDebugString (chBuffer1);
#endif

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU0 data

										chPeakVU[0] = chBuffer1[1];
										chPeakVU[1] = chBuffer1[2];
										chPeakVU[2] = chBuffer1[3];
										chPeakVU[3] = chBuffer1[4];
										chPeakVU[4] = 0;

										pVUData->wVUValue[0] = atoi(chPeakVU);

										if(pVUData->wVUValue[0] < 0 || pVUData->wVUValue[0] > 4095)
											pVUData->wVUValue[0] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU1 data
                    
                    chPeakVU[0] = chBuffer1[6];
										chPeakVU[1] = chBuffer1[7];
										chPeakVU[2] = chBuffer1[8];
										chPeakVU[3] = chBuffer1[9];
										chPeakVU[4] = 0;

										pVUData->wVUValue[1] = atoi(chPeakVU);

										if(pVUData->wVUValue[1] < 0 || pVUData->wVUValue[1] > 4095)
											pVUData->wVUValue[1] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU2 data
                    
                    chPeakVU[0] = chBuffer1[11];
										chPeakVU[1] = chBuffer1[12];
										chPeakVU[2] = chBuffer1[13];
										chPeakVU[3] = chBuffer1[14];
										chPeakVU[4] = 0;

										pVUData->wVUValue[2] = atoi(chPeakVU);

										if(pVUData->wVUValue[2] < 0 || pVUData->wVUValue[2] > 4095)
											pVUData->wVUValue[2] = 0;

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU3 data
                    
                    chPeakVU[0] = chBuffer1[16];
										chPeakVU[1] = chBuffer1[17];
										chPeakVU[2] = chBuffer1[18];
										chPeakVU[3] = chBuffer1[19];
										chPeakVU[4] = 0;

										pVUData->wVUValue[3] = atoi(chPeakVU);

										if(pVUData->wVUValue[3] < 0 || pVUData->wVUValue[3] > 4095)
											pVUData->wVUValue[3] = 0;



        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU4 data
                    
                    chPeakVU[0] = chBuffer1[21];
										chPeakVU[1] = chBuffer1[22];
										chPeakVU[2] = chBuffer1[23];
										chPeakVU[3] = chBuffer1[24];
										chPeakVU[4] = 0;

										pVUData->wVUValue[4] = atoi(chPeakVU);

										if(pVUData->wVUValue[4] < 0 || pVUData->wVUValue[4] > 4095)
											pVUData->wVUValue[4] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU5 data
                    
                    chPeakVU[0] = chBuffer1[26];
										chPeakVU[1] = chBuffer1[27];
										chPeakVU[2] = chBuffer1[28];
										chPeakVU[3] = chBuffer1[29];
										chPeakVU[4] = 0;

										pVUData->wVUValue[5] = atoi(chPeakVU);

										if(pVUData->wVUValue[5] < 0 || pVUData->wVUValue[5] > 4095)
											pVUData->wVUValue[5] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU6 data
                    
                    chPeakVU[0] = chBuffer1[31];
										chPeakVU[1] = chBuffer1[32];
										chPeakVU[2] = chBuffer1[33];
										chPeakVU[3] = chBuffer1[34];
										chPeakVU[4] = 0;

										pVUData->wVUValue[6] = atoi(chPeakVU);

										if(pVUData->wVUValue[6] < 0 || pVUData->wVUValue[6] > 4095)
											pVUData->wVUValue[6] = 0;

        ///////////////////////////////////////////////////////////////////////////////
				// Set the VU7 data
                    
                    chPeakVU[0] = chBuffer1[36];
										chPeakVU[1] = chBuffer1[37];
										chPeakVU[2] = chBuffer1[38];
										chPeakVU[3] = chBuffer1[39];
										chPeakVU[4] = 0;

										pVUData->wVUValue[7] = atoi(chPeakVU);

										if(pVUData->wVUValue[7] < 0 || pVUData->wVUValue[7] > 4095)
											pVUData->wVUValue[7] = 0;


        ///////////////////////////////////////////////////////////////////////////////
				// Set clip values


										chClip1[0] = chBuffer1[42];
										chClip1[1] = chBuffer1[43];
										chClip1[2] = chBuffer1[44];
										chClip1[3] = 0;

										pVUData->wPeakClipValue = atoi(chClip1);

										if(pVUData->wPeakClipValue < 0 || pVUData->wPeakClipValue > 4095)
											pVUData->wPeakClipValue = 0;


					// Send the vu data to the clients
					// only if we didn't get and ACK character from the read
					// since the client is sending control data and we are not
					// locking our resource at this time because of speed

//					if( (chPeakVU[1] != '$') || (chAverageVU[1] != '$') )

	          m_pDoc->m_pdcxNetwork->BroadcastMsgType(pVUData, sizeof(VU_READ), DCX_VU_DATA, NULL, 0);

				}

				// Get the index to the next VU meter data

					iVUtoRead = m_pDoc->m_VUMetersArray.GetNextReadIdx(iVUtoRead);

			}

		} // END WHILE

	}

	SetEvent(m_pDoc->m_hEventVUThreadKilled);		// Tell the GServerDOC destructor that we have died

	return 0;
}



