extern "C" {

#include "CONVERT_syscalls.h"

//
int HexToInt( unsigned char*buffer ){
unsigned char c;
int iresult=0;
	while ( buffer[0] ){
		HexToNibble( buffer[0], &c );
		iresult<<=4;
		iresult+=c;
		buffer++;
	}
	return iresult;
}

}

