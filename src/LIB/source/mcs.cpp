// syscalls are defined in syscall.hpp and syscall.cpp
// this file is used for derived functions

#ifndef __MCS_CPP__
#define __MCS_CPP__
#ifdef __cplusplus
extern "C" {
#endif

#include "APP_syscalls.h"
#include "MCS_syscalls.h"
#include "mcs.hpp"
#include "STRLIST.hpp"
#include "keyboard.hpp"
#include "disp_tools.hpp"
#include "STATUS_ICONS.hpp"
#include "MemViewClass.hpp"

//
int CheckMainMemory( unsigned char*dir, unsigned char*item ){
unsigned char default_dir[9];
int len;
	if ( dir == 0 ){
		GetAppName( default_dir );
		dir = default_dir;
	};
	return MCSGetDlen2( dir , item, &len );
}

//
int DeleteMainMemory( unsigned char*dir, unsigned char*item ){
unsigned char default_dir[9];
	if ( dir == 0 ){
		GetAppName( default_dir );
		dir = default_dir;
	};
	return MCSDelVar2( dir , item );
}

//
int ReadMainMemory( unsigned char*dir, unsigned char*item, int len, void*buffer ){
unsigned char default_dir[9];
int result;
	if ( dir == 0 ){
		GetAppName( default_dir );
		dir = default_dir;
	};
	result =MCSGetDlen2( dir , item, &len );
	if ( !result ) result = MCSGetData1( 0, len, buffer );
	return result;
}

//
int WriteMainMemory( unsigned char*dir, unsigned char*item, int len, void*buffer ){
unsigned char default_dir[9];
int result;
	if ( dir == 0 ){
		GetAppName( default_dir );
		dir = default_dir;
	};
	result = MCSPutVar2( dir, item, len, buffer );
	if ( result ) MCSOvwDat2( dir, item, len, buffer, 0 );
	return result;
}

/*

// INTERFACE
char *GetAppName(char *dest);

int MCS_CreateDirectoryH( unsigned char* );
int iMCS_DeleteDirectory( unsigned char*dir );	// if the directory is not empty, use MCS_DeleteDirectoryItems beforehand.
int MCS_DeleteDirectoryItems( unsigned char*dir );

int MCSPutVar2( unsigned char*dir, unsigned char*item, int data_len, void*buffer );
int MCSOvwDat2( unsigned char*dir, unsigned char*item, int bytes_to_write, void*buffer, int write_offset );
int MCSDelVar2( unsigned char*dir, unsigned char*item );
int MCSGetDlen2( unsigned char*dir, unsigned char*item, int*data_len );
int MCSGetData1( int offset, int data_len, void*buffer );

// IMPLEMENTATION
	SYSCALL 0366,	_iMCS_DeleteDirectory
	SYSCALL 0462,	_GetAppName
	SYSCALL 0829,	_MCS_CreateDirectoryH
	SYSCALL 082B,	_MCSPutVar2
	SYSCALL 0830,	_MCSOvwDat2
	SYSCALL 0836,	_MCSDelVar2
	SYSCALL 0840,	_MCSGetDlen2
	SYSCALL 0841,	_MCSGetData1
	SYSCALL 086F,	_MCS_DeleteDirectoryItems
	
*/	

////////// TMCSView //////////
//

//
TMCSView::~TMCSView(){
}

//
int TMCSView::FOnCreate(){
int i;
	i = 0;
//	SetFKey( 0, "DEL", FKT_STRING );
	SetFKey( 5, "SWAP", FKT_STRING );
	PViewMode = mvmLEN;

	return 0;
};

//
int TMCSView::vBeforeBrowse(){
int result=TListView::vBeforeBrowse();
  FCurrentItemId = FSize;
  return result;
}

//
int TMCSView::vSourceBOL() const{
  return FCurrentItemId == FSize;
}

//
int TMCSView::vSourceEOL() const{
  return FCurrentItemId == 1;
}

//
int TMCSView::vPrevLine(){
	FCurrentItemId++;
	return FSource[FCurrentItemId-1].name[0];
};

//
int TMCSView::vNextLine(){
	FCurrentItemId--;
	return FSource[FCurrentItemId-1].name[0];
};

//
int TMCSView::vSourceGetCurrentItem( TListViewItem w ){
unsigned char w2[10];
int i;
  memset( w, 0, sizeof( TListViewItem ) );
  strncpy( w, (char*)FSource[FCurrentItemId-1].name, 8 );
  for ( i = 8; i >= 0; i-- ){
    if ( w[i]==0 ) w[i]=' ';
    else break;
  }
  
  switch ( PViewMode ){
    case mvmADDR :
      LongToAscHex( (int)FSource[FCurrentItemId-1].addr, w2, 8 );
      break;
      
    case mvmLEN :
      LongToAscHex( FSource[FCurrentItemId-1].itemcount, w2, 4 );
      strcat( w, (char*)w2 );
      strcat( w, " " );
  
      LongToAscHex( FSource[FCurrentItemId-1].wossname, w2, 4 );
      break;
  }   
  strcat( w, (char*)w2 );
  return strlen( w );
};

//
int TMCSView::vSourceJump( TListViewJump dir ){
  switch ( dir ){
    case lvjTOP : FCurrentItemId = FSize; break;
    case lvjBOTTOM : FCurrentItemId = 1; break;
  };  
  return 0;
};

//
int TMCSView::FSubView() const{
char w[21];
TMCSItemView miv( 1,1,21,9 );
	
	miv.InitArray( FSource[FCurrentItemId-1].addr, FSource[FCurrentItemId-1].itemcount );

	memset( w, 0, sizeof( w ) );
	strcpy( w, "MCS:" );
	//now strncpy, not strcat! The item-names are not zero-terminated if of size 8!
	strncpy( w+4, (char*)FSource[FCurrentItemId-1].name, 8 );
	miv.SetHeading( w );
	miv.parent = &FSource[FCurrentItemId-1];
	miv.browse();

	return miv.FMessage.message;
}

//
// int TMCSView::FDelete(){
// int result = 0;
// unsigned char wdir[9];
// unsigned char smsg[100];
// 	memset( wdir, 0, 9 );
// 	strncpy( (char*)wdir, (char*)FSource[FCurrentItemId-1].name, 8 );
	
// 	memset( smsg, 0, 100 );
// 	StrListCat( smsg, "Delete MCS directory?" );
// 	StrListCat( smsg, wdir );
	
// 	if ( MessageDlg( (char*)smsg, mtConfirmation, 4 ) ){
// 	/*
// 		result = MCS_ClearDirectory( wdir );	// this clears the MCS-handles
// 		result = iMCS_DeleteDirectory( wdir );
// 		if ( result ){
		
// 			memset( smsg, 0, 100 );
// 			StrListCat( smsg, "MCS error" );
// 			LongToAscHex( result, (unsigned char*)wdir, 4 );
			
// 			StrListCat( smsg, wdir );
			
// 			MessageBox( smsg, 0 );
		
// 			result = 0;
// 		} result = 1;
// 		*/
// 	}
// 	return result;
// }

//
int TMCSView::vShowAll(){
int result = TListView::vShowAll();
	if ( PViewMode == mvmADDR ) SetStatusIcon( sticADDR );
	else SetStatusIcon( sticLEN );

	return result;
};


//
int TMCSView::vProcessMessage(){
int result = 1;
	switch ( FMessage.message ) {
		case lvmKEY +  KEY_CTRL_F1 :
			if ( FDelete() ) Refresh();
			break;
		case lvmKEY +  KEY_CTRL_EXE: 
			if ( FSource[FCurrentItemId-1].itemcount ){ 
			  FSubView();
			  Refresh();
			}
			break;
		case lvmKEY +  KEY_CTRL_F6: 
			if ( PViewMode == mvmADDR ) PViewMode = mvmLEN;
			else PViewMode = mvmADDR;
			Refresh();
			break;
		default : result = 1;
	};
	return result;
};

////////// TMCSView //////////

////////// TMCSItemView //////////
//
TMCSItemView::~TMCSItemView(){
}

//
int TMCSItemView::FOnCreate(){
	FSource=0;
	FSize=0; 
//	SetFKey( 0, "DEL", FKT_STRING );
	SetFKey( 1, "VIEW", FKT_STRING );
	SetFKey( 5, "SWAP", FKT_STRING );
	
	PViewMode = mvmLEN;
	parent = 0;
	
	return 0;
}

int TMCSItemView::InitArray( TMCSItem*pitem, int size ){
  FSize = size;
  FSource = pitem;
  return 0;
};

//
int TMCSItemView::vBeforeBrowse(){
int result=TListView::vBeforeBrowse();

  FCurrentItemId = 1;
  return result;
}

//
int TMCSItemView::vSourceBOL() const{
  return FCurrentItemId == 1;
}

//
int TMCSItemView::vSourceEOL() const{
  return FCurrentItemId == FSize;
}

//
int TMCSItemView::vPrevLine(){
	FCurrentItemId--;
	return 1;
};

//
int TMCSItemView::vNextLine(){
	FCurrentItemId++;
	return 1;
};

//
int TMCSItemView::vSourceGetCurrentItem( TListViewItem w ){
unsigned char w2[10];
int i;
  memset( w, 0, sizeof( TListViewItem ) );
  strncpy( w, (char*)(FSource[FCurrentItemId-1].name), 8 );
  for ( i = 8; i >= 0; i-- ){
    if ( w[i]==0 ) w[i]=' ';
    else break;
  }
  
  switch (PViewMode){
    case mvmLEN : 
      LongToAscHex( FSource[FCurrentItemId-1].length, w2, 5 );	// 1.01
      strcat( w, (char*)w2 );
      strcat( w, " " );
      LongToAscHex( FSource[FCurrentItemId-1].flags[0], w2, 2 );
      strcat( w, (char*)w2 );
      strcat( w, " " );
      LongToAscHex( FSource[FCurrentItemId-1].flags[1], w2, 2 );
      break;
      
    case mvmADDR :
      LongToAscHex( (int)FSource[FCurrentItemId-1].offset+(int)(*parent).addr, w2, 8 );
      break;
  }    
  strcat( w, (char*)w2 );
  
  return strlen( w );
}

//
int TMCSItemView::vSourceJump( TListViewJump dir ){
  switch ( dir ){
    case lvjTOP : FCurrentItemId = 1; break;
    case lvjBOTTOM : FCurrentItemId = FSize; break;
  };  
  return 0;
};

//
int TMCSItemView::FView(){
unsigned int start_address;  
TMemView mv;
	start_address = (int)FSource + (int)FSource[FCurrentItemId-1].offset;
	mv.SetAddress( start_address );
	mv.browse();
	return 0;
}

//
// int TMCSItemView::FDelete(){
// int result = 0;
// unsigned char wdir[9], witem[9];
// unsigned char smsg[100];
// 	memset( wdir, 0, 9 );
// 	memset( witem, 0, 9 );

// 	strncpy( (char*)wdir, (char*)(*parent).name, 8 );
// 	strncpy( (char*)witem, (char*)FSource[FCurrentItemId-1].name, 8 );
		
// 	memset( smsg, 0, 100 );
// 	StrListCat( smsg, "Delete MCS item?" );
// 	StrListCat( smsg, wdir );
// 	StrListCat( smsg, witem );
	
// 	if ( MessageDlg( (char*)smsg, mtConfirmation, 4 ) ){

// /*	
// 		result = MCSDelVar2( wdir, witem );
// 		if ( result ){
// 			memset( smsg, 0, 100 );
// 			StrListCat( smsg, "MCS error" );
// 			LongToAscHex( result, (unsigned char*)wdir, 4 );
// 			StrListCat( smsg, wdir );
// 			MessageBox( smsg, 0 );
// 			result = 0;
// 		}else{
// 			FSize--;
// 			result = 1;
// 		}
// 	*/
// 	}
// 	return result;
// }

//
int TMCSItemView::vProcessMessage(){
int result = 1;
	switch ( FMessage.message ) {
		case lvmKEY +  KEY_CTRL_F1 :
			FDelete();
			if ( (*parent).itemcount ) { Refresh( FLAG_RESET ); } else { result = 0; };
			break;
		case lvmKEY +  KEY_CTRL_EXE: 
			break;
		case lvmKEY +  KEY_CTRL_F2: 
			FView();
			Refresh();
			break;
		case lvmKEY +  KEY_CTRL_F6: 
			if ( PViewMode == mvmADDR ) PViewMode = mvmLEN;
			else PViewMode = mvmADDR;
			Refresh();
			break;
		default : result = 1;
	};
	return result;
};

////////// TMCSItemView //////////

#ifdef __cplusplus
}
#endif
#endif