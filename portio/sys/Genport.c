// DCX Port I/O driver for NT  VERSION 1.0
//
// Copyright 1997, CorTek Software, Inc.
//


#include "genport.h"
#include "stdlib.h"


#define erTempAllocFail -100
#define erNoInit        -101
#define erAlreadyInit   -102

#define	GSC_ACKERR		  -201
#define	GSC_NOTREADY	  -202
#define	GSC_TIMEOUT		  -203
#define	GRS_ACKERR		  -301
#define	GRS_NOTREADY	  -302
#define	GRS_TIMEOUT		  -303

// DCX Registers

#define DCX_BASE		0x300
#define DCX_CTRLREG		DCX_BASE		// Control Register
#define DCX_STATUSREG	DCX_BASE		// Status Register
#define DCX_XMITFIFO	DCX_BASE + 1	// Transmit Register
#define DCX_RCVFIFO		DCX_BASE + 1	// Receive Register
#define DCX_BOARDID		DCX_BASE + 2	// Board ID Register
#define DCX_BDABUS		DCX_BASE + 3

// Flags 

#define CS_RCVCTRL		0x01
#define CS_XMITCTRL		0x02
#define CS_RESET		0x04
#define CS_USER1		0x08
#define CS_ACKERR		0x10
#define CS_RFEMPTY		0x20
#define CS_OEO			0x40
#define CS_NEWDEVICE	0x80

#define     CS_NOTACKERR    0xffef
// #define     CS_READY        CS_RESET | CS_RCVCTRL | CS_XMITCTRL | CS_ACKERR  // 0x0017

#define     CS_READY        0x0017

#define     CS_XMITCTRL     0x02

//
// Board ID Register Defines
//

#define BI_DCXI_REV1	0xDC



//////////////////////////////////////////////////////////////////////////////
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:
    This routine is the entry point for the driver.  It is responsible
    for setting the dispatch entry points in the driver object and creating
    the device object.  Any resources such as ports, interrupts and DMA
    channels used must be reported.  A symbolic link must be created between
    the device name and an entry in \DosDevices in order to allow Win32
    applications to open the device.

    This routine called when the device is started, ie. NET START DCXPORT 

Arguments:
    
    DriverObject - Pointer to driver object created by the system.

Return Value:

    STATUS_SUCCESS if the driver initialized correctly, otherwise an error
    indicating the reason for failure.

--*/

{
    ULONG PortBase;                 // Port location, in NT's address form.
    ULONG PortCount;                // Count of contiguous I/O ports
    PHYSICAL_ADDRESS PortAddress;

    PLOCAL_DEVICE_INFO pLocalInfo;  // Device extension:
                                    //      local information for each device.
    NTSTATUS Status;
    PDEVICE_OBJECT DeviceObject;

    CM_RESOURCE_LIST ResourceList;  // Resource usage list to report to system
    BOOLEAN ResourceConflict;       // This is set true if our I/O ports
                                    //      conflict with another driver

    // Try to retrieve base I/O port and range from the Parameters key of our
    // entry in the Registry.
    // If there isn't anything specified then use the values compiled into
    // this driver.
    {
        static  WCHAR               SubKeyString[] = L"\\Parameters";
        UNICODE_STRING              paramPath;
        RTL_QUERY_REGISTRY_TABLE    paramTable[3];
        ULONG                       DefaultBase = BASE_PORT;
        ULONG                       DefaultCount = NUMBER_PORTS;

        //
        // Since the registry path parameter is a "counted" UNICODE string, it
        // might not be zero terminated.  For a very short time allocate memory
        // to hold the registry path as well as the Parameters key name zero
        // terminated so that we can use it to delve into the registry.
        //

        paramPath.MaximumLength = RegistryPath->Length + sizeof(SubKeyString);
        paramPath.Buffer = ExAllocatePool(PagedPool, paramPath.MaximumLength);

        if (paramPath.Buffer != NULL)
        {
            RtlMoveMemory(
                paramPath.Buffer, RegistryPath->Buffer, RegistryPath->Length);

            RtlMoveMemory(
                &paramPath.Buffer[RegistryPath->Length / 2], SubKeyString,
                sizeof(SubKeyString));

            paramPath.Length = paramPath.MaximumLength - 2;

            RtlZeroMemory(&paramTable[0], sizeof(paramTable));

            paramTable[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
            paramTable[0].Name = L"IoPortAddress";
            paramTable[0].EntryContext = &PortBase;
            paramTable[0].DefaultType = REG_DWORD;
            paramTable[0].DefaultData = &DefaultBase;
            paramTable[0].DefaultLength = sizeof(ULONG);

            paramTable[1].Flags = RTL_QUERY_REGISTRY_DIRECT;
            paramTable[1].Name = L"IoPortCount";
            paramTable[1].EntryContext = &PortCount;
            paramTable[1].DefaultType = REG_DWORD;
            paramTable[1].DefaultData = &DefaultCount;
            paramTable[1].DefaultLength = sizeof(ULONG);

            if (!NT_SUCCESS(RtlQueryRegistryValues(
                RTL_REGISTRY_ABSOLUTE | RTL_REGISTRY_OPTIONAL,
                paramPath.Buffer, &paramTable[0], NULL, NULL)))
            {
                PortBase = DefaultBase;
                PortCount = DefaultCount;
            }
            ExFreePool(paramPath.Buffer);
        }
    }

    PortAddress.LowPart  = PortBase;
    PortAddress.HighPart = 0;

    // Register resource usage (ports)
    //
    // This ensures that there isn't a conflict between this driver and
    // a previously loaded one or a future loaded one.

    RtlZeroMemory((PVOID)&ResourceList, sizeof(ResourceList));

    ResourceList.Count = 1;
    ResourceList.List[0].InterfaceType = Isa;
    ResourceList.List[0].PartialResourceList.Count = 1;
    ResourceList.List[0].PartialResourceList.PartialDescriptors[0].Type =
                                               CmResourceTypePort;
    ResourceList.List[0].PartialResourceList.PartialDescriptors[0].ShareDisposition =
                                               CmResourceShareDriverExclusive;
    ResourceList.List[0].PartialResourceList.PartialDescriptors[0].Flags =
                                               CM_RESOURCE_PORT_IO;
    ResourceList.List[0].PartialResourceList.PartialDescriptors[0].u.Port.Start =
                                               PortAddress;
    ResourceList.List[0].PartialResourceList.PartialDescriptors[0].u.Port.Length =
                                               PortCount;

    // Report our resource usage and detect conflicts
    Status = IoReportResourceUsage(
                   NULL,
                   DriverObject,
                   &ResourceList,
                   sizeof(ResourceList),
                   NULL,
                   NULL,
                   0,
                   FALSE,
                   &ResourceConflict);

    if (ResourceConflict)
	{
        Status = STATUS_DEVICE_CONFIGURATION_ERROR;
	}

    if (!NT_SUCCESS(Status))
    {
        KdPrint( ("Resource reporting problem %8X", Status) );
        return Status;
    }

    // Initialize the driver object dispatch table.
    // NT sends requests to these routines.

    DriverObject->MajorFunction[IRP_MJ_CREATE]          = GpdDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = GpdDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = GpdDispatch;
    DriverObject->DriverUnload                          = GpdUnload;

    // Create our device.
    Status = GpdCreateDevice(
                    GPD_DEVICE_NAME,
                    GPD_TYPE,
                    DriverObject,
                    &DeviceObject
                    );

    if ( NT_SUCCESS(Status) )
    {
        PHYSICAL_ADDRESS MappedAddress;
        ULONG MemType;

        // Convert the IO port address into a form NT likes.

        MemType = 1;                        // located in IO space

        HalTranslateBusAddress( Isa,
                                0,
                                PortAddress,
                                &MemType,
                                &MappedAddress );


        // Initialize the local driver info for each device object.

        pLocalInfo = (PLOCAL_DEVICE_INFO)DeviceObject->DeviceExtension;

        pLocalInfo->PortWasMapped = FALSE;
        pLocalInfo->PortBase = (PVOID)MappedAddress.LowPart;

        pLocalInfo->DeviceObject    = DeviceObject;
        pLocalInfo->DeviceType      = GPD_TYPE;
        pLocalInfo->PortCount       = PortCount;
        pLocalInfo->PortMemoryType  = MemType;

      }
    else
      {
        //
        // Error creating device - release resources
        //

        RtlZeroMemory((PVOID)&ResourceList, sizeof(ResourceList));
    
        // Unreport our resource usage

        Status = IoReportResourceUsage(
                       NULL,
                       DriverObject,
                       &ResourceList,
                       sizeof(ResourceList),
                       NULL,
                       NULL,
                       0,
                       FALSE,
                       &ResourceConflict);
      }

    return Status;
}


//////////////////////////////////////////////////////////////////////////////
NTSTATUS
GpdCreateDevice(
    IN   PWSTR              PrototypeName,
    IN   DEVICE_TYPE        DeviceType,
    IN   PDRIVER_OBJECT     DriverObject,
    OUT  PDEVICE_OBJECT     *ppDevObj
    )

/*++

Routine Description:
    This routine creates the device object and the symbolic link in
    \DosDevices.
    
    Ideally a name derived from a "Prototype", with a number appended at
    the end should be used.  For simplicity, just use the fixed name defined
    in the include file.  This means that only one device can be created.
    
    A symbolic link must be created between the device name and an entry
    in \DosDevices in order to allow Win32 applications to open the device.

    This routine called when the device is started, ie. NET START DCXPORT 

Arguments:

    PrototypeName - Name base, # WOULD be appended to this.

    DeviceType - Type of device to create

    DriverObject - Pointer to driver object created by the system.

    ppDevObj - Pointer to place to store pointer to created device object

Return Value:

    STATUS_SUCCESS if the device and link are created correctly, otherwise
    an error indicating the reason for failure.

--*/


{
    NTSTATUS Status;                        // Status of utility calls
    UNICODE_STRING NtDeviceName;
    UNICODE_STRING Win32DeviceName;

    // Get UNICODE name for device.

    RtlInitUnicodeString(&NtDeviceName, PrototypeName);

    Status = IoCreateDevice(                             // Create it.
                    DriverObject,
                    sizeof(LOCAL_DEVICE_INFO),
                    &NtDeviceName,
                    DeviceType,
                    0,
                    FALSE,                      // Not Exclusive
                    ppDevObj
                    );

    if (!NT_SUCCESS(Status))
        return Status;             // Give up if create failed.

    // Clear local device info memory

    RtlZeroMemory((*ppDevObj)->DeviceExtension, sizeof(LOCAL_DEVICE_INFO));

    //
    // Set up the rest of the device info
    //  These are used for IRP_MJ_READ and IRP_MJ_WRITE which we don't use
    //    
    //(*ppDevObj)->Flags |= DO_BUFFERED_IO;
    //(*ppDevObj)->AlignmentRequirement = FILE_BYTE_ALIGNMENT;
    //

    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    Status = IoCreateSymbolicLink( &Win32DeviceName, &NtDeviceName );

    if (!NT_SUCCESS(Status))    // If we we couldn't create the link then
    {                           //  abort installation.
        IoDeleteDevice(*ppDevObj);
    }

    return Status;
}

   
//////////////////////////////////////////////////////////////////////////////
NTSTATUS
GpdDispatch(
    IN    PDEVICE_OBJECT pDO,
    IN    PIRP pIrp             
    )

/*++

Routine Description:
    This routine is the dispatch handler for the driver.  It is responsible
    for processing the IRPs.

Arguments:
    
    pDO - Pointer to device object.

    pIrp - Pointer to the current IRP.

Return Value:

    STATUS_SUCCESS if the IRP was processed successfully, otherwise an error
    indicating the reason for failure.

--*/

{
    PLOCAL_DEVICE_INFO pLDI;
    PIO_STACK_LOCATION pIrpStack;
    NTSTATUS Status;


    //  Initialize the irp info field.
    //  This is used to return the number of bytes transfered.

    pIrp->IoStatus.Information = 0;
    pLDI = (PLOCAL_DEVICE_INFO)pDO->DeviceExtension;    // Get local info struct

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

    //  Set default return status

    Status = STATUS_NOT_IMPLEMENTED;

    // Dispatch based on major fcn code.

//		DbgPrint("GpdDispatch %x\n",pIrpStack->Parameters.DeviceIoControl.IoControlCode);


    switch (pIrpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
        case IRP_MJ_CLOSE:


            Status = STATUS_SUCCESS;
            break;

        case IRP_MJ_DEVICE_CONTROL:

            //  Dispatch on IOCTL

            switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
            {
				case IOCTL_GPD_READ_PORT_UCHAR:
				case IOCTL_GPD_READ_PORT_USHORT:
				case IOCTL_GPD_READ_PORT_ULONG:
				case IOCTL_GPD_READ_PORT_BUFFER:

					Status = GpdIoctlReadPort(
                            pLDI,
                            pIrp,
                            pIrpStack,
                            pIrpStack->Parameters.DeviceIoControl.IoControlCode
                            );
                break;

				case IOCTL_GPD_WRITE_PORT_UCHAR:
				case IOCTL_GPD_WRITE_PORT_USHORT:
				case IOCTL_GPD_WRITE_PORT_ULONG:
				case IOCTL_GPD_WRITE_PORT_BUFFER:

					Status = GpdIoctlWritePort(
                            pLDI, 
                            pIrp,
                            pIrpStack,
                            pIrpStack->Parameters.DeviceIoControl.IoControlCode
                            );
                break;
            }
            break;
    }

    // We're done with I/O request.  Record the status of the I/O action.

    pIrp->IoStatus.Status = Status;

    // Don't boost priority when returning since this took little time.

    IoCompleteRequest(pIrp, IO_NO_INCREMENT );

    return Status;
}



//////////////////////////////////////////////////////////////////////////////
NTSTATUS
GpdIoctlReadPort(
    IN PLOCAL_DEVICE_INFO pLDI,
    IN PIRP pIrp,
    IN PIO_STACK_LOCATION IrpStack,
    IN ULONG IoctlCode  )


/*++

Routine Description:
    This routine processes the IOCTLs which read from the ports.

Arguments:
    
    pLDI        - our local device data
    pIrp        - IO request packet
    IrpStack    - The current stack location
    IoctlCode   - The ioctl code from the IRP

Return Value:
    STATUS_SUCCESS           -- OK

    STATUS_INVALID_PARAMETER -- The buffer sent to the driver
                                was too small to contain the
                                port, or the buffer which
                                would be sent back to the driver
                                was not a multiple of the data size.

    STATUS_ACCESS_VIOLATION  -- An illegal port number was given.

--*/

{
                            // NOTE:  Use METHOD_BUFFERED ioctls.
ULONG InBufferSize;         // Amount of data avail. from caller.
ULONG OutBufferSize;        // Max data that caller can accept.
ULONG nPort;                // Port number to read
ULONG DataBufferSize;

DCXPORT_WRITE_INPUT   *pDCX_data;

NTSTATUS  status = STATUS_SUCCESS;

    // Size of buffer containing data from application

    InBufferSize  = IrpStack->Parameters.DeviceIoControl.InputBufferLength;

    // Size of buffer for data to be sent to application

    OutBufferSize = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    // NT copies inbuf here before entry and copies this to outbuf after
    // return, for METHOD_BUFFERED IOCTL's.

    pDCX_data     = (DCXPORT_WRITE_INPUT *)pIrp->AssociatedIrp.SystemBuffer;

    // Check to ensure input buffer is big enough to hold a port number and
    // the output buffer is at least as big as the port data width.
    //

//		DbgPrint("GpdIoctlReadPort \n");

    switch (IoctlCode)
    {
		default:                    // There isn't really any default but 
	/* FALL THRU */         // this will quiet the compiler.
	    case IOCTL_GPD_READ_PORT_UCHAR:
	      DataBufferSize = sizeof(UCHAR);
	    break;

	    case IOCTL_GPD_READ_PORT_USHORT:
		  DataBufferSize = sizeof(USHORT);
		break;

	    case IOCTL_GPD_READ_PORT_ULONG:
		  DataBufferSize = sizeof(ULONG);
		break;

		case IOCTL_GPD_READ_PORT_BUFFER:
		  DataBufferSize = sizeof(UCHAR); // if this is DCXPORT_WRITE_INPUT then it doesnt work : pDCX_data->ulLength;
		break;
    
	}

	if ( InBufferSize < (sizeof(ULONG) + DataBufferSize) )
	{
		return STATUS_INVALID_PARAMETER;
	}


    // Buffers are big enough.

    nPort = pDCX_data->PortNumber;	// Get the I/O port number from the buffer.

    if (nPort >= pLDI->PortCount)
    {
      return STATUS_ACCESS_VIOLATION;   // It was not legal.
	}
    
    // Address is in I/O space - ALWAYS WITH DCX
        
	switch (IoctlCode)
    {
	    case IOCTL_GPD_READ_PORT_UCHAR:
			pDCX_data->CharData = READ_PORT_UCHAR((PUCHAR)((ULONG)pLDI->PortBase + nPort) ); 
		break;

	    case IOCTL_GPD_READ_PORT_USHORT:
			pDCX_data->ShortData = READ_PORT_USHORT((PUSHORT)((ULONG)pLDI->PortBase + nPort) );
	    break;

		case IOCTL_GPD_READ_PORT_ULONG:
			pDCX_data->LongData = READ_PORT_ULONG((PULONG)((ULONG)pLDI->PortBase + nPort) );
	    break;

		case IOCTL_GPD_READ_PORT_BUFFER:
			status = DCXIoctlReadBuffer( (ULONG)pLDI->PortMemoryType , (PUCHAR)pDCX_data, (ULONG)InBufferSize, (ULONG)IoctlCode);
		break;

    }
  
    // Indicate # of bytes read
    //
    
    pIrp->IoStatus.Information = sizeof(DCXPORT_WRITE_INPUT);//DataBufferSize;
    //pIrp->IoStatus.Information = pDCX_data->ulLength;	//DataBufferSize;

    return status;

}


//////////////////////////////////////////////////////////////////////////////
NTSTATUS
GpdIoctlWritePort(
    IN PLOCAL_DEVICE_INFO pLDI, 
    IN PIRP pIrp, 
    IN PIO_STACK_LOCATION IrpStack,
    IN ULONG IoctlCode
    )

/*++

Routine Description:
    This routine processes the IOCTLs which write to the ports.

Arguments:
    
    pLDI        - our local device data
    pIrp        - IO request packet
    IrpStack    - The current stack location
    IoctlCode   - The ioctl code from the IRP

Return Value:
    STATUS_SUCCESS           -- OK

    STATUS_INVALID_PARAMETER -- The buffer sent to the driver
                                was too small to contain the
                                port, or the buffer which
                                would be sent back to the driver
                                was not a multiple of the data size.

    STATUS_ACCESS_VIOLATION  -- An illegal port number was given.

--*/

{
                                // NOTE:  Use METHOD_BUFFERED ioctls.
                                //      (treated as array of longs).
ULONG     InBufferSize ;        // Amount of data avail. from caller.
ULONG     OutBufferSize ;       // Max data that caller can accept.
ULONG     nPort;                // Port number to read or write.
ULONG     DataBufferSize;
NTSTATUS  status = STATUS_SUCCESS;

DCXPORT_WRITE_INPUT   *pDCX_data;

// Size of buffer containing data from application

	InBufferSize  = IrpStack->Parameters.DeviceIoControl.InputBufferLength;

// Size of buffer for data to be sent to application

	OutBufferSize = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

// NT copies inbuf here before entry and copies this to outbuf after return,
// for METHOD_BUFFERED IOCTL's.

	pDCX_data     = (DCXPORT_WRITE_INPUT *)pIrp->AssociatedIrp.SystemBuffer;

//	DbgPrint("GpdIoctlWritePort \n");

// We don't return any data on a write port.

	pIrp->IoStatus.Information = 0;

// Check to ensure input buffer is big enough to hold a port number as well
// as the data to write.
//
// The relative port # is a ULONG, and the data is the type appropriate to
// the IOCTL.
//

	switch (IoctlCode)
	{
		default:                    // There isn't really any default but 
/* FALL THRU */         // this will quiet the compiler.
		case IOCTL_GPD_WRITE_PORT_UCHAR:
			DataBufferSize = sizeof(UCHAR);
		break;
		case IOCTL_GPD_WRITE_PORT_USHORT:
			DataBufferSize = sizeof(USHORT);
		break;
		case IOCTL_GPD_WRITE_PORT_ULONG:
			DataBufferSize = sizeof(ULONG);
		break;
		case IOCTL_GPD_WRITE_PORT_BUFFER:
			DataBufferSize = sizeof(UCHAR); // // if this is DCXPORT_WRITE_INPUT then it doesnt work : pDCX_data->ulLength;
		break;
	}

	if ( InBufferSize < (sizeof(ULONG) + DataBufferSize) )
	{
		return STATUS_INVALID_PARAMETER;
	}

	nPort = pDCX_data->PortNumber;

	if (nPort >= pLDI->PortCount )
	{
		return STATUS_ACCESS_VIOLATION;   // Illegal port number
	}

// Address is in I/O space
  
		switch (IoctlCode)
		{
		    case IOCTL_GPD_WRITE_PORT_UCHAR:
				WRITE_PORT_UCHAR( (PUCHAR)((ULONG)pLDI->PortBase + nPort),
                          pDCX_data->CharData);
			break;

			case IOCTL_GPD_WRITE_PORT_USHORT:
				WRITE_PORT_USHORT( (PUSHORT)((ULONG)pLDI->PortBase + nPort),
                           pDCX_data->ShortData);
			break;

			case IOCTL_GPD_WRITE_PORT_ULONG:
				WRITE_PORT_ULONG( (PULONG)((ULONG)pLDI->PortBase + nPort),
                          pDCX_data->LongData);
	        break;

			case IOCTL_GPD_WRITE_PORT_BUFFER:
				status = DCXIoctlWriteBuffer( (ULONG)pLDI->PortMemoryType , (PUCHAR)pDCX_data, (ULONG)InBufferSize, (ULONG)IoctlCode);
			break;
		}
return status;
}



//////////////////////////////////////////////////////////////////////////////////
//FUNCTION:   Write a buffer to the DCX port 
//				This is the main routine for writing to the ARTI bus
//
//////////////////////////////////////////////////////////////////////////////
NTSTATUS
DCXIoctlWriteBuffer(
    IN ULONG  MemoryType,	// DON'T NEED TO PASS THIS SINCE ALWAYS 1
    IN PUCHAR pBuffer,
    IN ULONG  BufferSize,
    IN ULONG  IoctlCode
    )
{

DCXPORT_WRITE_INPUT   *pDCXBuffer;

UCHAR                 uchar;
ULONG                 ulPort;

pDCXBuffer = (DCXPORT_WRITE_INPUT *)pBuffer;

//		DbgPrint("DCXIoctlWriteBuffer \n");


  // Address is in I/O space
  // clear ack error bit
  //--------------------

	  ulPort = DCX_CTRLREG;
	  uchar = READ_PORT_UCHAR( (PUCHAR)ulPort );
	  uchar = uchar & (UCHAR) CS_NOTACKERR;

	  WRITE_PORT_UCHAR( (PUCHAR)ulPort , uchar );


  // make sure we are ready
  //-----------------------

	  uchar = READ_PORT_UCHAR( (PUCHAR)ulPort );
	  uchar = uchar & (UCHAR) CS_READY;
	  if(uchar)
	  {
//		  DbgPrint("WriteBuffer GSC_NOTREADY");
	      return GSC_NOTREADY;
	  }

	  ulPort = DCX_XMITFIFO;

	  uchar = (UCHAR)pDCXBuffer->Addr;			// DCX Module Board Address
	  WRITE_PORT_UCHAR( (PUCHAR)ulPort , uchar);

	  WRITE_PORT_BUFFER_UCHAR( (PUCHAR)ulPort , (PUCHAR)pDCXBuffer->arChar, (ULONG)pDCXBuffer->ulLength);

  // send the packet
  //----------------

	  ulPort = DCX_CTRLREG;
	  uchar = READ_PORT_UCHAR( (PUCHAR)ulPort );
	  uchar = uchar | (UCHAR) CS_XMITCTRL;
	  WRITE_PORT_UCHAR( (PUCHAR)ulPort , uchar);

return STATUS_SUCCESS;
};


//////////////////////////////////////////////////////////////////////////////////
//FUNCTION:   Read a buffer from the DCX port 
//				This is the main routine for reading from the ARTI bus
//
//
//////////////////////////////////////////////////////////////////////////////
NTSTATUS
DCXIoctlReadBuffer(
    IN ULONG  MemoryType, 	// DON'T NEED TO PASS THIS SINCE ALWAYS 1
    IN PUCHAR pBuffer,
    IN ULONG  BufferSize,
    IN ULONG  IoctlCode
    )
{

DCXPORT_WRITE_INPUT   *pDCXBuffer;

UCHAR                 uchar;
LARGE_INTEGER         liTimeStart;
LARGE_INTEGER         liTimeEnd;
ULONG                 ulCount;

LARGE_INTEGER   wait35ms;

// Set time out value for 35 ms 
//------------------------------

    wait35ms.QuadPart = (35*10*1000) + KeQueryTimeIncrement();

	pDCXBuffer = (DCXPORT_WRITE_INPUT *)pBuffer;

//		DbgPrint("DCXIoctlReadBuffer \n");

// Address is in I/O space
// clear ack error bit
//-------------------------

		uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
		uchar = uchar & (UCHAR) ~CS_ACKERR;

		WRITE_PORT_UCHAR( (PUCHAR)DCX_CTRLREG , uchar );


// make sure we are ready, if not the reset the DMX bus
//-----------------------------------------------------

		uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
		uchar = uchar & (UCHAR) CS_READY;

		if(uchar)
		{
//			DbgPrint("ReadBuffer GSC_NOTREADY \n");
			gBusReset();
			return GSC_NOTREADY;
		}
 
// Load target address into transmit FIFO 
//----------------------------------------

		uchar = (UCHAR)pDCXBuffer->Addr;			// DCX Module Board Address
		WRITE_PORT_UCHAR( (PUCHAR)DCX_XMITFIFO, uchar);


//Start Receive state machine 
//----------------------------

		uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
		uchar = uchar | (UCHAR) CS_RCVCTRL;
		WRITE_PORT_UCHAR( (PUCHAR)DCX_CTRLREG, uchar);
 
		KeQueryTickCount( (PLARGE_INTEGER)&liTimeStart);

		for(;;)
		{
// Are we done yet? Wait for recive control to go low 
//----------------------------------------------------

				uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
				if( (uchar & (UCHAR)CS_RCVCTRL) == 0x00)
				{
					break;
				}
 
// Check for timeout
//------------------

			KeQueryTickCount( (PLARGE_INTEGER)&liTimeEnd);

// correct for overspill ....
//-----------------------------

			if(liTimeEnd.QuadPart < liTimeStart.QuadPart) 
				liTimeStart.QuadPart = liTimeEnd.QuadPart;

			if( (ULONG)((liTimeEnd.QuadPart - liTimeStart.QuadPart)*KeQueryTimeIncrement() >
					wait35ms.QuadPart))
			{
				return(GRS_TIMEOUT);
			}
		}


// check for error
//----------------

		uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
		uchar = uchar & (UCHAR) CS_ACKERR;

		if(uchar)
		{
//			DbgPrint("ReadBuffer GSC_ACKERR \n");
			return GRS_ACKERR;
		}

// Move receive FIFO data (if any) to system buffer
// System will copy this buffer back to the user buffer		
//------------------------------------------------------
		ulCount = 0;
		uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );

		while ( ((uchar & (UCHAR)CS_RFEMPTY) == 0) && (ulCount < MAX_DCX_MESSAGE) )
//		while ( ((uchar & (UCHAR)CS_RFEMPTY) == 0) && (ulCount < BufferSize) )
	    {
			pDCXBuffer->arChar[ulCount++] = READ_PORT_UCHAR( (PUCHAR)DCX_RCVFIFO );
			uchar = READ_PORT_UCHAR( (PUCHAR)DCX_CTRLREG );
	    }
  
		pDCXBuffer->ulLength = ulCount;
//		DbgPrint("Buffer read = %s\n",pDCXBuffer->arChar);


return STATUS_SUCCESS;

}


//////////////////////////////////////////////////////////////////////
// FUNCTION: Rest the DCX bus
//

void
gBusReset(void)
{
LARGE_INTEGER   wait10ms;

    wait10ms.QuadPart = (10*10*1000) + KeQueryTimeIncrement();
	
	WRITE_PORT_UCHAR( (PUCHAR)DCX_CTRLREG, CS_RESET);
    KeDelayExecutionThread(KernelMode,
                           FALSE,
                           &wait10ms);
	WRITE_PORT_UCHAR( (PUCHAR)DCX_CTRLREG, 0x00);

}


//////////////////////////////////////////////////////////////////////////////
VOID
GpdUnload(
    PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:
    This routine prepares our driver to be unloaded.  It is responsible
    for freeing all resources allocated by DriverEntry as well as any 
    allocated while the driver was running.  The symbolic link must be
    deleted as well.

Arguments:
    
    DriverObject - Pointer to driver object created by the system.

Return Value:

    None

--*/

{
    PLOCAL_DEVICE_INFO pLDI;
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;
    UNICODE_STRING Win32DeviceName;

    // Find our global data

    pLDI = (PLOCAL_DEVICE_INFO)DriverObject->DeviceObject->DeviceExtension;

    // Unmap the ports

    if (pLDI->PortWasMapped)
    {
        MmUnmapIoSpace(pLDI->PortBase, pLDI->PortCount);
    }

    // Report we're not using any hardware.  If we don't do this
    // then we'll conflict with ourselves (!) on the next load

    RtlZeroMemory((PVOID)&NullResourceList, sizeof(NullResourceList));

    IoReportResourceUsage(
              NULL,
              DriverObject,
              &NullResourceList,
              sizeof(ULONG),
              NULL,
              NULL,
              0,
              FALSE,
              &ResourceConflict );

    // Assume all handles are closed down.
    // Delete the things we allocated - devices, symbolic links

    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    IoDeleteSymbolicLink(&Win32DeviceName);

    IoDeleteDevice(pLDI->DeviceObject);
}

