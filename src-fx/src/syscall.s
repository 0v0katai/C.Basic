    .text

    .global _RTC_GetTicks
    .global _RTC_SetDateTime
    .global _PutKey
    .global _Keyboard_ClrBuffer
    .global _Keyboard_GetKeyWait
    .global _EnableGetkeyToMainFunctionReturn
    .global _DisableGetkeyToMainFunctionReturn
    .global _CatalogDialog
    .global _HourGlass
    .global _Cursor_SetPosition
    .global _Cursor_SetFlashStyle
    .global _Cursor_SetFlashMode
    .global _Cursor_GetSettings
    .global _Cursor_GetFlashStyle
    .global _Cursor_SetFlashOn
    .global _Cursor_SetFlashOff
    .global _OpcodeToStr
    .global _MB_IsLead
    .global _MB_ElementCount
    .global _MB_ByteCount
    .global _MB_strcat
    .global _MB_strncat
    .global _MB_strcpy
    .global _MB_GetSecondElemPtr
    .global _MB_GetElement
    .global _MB_CopyToHeap
    .global _MCS_CreateDirectory
    .global _MCS_WriteItem
    .global _MCS_DeleteDirectory
    .global _MCS_DeleteItem
    .global _MCS_GetState
    .global _MCS_GetSystemDirectoryInfo
    .global _MCS_RenameItem
    .global _MCS_OverwriteData
    .global _MCS_GetItemData
    .global _MCS_RenameDirectory
    .global _MCS_SearchDirectory
    .global _MCS_SearchDirectoryItem
    .global _MCS_GetFirstDataPointerByDirno
    .global _MCS_GetDirectoryEntryByNumber
    .global _MCS_SearchItem
    .global _MCS_str8cpy
    .global _MCS_GetDirectoryEntryAddress
    .global _MCS_GetCurrentBottomAddress
    .global _MCS_GetCapa
    .global _MCS_GetMainMemoryStart
    .global _Serial_Open2
    .global _Serial_Open_57600
    .global _Serial_ResetAndDisable
    .global _Serial_GetInterruptHandler
    .global _Serial_SetInterruptHandler
    .global _Serial_ReadOneByte
    .global _Serial_ReadNBytes
    .global _Serial_BufferedTransmitOneByte
    .global _Serial_BufferedTransmitNBytes
    .global _Serial_DirectTransmitOneByte
    .global _Serial_GetReceivedBytesAvailable
    .global _Serial_GetFreeTransmitSpace
    .global _Serial_ClearReceiveBuffer
    .global _Serial_ClearTransmitBuffer
    .global _Serial_Open
    .global _Serial_Close
    .global _Serial_CallReceiveIntErrorResetHandler
    .global _Serial_CallReceiveIntHandler
    .global _Serial_CallTransmitIntErrorResetHandler
    .global _Serial_CallTransmitIntHandler
    .global _Serial_SpyNthByte
    .global _Serial_GetStatus
    .global _Serial_IsOpen
    .global _Bfile_RenameEntry
    .global _GetVRAMAddress
    .global _Bdisp_ShapeToVRAM
    .global _GetFKeyIconPointer
    .global _DisplayFKeyIcon
    .global _GUI_ProgressBar
    .global _DD_SetContrast
    .global _System_GetOSVersion
    .global _Calljmp
    .global _SysCalljmp
    .global _SysCalljmp12

    .macro BIOS_JUMP funo
    mov.l SYSCALL,r1
    mov.l 1f,r0
    jmp @r1
    nop
    1: .long \funo
    .endm

    .balign 4

_RTC_GetTicks:
    BIOS_JUMP 0x03B
_RTC_SetDateTime:
    BIOS_JUMP 0x23E
_PutKey:
    BIOS_JUMP 0x910
_Keyboard_ClrBuffer:
    BIOS_JUMP 0x241
_Keyboard_GetKeyWait:
    BIOS_JUMP 0x247
_EnableGetkeyToMainFunctionReturn:
    BIOS_JUMP 0x477
_DisableGetkeyToMainFunctionReturn:
    BIOS_JUMP 0x478
_CatalogDialog:
	BIOS_JUMP 0xAE8
_HourGlass:
    BIOS_JUMP 0x4E6
_Cursor_SetPosition:
    BIOS_JUMP 0x138
_Cursor_SetFlashStyle:
    BIOS_JUMP 0x139
_Cursor_SetFlashMode:
    BIOS_JUMP 0x13A
_Cursor_GetSettings:
    BIOS_JUMP 0x13B
_Cursor_GetFlashStyle:
    BIOS_JUMP 0x80E
_Cursor_SetFlashOn:
    BIOS_JUMP 0x811
_Cursor_SetFlashOff:
    BIOS_JUMP 0x812
_OpcodeToStr:
    BIOS_JUMP 0x07FC
_MB_IsLead:
    BIOS_JUMP 0x531
_MB_ElementCount:
    BIOS_JUMP 0x533
_MB_ByteCount:
    BIOS_JUMP 0x534
_MB_strcat:
    BIOS_JUMP 0x536
_MB_strncat:
    BIOS_JUMP 0x537
_MB_strcpy:
    BIOS_JUMP 0x538
_MB_GetSecondElemPtr:
    BIOS_JUMP 0x53C
_MB_GetElement:
    BIOS_JUMP 0x53D
_MB_CopyToHeap:
    BIOS_JUMP 0x53E
_MCS_CreateDirectory:
    BIOS_JUMP 0x363
_MCS_WriteItem:
    BIOS_JUMP 0x364
_MCS_DeleteDirectory:
    BIOS_JUMP 0x366
_MCS_DeleteItem:
    BIOS_JUMP 0x367
_MCS_GetState:
    BIOS_JUMP 0x368
_MCS_GetSystemDirectoryInfo:
    BIOS_JUMP 0x369
_MCS_RenameItem:
    BIOS_JUMP 0x370
_MCS_OverwriteData:
    BIOS_JUMP 0x371
_MCS_GetItemData:
    BIOS_JUMP 0x372
_MCS_RenameDirectory:
    BIOS_JUMP 0x373
_MCS_SearchDirectory:
    BIOS_JUMP 0x375
_MCS_SearchDirectoryItem:
    BIOS_JUMP 0x376
_MCS_GetFirstDataPointerByDirno:
    BIOS_JUMP 0x37C
_MCS_GetDirectoryEntryByNumber:
    BIOS_JUMP 0x37D
_MCS_SearchItem:
    BIOS_JUMP 0x37E
_MCS_str8cpy:
    BIOS_JUMP 0x37F
_MCS_GetDirectoryEntryAddress:
    BIOS_JUMP 0x380
_MCS_GetCurrentBottomAddress:
    BIOS_JUMP 0x381
_MCS_GetCapa:
    BIOS_JUMP 0x383
_MCS_GetMainMemoryStart:
    BIOS_JUMP 0x392
_Bfile_RenameEntry:
    BIOS_JUMP 0x438
_GetVRAMAddress:
    BIOS_JUMP 0x135
_Bdisp_ShapeToVRAM:
    BIOS_JUMP 0x02F
_Bfile_GetMediaFree_OS:
    BIOS_JUMP 0x42E
_GetFKeyIconPointer:
    BIOS_JUMP 0x268
_DisplayFKeyIcon:
    BIOS_JUMP 0x4D1
_GUI_ProgressBar:
    BIOS_JUMP 0x1A9
_DD_SetContrast:
    BIOS_JUMP 0x132
_Serial_Open2:
    BIOS_JUMP 0x02AB
_Serial_Open_57600:
    BIOS_JUMP 0x0285
_Serial_ResetAndDisable:
    BIOS_JUMP 0x0409
_Serial_GetInterruptHandler:
    BIOS_JUMP 0x040A
_Serial_SetInterruptHandler:
    BIOS_JUMP 0x040B
_Serial_ReadOneByte:
    BIOS_JUMP 0x040C
_Serial_ReadNBytes:
    BIOS_JUMP 0x040D
_Serial_BufferedTransmitOneByte:
    BIOS_JUMP 0x040E
_Serial_BufferedTransmitNBytes:
    BIOS_JUMP 0x040F
_Serial_DirectTransmitOneByte:
    BIOS_JUMP 0x0410
_Serial_GetReceivedBytesAvailable:
    BIOS_JUMP 0x0411
_Serial_GetFreeTransmitSpace:
    BIOS_JUMP 0x0412
_Serial_ClearReceiveBuffer:
    BIOS_JUMP 0x0413
_Serial_ClearTransmitBuffer:
    BIOS_JUMP 0x0414
_Serial_Open:
    BIOS_JUMP 0x0418
_Serial_Close:
    BIOS_JUMP 0x0419
_Serial_CallReceiveIntErrorResetHandler:
    BIOS_JUMP 0x041B
_Serial_CallReceiveIntHandler:
    BIOS_JUMP 0x041C
_Serial_CallTransmitIntErrorResetHandler:
    BIOS_JUMP 0x041D
_Serial_CallTransmitIntHandler:
    BIOS_JUMP 0x041E
_Serial_SpyNthByte:
    BIOS_JUMP 0x0422
_Serial_GetStatus:
    BIOS_JUMP 0x0423
_Serial_IsOpen:
    BIOS_JUMP 0x0425
_System_GetOSVersion:
    BIOS_JUMP 0x02EE

_Calljmp:
    mov.l	@r15,r1
    jmp	@r1
    nop

_SysCalljmp:
    mov.l	SYSCALL,r1
    mov.l	@r15,r0
    jmp	@r1
    nop

_SysCalljmp12:
    mov.l	@(32,R15),r0
    mov.l	SYSCALL,r1
    jmp	@r1
    nop

    .balign 4

SYSCALL: .long 0x80010070