// syscalls are defined in syscall.hpp and syscall.cpp
// this file is used for derived functions

#ifndef __MEMVIEW_CPP__
#define __MEMVIEW_CPP__
#ifdef __cplusplus
extern "C" {
#endif

// #include "base.hpp"

#include "keyboard.hpp"
#include "disp_tools.hpp"
#include "display_tools.h"
#include "MEMVIEW2.hpp"
// #include incTOOLS

////////// TMemView //////////

// static const TFKDef FKD[] = {  FKS_ASCII, FKS_HEX, FKS_EXIT, { 0, 0 } };
// static const int FKD5[] = { fksfCYCLE, FK_HEX, FK_ASCII, 0 };
// static const int FKD6[] = { fksfNORMAL, FK_EXIT, 0 };

// extern TFKeyRegister FKeyRegInst;

//
void TMemView::SetRecurrent( const int recurrent ){
  FRecurrent = recurrent;
};

//
int TMemView::FShowMemoryBlock(){
int workaddr = FBaseAddress + FOffset;
int lReverse;
char work[40];
int i, j;
unsigned char c;
//  LongToAscHex( workaddr>>16, (unsigned char*)work, 4 );
  locate_OS(18,1);
  Print((unsigned char*)work);
//  Bdisp_SetPoint_VRAM( 13*6, 7, 1 );
    
  for (i=2;i<8;i++){
//    LongToAscHex( workaddr, (unsigned char*)work, 4 );
    locate_OS(1,i);
    Print( (unsigned char*)work ); Print( " " );
    for (j=0;j<8;j++){
	/*
      switch ( FViewMode ){
        case FK_HEX : 
//          LongToAscHex( *(char*)workaddr, (unsigned char*)work, 2 );
          break;
        case FK_ASCII : 
          work[0] = ' ';
          c = *(unsigned char*)workaddr;
          if ((c<0x20)||(c>0x7E)){work[1] = '.';}
          else{work[1] = c;};
          work[2]=0;
          break;
      };
	  */
      lReverse = 0;
      if( FMemWindowEnd ){
        if( FMemWindowEnd < workaddr ){lReverse=1;}
        else if ( FMemWindowStart > workaddr ){lReverse=1;};
      };
//      if (lReverse) PrintR( (unsigned char*)work );
//      else Print( (unsigned char*)work );
      
      workaddr++;
    }
  };  
 
//  Bdisp_PutDisp_DD();
  return 0;
}

//
int TMemView::SetMemWindow( const int base, const int start, const int end, const int recurrent ){
  FBaseAddress = base;
  FMemWindowStart = start;
  FMemWindowEnd = end;
  FRecurrent = recurrent;
  return 0;
};

//
int TMemView::browse( const void*base ){
unsigned int key;
int lContinue = 1;
int i;
/*
int*test;
int*test2;
*/
// TInput inp;

int VRAMbuffer[0x100];
//  FastVRAMSave( (int*)&VRAMbuffer, 1 );
  
  if ( base ){ FBaseAddress = (int)base; };
  
  FOffset = 0;
  
  i = 0;
  while( 1 ){
//    if (FKD[i].fk_no == 0 ) break;
//    FKeyRegInst.RegisterFKey( (TFKDef*)&FKD[i++] );
  };  
//  inp.SetFKey( 5, (int*)&FKD5 );
//  inp.SetFKey( 6, (int*)&FKD6 );
  
//  inp.ShowFKeys();
  

 /*
  test2 = (int*)malloc( 1 );
  test = (int*)malloc( 100 );
  for (i=0;i<25;i++)test[i]=0x55550000 + i;
  locate_OS(1,1); PrintHex( (int)test, 8 );
  locate_OS(1,2); PrintHex( (int)test2, 8 ); Wait();
*/  
  
//  locate_OS(1,1); Print("addr: ");PrintHex( FBaseAddress, 8 );
  
  while ( lContinue ){
    key = 0;
    while( !key ){
      FShowMemoryBlock();
//      if (FRecurrent) key = inp.keypressed();
//      else key = inp.getkey();
    };
    switch ( key ){
      case KEY_CTRL_UP : FOffset -= 0x08; break;
      case KEY_CTRL_DOWN : FOffset += 0x08; break;
      case KEY_CTRL_RIGHT : FOffset += 0x28; break;
      case KEY_CTRL_LEFT : FOffset -= 0x28; break;
//      case FK_HEX : 
      // case FK_ASCII : 
        // FViewMode = key;
        // break;
      // case FK_EXIT :
      case KEY_CTRL_QUIT :
      case KEY_CTRL_EXIT : lContinue = 0; break;
    }
  };

//  KBD_Clear(); 
/*  
  free( test );
  free( test2 );
*/
//  FastVRAMLoad( (int*)&VRAMbuffer, 1 );
  return 0;
};

void TMemView::Draw(  int mode  )
{};

////////// TMemView //////////

#ifdef __cplusplus
}
#endif
#endif