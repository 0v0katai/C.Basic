//------------------------------------------- source code refer to (WSC) file.c
#ifndef _FILE_H_
#define _FILE_H_

#include "fxlib.h"

/* return a filename */
unsigned int SelectFile (char *filename);

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


unsigned char *loadFile( const char *name , int editMax);
int  storeFile( const char *name, unsigned char* codes, int size);
int LoadProgfile( char *name ) ;
int SaveG1M( unsigned char *filebase );
int SaveProgfile(int progNo );

int SavePicture( unsigned char *filebase, int pictNo );
unsigned char * LoadPicture( int pictNo );

void DeleteFile(char *name) ;
int NewProg();
void CB_ProgEntry( unsigned char *SRC ) ; //	Prog "..." into memory
