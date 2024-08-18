//  こちらのサイトよりカナフォントデータおよび表示ルーチンを引用させていただいています。
//	http://baticadila.dip.jp/fx_sdk_005.html
//

extern char GBcode;			// GB font enable:1
extern char ExtCharAnk;				// 0:Normal 	1:Ext Ank   font 
extern char ExtCharKana;			// 0:Normal 	1:Ext Kana  font 
extern char ExtCharGaiji;			// 0:Normal 	1:Ext Gaiji font 
extern char ExtCharAnkMini;				// 0:Normal 	1:Ext Ank   font 
extern char ExtCharKanaMini;			// 0:Normal 	1:Ext Kana  font 
extern char ExtCharGaijiMini;			// 0:Normal 	1:Ext Gaiji font 

extern char ExtCharAnkFX;			// 0:Normal 	1:Ext Ank   font FX
extern char ExtCharKanaFX;			// 0:Normal 	1:Ext Kana  font FX
extern char ExtCharGaijiFX;			// 0:Normal 	1:Ext Gaiji font FX
extern char ExtCharAnkMiniFX;			// 0:Normal 	1:Ext Ank   font FX
extern char ExtCharKanaMiniFX;			// 0:Normal 	1:Ext Kana  font FX
extern char ExtCharGaijiMiniFX;			// 0:Normal 	1:Ext Gaiji font FX

void ClearExtFontflag();
int Make_FontFoloder();
void LoadExtFontKanafolder(  int flag, char* sname, int folder, int no );		// LFONTK.bmp -> font 18x24
void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no );		// LFONTG.bmp -> font 18x24
void LoadExtFontAnkfolder(   int flag, char* sname, int folder, int no );			// LFONTK.bmp -> font 18x24
void LoadExtFontKana(  int flag, char* sname, int no );			// LFONTK.bmp -> font 18x24
void LoadExtFontGaiji( int flag, char* sname, int no );		// LFONTG.bmp -> font 18x24
void LoadExtFontAnk(   int flag, char* sname, int no );		// LFONTK.bmp -> font 18x24

void SaveExtFontKana(  int flag, char* sname, int folder, int no, int check );		// font 18x24 -> LFONTK.bmp
void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check );		// font 18x24 -> LFONTG.bmp
void SaveExtFontAnk(   int flag, char* sname, int folder, int no, int check );	// font 18x24 -> LFONTA.bmp

int KPrintCharSub( int px, int py, unsigned char *str, int mode) ; //	gaiji カナ一文字表示	px,py  (0,-24)-(383,191)

int KPrintCharMini( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini FX (0,-24)-(383,191)
int KPrintCharMiniFX( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini FX (0,-24)-(383,191)
int KPrintCharMiniCG( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini CG (0,-24)-(383,191)
int KPrintCharMiniMini( int px, int py, unsigned char *str, int mode ) ; // PrintMiniMini (0,-24)-(383,191)
int KPrintCharMiniMiniBold( int px, int py, unsigned char *str, int mode ) ; // PrintMiniMini (0,-24)-(383,191) Bold
int KPrintCharMiniMiniFX( int px, int py, unsigned char *str, int mode ) ; // PrintMiniMiniFX (0,-24)-(383,191) fx 6*8dot font

int CB_GetFont( char *SRC );		// GetFont(0xFFA0)->Mat C
int CB_GetFontMini( char *SRC );	// GetFont(0xFFA0)->Mat C
void CB_SetFont( char *SRC );		// SetFont 0xFFA0,Mat C
void CB_SetFontMini( char *SRC );	// SetFont 0xFFA0,Mat C

void ReadExtFont();
