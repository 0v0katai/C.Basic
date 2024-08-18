extern "C" {

#include "convert_syscalls.h"

//
void IntToHex( unsigned int value, unsigned char*buffer ){
	WordToHex( value >> 16, buffer );
	WordToHex( value & 0xFFFF, buffer+4 );
	buffer[ 8 ] = 0;
}

}

