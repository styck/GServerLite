// DCX Port I/O driver test routine for NT  VERSION 1.0
//
// Copyright 1997, CorTek Software, Inc.
//


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include <time.h>

#include "..\\dcxoctl.h"        // This defines the IOCTL constants.

void	Wait(double seconds);

void __cdecl main(int argc, char ** argv)
{

    // The following is returned by IOCTL.  It is true if the read succeeds.
    BOOL    IoctlResult;

    // The following parameters are used in the IOCTL call

    HANDLE              hndFile;        // Handle to device, obtain from CreateFile
    DCXPORT_WRITE_INPUT WriteBuffer;    // Input buffer for DeviceIoControl

    LONG                IoctlCode;
    ULONG               DataValue;
    ULONG               DataLength;
    ULONG               ReturnedLength; // Number of bytes returned in output buffer

	UCHAR				ReadBuffer[525];	// 525 = sizeof(DCXPORT_WRITE_INPUT)
	DCXPORT_WRITE_INPUT   *RBuffer;			// pointer to our receive buffer

	RBuffer = (DCXPORT_WRITE_INPUT *)ReadBuffer;

    if ( argc < 2 )
    {
        printf("GPDREAD <ARTI COMMAND MESSAGE>\n");
        printf("                        \n");
        printf("                        Example:\n");
        printf("                        \n");
        printf("                        GPDREAD *033XVU2\n");
        printf("                            or\n");
        printf("                        GPDREAD *033XPI\n");
        exit(1);
    }


    hndFile = CreateFile(
                    "\\\\.\\DcxDev",				// Open the Device "file"
                    GENERIC_READ | GENERIC_WRITE,	// open for r/w
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
                
    if (hndFile == INVALID_HANDLE_VALUE)        // Was the device opened?
    {
        printf("Unable to open the device.\n");
        exit(1);
    }



		WriteBuffer.Addr=33;
		strcpy(WriteBuffer.arChar,argv[1]);
		strcat(WriteBuffer.arChar,"\n");
		WriteBuffer.ulLength = strlen(WriteBuffer.arChar);
		WriteBuffer.PortNumber = 0;

        DataLength = sizeof(DCXPORT_WRITE_INPUT);


/////////////////////////////////////////////////////////////////////////////
// Write the command

    IoctlResult = DeviceIoControl(
                            hndFile,            // Handle to device
                            IOCTL_GPD_WRITE_PORT_BUFFER,          // IO Control code for Write
                            &WriteBuffer,       // Buffer to driver.  Holds port & data.
                            DataLength,         // Length of buffer in bytes.
                            NULL,				// Buffer from driver.
                            0,					// Length of buffer in bytes.
                            &ReturnedLength,    // Bytes placed in DataBuffer.
                            NULL                // NULL means wait till op. completes.
                            );

    if (!IoctlResult)                            // Did the IOCTL succeed?
	{
        printf("Ioctl = %ld failed on write with code %ld\n", IoctlResult,GetLastError() );
		printf("=======================================================================\n");
    }

	Wait(0.10);

////////////////////////////////////////////////////////////////////////
// Read the response 

    IoctlResult = DeviceIoControl(
                            hndFile,            // Handle to device
                            IOCTL_GPD_READ_PORT_BUFFER,          // IO Control code for Read
                            &WriteBuffer,       // Buffer to driver.  Holds port & data.
                            DataLength,         // Length of buffer in bytes.
                            RBuffer,			// Buffer from driver.
                            DataLength,			// Length of buffer in bytes.
                            &ReturnedLength,    // Bytes placed in DataBuffer.
                            NULL                // NULL means wait till op. completes.
                            );

    if (IoctlResult)                            // Did the IOCTL succeed?
    {
		printf("%s \n",RBuffer->arChar);

    }
    else
    {
        printf("Ioctl = %ld failed with code %ld\n", IoctlResult,GetLastError() );
		printf("=======================================================================\n");
    }



////////////////////////////////////////////////////////////////////////////


    if (!CloseHandle(hndFile))                  // Close the Device "file".
    {
        printf("Failed to close device.\n");
    }

    exit(0);
}


void 
Wait(double seconds)
{
	clock_t goal;

	goal = (clock_t)(seconds * (float)CLOCKS_PER_SEC) + clock() + 1;
	while (goal > clock())
		;
}

