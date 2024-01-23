#define EditMaxfree 2024
#define ClipMax 2048
extern unsigned char ClipBuffer[];

void CB_PreProcess(unsigned char *SRC);
int SrcSize( unsigned char *src ) ;

void EditRun(int run);
