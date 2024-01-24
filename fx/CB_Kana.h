//  こちらのサイトよりカナフォントデータおよび表示ルーチンを引用させていただいています。
//	http://baticadila.dip.jp/fx_sdk_005.html
//

extern char ExtCharAnkFX;				// 0:Normal 	1:Ext Kana  font 
extern char ExtCharKanaFX;			// 0:Normal 	1:Ext Kana  font 
extern char ExtCharGaijiFX;			// 0:Normal 	1:Ext Gaiji font 
extern char ExtCharAnkMiniFX;			// 0:Normal 	1:Ext Ank   font FX
extern char ExtCharKanaMiniFX;			// 0:Normal 	1:Ext Kana  font FX
extern char ExtCharGaijiMiniFX;			// 0:Normal 	1:Ext Gaiji font FX

extern unsigned char *ExtAnkFontFX;		// Ext Ank font
extern unsigned char *ExtAnkFontFXmini;	// Ext Ank font mini
extern unsigned char *ExtKanaFontFX;		// Ext Kana & Gaiji font
extern unsigned char *ExtKanaFontFXmini;	// Ext Kana & Gaiji font mini

extern const unsigned char Font00[][8] ;
extern const unsigned char KanaFont[][8] ;
extern const unsigned char Fontmini[][8] ;
extern const unsigned char KanaFontmini[][8];

int Make_FontFoloder();
void LoadExtFontKana(  int flag, char* sname, int no );			// FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaiji( int flag, char* sname, int no );			// FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnk(   int flag, char* sname, int no );			// FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6

void SaveExtFontKana(  int flag, char* sname, int folder, int no, int check );		// font 6x8 -> FONTK8L.bmp    font 6x6 -> FONTK6M.bmp
void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check );		// font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
void SaveExtFontAnk(   int flag, char* sname, int folder, int no, int check );		// font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp

void KPrintChar( int px, int py, unsigned char *c) ; //カナ一文字表示
void KPrintRevChar( int px, int py, unsigned char *c) ; //カナ一文字表示

int KPrintCharMini( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini

int CB_GetFont( char *SRC );		// DotChar(0xFFA0)->Mat C
int CB_GetFontMini( char *SRC );	// DotChar(0xFFA0)->Mat C
void CB_SetFont( char *SRC );		// SetFont 0xFFA0,Mat C
void CB_SetFontMini( char *SRC );	// SetFont 0xFFA0,Mat C
