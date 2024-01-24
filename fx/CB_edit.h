#define EditMaxfree 1024
#define ClipMax 1024
extern unsigned char ClipBuffer[];
extern int CB_INT;	// 0:normal  1: integer mode
extern int selectSetup;
extern int selectVar;
extern int selectMatrix;

extern int Contflag;

void CB_PreProcess(unsigned char *SRC);
int SrcSize( unsigned char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

