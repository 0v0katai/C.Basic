//------------------------------------------- source code refer to (WSC) file.c
#ifndef _FILE_H_
#define _FILE_H_

#include "fxlib.h"

/* return a filename */
unsigned int SelectFile (char *filename);
void FileListfree() ;

// #define DEBUG
// if define DEBUG, the files are in SD card
// else the files are in storage memory

#ifdef DEBUG
#define ROOT "crd0"
#else
#define ROOT "fls0"
#endif

FONTCHARACTER *CharToFont( const char *cFileName, FONTCHARACTER *fFileName );
char *FontToChar( const FONTCHARACTER *fFileName, char *cFileName );

#endif

#define FILENAMEMAX 13
#define FOLDERMAX 9
#define N_LINE 6

#define FOLDER_FLAG       -1
#define FOLDER_SEPALATOR  -2

typedef struct{
	char filename[FILENAMEMAX];
	char folder[FOLDERMAX];
	int filesize;
}Files;

#define FavoritesMAX 7

extern Files *files;
extern char folder[FILENAMEMAX];

extern char FileListUpdate;
extern char StorageMode;		// 0:Storage memory   1:SD		2:MCS		3:SD/MCS

extern char ForceG1Msave;		//    1: force g1m save
extern char AutoSaveMode;		//    1: Auto save ( not pop up )

#define FileCMD_Prog   10000
#define FileCMD_RUN    10001
#define FileCMD_RUN_F1 100011
#define FileCMD_DebugRUN    10002
#define FileCMD_EDIT   10003
#define FileCMD_NEW    10004
#define FileCMD_RENAME 10005
#define FileCMD_DEL    10006
#define FileCMD_COPY   10007
#define FileCMD_TEXT   10008
#define FileCMD_PASS   10009
#define FileCMD_MKDIR  10010
#define FileCMD_RENDIR 10011
#define FileCMD_DELDIR 10016

char *loadFile( const char *name , int editMax, int disperror, int hiddenflag );
int  storeFile( const char *name, unsigned char* codes, int size);
int LoadProgfile( char *name, int prgNo, int editsize, int disperror ) ;
int SaveG1M( char *filebase );
int SaveProgfile(int progNo );

char * Load1st2nd( char *name, char *dir2nd, char *ext );

int SavePicture( char *filebase, int pictNo );
char * LoadPicture( int pictNo );

int SaveCapture( char *filebase, int pictNo );
char * LoadCapture( int pictNo );

int  RenameFile(char *name) ;
void DeleteFile(char *name) ;
void DeleteFileFav(char *sname, int yesno) ;
int DeleteFolder(char *foldername, int yesno ) ;	// delete folder
int CopyFile( char *sname ) ;

void ConvertToText( char *sname );

int NewProg();
void CB_Local( char *SRC ) ;
void CB_ProgEntry( char *SRC ) ; //	Prog "..." into memory

void SaveFavorites();
void SaveConfig();
void LoadConfig();

int CB_IsExist( char *SRC, int calcflag ) ;	//	IsExist("TEST")		//  no exist: return 0     exist: return filesize
char * CB_SaveLoadOprand( char *SRC , int *reg, int *matsize ) ;
void CB_SaveSub( char *sname, char* FilePtr, int size, int check, char* extname ) ;
char * CB_LoadSub( char *sname, int ptr, int *size, char* extname ) ;
void CB_Save( char *SRC ) ; //	Save "TEST",Mat A[1,3]
void CB_Load( char *SRC ) ; //	Load ("TEST" [, Ptr])->Mat A[1,3] 
void CB_Delete( char *SRC ) ;	// Delete "ABC.bin"[,Q]

void Setfoldername16( char *folder16, char *sname ) ;
int CheckPassWord( char *filebase );	// 1:cancel  0:Ok  -1:no pass
void NewPassWord(char *name) ;

void PP_ReplaceCode( char *SRC );
int CheckSD();	// SD model  return : 1

int MakeDirectory();
int RenameDirectory();
int RenameCopyFile( char *fname ,int select ) ;	// select:0 rename  select:1 copy
int RenameCopyFilesBmp( char *fname, char *ext,int select  );	// bmp copy/rename

void SetShortName( char *sname, char *filename) ;	// fullpath filename -> short name
void ErrorMSGfile( char *buffer, char *filename, int err);
void ErrorMSGfiletmp( char *buffer, char *buffer2, int err);
void basname8ToG1MHeader( char *filebase, char *basname) ;	// abcd -> header
void G1MHeaderTobasname8( char *filebase, char *basname) ;	// header -> abcd
void G1M_header( char *filebase ,int *size ) ;
void G1M_Basic_header( char *filebase ) ;

//-----------------------------------------------------------------------------
int DecodeBmp2Vram( char *filebase, int px, int py );	//	bmp -> vram
char * LoadBmp( char *filebase );
int SaveBmp( char *filebase, char *sname, int width, int height );
void CB_BmpSave( char *SRC ) ; //	BmpSave "TEST"[,x][,y][,x2][,y2][,Q]
void CB_BmpLoad( char *SRC ) ; //	BmpLoad("TEST" [, Ptr])->Mat A[1,3] 

//------------------------------------------------------------------------- MCS
int MCS_ReadFileList() ;	// MSC file list -> files
void ShortName2basname( char *basname, char *sname );
void SetBasName( char *basname, char *fname );
char * MCS_LoadG1M( char *fname , int editMax, int disperror, int hiddenflag  ) ;	// MCS -> g1m file
int MCS_SaveG1M( char *filebase ) ;	// g1m file -> MCS
int MCS_DeleteG1M( char *fname  ) ;	// delete g1m
int MCS_ExistFile( char *fname  ) ;	// exist?
int MCS_Free();
char * MCS_LoadCAPT( char *pictname ) ;
char * MCS_LoadPICT( char *pictname ) ;
int MCS_SaveCAPT( char *pictname, char *filebase  ) ;
int MCS_SavePICT( char *pictname, char *filebase  ) ;
