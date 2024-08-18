#ifndef __MEMVIEW_HPP__
#define __MEMVIEW_HPP__

// #include "base.hpp"
// #include incUI

////////// TMemView //////////
class TMemView{
  public:
    TMemView( void*baseaddr = (void*)0x80000000 ){
//      FViewMode = FK_HEX;
      FBaseAddress = (int)baseaddr;
      FOffset = 0;
      FMemWindowStart = 0;
      FMemWindowEnd = 0;
      FRecurrent = 0;
    };
    int SetMemWindow( const int base, const int start = 0, const int end = 0, const int recurrent = 0 );
    void SetRecurrent( const int recurrent );
    int browse( const void*base = 0 );
	virtual void Draw(  int mode  );

  protected:
  private:
    int FViewMode;
    int FBaseAddress;
    int FOffset;
    int FMemWindowStart;
    int FMemWindowEnd;
    int FRecurrent;
    
    int FShowMemoryBlock();

};
////////// TMemView //////////

#endif