// refer to siomn's fxDocumentation

int RTC_GetTicks();  // 1/128s counter
void RTC_SetDateTime( unsigned char *timestr );

void PutKey(unsigned int keycode, int mode);
void Keyboard_ClrBuffer();
int Keyboard_GetKeyWait(int*column, int*row, int type_of_waiting, int timeout_period, int menu, unsigned short*keycode );
void EnableGetkeyToMainFunctionReturn(); 
void DisableGetkeyToMainFunctionReturn(); 

int Cursor_SetPosition(char x, char y); 

int Cursor_SetFlashStyle(short int flashstyle);

void Cursor_SetFlashMode(long flashmode);

unsigned int Cursor_GetSettings(unsigned int settingsarray[]);

int Cursor_GetFlashStyle();

void Cursor_SetFlashOn( char flash_style ); 

void Cursor_SetFlashOff(); 

int OpcodeToStr( unsigned short opcode, unsigned char *string );

int MB_IsLead(char character);
int MB_ElementCount(char *str);
int MB_ByteCount(char *str);
char * MB_strcat(char *dest, char *src);
char * MB_strncat(char *dest, char *src, int bytes);
char * MB_strcpy(char *dest, char *src);
char * MB_GetSecondElemPtr(char *str) ;
short int MB_GetElement(char *str) ;
char * MB_CopyToHeap(char *src) ;

//------------------------------------------------------- MCS
typedef struct {
  unsigned char name[8];
  void*addr;
  unsigned short count;
  char flags[2];
} TMainMemoryDirectoryEntry;

typedef struct {
  unsigned char name[8];
  int offset;
  int length;
  char flags[4];
} TDirectoryItem;

int MCS_CreateDirectory( unsigned char*dir, char*dirno );
int MCS_WriteItem( unsigned char*dir, unsigned char*item, short itemtype, int data_length, int buffer );
int MCS_DeleteDirectory( unsigned char*dir );
int MCS_DeleteItem( unsigned char*dir, unsigned char*item );
int MCS_GetState( int*maxspace, int*currentload, int*remainingspace );
int MCS_GetSystemDirectoryInfo( unsigned char*dir, unsigned int*pdir, char*dirno );
int MCS_RenameItem( unsigned char*srcdir, unsigned char*srcitem, unsigned char*tgtdir, int tgtitem );
int MCS_OverwriteData( unsigned char*dir, unsigned char*item, int write_offset, int bytes_to_write, void*buffer );
int MCS_GetItemData( unsigned char*dir, unsigned char*item, int offset, int bytes_to_read, void*buffer );
int MCS_RenameDirectory( unsigned char*oldname, unsigned char*newname );
int MCS_SearchDirectory( unsigned char*dir, TMainMemoryDirectoryEntry*pdir, char*dirno );
int MCS_SearchDirectoryItem( unsigned char*dir, unsigned char*item, char*flags_0, TDirectoryItem*item_ptr, int*data_ptr, int*data_length );
int MCS_GetFirstDataPointerByDirno( char*dirno, void*pdata );
int MCS_GetDirectoryEntryByNumber( char dirno, TMainMemoryDirectoryEntry*pdir );
int MCS_SearchItem( unsigned char*item, TMainMemoryDirectoryEntry*pdir, unsigned short*itemno );
int MCS_str8cpy( unsigned char*source, unsigned char*target, int mode );
void MCS_GetDirectoryEntryAddress( void*directory_entry_address );
void MCS_GetCurrentBottomAddress( void*current_bottom_address );
int MCS_GetCapa( int*current_bottom );
int MCS_GetMainMemoryStart( void );

//------------------------------------------------------- Bdisp
unsigned char* GetVRAMAddress();

typedef struct{
    unsigned int x1;
    unsigned int y1;
    unsigned int x2;
    unsigned int y2;
    unsigned char f[4];
    unsigned int on_bits;
    unsigned int off_bits;
} TShape;

void Bdisp_ShapeToVRAM( TShape *p ); 

//------------------------------------------------------- Call

int Calljmp(    int r4, int r5, int r6, int r7, int adrs );
int SysCalljmp( int r4, int r5, int r6, int r7, int no );
