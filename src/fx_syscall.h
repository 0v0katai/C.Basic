// refer to siomn's fxDocumentation

int RTC_GetTicks();  // 1/128s counter
void RTC_SetDateTime( unsigned char *timestr );

void PutKey(unsigned int keycode, int mode);
void Keyboard_ClrBuffer();
int Keyboard_GetKeyWait(int*column, int*row, int type_of_waiting, int timeout_period, int menu, unsigned short*keycode );
void EnableGetkeyToMainFunctionReturn(); 
void DisableGetkeyToMainFunctionReturn();

void HourGlass();

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

void GetFKeyIconPointer( int FKeyNo, unsigned char *pBitmap );
void DisplayFKeyIcon( int FKeyPos, unsigned char *pBitmap );
void GUI_ProgressBar( int current, int max);

void System_GetOSVersion( unsigned char*version );

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
int MCSPutVar2( unsigned char*dir, unsigned char*item, int data_len, void*buffer );
int MCS_WriteItem( unsigned char*dir, unsigned char*item, short itemtype, int data_length, int buffer );
int MCS_DeleteDirectory( unsigned char*dir );
int MCS_DeleteItem( unsigned char*dir, unsigned char*item );
int MCS_GetState( int*maxspace, int*currentload, int*remainingspace );
int MCS_GetSystemDirectoryInfo( unsigned char*dir, unsigned int*pdir, char*dirno );
int MCS_RenameItem( unsigned char*srcdir, unsigned char*srcitem, unsigned char*tgtdir, int tgtitem );
int MCS_OverwriteData( unsigned char*dir, unsigned char*item, int write_offset, int bytes_to_write, void*buffer );
int MCS_GetItemData( unsigned char*dir, unsigned char*item, int offset, int bytes_to_read, void*buffer );
int MCS_RenameDirectory( unsigned char*oldname, unsigned char*newname );
int MCS_SearchDirectory(unsigned char *dir, TMainMemoryDirectoryEntry **pdir, char *dirno);
int MCS_SearchDirectoryItem( unsigned char*dir, unsigned char*item, char*flags_0, TDirectoryItem*item_ptr, int*data_ptr, int*data_length );
int MCS_GetFirstDataPointerByDirno( char*dirno, void*pdata );
int MCS_GetDirectoryEntryByNumber( char dirno, TMainMemoryDirectoryEntry*pdir );
int MCS_SearchItem( unsigned char*item, TMainMemoryDirectoryEntry*pdir, unsigned short*itemno );
int MCS_str8cpy( unsigned char*source, unsigned char*target, int mode );
void MCS_GetDirectoryEntryAddress( void*directory_entry_address );
void MCS_GetCurrentBottomAddress( void*current_bottom_address );
int MCS_GetCapa( int*current_bottom );
int MCS_GetMainMemoryStart( void );
int Bfile_RenameEntry( const unsigned short *oldname, const unsigned short *newname );
int DD_SetContrast(int value);
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
int SysCalljmp12( int r1, int r2, int r3, int r4, int r5, int r6, int r7, int r8, int r9, int r10, int r11, int r12, int No);

//------------------------------------------------------- Serial
int Serial_Open2( unsigned short parameters );	//0x02AB
int Serial_Open_57600( void );	//0x0285
void*Serial_ResetAndDisable( void );	//0x0409
void*Serial_GetInterruptHandler( int type );	//0x040A
int Serial_SetInterruptHandler( int type, void*handler );	//0x040B
int Serial_ReadOneByte( unsigned char*result );	//0x040C
int Serial_ReadNBytes( unsigned char*result, int max_size, short*actually_transferred );	//0x040D
int Serial_BufferedTransmitOneByte( unsigned char byte_to_transmit );	//0x040E
int Serial_BufferedTransmitNBytes( unsigned char*bytes_to_transmit, int requested_count );	//0x040F
int Serial_DirectTransmitOneByte( unsigned char byte_to_transmit );	//0x0410
int Serial_GetReceivedBytesAvailable( void );	//0x0411
int Serial_GetFreeTransmitSpace( void );	//0x0412
int Serial_ClearReceiveBuffer( void );	//0x0413
int Serial_ClearTransmitBuffer( void );	//0x0414
int Serial_Open( unsigned char *mode );	//0x0418
int Serial_Close( int mode );	//0x0419
void*Serial_CallReceiveIntErrorResetHandler( void );	//0x041B
void*Serial_CallReceiveIntHandler( void );	//0x041C
void*Serial_CallTransmitIntErrorResetHandler( void );	//0x041D
void*Serial_CallTransmitIntHandler( void );	//0x041E
int Serial_SpyNthByte( int byteno_to_spy, unsigned char*result );	//0x0422
void Serial_GetStatus( unsigned int*serial_status );	//0x0423 
int Serial_IsOpen( void );	// 0x0425

