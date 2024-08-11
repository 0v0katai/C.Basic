#ifndef __LISTVIEW_CPP__
#define __LISTVIEW_CPP__
extern "C" {

#include "ListView.hpp"
#include "disp_tools.hpp"
#include "display_tools.h"
#include "fxCG_Display.h"
#include "display_syscalls.h"
#include "keyboard.hpp"
#include "keyboard_syscalls.h"
#include "CONVERT_syscalls.h"
#include "STATUS_ICONS.hpp"

#define FK_TOP 123
#define FK_BOT 124
#define FK_EXIT 125

/*	
#include incMEMVIEW
static TMemView*mv; 
    mv = new TMemView;  
    (*mv).browse( &FSource );  
    delete mv;
*/ 

// static const TFKDef FKD[] = { FKS_EXIT, { 0, 0 } };
// static const int FKM3[] = { fksfNORMAL, FK_TOP, 0 };
// static const int FKM4[] = { fksfNORMAL, FK_BOT, 0 };
// static const int FKM6[] = { fksfNORMAL, FK_EXIT, 0 };

// extern TFKeyRegister FKeyRegInst;

////////// TListView //////////
/*
TListView is the base class for a universal scrollable data-viewer
*/

//
TListView::~TListView(){
#ifdef __LISTVIEW_DEBUG__
OBJECT_DEBUG("TListView.constr")
#endif
TStatusGlyphDefinition sgd;
	sgd.dx = STICON_WIDTH;
	sgd.dy = STICON_HEIGHT;
	sgd.xalignment = 2;
	sgd.xoffset = 1;

	sgd.shortcolor_bitmap = GetStatusIconPtr( sticEMPTY );
	DefineStatusGlyph( 0, &sgd );
}

//
int TListView::vSourceMove( int diff ){
int result=0;
  if (diff<0){
    while ( !vSourceBOL() ){
	  if ( vPrevLine() ){
		diff++;
		result--;
	}
      if ( diff==0 ){ break; };
    };
  }else if (diff > 0){
    while ( !vSourceEOL() ){
	  if ( vNextLine() ){
      diff--;
      result++;
	  }
      if ( diff==0 ){ break; };
    };
  };
  return result;
}

//
int TListView::vOnShowCurrentItem(){
  return 0;
};

#define XOFFS 4
#define DX 6
#define YOFFS 24+2
#define DY 18

//
void TListView::locate( int x, int y ){
	FX = XOFFS+(x-1)*DX;
	FY = YOFFS+(y-1)*DY;
};

#define UPPER_X_LIMIT PMF_UPPER_X_LIMIT_IGNORED

//
void TListView::Print( const unsigned char* w ){
int x=FX, y=FY;	
unsigned char xline[100];
	xline[0] = 0;
	strcat( (char*)xline, " " );
	strcat( (char*)xline, (char*)w );
	strcat( (char*)xline, " " );
	PrintMiniFix( x, y, (char*)xline, PMF_ALLOW_STATUSAREA, 0x0000, 0xFFFF );
//	PrintMini( &x, &y, xline, PMF_ALLOW_STATUSAREA, UPPER_X_LIMIT, (int)0, (int)0, (int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
};

//
void TListView::PrintR( const unsigned char* w ){
int x=FX, y=FY;	
unsigned char xline[100];
	xline[0] = 0;
	strcat( (char*)xline, " " );
	strcat( (char*)xline, (char*)w );
	strcat( (char*)xline, " " );
	PrintMiniFix( x, y, (char*)xline, PMF_ALLOW_STATUSAREA|PMF_INVERT, 0x0000, 0xFFFF );
//	PrintMini( &x, &y, xline, PMF_ALLOW_STATUSAREA|PMF_INVERT, UPPER_X_LIMIT, (int)0, (int)0, (int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
};

//
void TListView::FShowLine( const int iLine, int reverse ){
TListViewItem w;
	vSourceGetCurrentItem( w );
	locate( FMinX, iLine );
	if (reverse){ 
		PrintR( (unsigned char*)w );
		vOnShowCurrentItem();
	}else{ Print( (unsigned char*)w ); };
}

//
void TListView::FShowAll(){
int i;
int offset;
TBdispFillArea dsb;
int OldCurrentItemId = FCurrentItemId;
unsigned char wh[0x55];
	vShowAll();

	if ( FFlags & FLAG_RESET ){
		vSourceJump( lvjTOP );
		FYDisp = FMinY; 
		FFlags &= ~FLAG_RESET;
		FCurrentItemId == 1;
		OldCurrentItemId = FCurrentItemId;
		offset = 0;
	}else{ offset = vSourceMove( FMinY - FYDisp ); }
	
	dsb.x1 = 0;
	dsb.y1 = 24;
	dsb.x2 = 383;
	dsb.y2 = 215;
	dsb.mode = 1;
	Bdisp_AreaClr( &dsb, 1, 0xF81F );
	
	for ( i = FMinY; i <= FMaxY; i++ ){ 
	    FShowLine( i ); 
	    if ( vSourceMove( 1 ) ) offset++;
	    else break;
	};
	vSourceMove( -offset );
	FShowLine( FYDisp, 1 );
	FCurrentItemId = OldCurrentItemId;
	if ( FFlags & FLAG_SHOWFKEYS ) ShowFKeys();
	SetStatusMessage( FHeading );
	FFlags &= ~FLAG_REFRESH;
}    

//
int TListView::SetFYDisp( int value ){
	if ( value > FMinY ) FYDisp = value;
	else FYDisp = FMinY;
	return FYDisp;
};


//
int TListView::vGetMessage( TMessage&msg ){
int result=1;
	msg.message = 0;
	return result;
}

//
int TListView::vProcessMessage(){
int result=1;
	return result;
}

//
void TListView::Refresh( int mode ){
  FFlags |= FLAG_REFRESH | mode;
};

//
int TListView::FProcessMessage(){
int diff;
int result=1;

  switch ( FMessage.message ) {
    case lvmINIT :
      Refresh();  
      break;
      
    case lvmKEY +  KEY_CTRL_UP:
      diff = vSourceMove( -1 );
      if (diff){
        if ( FYDisp > FMinY ){ FYDisp--; };
        Refresh();  
      };  
      break;
      
    case lvmKEY +  KEY_CTRL_DOWN:
      diff = vSourceMove( 1 );
      if (diff){
        if ( FYDisp < FMaxY ){ FYDisp++; };
        Refresh();  
      }  
      break;
      
    case lvmKEY +  KEY_CTRL_LEFT:
      diff = vSourceMove( FMinY - FMaxY - 1 );
      if (diff){
        FYDisp += diff;
        if ( FYDisp < FMinY ) FYDisp = FMinY;
        Refresh();  
      };  
      break;
      
    case lvmKEY +  KEY_CTRL_RIGHT:
      diff = vSourceMove( FMaxY - FMinY + 1 );
      if (diff){
        FYDisp += diff;
        if ( FYDisp > FMaxY ) FYDisp = FMaxY;
        Refresh();  
      }  
      break;
      
    case lvmKEY +  FK_TOP:
      vSourceJump( lvjTOP );
      FYDisp = FMinY; 
      Refresh();  
      break;
      
    case lvmKEY +  FK_BOT:
      vSourceJump( lvjBOTTOM );
      diff = vSourceMove( FMinY - FMaxY );
      vSourceJump( lvjBOTTOM );
      FYDisp = -diff + FMinY; 
      Refresh();  
      break;
      
    case lvmKEY +  FK_EXIT:
    case lvmKEY +  KEY_CTRL_EXIT:
    case lvmKEY +  KEY_CTRL_QUIT:
	result=0;  
	break;
      
    case lvmKEY +  KEY_CTRL_EXE:
	  if ( FCallback ){
		vGetMessage( FMessage );
		result = FCallback( FMessage );
	  }else{
	    result = vProcessMessage();  ;
	  }  
      break;
      
    default : result = vProcessMessage();
  }
  if ( FFlags & FLAG_REFRESH ) FShowAll();  
  return result;
}

//
int TListView::FKeyDefined( int FKeyNo ){
	switch ( FKeyNo ){
		case lvmKEY +  KEY_CTRL_F1 :
		case KEY_CTRL_F1 :
			FKeyNo = 0;
			break;
		case lvmKEY +  KEY_CTRL_F2 :
		case KEY_CTRL_F2 :
			FKeyNo = 1;
			break;
		case lvmKEY +  KEY_CTRL_F3 :
		case KEY_CTRL_F3 :
			FKeyNo = 2;
			break;
		case lvmKEY +  KEY_CTRL_F4 :
		case KEY_CTRL_F4 :
			FKeyNo = 3;
			break;
		case lvmKEY +  KEY_CTRL_F5 :
		case KEY_CTRL_F5 :
			FKeyNo = 4;
			break;
		case lvmKEY +  KEY_CTRL_F6 :
		case KEY_CTRL_F6 :
			FKeyNo = 5;
			break;
	}
	if ( FKeyNo < 0 ) return 0;
	if ( FKeyNo > 5 ) return 0;
	return (int)FFKeys[ FKeyNo ].p;
}

//
int TListView::ShowFKeys( void ){
int idx, x, y, dx;
void*ptr;
TBdispFillArea dsb;
	for (idx=0;idx<=5;idx++){
		if ( FFKeys[ idx ].p ){
			switch ( FFKeys[ idx ].fkt ){
				case FKT_BITMAP :
					FKey_Display( idx, FFKeys[ idx ].p );
					break;
				case FKT_STRING :
					dsb.x1 = idx * 0x40;
					dsb.y1 = 216 - 24;
					dsb.x2 = dsb.x1 + 0x40;
					if ( idx == 5 ) dsb.x2--;
					dsb.y2 = 215;
					dsb.mode = 1;
					Bdisp_AreaClr( &dsb, 1, 0xFFFF );
					
					dsb.x1 = dsb.x1 + 1;
					dsb.y1 = dsb.y1 + 1;
					dsb.x2 = dsb.x2 - 2;
					dsb.y2 = dsb.y2 - 1;
					dsb.mode = 1;
					Bdisp_AreaClr( &dsb, 1, 0x0000 );
					
					Bdisp_SetPoint_VRAM( dsb.x1, dsb.y1, 0xFFFF );
					Bdisp_SetPoint_VRAM( dsb.x1, dsb.y2, 0xFFFF );
					Bdisp_SetPoint_VRAM( dsb.x2, dsb.y1, 0xFFFF );
					Bdisp_SetPoint_VRAM( dsb.x2, dsb.y2, 0xFFFF );
				
					dx = 2 + ( 0x38 - 12*strlen( (char*)FFKeys[ idx ].p ) ) / 2;
					if ( dx < 2 ) dx = 2;
					x = dsb.x1 + dx;
					y = 216-24+4;
					PrintMini( &x, &y, (unsigned char*)FFKeys[ idx ].p, PMF_ALLOW_STATUSAREA, 
				               x + 0x40, (int)0, (int)0, (int)0xFFFF, (int)0x0000, (int)1,  (int)0 );

					break;
			}
		}
	}	
	return 0;
}

//
int TListView::SetFKey( int idx, const void*value, TFKeyType _fkt ){
	if (( idx >= 0 ) && ( idx <= 5 )){
		FFKeys[ idx ].p = (void*)value;
		FFKeys[ idx ].fkt = _fkt;
	}
	else for (idx=0;idx<=5;idx++) FFKeys[ idx ].p = 0;
	FFlags = FFlags & ~FLAG_SHOWFKEYS;
	for (idx=0;idx<=5;idx++) if ( FFKeys[ idx ].p ){
		FFlags = FFlags | FLAG_SHOWFKEYS;
		break;
	}
	return 0;
}

//
int TListView::vBeforeBrowse(){
	FMessage.message = lvmINIT;
	if ( FHeading ){
		SetStatusMessage( FHeading );
	};
	return 1;
}

//
int TListView::browse(){
int key;
	if ( vBeforeBrowse() ){
		while ( FProcessMessage() ){
			GetKey( &key );
			FMessage.message = lvmKEY + key;
		}
	} else FMessage.message =  lvmEMPTY_LIST;
	return FMessage.message;
};

//
int TListView::vShowAll(){
	return 0;
};

//
int TListView::SetHeading( char* heading ){
int result=1;
  strcpy( FHeading, heading );
  return result;
};

//
void TListView::SetCallback( TListviewCallback callback ){
  FCallback = callback;
};
////////// TListView //////////

////////// TIntArrayView //////////
/*
TIntArrayView is class for a universal scrollable data-viewer
*/
//
TIntArrayView::~TIntArrayView(){
  if ( FSource ){ free( FSource );};
}

int TIntArrayView::InitArray( int size, int itemsize ){
  FSize = size;
  FSource = (int*)malloc( FSize*itemsize );
  return 0;
};

//
int TIntArrayView::vBeforeBrowse(){
int i;
int result = TListView::vBeforeBrowse();
	for ( i= 0; i < FSize; i++ ){ FSource[ i ] = FSize - i ; };
	return result;
}

//
int TIntArrayView::vSourceBOL() const{
  return FCurrentItemId == 1;
}

//
int TIntArrayView::vSourceEOL() const{
  return FCurrentItemId == FSize;
}

//
int TIntArrayView::vPrevLine(){
      FCurrentItemId--;
	  return 1;
};

//
int TIntArrayView::vNextLine(){
      FCurrentItemId++;
	  return 1;
};


//
int TIntArrayView::vSourceGetCurrentItem( TListViewItem w ){
TListViewItem w2;
  strcpy( w, "Line " );
//  LongToAscHex( FSource[ FCurrentItemId - 1 ], (unsigned char*)w2, 8 );
  LongToAscHex( FCurrentItemId, (unsigned char*)w2, 8 );
  strcat( w, (char*)w2 );
  return strlen( w );
}

//
int TIntArrayView::vSourceJump( TListViewJump dir ){
  switch ( dir ){
    case lvjTOP : FCurrentItemId = 1; break;
    case lvjBOTTOM : FCurrentItemId = FSize; break;
  };  
  return 0;
};

#define BOXHEIGHT 3

//
int TIntArrayView::ProcessMessageDemo(){
int result=1,key,top,bottom;
int x, y;
TListViewItem w;
unsigned char hb[15];
	BoxYLimits( BOXHEIGHT, &top, &bottom );
	top += 8;
	x = BOX_XMIN + 1;
	y = top;		   
	MsgBoxPush( BOXHEIGHT );
	PrintMini( &x, &y, "Test!", 0, 
			   UPPER_X_LIMIT, (int)0, (int)0, (int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
			   
	vSourceGetCurrentItem( w );		   
	x = BOX_XMIN + 1;
	y = top + 18;		   
	PrintMini( &x, &y, (unsigned char*)w, 0, 
			   UPPER_X_LIMIT, (int)0, (int)0, (int)0x0000, (int)0xFFFF, (int)1,  (int)0 );

	LongToAscHex( FMessage.message, hb, 8 );
	x = BOX_XMIN + 1;
	y = top + 2*18;		   
	PrintMini( &x, &y, hb, 0, 
			   UPPER_X_LIMIT, (int)0, (int)0, (int)0x0000, (int)0xFFFF, (int)1,  (int)0 );

	GetKey( &key );

	MsgBoxPop();
	return result;
}

// this is a demo how to process a key-hit.
int TIntArrayView::vProcessMessage(){
int result=1;
	switch ( FMessage.message ){
		case lvmKEY +  KEY_CTRL_F1 :
		case lvmKEY +  KEY_CTRL_F2 :
		case lvmKEY +  KEY_CTRL_F3 :
		case lvmKEY +  KEY_CTRL_F4 :
		case lvmKEY +  KEY_CTRL_F5 :
		case lvmKEY +  KEY_CTRL_F6 :
			if ( FKeyDefined( FMessage.message ) ){
				ProcessMessageDemo();
			}
			break;
		case lvmKEY +  KEY_CTRL_EXE :
			ProcessMessageDemo();
			break;
	}

	return result;
}

////////// TIntArrayView //////////

////////// TStrListView //////////
//
void TStrListView::FDestroyPrivateStringList(){
  if ( FPrivateStringList ){
    delete FSource;
    FSource = 0;
    FPrivateStringList = 0;
  }
};

//
int TStrListView::Assign( TStringList*source ){
  FSource = source;
  return 0;
};

//
int TStrListView::Assign( char*strings ){
  if ( FSource ){
    FDestroyPrivateStringList();
  }
  if ( !FSource ){
    FSource = new TStringList;
    (*FSource).Assign( strings );
    FPrivateStringList = 1;
  };
  return 0;
};

//
int TStrListView::vBeforeBrowse(){
int result = TListView::vBeforeBrowse();
	if ( result ) 
		if ( FSource ){
			if ( (*FSource).Buffer == 0 ) result = 0;
		} else result = 0;
	return result;
}

//
int TStrListView::vSourceBOL() const{
  return (*FSource).IsBeginOfList();
}

//
int TStrListView::vSourceEOL() const{
  return (*FSource).IsEndOfList();
}


//
int TStrListView::vSourceGetCurrentItem( TListViewItem w ){
int len = (*FSource).StringLength();
  memset( w, 0, sizeof( TListViewItem ) );
  if ( (*FSource).pBOL ) strncpy( w, (*FSource).pBOL, len );
  else w[0] = 0;
  return strlen( w );
}

//
int TStrListView::vSourceJump( TListViewJump dir ){
  switch ( dir ){
    case lvjTOP : (*FSource).GoBeginOfList(); break;
    case lvjBOTTOM : (*FSource).GoEndOfList(); break;
  };  
  return 0;
};

//
int TStrListView::vPrevLine(){
      (*FSource).PrevLine();
	  return 1;
};

//
int TStrListView::vNextLine(){
      (*FSource).NextLine();
	  return 1;
};


////////// TStrListView //////////


}
#endif
