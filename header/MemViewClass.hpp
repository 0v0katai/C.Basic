// syscalls are defined in syscall.hpp and syscall.cpp
// this file is used for derived functions

#ifndef __MEMVIEWCLASS_HPP__
#define __MEMVIEWCLASS_HPP__

#include "ListView.hpp"


#define MINADDR 0x00000000
#define MAXADDR 0xFFFFFFFF
#define BYTESPERLINE 8
#define LINESPERPAGE 9 

#define MEMORY_MAXSEARCHSTRINGSIZE 8 


////////// TMemView //////////
class TMemView : public TListView {
	public:
		TMemView( int X1=1, int Y1=1, int X2=BYTESPERLINE*2+8+3, int Y2=LINESPERPAGE )  : TListView( X1, Y1, X2, Y2 ) { 
			FSource = (void*)MINADDR;
			SetHeading( "Memory View" );
			FOnCreate();
			FLineBuffer[0] = 0;
			memset( PLastSearchString, 0, sizeof(PLastSearchString) );
		}
		virtual ~TMemView();
		int SetAddress( int addr );
		int GetAddress();
		    
	protected:  
		virtual int vBeforeBrowse();
	    virtual int vPrevLine();
		virtual int vNextLine();
		virtual int vSourceGetCurrentItem( TListViewItem w );
		virtual int vSourceBOL() const;
		virtual int vSourceEOL() const;
		virtual int vSourceJump( TListViewJump dir );
		virtual int vProcessMessage();
		virtual int vShowAll();
		TListViewItem*MakeBuffer();
		unsigned char PLastSearchString[MEMORY_MAXSEARCHSTRINGSIZE+1];
	private:
		TListViewItem FLineBuffer;
		void*FSource;
		int FDelete();
		int FOnCreate();
	
};
////////// TMemView //////////



#endif