#define EditMaxfree 1024
#define ClipMax 1024
extern unsigned char ClipBuffer[];

void CB_PreProcess(unsigned char *SRC);
int SrcSize( unsigned char *src ) ;

void EditRun(int run);
