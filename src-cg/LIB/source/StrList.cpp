extern "C" {

#include "STD_syscalls.h"
#include "filebios.h"
#include "BFile_syscalls.h"
#include "STD_tools.h"
#include "HEAP_syscalls.h"
#include "StrList.hpp"


/* ASCIIZZ lists of the form:
  "abc",0
  "xyz",0
  "12334",0,0
*/

// pointer to the first element of list
unsigned char*StrListTop( unsigned char*list ){
  return list;
}

// pointer to the last element of list
unsigned char*StrListBottom( unsigned char*list ){
unsigned char*result=0;
  while(list[0]){
    result = list;
    while ((++list)[0]);
    list++;
  };
  return result;
}

// pointer to the next element of list
unsigned char*StrListNext( unsigned char*current ){
  while ((current++)[0]);
  return current;
}

// pointer to the prev element of list
unsigned char*StrListPrev( unsigned char*top, unsigned char*current ){
  if (top==current--) return top;
  while ((--current)[0]){
    if (top==current) return top;	// si 12.7.2008
  };
  return ++current;
}

// count of strings in strliste
int StrListCnt( unsigned char*strliste ){
int iLen=0,result=0;
  while (strliste[iLen]) {
    while(strliste[iLen++]){};
    result++;
  };
  return result;
}

// count of chars in strliste, including the zeros, but excluding the last zero!
int StrListLen( unsigned char*strliste ){
int iLen=0;
  while (strliste[iLen]) while(strliste[iLen++]);
  return iLen;
}

// adds newstr to strliste, caution strliste must have enough space!
int StrListCat( unsigned char*strliste, unsigned char*newstr ){
int iLen = StrListLen( strliste );
int iLen2 = strlen( (char*)newstr );
  memcpy( strliste+iLen, newstr, iLen2+1 );
  strliste[ iLen + iLen2 ] = 0;
  return iLen + iLen2;
}

//
int StrListNearFind( unsigned char*where,  unsigned char*what ){
int iPos=0;
int result;
  while (where[iPos]) {
    result = strcmp( (char*)(where+iPos), (char*)what );
    if (result == 0){ return iPos; };
    if (result > 0){ return 0x20000000 | iPos; };
    while(where[iPos++]){};
  };
  return 0x40000000;
}

// the sort-function requires a heap-buffer of the stringlist's size
void iStrListSort( unsigned char*strliste, TStrListSortFunction sorter = 0 ){
int iLen, iLastMin, iLauf, iLen0, iTarget;
unsigned char*work;
  iLen = StrListCnt( strliste );
  if (iLen<=1){ return; };
  
  if (!sorter) sorter = strcmp;

  iLen0 = StrListLen( strliste )+1;
  work = (unsigned char*)malloc(iLen0);
//  memset( work, 0x55, iLen0 );
  iTarget=0;

  while (iLen){
    iLauf=0;
    iLastMin=0;
// find the lowest
    while ( strliste[iLauf] ) {
      while( strliste[iLauf++] );	// skip to next string
      if ( strliste[iLauf]==0 ) break;	// list-terminating zero encountered
      if ( strliste[iLauf]==0xFF ) continue;	// invalidated, ignore
      if ( strliste[iLastMin]==0xFF ) iLastMin=iLauf;
      else if (sorter( (char*)(strliste+iLauf), (char*)(strliste+iLastMin) )<0) iLastMin=iLauf;
    };
// copy the lowest to the result buffer
    strcpy( (char*)(work+iTarget), (char*)(strliste+iLastMin) );
// mark the next result buffer position
    iTarget += strlen( (char*)(strliste+iLastMin) )+1;
// invalidate the lowest
    strliste[iLastMin] = 0xFF;
// dec the count of strings to process    
    iLen--;
  };  
  work[iTarget]=0;
  memcpy( strliste, work, iLen0 );

  free( work );
}

////////// TStringList //////////
/*  TStringList maintains a list of ASCIIZ-strings, terminated by a last addtional zero (ASCIIZZ).
*/

// example-TStrListSortFunction to sort a stringlist in descending order.
int SortStrDescending( const char*s1, const char*s2 ){
	return -strcmp( s1, s2 );
}

//
int TStringList::FReleaseBuffer(){
// the buffer must not be released if it is a foreign source
// in this case the provider of the buffer is responsible for releasing the buffer.
  if ( !FForeignSource ) if ( Buffer ){ free(Buffer); };
  Buffer = 0;
  pBOL = 0;
  FForeignSource = 0;
  
  return 0;
};

// add a string to the end of the list.
int TStringList::AddString( const char*string ){
int result=0;
int len = strlen( string );
  if ( FForeignSource ) return 0;
  
  if ( !Buffer ){
    FSize = len + 2; // + 0x00 0x00
    Buffer = (char*)malloc( FSize );
    pBOL = Buffer;
  }else{
    result = FSize;
    FSize += len + 1; // + 0x00
    Buffer = (char*)realloc( Buffer, FSize );
    pBOL = Buffer + result - 1;
  }
  memmove( (void*)pBOL, (void*)string, len+1 );
  pBOL[ len+1 ] = 0;

  return result;
};

// load a textfile as ASCIIZZ-list
int TStringList::Load( const unsigned short*fn ){
int result=0;
int handle;
int len, i, j, k;
char*TB;
//    TMemView mv;

  len = 0;
  FReleaseBuffer();
  
  handle = Bfile_OpenFile_OS( fn, _OPENMODE_READ );
  if (handle>0){
    len = Bfile_GetFileSize_OS( handle, 0 ); 
    FSize = len + 1;
    TB = (char*)malloc( FSize );
    Bfile_ReadFile_OS( handle, TB, len, 0 );
    Bfile_CloseFile_OS( handle );
    TB[ len ] = 0x1A;
    i = 0;
    k = 0;
    while ( TB[i] ){
      switch (TB[i]){
          case 0x0A : case 0x0D : case 0x1A :
          j = TB[ i ];
          TB[ i ] = 0;
          break;
        default : j = 0;  
      }
      if (j){
        if (TB[k] ) AddString( &TB[k] );
        else AddString( " " );
        if ( j == 0x1A ) break;
        i++;
        if ( j == 0x0A ){
          if ( TB[i] == 0x0D ) i++;
        }else if ( j == 0x0D ){
          if ( TB[i] == 0x0A ) i++;
        }
        k = i;
      }else i++;
    }
    free( TB );
    pBOL = Buffer;
    result = 1;
  };  
  return result;
};

const char END_OF_NAME[]= "\x09= ";
const char SPACES[]= "\x09 ";
#define NAME_VALUE_SEPARATOR '='

//
char*TStringList::FSkipChars( const char*c, char*start ){
char*result = 0;
  while ( start[0] ){
    if ( !start[0] ){
      result = start; 
      break;
    };
    if ( !strchr( c, start[0] ) ){
      result = start; 
      break;
    };
    start++;
  };  
  return result;
}

//
char*TStringList::FWaitChars( const char*c, char*start ){
char*result = 0;
  while ( start[ 0 ] ){
    if ( !start[0] ){
      result = start; 
      break;
    };
    if ( strchr( c, start[0] ) ){
      result = start; 
      break;
    };
    start++;
  };  
  return result;
}

// is End of Line
int TStringList::isEOL( const int pos ){
  if ( Buffer[ pos ] ) return 0;
  return 1;
}

// go End of Line
int TStringList::goEOL(){
int result;
  result = pBOL - Buffer;
  while ( !isEOL( result ) ){ result++; }
  return result;  
};

// moves the current linepointer to the previous line
int TStringList::PrevLine(){
char* result = pBOL;
  if ( result == Buffer ) return 0;
  result -= 2;
  while ( result > Buffer ){
    if ( !result[0] ) break;
    result--;
  }
  if ( result != Buffer ) result++;
  pBOL = result;
  return 1;  
}

// return the current line
char* TStringList::Line(){
	return pBOL;
}

// moves the current linepointer to the next line
int TStringList::NextLine(){
int pos = goEOL()+1;
  if ( Buffer[ pos ] ){
    pBOL = Buffer + pos;
    return 1;
  };  
  return 0;
}

// length of the current line
int TStringList::StringLength(){
  if ( pBOL ) return strlen( &pBOL[ 0 ] );
  else return 0;
};

// go to end of list
void TStringList::GoEndOfList(){
  while( NextLine() ){};
}

// go to begin of list
void TStringList::GoBeginOfList(){
  pBOL = Buffer;
}

// is begin of list
int TStringList::IsBeginOfList(){
  return ( pBOL == Buffer );
}  

// is end of list
int TStringList::IsEndOfList(){
int result = NextLine();
  if ( result ) PrevLine();
  return !result;
}

// returns the name of the current line based on the convention that the name is separated by 
// NAME_VALUE_SEPARATOR (usually "=") from the value, like in INI-files (<name>=<value>).
int TStringList::Name( char*c ){
int iresult=0;
char*work;
char*endofname;
char*curpos;
  curpos = FSkipChars( SPACES, pBOL );
  if (curpos[0]){ 
    endofname = FWaitChars( END_OF_NAME, curpos );
    if ( endofname[0] ){
      work = endofname;
      while ( work[0] != NAME_VALUE_SEPARATOR ){
        if ( !work[0] ) break;
        work++;
      }  
      if ( work[0] ){
        iresult = endofname - curpos;  
      }else{ iresult = 0; };
    }else{ iresult = 0; }  
  }
  if ( iresult ) memcpy( c, curpos, iresult );
  c[ iresult ] = 0;
  return iresult;
};

// returns the value of the current line based on the convention that the value is separated by 
// NAME_VALUE_SEPARATOR (usually "=") from the name, like in INI-files.
int TStringList::Value( char*c ){
char*work=0;
  work = pBOL;
  while ( work[0] != NAME_VALUE_SEPARATOR ){
    if ( !work[0] ) break;
    work++;
  }  
  if ( work[0] ){
    strcpy( c, ++work );
    return 1;
  }else{
    c[0] = 0;
    return 0;
  };  
};

//
int TStringList::Param( int pos, char*c ){
int iresult=0;
char*work=pBOL;
char*w2=work;
  while ( pos ){
    while ( work[0] ) {
      if ( work[0] != ' ' )
        if ( work[0] != 0x09 ) break;
      work++;
    };
    if ( !work[0] ) break;
    w2 = work;
    while ( w2[0] ) {
      if ( w2[0] == ' ' ) break;
      if ( w2[0] == 0x09 ) break;
      w2++;
    };
    if ( work == w2 ) break;
    pos--;
    if (!pos)  break;
    work = w2;
  }
  if ( !pos ){ 
    iresult = w2 - work;
    memcpy( c, work, iresult );
  }  
  c[ iresult ] = 0;
  return iresult;
};

// sort the lines
void TStringList::Sort( TStrListSortFunction sorter ){
	iStrListSort( (unsigned char*)Buffer, sorter );
}

// assign a foreign list source
// a foreign source must not be released automatically by TStringList!
int TStringList::Assign( char*c ){
  FReleaseBuffer();
  Buffer = c;
  GoBeginOfList();
  FSize = StrListLen( (unsigned char*)Buffer ) + 1;
  if ( Buffer ) FForeignSource = 1;
  return 0;
}

////////// TStringList //////////

}
