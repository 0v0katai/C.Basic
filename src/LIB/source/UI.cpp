// syscalls are defined in syscall.hpp and syscall.cpp
// this file is used for derived functions

#ifndef __UI_CPP__
#define __UI_CPP__
extern "C" {

#include "UI.HPP"

TFKeyRegister FKeyRegInst;

////////// TFKeyRegister //////////
//
int TFKeyRegister::RegisterFKey( TFKDef*fk_def ){
int result=0;
int i;
  if ( !FRegisteredFKeys ){
    FKeyDefCount = 1;
    FRegisteredFKeys = (TFKDef*)malloc( sizeof( TFKDef )*2  );
    FRegisteredFKeys[0] = *fk_def;
    FRegisteredFKeys[1].fk_no  = 0;
    result = 1; // ready
  }    
  if ( !result ){
    if( FKeyDefCount ){
      for (i=0;i<FKeyDefCount;i++){
        if ( (*fk_def).fk_no == FRegisteredFKeys[i].fk_no  ){
          result = FKeyDefCount; // fk_no already present
        }
      }
      if ( !result ){
        FKeyDefCount++;
        FRegisteredFKeys = (TFKDef*)realloc( FRegisteredFKeys, (FKeyDefCount+1)*sizeof( TFKDef )  );
        FRegisteredFKeys[ FKeyDefCount - 1 ] = *fk_def;
        FRegisteredFKeys[ FKeyDefCount ].fk_no = 0;
        result = FKeyDefCount;
      }
    }  
  }  
  return result;
};

//
int TFKeyRegister::Count(){
  return FKeyDefCount;
};

//
unsigned char*TFKeyRegister::FKEY_Ptr( int fk_num ){
unsigned char*result=0;
int idx;

  if ( fk_num < 0x8000 ){
    result = FKeyPtr( fk_num );
  }else{
    if ( FKeyDefCount ){
      idx = 0;
      while (1){
        if ( FRegisteredFKeys[ idx ].fk_no ){
          if ( FRegisteredFKeys[ idx ].fk_no == fk_num )break;
        }else{
          idx = -1;
          break;
        };
        idx++;
      };
      if (idx>=0){
        result = FRegisteredFKeys[ idx ].fk_bitmap;
      }
    }
    if (!result) result = FKeyPtr( FK_DBLCROSS );
  };
  return result;
}
////////// TFKeyRegister //////////

////////// TInput //////////
//
TInput::TInput(){
int i;
  for (i=0;i<6;i++){
    FKey[i] = 0;
    FSubFuncNo[i] = 1;
  };
};

//
TInput::~TInput(){
}

//
void TInput::SetFKey(  const int fk_no, int*fk_nums  ){
  FKey[ fk_no - 1 ] = fk_nums;
};

//
void TInput::ShowFKeys() const{
int i;
  for (i=0;i<6;i++){
    if ( FKey[i] ){
      Print_FKeyDescr( i, FKeyRegInst.FKEY_Ptr( FKey[i][FSubFuncNo[i]] ) );
    };
  };    
};

//
int TInput::SetFKeyState( const int fk_no, const int value ){
         FSubFuncNo[fk_no - 1] = value;
	return FSubFuncNo[fk_no - 1];
}

//
unsigned int TInput::FProcessKey( unsigned int key ){
unsigned int result = key;
int i, lChanged = 0;
int idx = -1;
  switch ( result ){
    case KEY_CTRL_F1 : idx = 0;  break;
    case KEY_CTRL_F2 : idx = 1;  break;
    case KEY_CTRL_F3 : idx = 2;  break;
    case KEY_CTRL_F4 : idx = 3;  break;
    case KEY_CTRL_F5 : idx = 4;  break;
    case KEY_CTRL_F6 : idx = 5;  break;
  }
  if ( idx != -1 ){
    if ( FKey[ idx ] ){
      switch ( (TFKeySubFunc)(FKey[ idx ][0]) ){
        case fksfNEXTMENU :
          for (i=0;i<6;i++){
            if ( FKey[ i ] ){
              if ( FKey[ i ][0] == fksfNORMAL ){
                FSubFuncNo[ i ]++;
                if ( FKey[ i ][ FSubFuncNo[ i ] ] == 0 ) FSubFuncNo[ i ] = 1;
                lChanged = 1;
              }  
            }  
          }  
          break;
          
        case fksfCYCLE :
          FSubFuncNo[idx]++;
          if (FKey[idx][FSubFuncNo[idx]] == 0 ) FSubFuncNo[idx] = 1;
          lChanged = 1;
          break;
      }    
      if (lChanged)ShowFKeys();
      result = FKey[idx][FSubFuncNo[idx]];
    }
  }
  return result;
};

//
unsigned int TInput::getkey(){
unsigned int key;
	if ( flags & ifALWAYS_ALPHA ) Setup_SetEntry( 0x14, 0x04 );
	GetKey( &key );
	return FProcessKey( key );
};

//
unsigned int TInput::keypressed(){
unsigned int key;
	if ( flags & ifALWAYS_ALPHA ) Setup_SetEntry( 0x14, 0x04 );	// ALPHA ON
	key = KeyPressed();
	return FProcessKey( key );
}    

////////// TInput //////////

}
#endif