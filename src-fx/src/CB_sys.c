#include "CB.h"

int Bfile_OpenFile_OS(const unsigned short *filename, int mode, int null);
int Bfile_DeleteEntry( const unsigned short *filename );
int Bfile_CreateEntry_OS( const unsigned short*filename, int mode, int *size);
int Bfile_GetFileSize_OS(int handle);
int Timer_Install( int InternalTimerID, void*handler, int elapse );
int Timer_Start( int InternalTimerID );
int Timer_Deinstall( int InternalTimerID );
int Timer_Stop( int InternalTimerID );

//--------------------------------------------------------------------------------------------- Bfile
int Bfile_OpenFile(const FONTCHARACTER *filename, int mode){
	return Bfile_OpenFile_OS( filename, mode, 0 );
}
int Bfile_DeleteFile(const FONTCHARACTER *filename){
	return Bfile_DeleteEntry( filename);
}
int Bfile_DeleteDirectory(const FONTCHARACTER *pathname){
	return Bfile_DeleteEntry( pathname);
}
int Bfile_CreateFile(const FONTCHARACTER *filename, int size){
	// refer to http://www.casiopeia.net/forum/viewtopic.php?f=11&t=1740&start=50#p14530
	return Bfile_CreateEntry_OS(  filename, 1, &size  );	// file
}
int Bfile_CreateDirectory(const FONTCHARACTER *pathname){
	int size=0;
	return Bfile_CreateEntry_OS(  pathname, 5, &size  );	// Directory
}
int Bfile_GetFileSize(int HANDLE){
	return Bfile_GetFileSize_OS( HANDLE );
}

int SetTimer(int ID, int elapse, void (*handler)(void)){
	int id;
	id=Timer_Install( ID,  (void*)handler, elapse ); 
	Timer_Start( id );
	return id;
}

int KillTimer(int ID){
	Timer_Stop( ID );
	return Timer_Deinstall( ID );
}
