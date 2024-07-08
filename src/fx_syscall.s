# refer to siomn's fxDocumentation

.text
 .global _RTC_GetTicks
 .global _RTC_SetDateTime
 .global _PutKey
 .global _Keyboard_ClrBuffer
 .global _Keyboard_GetKeyWait
 .global _EnableGetkeyToMainFunctionReturn
 .global _DisableGetkeyToMainFunctionReturn
# .global _CatalogDialog
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

 .ALIGN	4

_RTC_GetTicks:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x03B

_RTC_SetDateTime:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x23E

_PutKey:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x910

_Keyboard_ClrBuffer:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x241

_Keyboard_GetKeyWait:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x247

_EnableGetkeyToMainFunctionReturn:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x477

_DisableGetkeyToMainFunctionReturn:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x478

# _CatalogDialog:
#	mov.l SYSCALL,r1
#	mov.l 1f,r0
#	jmp @r1
#	nop
# 1: .long 0xAE8

_HourGlass:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x4E6

_Cursor_SetPosition:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x138

_Cursor_SetFlashStyle:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x139

_Cursor_SetFlashMode:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x13A

_Cursor_GetSettings:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x13B

_Cursor_GetFlashStyle:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x80E

_Cursor_SetFlashOn:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x811

_Cursor_SetFlashOff:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x812


_OpcodeToStr:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x07FC

_MB_IsLead:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x531

_MB_ElementCount:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x533

_MB_ByteCount:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x534

_MB_strcat:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x536

_MB_strncat:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x537

_MB_strcpy:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x538

_MB_GetSecondElemPtr:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x53C

_MB_GetElement:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x53D

_MB_CopyToHeap:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x53E


_MCS_CreateDirectory:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x363
_MCS_WriteItem:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x364
_MCS_DeleteDirectory:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x366
_MCS_DeleteItem:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x367
_MCS_GetState:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x368
_MCS_GetSystemDirectoryInfo:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x369
_MCS_RenameItem:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x370
_MCS_OverwriteData:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x371
_MCS_GetItemData:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x372
_MCS_RenameDirectory:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x373
_MCS_SearchDirectory:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x375
_MCS_SearchDirectoryItem:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x376
_MCS_GetFirstDataPointerByDirno:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x37C
_MCS_GetDirectoryEntryByNumber:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x37D
_MCS_SearchItem:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x37E
_MCS_str8cpy:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x37F
_MCS_GetDirectoryEntryAddress:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x380
_MCS_GetCurrentBottomAddress:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x381
_MCS_GetCapa:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x383
_MCS_GetMainMemoryStart:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x392
_Bfile_RenameEntry:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x438

_GetVRAMAddress:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x135

_Bdisp_ShapeToVRAM:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x02F

_GetFKeyIconPointer:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x268

_DisplayFKeyIcon:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x4D1

_GUI_ProgressBar:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x1A9

_DD_SetContrast:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x132


_Serial_Open2:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x02AB
_Serial_Open_57600:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0285
_Serial_ResetAndDisable:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0409
_Serial_GetInterruptHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040A
_Serial_SetInterruptHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040B
_Serial_ReadOneByte:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040C
_Serial_ReadNBytes:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040D
_Serial_BufferedTransmitOneByte:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040E

_Serial_BufferedTransmitNBytes:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x040F

_Serial_DirectTransmitOneByte:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0410

_Serial_GetReceivedBytesAvailable:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0411

_Serial_GetFreeTransmitSpace:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0412

_Serial_ClearReceiveBuffer:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0413

_Serial_ClearTransmitBuffer:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0414

_Serial_Open:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0418

_Serial_Close:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0419

_Serial_CallReceiveIntErrorResetHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x041B

_Serial_CallReceiveIntHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x041C

_Serial_CallTransmitIntErrorResetHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x041D

_Serial_CallTransmitIntHandler:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x041E

_Serial_SpyNthByte:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0422

_Serial_GetStatus:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0423

_Serial_IsOpen:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x0425

_System_GetOSVersion:
	mov.l SYSCALL,r1
	mov.l 1f,r0
	jmp @r1
	nop
1: .long 0x02EE


# int Calljmp( int r4, int r5, int r6, int r7, int adrs )

_Calljmp:
	mov.l	@r15,r1
	jmp	@r1
	nop

# int SysCalljmp( int r4, int r5, int r6, int r7, int No)

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



 .ALIGN	4
SYSCALL: .long 0x80010070
