	.text

    .global _Setup_GetEntry
    .global _LoadG3P
    .global _Setup_SetEntry
    .global _Bkey_Get_RepeatTime
    .global _Bkey_Set_RepeatTime
    .global _PutKey
    .global _ColorIndexDialog0
    .global _Bfile_GetFileInfo
    .global _MCS_SearchDirectoryItem
    .global _Bdisp_FkeyColor
    .global _ProgressBar1
    .global _Bdisp_WriteDDRegister5A1
    .global _Keyboard_ClrBuffer
    .global _GetGlyphPtr_E5
    .global _GetGBGlyphPtr
    .global _GetGlyphPtr
    .global _GetGlyphPtr_7F
    .global _GetGlyphPtr_E6
    .global _GetGlyphPtr_E7
    .global _GetMiniGlyphPtr_MB
    .global _GetGBMiniGlyphPtr
    .global _GetMiniMiniGlyphPtr_MB
    .global _GetBoldMiniMiniGlyphPtr_MB
    .global _GetVRAMWorkBuffer
    .global _MCS_SearchDirectory
    .global _Bdisp_Fill_DD
    .global _Serial_BufferedTransmitOneByte
    .global _Serial_GetFreeTransmitSpace
    .global _Serial_ClearTransmitBuffer
    .global _Serial_ClearReceiveBuffer
    .global _Serial_ReadOneByte
    .global _Serial_GetReceivedBytesAvailable
    .global _RTC_GetDateTime
    .global _IsCG10
    .global _Bkey_ClrFlag
    .global _OpCodeToStr

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

_GetVRAMWorkBuffer:
    BIOS_JUMP 0x1B0B

_Bdisp_Fill_DD:
    BIOS_JUMP 0x0276

_PutKey:
	BIOS_JUMP 0x0EA9
_OpCodeToStr:
    BIOS_JUMP 0x128F

_Bfile_GetFileInfo:
    BIOS_JUMP 0x1DA7

_ColorIndexDialog0:
    BIOS_JUMP 0x1814

_Bkey_Set_RepeatTime:
    BIOS_JUMP 0x12A4

_Bkey_Get_RepeatTime:
    BIOS_JUMP 0x12A5

_Keyboard_ClrBuffer:
    BIOS_JUMP 0x12c7

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

_GetGlyphPtr_E7:
    BIOS_JUMP 0x0CF0

_GetGlyphPtr_E6:
    BIOS_JUMP 0x0CF1

_GetGlyphPtr_E5:
    BIOS_JUMP 0x0CF2

_GetGlyphPtr_7F:
    BIOS_JUMP 0x0CF3

_ProgressBar1:
    BIOS_JUMP 0x180E

_Bdisp_WriteDDRegister5A1:
    BIOS_JUMP 0x019A

_Bkey_ClrFlag:
    BIOS_JUMP 0x0EA4

_Setup_GetEntry:
    BIOS_JUMP 0x0031
_Setup_SetEntry:
    BIOS_JUMP 0x0032

_IsCG10:
    BIOS_JUMP 0x1196

_MCS_SearchDirectory:
    BIOS_JUMP 0x1514

_MCS_SearchDirectoryItem:
    BIOS_JUMP 0x1519

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

SYSCALL: .long 0x80020070
