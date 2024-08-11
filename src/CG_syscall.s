	.text
    .global _GetVRAMAddress
    .global _GetVRAMWorkBuffer
    .global _Bdisp_Fill_DD
	.global _Bfile_RenameEntry
    .global _Cursor_SetPosition
    .global _PutKey
	.global _OpcodeToStr
    .global _Bfile_GetFileInfo
    .global _Cursor_GetSettings
    .global _ColorIndexDialog0
    .global _Bkey_Set_RepeatTime
    .global _Bkey_Get_RepeatTime
    .global _Keyboard_ClrBuffer
    .global _Bfile_GetMediaFree_OS
    .global _RTC_GetDateTime
    .global _GetMiniGlyphPtr_MB
    .global _GetMiniMiniGlyphPtr_MB
    .global _GetBoldMiniMiniGlyphPtr_MB
    .global _GetGBGlyphPtr
    .global _GetGBMiniGlyphPtr
    .global _GetGlyphPtr
    .global _GetGlyphPtr_F9
    .global _GetGlyphPtr_E7
    .global _GetGlyphPtr_E6
    .global _GetGlyphPtr_E5
    .global _GetGlyphPtr_7F
    .global _GetGlyphPtr_F7
    .global _GetMiniGlyphPtr_E7
    .global _ProgressBar0
    .global _ProgressBar1
    .global _ProgressBar2
#    .global _GetLightLevel
#    .global _DecLightLevel
#    .global _IncLightLevel
#    .global _ClrLightLevel
#    .global _Bdisp_SetBacklightLevel
    .global _Bdisp_WriteDDRegister5A1
    .global _Bdisp_DDRegisterSelect
    .global _Bkey_SetFlag
    .global _Bkey_ClrFlag
    .global _Setup_GetEntry
    .global _Setup_SetEntry
    .global _IsCG10
    .global _Bdisp_SetDDRegisterB
#    .global _Bdisp_IsZeroDDRegisterB
    .global _MCS_SearchDirectory
	.global _MCS_CreateDirectory
 	.global _MCS_DeleteDirectory
    .global _MCS_GetSystemDirectoryInfo
    .global _MCS_SearchDirectoryItem
	.global _MCS_WriteItem
    .global _MCS_DeleteItem
    .global _MCS_GetState
    .global _Serial_GetFreeTransmitSpace
    .global _Serial_BufferedTransmitOneByte
    .global _Serial_ReadOneByte
    .global _Serial_GetReceivedBytesAvailable
    .global _Serial_ClearReceiveBuffer
    .global _Serial_ClearTransmitBuffer
    .global _SaveG3P
    .global _LoadG3P
    .global _Bdisp_FkeyColor
#    .global _OpenFileDialog
    .global _Calljmp
    .global _SysCalljmp
    .global _SysCalljmp12
    
	.balign 4

	.macro BIOS_JUMP funo
	mov.l 2f,r1
	mov.l 1f,r0
	jmp @r1
	nop
	1: .long \funo
	2: .long 0x80020070
	.endm
	.balign 4

_GetVRAMAddress:
    BIOS_JUMP 0x01E6

_GetVRAMWorkBuffer:
    BIOS_JUMP 0x1B0B

_Bdisp_Fill_DD:
    BIOS_JUMP 0x0276

_Bfile_RenameEntry:
    BIOS_JUMP 0x1DB3

_Cursor_SetPosition:
    BIOS_JUMP 0x01F1

_PutKey:
	BIOS_JUMP 0x0EA9
_OpcodeToStr:
    BIOS_JUMP 0x128F

_Bfile_GetFileInfo:
    BIOS_JUMP 0x1DA7

_Cursor_GetSettings:
    BIOS_JUMP 0x01F5


_ColorIndexDialog0:
    BIOS_JUMP 0x1814


_Bkey_Set_RepeatTime:
    BIOS_JUMP 0x12A4

_Bkey_Get_RepeatTime:
    BIOS_JUMP 0x12A5

_Keyboard_ClrBuffer:
    BIOS_JUMP 0x12c7

_Bfile_GetMediaFree_OS:
    BIOS_JUMP 0x1DA5

_RTC_GetDateTime:
    BIOS_JUMP 0x11E2

_GetMiniGlyphPtr_MB:
    BIOS_JUMP 0x01E9

_GetMiniMiniGlyphPtr_MB:
    BIOS_JUMP 0x01EA

_GetBoldMiniMiniGlyphPtr_MB:
    BIOS_JUMP 0x01EB

_GetGBGlyphPtr:
    BIOS_JUMP 0x01EC

_GetGBMiniGlyphPtr:
    BIOS_JUMP 0x01ED

_GetGlyphPtr:
    BIOS_JUMP 0x0CEE

_GetGlyphPtr_F9:
    BIOS_JUMP 0x0CEF

_GetGlyphPtr_E7:
    BIOS_JUMP 0x0CF0

_GetGlyphPtr_E6:
    BIOS_JUMP 0x0CF1

_GetGlyphPtr_E5:
    BIOS_JUMP 0x0CF2

_GetGlyphPtr_7F:
    BIOS_JUMP 0x0CF3

_GetGlyphPtr_F7:
    BIOS_JUMP 0x0CF4

_GetMiniGlyphPtr_E7:
    BIOS_JUMP 0x0CFB

_ProgressBar0:
    BIOS_JUMP 0x180B
_ProgressBar1:
    BIOS_JUMP 0x180E
_ProgressBar2:
    BIOS_JUMP 0x1809

#_GetLightLevel:
#	BIOS_JUMP 0x1E8F

#_DecLightLevel:
#	BIOS_JUMP 0x119A

#_IncLightLevel:
#	BIOS_JUMP 0x119B

#_ClrLightLevel:
#	BIOS_JUMP 0x119C
#_Bdisp_SetBacklightLevel:
#	BIOS_JUMP 0x0199
_Bdisp_WriteDDRegister5A1:
    BIOS_JUMP 0x019A
_Bdisp_DDRegisterSelect:
    BIOS_JUMP 0x01A2

_Bkey_SetFlag:
    BIOS_JUMP 0x0EA2

_Bkey_ClrFlag:
    BIOS_JUMP 0x0EA4

_Setup_GetEntry:
    BIOS_JUMP 0x0031
_Setup_SetEntry:
    BIOS_JUMP 0x0032

_IsCG10:
    BIOS_JUMP 0x1196


_Bdisp_SetDDRegisterB:
    BIOS_JUMP 0x0194

_Bdisp_IsZeroDDRegisterB:
    BIOS_JUMP 0x0195


_MCS_SearchDirectory:
    BIOS_JUMP 0x1514

_MCS_CreateDirectory:
	BIOS_JUMP 0x1515

_MCS_DeleteDirectory:
	BIOS_JUMP 0x1516

_MCS_GetSystemDirectoryInfo:
    BIOS_JUMP 0x1518
_MCS_SearchDirectoryItem:
    BIOS_JUMP 0x1519

_MCS_WriteItem:
    BIOS_JUMP 0x151A

_MCS_DeleteItem:
    BIOS_JUMP 0x151C
_MCS_GetState:
    BIOS_JUMP 0x1529

_SaveG3P:
    BIOS_JUMP 0x1009
_LoadG3P:
    BIOS_JUMP 0x100F
_Bdisp_FkeyColor:
    BIOS_JUMP 0x0130

_Serial_GetFreeTransmitSpace:
    BIOS_JUMP 0x1bc0
_Serial_BufferedTransmitOneByte:
    BIOS_JUMP 0x1bbc
_Serial_ReadOneByte:
    BIOS_JUMP 0x1bb9
_Serial_GetReceivedBytesAvailable:
    BIOS_JUMP 0x1bbf
_Serial_ClearReceiveBuffer:
    BIOS_JUMP 0x1bc1
_Serial_ClearTransmitBuffer:
    BIOS_JUMP 0x1bc2

#_OpenFileDialog:
#	BIOS_JUMP 0x17E9

# int Calljmp( int r4, int r5, int r6, int r7, int adrs );

_Calljmp:
    mov.l	@r15,r1
    jmp	@r1
    nop

# int SysCalljmp( int r4, int r5, int r6, int r7, int No);

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

    .align 4

SYSCALL: .long 0x80020070
LCDC:    .long 0xB4000000
