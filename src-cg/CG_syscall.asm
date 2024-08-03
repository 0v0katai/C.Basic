; refer to siomn's fxDocumentation

 .SECTION P,CODE

 .EXPORT _GetVRAMAddress
 .EXPORT _GetVRAMWorkBuffer
 .EXPORT _Bdisp_Fill_DD
 .EXPORT _Bfile_RenameEntry
 .EXPORT _Cursor_SetPosition
 .EXPORT _PutKey
 .EXPORT _OpcodeToStr
 .EXPORT _Bfile_GetFileInfo
 .EXPORT _Cursor_GetSettings
 .EXPORT _ColorIndexDialog0
 .EXPORT _Bkey_Set_RepeatTime
 .EXPORT _Bkey_Get_RepeatTime
 .EXPORT _Keyboard_ClrBuffer
 .EXPORT _Bfile_GetMediaFree_OS
 .EXPORT _RTC_GetDateTime
 .EXPORT _GetMiniGlyphPtr_MB
 .EXPORT _GetMiniMiniGlyphPtr_MB
 .EXPORT _GetBoldMiniMiniGlyphPtr_MB
 .EXPORT _GetGBGlyphPtr
 .EXPORT _GetGBMiniGlyphPtr
 .EXPORT _GetGlyphPtr
 .EXPORT _GetGlyphPtr_F9
 .EXPORT _GetGlyphPtr_E7
 .EXPORT _GetGlyphPtr_E6
 .EXPORT _GetGlyphPtr_E5
 .EXPORT _GetGlyphPtr_7F
 .EXPORT _GetGlyphPtr_F7
 .EXPORT _GetMiniGlyphPtr_E7
; .EXPORT _ProgressBar0
 .EXPORT _ProgressBar1
 .EXPORT _ProgressBar2
; .EXPORT _GetLightLevel
; .EXPORT _DecLightLevel
; .EXPORT _IncLightLevel
; .EXPORT _ClrLightLevel
; .EXPORT _Bdisp_SetBacklightLevel
 .EXPORT _Bdisp_WriteDDRegister5A1
 .EXPORT _Bdisp_DDRegisterSelect
 .EXPORT _Bkey_SetFlag
 .EXPORT _Bkey_ClrFlag
 .EXPORT _Setup_GetEntry
 .EXPORT _Setup_SetEntry
 .EXPORT _IsCG10
 .EXPORT _Bdisp_SetDDRegisterB
; .EXPORT _Bdisp_IsZeroDDRegisterB

 .EXPORT _MCS_SearchDirectory
 .EXPORT _MCS_CreateDirectory
 .EXPORT _MCS_DeleteDirectory
 .EXPORT _MCS_GetSystemDirectoryInfo
 .EXPORT _MCS_SearchDirectoryItem
 .EXPORT _MCS_WriteItem
 .EXPORT _MCS_DeleteItem
 .EXPORT _MCS_GetState

 .EXPORT _Serial_GetFreeTransmitSpace
 .EXPORT _Serial_BufferedTransmitOneByte
 .EXPORT _Serial_ReadOneByte
 .EXPORT _Serial_GetReceivedBytesAvailable
 .EXPORT _Serial_ClearReceiveBuffer
 .EXPORT _Serial_ClearTransmitBuffer

 .EXPORT _SaveG3P
 .EXPORT _LoadG3P
 .EXPORT _Bdisp_FkeyColor

; .EXPORT _OpenFileDialog

 .EXPORT _Calljmp
 .EXPORT _SysCalljmp
 .EXPORT _SysCalljmp12

 .ALIGN	4


_GetVRAMAddress:
	mov.l SYSCALL,r1
	mov.l #H'01E6,r0
	jmp @r1
	nop

_GetVRAMWorkBuffer:
	mov.l SYSCALL,r1
	mov.l #H'1B0B,r0
	jmp @r1
	nop

_Bdisp_Fill_DD;
	mov.l SYSCALL,r1
	mov.l #H'0276,r0
	jmp @r1
	nop

_Bfile_RenameEntry:
	mov.l SYSCALL,r1
	mov.l #H'1DB3,r0
	jmp @r1
	nop

_Cursor_SetPosition:
	mov.l SYSCALL,r1
	mov.l #H'01F1,r0
	jmp @r1
	nop

_PutKey:
	mov.l SYSCALL,r1
	mov.l #H'0EA9,r0
	jmp @r1
	nop

_OpcodeToStr:
	mov.l SYSCALL,r1
	mov.l #H'128F,r0
	jmp @r1
	nop

_Bfile_GetFileInfo:
	mov.l SYSCALL,r1
	mov.l #H'1DA7,r0
	jmp @r1
	nop

_Cursor_GetSettings:
	mov.l SYSCALL,r1
	mov.l #H'01F5,r0
	jmp @r1
	nop


_ColorIndexDialog0:
	mov.l SYSCALL,r1
	mov.l #H'1814,r0
	jmp @r1
	nop


_Bkey_Set_RepeatTime:
	mov.l SYSCALL,r1
	mov.l #H'12A4,r0
	jmp @r1
	nop

_Bkey_Get_RepeatTime:
	mov.l SYSCALL,r1
	mov.l #H'12A5,r0
	jmp @r1
	nop

_Keyboard_ClrBuffer:
	mov.l SYSCALL,r1
	mov.l #H'12c7,r0
	jmp @r1
	nop

_Bfile_GetMediaFree_OS:
	mov.l SYSCALL,r1
	mov.l #H'1DA5,r0
	jmp @r1
	nop

_RTC_GetDateTime:
	mov.l SYSCALL,r1
	mov.l #H'11E2,r0
	jmp @r1
	nop

_GetMiniGlyphPtr_MB:
	mov.l SYSCALL,r1
	mov.l #H'01E9,r0
	jmp @r1
	nop

_GetMiniMiniGlyphPtr_MB:
	mov.l SYSCALL,r1
	mov.l #H'01EA,r0
	jmp @r1
	nop

_GetBoldMiniMiniGlyphPtr_MB:
	mov.l SYSCALL,r1
	mov.l #H'01EB,r0
	jmp @r1
	nop

_GetGBGlyphPtr:
	mov.l SYSCALL,r1
	mov.l #H'01EC,r0
	jmp @r1
	nop

_GetGBMiniGlyphPtr:
	mov.l SYSCALL,r1
	mov.l #H'01ED,r0
	jmp @r1
	nop

_GetGlyphPtr:
	mov.l SYSCALL,r1
	mov.l #H'0CEE,r0
	jmp @r1
	nop

_GetGlyphPtr_F9:
	mov.l SYSCALL,r1
	mov.l #H'0CEF,r0
	jmp @r1
	nop

_GetGlyphPtr_E7:
	mov.l SYSCALL,r1
	mov.l #H'0CF0,r0
	jmp @r1
	nop

_GetGlyphPtr_E6:
	mov.l SYSCALL,r1
	mov.l #H'0CF1,r0
	jmp @r1
	nop

_GetGlyphPtr_E5:
	mov.l SYSCALL,r1
	mov.l #H'0CF2,r0
	jmp @r1
	nop

_GetGlyphPtr_7F:
	mov.l SYSCALL,r1
	mov.l #H'0CF3,r0
	jmp @r1
	nop

_GetGlyphPtr_F7:
	mov.l SYSCALL,r1
	mov.l #H'0CF4,r0
	jmp @r1
	nop

_GetMiniGlyphPtr_E7:
	mov.l SYSCALL,r1
	mov.l #H'0CFB,r0
	jmp @r1
	nop

_ProgressBar0:
	mov.l SYSCALL,r1
	mov.l #H'180B,r0
	jmp @r1
	nop
_ProgressBar1:
	mov.l SYSCALL,r1
	mov.l #H'180E,r0
	jmp @r1
	nop
_ProgressBar2:
	mov.l SYSCALL,r1
	mov.l #H'1809,r0
	jmp @r1
	nop

;_GetLightLevel:
;	mov.l SYSCALL,r1
;	mov.l #H'1E8F,r0
;	jmp @r1
;	nop
;_DecLightLevel:
;	mov.l SYSCALL,r1
;	mov.l #H'119A,r0
;	jmp @r1
;	nop
;_IncLightLevel:
;	mov.l SYSCALL,r1
;	mov.l #H'119B,r0
;	jmp @r1
;	nop
;_ClrLightLevel:
;	mov.l SYSCALL,r1
;	mov.l #H'119C,r0
;	jmp @r1
;	nop
;_Bdisp_SetBacklightLevel:
;	mov.l SYSCALL,r1
;	mov.l #H'0199,r0
;	jmp @r1
;	nop
_Bdisp_WriteDDRegister5A1:
	mov.l SYSCALL,r1
	mov.l #H'019A,r0
	jmp @r1
	nop
_Bdisp_DDRegisterSelect:
	mov.l SYSCALL,r1
	mov.l #H'01A2,r0
	jmp @r1
	nop

_Bkey_SetFlag:
	mov.l SYSCALL,r1
	mov.l #H'0EA2,r0
	jmp @r1
	nop

_Bkey_ClrFlag:
	mov.l SYSCALL,r1
	mov.l #H'0EA4,r0
	jmp @r1
	nop

_Setup_GetEntry:
	mov.l SYSCALL,r1
	mov.l #H'0031,r0
	jmp @r1
	nop
_Setup_SetEntry:
	mov.l SYSCALL,r1
	mov.l #H'0032,r0
	jmp @r1
	nop

_IsCG10:
	mov.l SYSCALL,r1
	mov.l #H'1196,r0
	jmp @r1
	nop

_Bdisp_SetDDRegisterB:
	mov.l SYSCALL,r1
	mov.l #H'0194,r0
	jmp @r1
	nop
_Bdisp_IsZeroDDRegisterB:
	mov.l SYSCALL,r1
	mov.l #H'0195,r0
	jmp @r1
	nop

_MCS_SearchDirectory:
	mov.l SYSCALL,r1
	mov.l #H'1514,r0
	jmp @r1
	nop
_MCS_CreateDirectory:
	mov.l SYSCALL,r1
	mov.l #H'1515,r0
	jmp @r1
	nop
_MCS_DeleteDirectory:
	mov.l SYSCALL,r1
	mov.l #H'1516,r0
	jmp @r1
	nop
_MCS_GetSystemDirectoryInfo:
	mov.l SYSCALL,r1
	mov.l #H'1518,r0
	jmp @r1
	nop
_MCS_SearchDirectoryItem:
	mov.l SYSCALL,r1
	mov.l #H'1519,r0
	jmp @r1
	nop
_MCS_WriteItem:
	mov.l SYSCALL,r1
	mov.l #H'151A,r0
	jmp @r1
	nop
_MCS_DeleteItem:
	mov.l SYSCALL,r1
	mov.l #H'151C,r0
	jmp @r1
	nop
_MCS_GetState:
	mov.l SYSCALL,r1
	mov.l #H'1529,r0
	jmp @r1
	nop

_SaveG3P:
	mov.l SYSCALL,r1
	mov.l #H'1009,r0
	jmp @r1
	nop
_LoadG3P:
	mov.l SYSCALL,r1
	mov.l #H'100F,r0
	jmp @r1
	nop
_Bdisp_FkeyColor:
	mov.l SYSCALL,r1
	mov.l #H'0130,r0
	jmp @r1
	nop

_Serial_GetFreeTransmitSpace:
	mov.l SYSCALL,r1
	mov.l #H'1bc0,r0
	jmp @r1
	nop
_Serial_BufferedTransmitOneByte:
	mov.l SYSCALL,r1
	mov.l #H'1bbc,r0
	jmp @r1
	nop
_Serial_ReadOneByte:
	mov.l SYSCALL,r1
	mov.l #H'1bb9,r0
	jmp @r1
	nop
_Serial_GetReceivedBytesAvailable:
	mov.l SYSCALL,r1
	mov.l #H'1bbf,r0
	jmp @r1
	nop
_Serial_ClearReceiveBuffer:
	mov.l SYSCALL,r1
	mov.l #H'1bc1,r0
	jmp @r1
	nop
_Serial_ClearTransmitBuffer:
	mov.l SYSCALL,r1
	mov.l #H'1bc2,r0
	jmp @r1
	nop

;_OpenFileDialog:
;	mov.l SYSCALL,r1
;	mov.l #H'17E9,r0
;	jmp @r1
;	nop

;int Calljmp( int r4, int r5, int r6, int r7, int adrs );

_Calljmp:
	mov.l	@r15,r1
	jmp	@r1
	nop

;int SysCalljmp( int r4, int r5, int r6, int r7, int No);

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
SYSCALL: .DATA.L H'80020070
LCDC:    .DATA.L H'B4000000
 .END
