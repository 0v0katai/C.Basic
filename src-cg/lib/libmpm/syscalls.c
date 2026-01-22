#define CAST(address, name) \
  void name() { \
    void (*ptr)() = address; \
    return ptr(); \
  }

#define NOTFOUND(name) \
  int name() { \
    return -1; \
  }

#define KEY_CTRL_PREVTAB    30075
#define KEY_CTRL_UP         30018
#define KEY_CTRL_NEXTTAB    30074
#define KEY_CTRL_PGUP       30065
#define KEY_CTRL_SETUP      30037
#define KEY_CTRL_SETTINGS   KEY_CTRL_SETUP // reused from old API
#define KEY_CTRL_EXIT       30002
#define KEY_CTRL_BACK       KEY_CTRL_EXIT // reused from old API
#define KEY_CTRL_LEFT       30020
#define KEY_CTRL_EXE        30004
#define KEY_CTRL_OK         KEY_CTRL_EXE // cannot be distinguished from EXE
#define KEY_CTRL_RIGHT      30021
#define KEY_CTRL_PGDOWN     30066
#define KEY_CTRL_SHIFT      30006
#define KEY_CTRL_ALPHA      30007
#define KEY_CTRL_VARS       30016
#define KEY_CTRL_DOWN       30023
#define KEY_CTRL_CATALOG    30100
#define KEY_CTRL_TOOLS      30068
#define KEY_CTRL_FORMAT     30067
#define KEY_CTRL_MENU       30003
#define KEY_CTRL_F1         30009
#define KEY_CTRL_F2         30010
#define KEY_CTRL_F3         30011
#define KEY_CTRL_F4         30012
#define KEY_CTRL_F5         30013
#define KEY_CTRL_F6         30014

int GetKey(int *key) {
  int (*ptr)(int *) = 0x801d2bba;
  int res = ptr(key);

  if (*key==KEY_CTRL_SETUP) // can we intercept ON?
    *key=KEY_CTRL_F1;
  if (*key==KEY_CTRL_MENU) // 
    *key=KEY_CTRL_F2;
  if (*key==KEY_CTRL_PREVTAB) // should also handle shift-3
    *key=KEY_CTRL_F3;
  if (*key==KEY_CTRL_PGDOWN)
    *key=KEY_CTRL_F4;
  if (*key==KEY_CTRL_NEXTTAB)
    *key=KEY_CTRL_F5;
  if (*key==KEY_CTRL_PGUP) // should also handle shift-6
    *key=KEY_CTRL_F6;
  return res;
}

CAST(0x8007a6b0, Bdisp_AllClr_VRAM)
CAST(0x80366708, sys_malloc)
CAST(0x803664d4, sys_free)
CAST(0x803672c8, sys_realloc)
CAST(0x8007a2d4, Bdisp_PutDisp_DD)
CAST(0x8007569e, GetVRAMAddress)
CAST(0x80075698, GetVRAMBackgroundAddress)
CAST(0x8007718c, PrintMini)
CAST(0x800762c4, PrintMiniMini)
CAST(0x8018c328, Bdisp_MMPrint)
CAST(0x8018c220, Bdisp_MMPrintRef)
CAST(0x8007b024, DrawFrame)
CAST(0x802382fe, GetKeyWait_OS)
CAST(0x80334212, Bfile_DeleteEntry)
CAST(0x803341c2, Bfile_RenameEntry)
CAST(0x80333ef0, Bfile_CreateEntry_OS)
CAST(0x803338d0, Bfile_OpenFile_OS)
CAST(0x80333a4e, Bfile_CloseFile_OS)
CAST(0x80333b04, Bfile_GetFileSize_OS)
CAST(0x80333c6c, Bfile_SeekFile_OS)
CAST(0x80333d7c, Bfile_TellFile_OS)
CAST(0x80333f9e, Bfile_WriteFile_OS)
CAST(0x80333dc2, Bfile_ReadFile_OS)
CAST(0x803345c8, Bfile_FindFirst)
CAST(0x80334846, Bfile_FindNext)
CAST(0x80334950, Bfile_FindClose)
CAST(0x803358a6, Bfile_NameToStr_ncpy)
CAST(0x803358ec, Bfile_StrToName_ncpy)
CAST(0x8033207c, BatteryIcon)
CAST(0x80238f68, UpdateBatteryState)
CAST(0x800e7988, USBPopup)
CAST(0x8030a2dc, OS_InnerWait_ms)
CAST(0x8007cc0c, RTC_GetTicks)
CAST(0x80230d94, RTC_SetDateTime)
CAST(0x8010def0, Timer_Deinstall)
CAST(0x8010de28, Timer_Install)
CAST(0x8010df72, Timer_Start)
CAST(0x8010dfce, Timer_Stop)
CAST(0x80331aba, DefineStatusMessage)
CAST(0x8007c058, DefineStatusAreaFlags)
CAST(0x80331ce0, DisplayStatusArea)
CAST(0x8007c032, EnableStatusArea)
CAST(0x8005bf5c, GetSetupSetting)
CAST(0x8005bf66, SetSetupSetting)
CAST(0x803357e0, Bfile_Name_MatchMask)
CAST(0x80115d50, Bdisp_EnableColor)
CAST(0x803345c8, Bfile_FindFirst_NON_SMEM)
CAST(0x80334846, Bfile_FindNext_NON_SMEM)
CAST(0x80333aba, Bfile_GetMediaFree_OS)
CAST(0x80006490, Keyboard_PutKeycode)
CAST(0x8022995a, MB_ElementCount)
CAST(0x8025bb3c, MCSDelVar2)
CAST(0x8025c392, MCSGetDlen2)
CAST(0x8025b166, MCS_CreateDirectory)
CAST(0x8025b222, MCSPutVar2)
CAST(0x801900cc, PRGM_GetKey_OS)
CAST(0x80076bb4, PrintCXY)
CAST(0x802a931a, PrintXY)
CAST(0x801d2ada, Bkey_SetAllFlags)
CAST(0x8010d300, Cursor_SetFlashOn)
CAST(0x8010d392, Cursor_SetFlashOff)
CAST(0x802d9f9e, Scrollbar)
CAST(0x8007cdd8, HourGlass)
CAST(0x8030a89e, Serial_IsOpen)
CAST(0x8030a358, Serial_Open)
CAST(0x8030a3f6, Serial_Close)
CAST(0x8030a6ea, Serial_PollTX)
CAST(0x8030a6d6, Serial_PollRX)
CAST(0x8030a4ee, Serial_Read)
CAST(0x8030a678, Serial_Write)
CAST(0x80229d98, itoa)
CAST(0x80074268, Bdisp_DDRegisterSelect)
CAST(0x800742a0, Bdisp_DefineDMARange)
CAST(0x800743de, Bdisp_WriteDDRegister3_bit7)
CAST(0x8007cbbc, RTC_GetTime)
CAST(0x8007a6ca, Bdisp_Fill_VRAM)
CAST(0x8007a2da, Bdisp_PutDisp_DD_stripe)
CAST(0x80251824, SaveVRAM_1)
CAST(0x80251838, LoadVRAM_1)
CAST(0x8025c3f8, MCSGetData1)
CAST(0x80230b4e, CMT_Delay_micros)
CAST(0x80259388, MCS_WriteItem)
CAST(0x80333cf2, Bfile_GetBlockAddress)
CAST(0x8007bbcc, Bdisp_AreaClr)
CAST(0x8007a5a2, Bdisp_SetPoint_DD)
CAST(0x801d2ae0, Bkey_SetFlag)
CAST(0x80331bc2, DefineStatusGlyph)
CAST(0x8022fb9e, GetMainBatteryVoltage)
CAST(0x802e11fc, GetStackPtr)
CAST(0x8025a4c2, MCS_GetState)
CAST(0x8025b690, MCSOvwDat2)
CAST(0x8029a550, ProgressBar0)
CAST(0x8029a8ba, ProgressBar1)
CAST(0x8029a518, ProgressBar2)
CAST(0x802a5616, PrintLine)
CAST(0x80075acc, Print_OS)
CAST(0x802a5434, locate_OS)
CAST(0x8007a78c, Bdisp_Fill_DD)
CAST(0x80073fa6, Bdisp_WriteDDRegister5A1)
CAST(0x80333b78, Bfile_GetFileInfo)
CAST(0x801d2af2, Bkey_ClrFlag)
CAST(0x80237702, Bkey_Get_RepeatTime)
CAST(0x802376f8, Bkey_Set_RepeatTime)
CAST(0x8029ade2, ColorIndexDialog0)
CAST(0x80115d50, EnableColor)
CAST(0x8007589c, GetBoldMiniMiniGlyphPtr_MB)
CAST(0x8018bae8, GetGlyphPtr)
CAST(0x8018bb6c, GetGlyphPtr_7F)
CAST(0x8018bb4e, GetGlyphPtr_E5)
CAST(0x8018bb30, GetGlyphPtr_E6)
CAST(0x8018bb12, GetGlyphPtr_E7)
CAST(0x8007571c, GetMiniGlyphPtr_MB)
CAST(0x800757ba, GetMiniMiniGlyphPtr_MB)
CAST(0x802f74f0, GetVRAMWorkBuffer)
CAST(0x800064e8, Keyboard_ClrBuffer)
CAST(0x80205642, LoadG3P)
CAST(0x80258f96, MCS_SearchDirectory)
CAST(0x8025926e, MCS_SearchDirectoryItem)
CAST(0x80236b78, OpCodeToStr)
CAST(0x801d2b22, PutKey)
CAST(0x80230f34, RTC_GetDateTime)
CAST(0x8005bf5c, Setup_GetEntry)
CAST(0x8005bf66, Setup_SetEntry)
CAST(0x8030a5f6, Serial_BufferedTransmitOneByte)
CAST(0x8030a6fe, Serial_ClearReceiveBuffer)
CAST(0x8030a724, Serial_ClearTransmitBuffer)
CAST(0x8030a6ea, Serial_GetFreeTransmitSpace)
CAST(0x8030a6d6, Serial_GetReceivedBytesAvailable)
CAST(0x8030a4aa, Serial_ReadOneByte)

NOTFOUND(IsCG10)
NOTFOUND(GetGBGlyphPtr)
NOTFOUND(GetGBMiniGlyphPtr)
NOTFOUND(Bdisp_FkeyColor)
NOTFOUND(GetFKeyPtr)
NOTFOUND(FKey_Display)
NOTFOUND(MsgBoxPop)
NOTFOUND(MsgBoxPush)

