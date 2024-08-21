//------------------------------------------- source code refer to (WSC) file.c
/* return a filename */
unsigned int SelectFile (char *filename);
void FileListfree() ;

// #define DEBUG
// if define DEBUG, the files are in SD card
// else the files are in storage memory

#ifdef DEBUG
#define ROOT "crd0"
#else
#define ROOT "fls0"
#endif

FONTCHARACTER *CharToFont( const char *cFileName, FONTCHARACTER *fFileName );
char *FontToChar( const FONTCHARACTER *fFileName, char *cFileName );

#define FILEMAX 512
#define FILENAMEMAX 13
#define FOLDERMAX 9
#define N_LINE 6

#define FOLDER_FLAG       -1
#define FOLDER_SEPALATOR  -2

typedef struct{
	char filename[FILENAMEMAX];
	char folder[FOLDERMAX];
	int filesize;
}Files;

#define FavoritesMAX 7
#define	root2_MAX 31

extern Files *files;
//extern Files *filesr;
extern char folder[FILENAMEMAX];
extern int index;

extern char FileListUpdate;
extern char StorageMode;		// 0:Storage memory   1:SD

extern char ForceG1Msave;		//    1: force g1m save
extern char AutoSaveMode;		//    1: Auto save ( not pop up )
extern char textmode;

extern const char extG1MorG3M[][4];
extern char root2[root2_MAX];
extern char root3[root2_MAX];
void StoreRoot2();
void RestoreRoot2();
int SetRoot2( char* SRC ) ;

#define FileCMD_Prog   10000
#define FileCMD_RUN    10001
#define FileCMD_RUN_F1 100011
#define FileCMD_DebugRUN    10002
#define FileCMD_EDIT   10003
#define FileCMD_NEW    10004
#define FileCMD_RENAME 10005
#define FileCMD_DEL    10006
#define FileCMD_COPY   10007
#define FileCMD_TEXT   10008
#define FileCMD_PASS   10009
#define FileCMD_MKDIR  10010
#define FileCMD_RENDIR 10011
#define FileCMD_DELDIR 10016

char * loadFile( const char *name, int *editMax, int disperror, int *filesize );
int  storeFile( const char *name, unsigned char* codes, int size);
int LoadProgfile( char *name, int prgNo, int editsize, int disperror ) ;
int SaveG1Mbin( char *filebase );
int SaveG1M( char *filebase );
int SaveProgfile(int progNo );

char * Load1st2nd( char *name, char *dir2nd, char *ext );

int SavePicture( char *filebase, int pictNo );
char * LoadPicture( int pictNo );

int SaveCapture( char *filebase, int pictNo );
char * LoadCapture( int pictNo );

int  RenameFile(char *name) ;
void DeleteFile(char *name) ;
void DeleteFileFav(char *sname, int yesno) ;
int DeleteFolder(char *foldername, int yesno ) ;	// delete folder
int CopyFile( char *sname ) ;

void ConvertToText( char *sname );

int NewProg();
void CB_Local( char *SRC ) ;
void CB_ProgEntry( char *SRC ) ; //	Prog "..." into memory
int CB_GetProgEntry( char *SRC, char *folder16 ) ; //	Prog "..." into memory
void CB_GetAliasLocalProg( char *SRC ) ; //	Preprocess Alias/Local

void CopyFilesToFavorites();
void CopyFavoritesToFiles();
void SaveFavorites();
void SaveConfig();
void LoadConfig();

void ChangeStorageMode( int newMode ) ;
void ChangeFavorites( int oldStorageMode, int newStorageMode );	// old <> new favorite

int CB_IsExist( char *SRC, int calcflag ) ;	//	IsExist("TEST")		//  no exist: return 0     exist: return filesize
char * CB_SaveLoadOprand( char *SRC , int *reg, int *matsize ) ;
void CB_SaveSub( char *sname, char* FilePtr, int size, int check, char* extname ) ;
char * CB_LoadSub( char *sname, int ptr, int *size, char* extname ) ;
void CB_Save( char *SRC ) ; //	Save "TEST",Mat A[1,3]
void CB_Load( char *SRC ) ; //	Load ("TEST" [, Ptr])->Mat A[1,3]
void CB_Delete( char *SRC ) ;	// Delete "ABC.bin"[,Q]
void CB_Rename( char *SRC ) ;	// Rename "Old.bin","New.bin"

void Getfolder( char *sname ) ;
void Restorefolder() ;
void SetFullfilenameBin( char *fname, char *sname, char *extname ) ;
void Setfoldername16( char *folder16, char *sname ) ;
int CheckPassWord( char *filebase );	// 1:cancel  0:Ok  -1:no pass
void NewPassWord(char *name) ;
void SetFullfilenameExt( char *filename, char *sname, char *extname );

void PP_ReplaceCode( char *SRC );
int CheckSD();	// SD model  return : 1

int RenameCopyFile( char *fname ,int select ) ;	// select:0 rename  select:1 copyint MakeDirectory();
int RenameDirectorys( char * foldername );
int RenameFiles( char * sname );	// select:0 rename  serect:1 copy
int CopyFiles( char * sname );
int CreateDirectorySub( char *newfolder, int dsperror );
int MakeDirectory();
void Abort();

void SetShortName( char *sname, char *filename) ;	// fullpath filename -> short name
void ErrorMSGfile( char *buffer, char *filename, int err);
void ErrorMSGfiletmp( char *buffer, char *buffer2, int err);
void basname8ToG1MHeader( char *filebase, char *basname) ;	// abcd -> header
void G1MHeaderTobasname8( char *filebase, char *basname) ;	// header -> abcd
void G1M_header( char *filebase ,int *size ) ;
void G1M_Basic_header( char *filebase ) ;
int SaveConfigWriteFileSub( unsigned char *buffer, unsigned char *dir, unsigned char *basname, int size ) ;

int CB_PreProcessIndent( char *filebase, int progno ) ; //
void CB_PostProcessIndentRemove( char *filebase ) ; //

int BG_OpenFileDialog( char *buffer ) ;

void LoadConfig1data( int n ) ;	// config data ->List Ans
//-----------------------------------------------------------------------------
int DecodeBmp2Vram( char *filebase, int px, int py );	//	bmp -> vram
char * LoadBmp( char *filebase );
int SaveBmp( char *filebase, char *sname, int width, int height );
void CB_BmpSave( char *SRC ) ; //	BmpSave "TEST"[,x][,y][,x2][,y2][,Q]
void CB_BmpLoad( char *SRC ) ; //	BmpLoad("TEST" [, Ptr])->Mat A[1,3]

//----------------------------------------------------------------------------------------------
typedef struct {
	void*LoadG3B_buffer_ptr;
	unsigned char colormode;
	unsigned char c2;
	unsigned char c3;
	unsigned char c4;
	int i3;
	unsigned short width;
	unsigned short height;
	int i5;
} TLoadG3P_array;

extern 	TLoadG3P_array	loadg3p_array;

int SaveG3P( TLoadG3P_array*load_G3P_record, FONTCHARACTER *filename, int, int, int  );
int LoadG3P( TLoadG3P_array*load_G3P_record, FONTCHARACTER *filename );

void ConvertC3toFULL( unsigned short *fullcolor, unsigned char *c3color, int dx, int dy );	//	c3color(4bit color) -> fullcolor(16bit color)
void ConvertFULLtoC3( unsigned char *c3color, unsigned short *fullcolor, int dx, int dy );	//	fullcolor(16bit color) -> c3color(4bit color)
int LoadG3Psub( char *fname );

int SavePictCaptCG( char*pictcapt, char *sname, char *filebase, int colormode );
int LoadPictCaptCG( char *pictcapt, char *sname );

//------------------------------------------------------------------------- MCS
int MCS_ReadFileList() ;	// MSC file list -> files
void ShortName2basname( char *basname, char *sname );
void SetBasName( char *basname, char *fname );
char * MCS_LoadG3M( char *fname , int *editMax, int disperror, int *filesize  ) ;	// MCS -> g3m file
int MCS_SaveG3M( char *filebase ) ;	// g3m file -> MCS
int MCS_DeleteG3M( char *fname  ) ;	// delete g3m
int MCS_ExistFile( char *fname  ) ;	// exist?

//----------------------------------------------------------------------------------------------
void WaitKeyAC();
void WaitKeyEXIT();
void WaitKeyF1();
void WaitKeyEXE();


//----------------------------------------------------------------------------------------------
void CB_StatusDisp_Run();
void CB_SetStatusDisp( int a );

