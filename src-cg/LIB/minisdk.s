.text
    .global _APP_EACT_StatusIcon
    .global _APP_FINANCE
    .global _APP_Program
    .global _APP_SYSTEM_BATTERY
    .global _APP_SYSTEM_DISPLAY
    .global _APP_SYSTEM_LANGUAGE
    .global _APP_SYSTEM_POWER
    .global _APP_SYSTEM_RESET
    .global _APP_SYSTEM_VERSION
    .global _APP_SYSTEM
    .global _AUX_DisplayErrorMessage
    .global _BatteryIcon
    .global _Bdisp_AllClr_VRAM
    .global _Bdisp_AreaClr_DD_x3
    .global _Bdisp_AreaClr
    .global _Bdisp_Fill_VRAM
    .global _Bdisp_GetPoint_DD_Workbench
    .global _Bdisp_GetPoint_DD
    .global _Bdisp_GetPoint_VRAM
    .global _Bdisp_HeaderFill
    .global _Bdisp_HeaderFill2
    .global _Bdisp_HeaderText
    .global _Bdisp_HeaderText2
    .global _Bdisp_PutDisp_DD_stripe
    .global _Bdisp_PutDisp_DD
    .global _Bdisp_SetPoint_DD
    .global _Bdisp_SetPoint_VRAM
    .global _Bdisp_SetPointWB_VRAM
    .global _Bdisp_ShapeBase
    .global _Bdisp_ShapeBase3XVRAM
    .global _Bdisp_ShapeToVRAM16C
    .global _Bdisp_WriteGraphVRAM_WB
    .global _Bdisp_WriteGraphVRAM
    .global _Bfile_CloseFile_OS
    .global _Bfile_CreateEntry_OS
    .global _Bfile_DeleteEntry
    .global _Bfile_FindClose
    .global _Bfile_FindFirst_NON_SMEM
    .global _Bfile_FindFirst
    .global _Bfile_FindNext_NON_SMEM
    .global _Bfile_FindNext
    .global _Bfile_GetFileSize_OS
    .global _Bfile_NameToStr_ncpy
    .global _Bfile_OpenFile_OS
    .global _Bfile_ReadFile_OS
#    .global _Bfile_RenameEntry
    .global _Bfile_SeekFile_OS
    .global _Bfile_StrToName_ncpy
    .global _Bfile_WriteFile_OS
    .global _Box
    .global _Box2
    .global _BoxInnerClear
    .global _BoxYLimits
    .global _ByteToHex
    .global _CharacterSelectDialog
    .global _CMT_Delay_100micros
    .global _CMT_Delay_micros
    .global _Cursor_SetFlashOff
    .global _Cursor_SetFlashOn
    .global _d_c_Icon
    .global _DefineStatusAreaFlags
    .global _DefineStatusGlyph
    .global _DefineStatusMessage
    .global _DirectDrawRectangle
    .global _DispInt
    .global _DisplayMainMenu
    .global _DisplayMBString
    .global _DisplayMBString2
    .global _DisplayMessageBox
    .global _DisplayStatusArea
    .global _DrawFrame
    .global _DrawFrameWorkbench
    .global _DrawHeaderLine
    .global _EditMBStringChar
    .global _EditMBStringCtrl
    .global _EditMBStringCtrl2
    .global _EditMBStringCtrl3
    .global _EditMBStringCtrl4
    .global _EnableColor
    .global _EnableDisplayHeader
    .global _EnableStatusArea
    .global _FKey_Display
    .global _free
    .global _GetAppName
    .global _GetFKeyPtr
    .global _GetKey
    .global _GetKeyWait_OS
    .global _GetMainBatteryVoltage
    .global _GetMiniGlyphPtr
    .global _GetSetupSetting
    .global _GetStackPtr
    .global _HexToByte
    .global _HexToNibble
    .global _HexToWord
    .global _HourGlass
    .global _ItoA_10digit
    .global _itoa
    .global _Keyboard_CursorFlash
    .global _KeyboardIcon
    .global _LineIcon
    .global _LoadVRAM_1
    .global _locate_OS
    .global _LongToAscHex
    .global _malloc
#    .global _MCS_CreateDirectory
#    .global _MCS_DeleteDirectory
    .global _MCS_GetCapa
    .global _MCS_GetMainMemoryStart
#    .global _MCS_WriteItem
    .global _MCSDelVar2
    .global _MCSGetData1
    .global _MCSGetDlen2
    .global _MCSGetOpenItem
    .global _MCSOvwDat2
    .global _MCSPutVar2
#    .global _memcmp
    .global _memcmp3
    .global _memcpy
    .global _memset
    .global _MsgBoxPop
    .global _MsgBoxPush
    .global _NibbleToHex
    .global _NormIcon
    .global _OpCodeToStr
    .global _OS_InnerWait_ms
    .global _PRGM_GetKey_OS
    .global _Print_OS
    .global _PrintCXY
    .global _PrintGlyph
    .global _PrintLine
    .global _PrintLine2
    .global _PrintMini
    .global _PrintMiniGlyph
    .global _PrintXY_2
    .global _PrintXY
#    .global _PutKey
    .global _RadIcon
    .global _RealIcon
    .global _realloc
    .global _ResetAllDialog
    .global _RTC_Elapsed_ms
    .global _RTC_GetTicks
    .global _RTC_GetTime
    .global _RTC_Reset
    .global _RTC_SetDateTime
    .global _SaveVRAM_1
    .global _Scrollbar
    .global _Serial_Close
    .global _Serial_DirectTransmitOneByte
    .global _Serial_Open
    .global _Set_FKeys1
    .global _Set_FKeys2
    .global _SetBackGround
    .global _SetCursorFlashToggle
    .global _SetSetupSetting
    .global _SetupMode_StatusIcon
    .global _SMEM_FindFirst
    .global _SMEM_maint
    .global _StandardScrollbar
    .global _strcmp_ignore_case
    .global _strlen
    .global _Syscall02A4
    .global _Syscall02A7
    .global _Syscall02A9
    .global _Syscall1816
    .global _Test01A8
    .global _Test1B0E
    .global _Test1E44
    .global _Test17BC
    .global _Test127E
    .global _TestMode
    .global _Timer_Deinstall
    .global _Timer_Install
    .global _Timer_Start
    .global _Timer_Stop
    .global _WordToHex
    .global _WriteBackground
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

_APP_EACT_StatusIcon:
    BIOS_JUMP 0x0A87

_APP_FINANCE:
    BIOS_JUMP 0x0C6B

_APP_Program:
    BIOS_JUMP 0x1945

_APP_SYSTEM_BATTERY:
    BIOS_JUMP 0x1e07

_APP_SYSTEM_DISPLAY:
    BIOS_JUMP 0x1E0A

_APP_SYSTEM_LANGUAGE:
    BIOS_JUMP 0x1E0D

_APP_SYSTEM_POWER:
    BIOS_JUMP 0x1e05

_APP_SYSTEM_RESET:
    BIOS_JUMP 0x1e17

_APP_SYSTEM_VERSION:
    BIOS_JUMP 0x1E34

_APP_SYSTEM:
    BIOS_JUMP 0x1E13

_AUX_DisplayErrorMessage:
    BIOS_JUMP 0x0C01

_BatteryIcon:
    BIOS_JUMP 0x1D89

_Bdisp_AllClr_VRAM:
    BIOS_JUMP 0x0272

_Bdisp_AreaClr_DD_x3:
    BIOS_JUMP 0x01B6

_Bdisp_AreaClr:
    BIOS_JUMP 0x02B2

_Bdisp_Fill_VRAM:
    BIOS_JUMP 0x0275

_Bdisp_GetPoint_DD_Workbench:
    BIOS_JUMP 0x026E

_Bdisp_GetPoint_DD:
    BIOS_JUMP 0x026F

_Bdisp_GetPoint_VRAM:
    BIOS_JUMP 0x0267

_Bdisp_HeaderFill:
    BIOS_JUMP 0x1D86

_Bdisp_HeaderFill2:
    BIOS_JUMP 0x1D87

_Bdisp_HeaderText:
    BIOS_JUMP 0x1D82

_Bdisp_HeaderText2:
    BIOS_JUMP 0x1D85

_Bdisp_PutDisp_DD_stripe:
    BIOS_JUMP 0x0260

_Bdisp_PutDisp_DD:
    BIOS_JUMP 0x025F

_Bdisp_SetPoint_DD:
    BIOS_JUMP 0x026B

_Bdisp_SetPoint_VRAM:
    BIOS_JUMP 0x0263

_Bdisp_SetPointWB_VRAM:
    BIOS_JUMP 0x0262

_Bdisp_ShapeBase:
    BIOS_JUMP 0x01C7

_Bdisp_ShapeBase3XVRAM:
    BIOS_JUMP 0x01BE

_Bdisp_ShapeToVRAM16C:
    BIOS_JUMP 0x01C4

_Bdisp_WriteGraphVRAM_WB:
    BIOS_JUMP 0x028f

_Bdisp_WriteGraphVRAM:
    BIOS_JUMP 0x0290

_Bfile_CloseFile_OS:
    BIOS_JUMP 0x1DA4

_Bfile_CreateEntry_OS:
    BIOS_JUMP 0x1DAE

_Bfile_DeleteEntry:
    BIOS_JUMP 0x1DB4

_Bfile_FindClose:
    BIOS_JUMP 0x1DBA

_Bfile_FindFirst_NON_SMEM:
    BIOS_JUMP 0x1DB6

_Bfile_FindFirst:
    BIOS_JUMP 0x1DB7

_Bfile_FindNext_NON_SMEM:
    BIOS_JUMP 0x1DB8

_Bfile_FindNext:
    BIOS_JUMP 0x1DB9

_Bfile_GetFileSize_OS:
    BIOS_JUMP 0x1DA9

_Bfile_NameToStr_ncpy:
    BIOS_JUMP 0x1DDB

_Bfile_OpenFile_OS:
    BIOS_JUMP 0x1DA3

_Bfile_ReadFile_OS:
    BIOS_JUMP 0x1DAC

_Bfile_RenameEntry:
    BIOS_JUMP 0x1DB3

_Bfile_SeekFile_OS:
    BIOS_JUMP 0x1DAB

_Bfile_StrToName_ncpy:
    BIOS_JUMP 0x1DDC

_Bfile_WriteFile_OS:
    BIOS_JUMP 0x1DAF

_Box:
    BIOS_JUMP 0x092B

_Box2:
    BIOS_JUMP 0x17FC

_BoxInnerClear:
    BIOS_JUMP 0x17FD

_BoxYLimits:
    BIOS_JUMP 0x17FB

_ByteToHex:
    BIOS_JUMP 0x1347

_CharacterSelectDialog:
    BIOS_JUMP 0x07A2

_CMT_Delay_100micros:
    BIOS_JUMP 0x11D7

_CMT_Delay_micros:
    BIOS_JUMP 0x11D6

_Cursor_SetFlashOff:
    BIOS_JUMP 0x08C8

_Cursor_SetFlashOn:
    BIOS_JUMP 0x08C7

_d_c_Icon:
    BIOS_JUMP 0x1D8E

_DefineStatusAreaFlags:
    BIOS_JUMP 0x02B8

_DefineStatusGlyph:
    BIOS_JUMP 0x1D7B

_DefineStatusMessage:
    BIOS_JUMP 0x1D77

_DirectDrawRectangle:
    BIOS_JUMP 0x02AA

_DispInt:
    BIOS_JUMP 0x0259

_DisplayMainMenu:
    BIOS_JUMP 0x1E6A

_DisplayMBString:
    BIOS_JUMP 0x121D

_DisplayMBString2:
    BIOS_JUMP 0x1218

_DisplayMessageBox:
    BIOS_JUMP 0x1E4B

_DisplayStatusArea:
    BIOS_JUMP 0x1D81

_DrawFrame:
    BIOS_JUMP 0x02A8

_DrawFrameWorkbench:
    BIOS_JUMP 0x0923

_DrawHeaderLine:
    BIOS_JUMP 0x02BB

_EditMBStringChar:
    BIOS_JUMP 0x1224

_EditMBStringCtrl:
    BIOS_JUMP 0x120E

_EditMBStringCtrl2:
    BIOS_JUMP 0x120A

_EditMBStringCtrl3:
    BIOS_JUMP 0x120B

_EditMBStringCtrl4:
    BIOS_JUMP 0x120C

_EnableColor:
    BIOS_JUMP 0x0921

_EnableDisplayHeader:
    BIOS_JUMP 0x1D7F

_EnableStatusArea:
    BIOS_JUMP 0x02B7

_FKey_Display:
    BIOS_JUMP 0x0131

_free:
    BIOS_JUMP 0x1F42

_GetAppName:
    BIOS_JUMP 0x1E9F

_GetFKeyPtr:
    BIOS_JUMP 0x12f3

_GetKey:
    BIOS_JUMP 0x0EAB

_GetKeyWait_OS:
    BIOS_JUMP 0x12bf

_GetMainBatteryVoltage:
    BIOS_JUMP 0x1186

_GetMiniGlyphPtr:
    BIOS_JUMP 0x01E9

_GetSetupSetting:
    BIOS_JUMP 0x0031

_GetStackPtr:
    BIOS_JUMP 0x1A2C

_HexToByte:
    BIOS_JUMP 0x1344

_HexToNibble:
    BIOS_JUMP 0x1343

_HexToWord:
    BIOS_JUMP 0x1345

_HourGlass:
    BIOS_JUMP 0x02C7

_ItoA_10digit:
    BIOS_JUMP 0x1633

_itoa:
    BIOS_JUMP 0x1170

_Keyboard_CursorFlash:
    BIOS_JUMP 0x08CA

_KeyboardIcon:
    BIOS_JUMP 0x1D8A

_LineIcon:
    BIOS_JUMP 0x1D8F

_LoadVRAM_1:
    BIOS_JUMP 0x1E63

_locate_OS:
    BIOS_JUMP 0x1863

_LongToAscHex:
    BIOS_JUMP 0x1841

_malloc:
    BIOS_JUMP 0x1F44

_MCS_CreateDirectory:
    BIOS_JUMP 0x154B

_MCS_DeleteDirectory:
    BIOS_JUMP 0x1516

_MCS_GetCapa:
    BIOS_JUMP 0x1532

_MCS_GetMainMemoryStart:
    BIOS_JUMP 0x1543

_MCS_WriteItem:
    BIOS_JUMP 0x151A

_MCSDelVar2:
    BIOS_JUMP 0x1558

_MCSGetData1:
    BIOS_JUMP 0x1563

_MCSGetDlen2:
    BIOS_JUMP 0x1562

_MCSGetOpenItem:
    BIOS_JUMP 0x1560

_MCSOvwDat2:
    BIOS_JUMP 0x1552

_MCSPutVar2:
    BIOS_JUMP 0x154D

#_memcmp:
#    BIOS_JUMP 0x1DD1

_memcmp3:
    BIOS_JUMP 0x115E

_memcpy:
    BIOS_JUMP 0x1DD0

_memset:
    BIOS_JUMP 0x1511

_MsgBoxPop:
    BIOS_JUMP 0x17F9

_MsgBoxPush:
    BIOS_JUMP 0x17F7

_NibbleToHex:
    BIOS_JUMP 0x1346

_NormIcon:
    BIOS_JUMP 0x1D8D

_OpCodeToStr:
    BIOS_JUMP 0x128F

_OS_InnerWait_ms:
    BIOS_JUMP 0x1BB4

_PRGM_GetKey_OS:
    BIOS_JUMP 0x0D39

_Print_OS:
    BIOS_JUMP 0x01F9

_PrintCXY:
    BIOS_JUMP 0x0239

_PrintGlyph:
    BIOS_JUMP 0x0238

_PrintLine:
    BIOS_JUMP 0x1883

_PrintLine2:
    BIOS_JUMP 0x1865

_PrintMini:
    BIOS_JUMP 0x023c

_PrintMiniGlyph:
    BIOS_JUMP 0x023B

_PrintXY_2:
    BIOS_JUMP 0x18ec

_PrintXY:
    BIOS_JUMP 0x18F9

_PutKey:
    BIOS_JUMP 0x0EA9

_RadIcon:
    BIOS_JUMP 0x1D8B

_RealIcon:
    BIOS_JUMP 0x1D8C

_realloc:
    BIOS_JUMP 0x1F46

_ResetAllDialog:
    BIOS_JUMP 0x1E23

_RTC_Elapsed_ms:
    BIOS_JUMP 0x02C2

_RTC_GetTicks:
    BIOS_JUMP 0x02C1

_RTC_GetTime:
    BIOS_JUMP 0x02C0

_RTC_Reset:
    BIOS_JUMP 0x02BF

_RTC_SetDateTime:
    BIOS_JUMP 0x11DE

_SaveVRAM_1:
    BIOS_JUMP 0x1E62

_Scrollbar:
    BIOS_JUMP 0x1A0A

_Serial_Close:
    BIOS_JUMP 0x1BB8

_Serial_DirectTransmitOneByte:
    BIOS_JUMP 0x1BBD

_Serial_Open:
    BIOS_JUMP 0x1BB7

_Set_FKeys1:
    BIOS_JUMP 0x012B

_Set_FKeys2:
    BIOS_JUMP 0x0129

_SetBackGround:
    BIOS_JUMP 0x1EF8

_SetCursorFlashToggle:
    BIOS_JUMP 0x08D2

_SetSetupSetting:
    BIOS_JUMP 0x0032

_SetupMode_StatusIcon:
    BIOS_JUMP 0x0A8B

_SMEM_FindFirst:
    BIOS_JUMP 0x0DAC

_SMEM_maint:
    BIOS_JUMP 0x0DDF

_StandardScrollbar:
    BIOS_JUMP 0x0C5A

_strcmp_ignore_case:
    BIOS_JUMP 0x1635

_strlen:
    BIOS_JUMP 0x1349

_Syscall02A4:
    BIOS_JUMP 0x02A4

_Syscall02A7:
    BIOS_JUMP 0x02A7

_Syscall02A9:
    BIOS_JUMP 0x02A9

_Syscall1816:
    BIOS_JUMP 0x1816

_Test01A8:
    BIOS_JUMP 0x01A8

_Test1B0E:
    BIOS_JUMP 0x1B0E

_Test1E44:
    BIOS_JUMP 0x1E44

_Test17BC:
    BIOS_JUMP 0x17BC

_Test127E:
    BIOS_JUMP 0x127E

_TestMode:
    BIOS_JUMP 0x0EA7

_Timer_Deinstall:
    BIOS_JUMP 0x08DA

_Timer_Install:
    BIOS_JUMP 0x08D9

_Timer_Start:
    BIOS_JUMP 0x08DB

_Timer_Stop:
    BIOS_JUMP 0x08DC

_WordToHex:
    BIOS_JUMP 0x1348

_WriteBackground:
    BIOS_JUMP 0x1EF7
