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


char *loadFile( const char *name , int editMax);
int  storeFile( const char *name, unsigned char* codes, int size);
int LoadProgfile( char *name, int searchProg ) ;
int SaveG1M( char *filebase );
int SaveProgfile(int progNo );

int SavePicture( char *filebase, int pictNo );
char * LoadPicture( int pictNo );

int  RenameFile(char *name) ;
void DeleteFile(char *name) ;
void DeleteFileFav(char *sname) ;

void ConvertToText( char *sname );

int NewProg();
void CB_Local( char *SRC ) ;
void CB_ProgEntry( char *SRC ) ; //	Prog "..." into memory

void SaveFavorites();
void SaveConfig();
void LoadConfig();

int CB_IsExist( char *SRC ) ;	//	IsExist("TEST")		//  no exist: return 0     exist: return filesize
void CB_Save( char *SRC ) ; //	Save "TEST",Mat A[1,3]
void CB_Load( char *SRC ) ; //	Load ("TEST" [, Ptr])->Mat A[1,3] 
