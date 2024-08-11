extern "C" {

static unsigned int lastrandom=0x12345678;
unsigned int random( int seed = 0 ){
    if (seed) lastrandom=seed;
    lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039;
    return ( lastrandom >> 16 );
}

}

