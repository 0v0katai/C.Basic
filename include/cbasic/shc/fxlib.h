/*****************************************************************/
/*                                                               */
/*   CASIO fx-9860G SDK Library                                  */
/*                                                               */
/*   File name : fxlib.h                                         */
/*                                                               */
/*   Copyright (c) 2006 CASIO COMPUTER CO., LTD.                 */
/*                                                               */
/*****************************************************************/
#ifndef __FXLIB_H__
#define __FXLIB_H__

#include "dispbios.h"
#include "filebios.h"
#include "keybios.h"


// Prototypes

__attribute__((renesas)) void Bdisp_AllClr_DD(void);
__attribute__((renesas)) void Bdisp_AllClr_VRAM(void);
__attribute__((renesas)) void Bdisp_AllClr_DDVRAM(void);
__attribute__((renesas)) void Bdisp_AreaClr_DD(const DISPBOX *pArea);
__attribute__((renesas)) void Bdisp_AreaClr_VRAM(const DISPBOX *pArea);
__attribute__((renesas)) void Bdisp_AreaClr_DDVRAM(const DISPBOX *pArea);
__attribute__((renesas)) void Bdisp_AreaReverseVRAM(int x1, int y1, int x2, int y2);
__attribute__((renesas)) void Bdisp_GetDisp_DD(unsigned char *pData);
__attribute__((renesas)) void Bdisp_GetDisp_VRAM(unsigned char *pData);
__attribute__((renesas)) void Bdisp_PutDisp_DD(void);
__attribute__((renesas)) void Bdisp_PutDispArea_DD(const DISPBOX *PutDispArea);
__attribute__((renesas)) void Bdisp_SetPoint_DD(int x, int y, unsigned char point);
__attribute__((renesas)) void Bdisp_SetPoint_VRAM(int x, int y, unsigned char point);
__attribute__((renesas)) void Bdisp_SetPoint_DDVRAM(int x, int y, unsigned char point);
__attribute__((renesas)) int  Bdisp_GetPoint_VRAM(int x, int y);
__attribute__((renesas)) void Bdisp_WriteGraph_DD(const DISPGRAPH *WriteGraph);
__attribute__((renesas)) void Bdisp_WriteGraph_VRAM(const DISPGRAPH *WriteGraph);
__attribute__((renesas)) void Bdisp_WriteGraph_DDVRAM(const DISPGRAPH *WriteGraph);
__attribute__((renesas)) void Bdisp_ReadArea_DD(const DISPBOX *ReadArea, unsigned char *ReadData);
__attribute__((renesas)) void Bdisp_ReadArea_VRAM(const DISPBOX *ReadArea, unsigned char *ReadData);
__attribute__((renesas)) void Bdisp_DrawLineVRAM(int x1, int y1, int x2, int y2);
__attribute__((renesas)) void Bdisp_ClearLineVRAM(int x1, int y1, int x2, int y2);

__attribute__((renesas)) void locate(int x, int y);
__attribute__((renesas)) void Print(const unsigned char *str);
__attribute__((renesas)) void PrintRev(const unsigned char *str);
__attribute__((renesas)) void PrintC(const unsigned char *c);
__attribute__((renesas)) void PrintRevC(const unsigned char *str);
__attribute__((renesas)) void PrintLine(const unsigned char *str, int max);
__attribute__((renesas)) void PrintRLine(const unsigned char *str, int max);
__attribute__((renesas)) void PrintXY(int x, int y, const unsigned char *str, int type);
__attribute__((renesas)) int PrintMini(int x, int y, const unsigned char *str, int type);
__attribute__((renesas)) void SaveDisp(unsigned char num);
__attribute__((renesas)) void RestoreDisp(unsigned char num);
__attribute__((renesas)) void PopUpWin(int n);

__attribute__((renesas)) int Bfile_OpenFile(const FONTCHARACTER *filename, int mode);
__attribute__((renesas)) int Bfile_OpenMainMemory(const unsigned char *name);
__attribute__((renesas)) int Bfile_ReadFile(int HANDLE, void *buf, int size, int readpos);
__attribute__((renesas)) int Bfile_WriteFile(int HANDLE, const void *buf, int size);
__attribute__((renesas)) int Bfile_SeekFile(int HANDLE, int pos);
__attribute__((renesas)) int Bfile_CloseFile(int HANDLE);
__attribute__((renesas)) int Bfile_GetMediaFree(enum DEVICE_TYPE devicetype, int *freebytes);
__attribute__((renesas)) int Bfile_GetFileSize(int HANDLE);
__attribute__((renesas)) int Bfile_CreateFile(const FONTCHARACTER *filename, int size);
__attribute__((renesas)) int Bfile_CreateDirectory(const FONTCHARACTER *pathname);
__attribute__((renesas)) int Bfile_CreateMainMemory(const unsigned char *name);
__attribute__((renesas)) int Bfile_RenameMainMemory(const unsigned char *oldname, const unsigned char *newname);
__attribute__((renesas)) int Bfile_DeleteFile(const FONTCHARACTER *filename);
__attribute__((renesas)) int Bfile_DeleteDirectory(const FONTCHARACTER *pathname);
__attribute__((renesas)) int Bfile_DeleteMainMemory(const unsigned char *name);
__attribute__((renesas)) int Bfile_FindFirst(const FONTCHARACTER *pathname, int *FindHandle, FONTCHARACTER *foundfile, FILE_INFO *fileinfo);
__attribute__((renesas)) int Bfile_FindNext(int FindHandle, FONTCHARACTER *foundfile, FILE_INFO *fileinfo);
__attribute__((renesas)) int Bfile_FindClose(int FindHandle);

__attribute__((renesas)) void Bkey_Set_RepeatTime(long FirstCount, long NextCount);
__attribute__((renesas)) void Bkey_Get_RepeatTime(long *FirstCount, long *NextCount);
__attribute__((renesas)) void Bkey_Set_RepeatTime_Default(void);
__attribute__((renesas)) int GetKeyWait(int sel, int time, int menu, unsigned int *keycode);
__attribute__((renesas)) int IsKeyDown(int keycode);
__attribute__((renesas)) int IsKeyUp(int keycode);
__attribute__((renesas)) int GetKey(unsigned int *keycode);

__attribute__((renesas)) int SetTimer(int ID, int elapse, void (*hander)(void));
__attribute__((renesas)) int KillTimer(int ID);
__attribute__((renesas)) void Sleep(int millisecond);

__attribute__((renesas)) void SetQuitHandler(void (*callback)(void));
__attribute__((renesas)) int INIT_ADDIN_APPLICATION(int isAppli, unsigned short OptionNum);

__attribute__((renesas)) int Bkey_GetKeyWait(int *code1, int *code2, int wait_type, int time, int menu, short *unused);

#endif
