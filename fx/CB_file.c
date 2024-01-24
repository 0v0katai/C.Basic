#include "fxlib.h"
#include "timer.h"
#include "stdio.h"
#include "string.h"

#include "CB_io.h"
#include "CB_error.h"

#include "CB_inp.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_interpreter.h"
#include "CB_Matrix.h"

#include "CBI_interpreter.h"

//-------------------------------------------------------------- source code refer to (WSC) file.c
//---------------------------------------------------------------------------------------------
#define FILENAMEMAX 13
#define N_LINE 6

typedef struct{
	char filename[FILENAMEMAX];
	unsigned long filesize;
}Files;

#define FavoritesMAX 7

static Files *files;
static int index = 0;

static int ReadFile( char *folder );
static int IsFileNeeded( FONTCHARACTER *FileName );
static FONTCHARACTER *FilePath( char *sFolder, FONTCHARACTER *sFont );
unsigned int Explorer( int size, char *folder );
static int FileCmp( const void *p1, const void *p2 );

static int size=0;
static char folder[FILENAMEMAX] = "", name[FILENAMEMAX];
Files Favoritesfiles[FavoritesMAX];
int	FavoritesSize=-1;


unsigned int SelectFile (char *filename)
{
	unsigned int key;

	Bdisp_AllClr_DDVRAM();
	while( 1 ){
		size = ReadFile( folder );
		qsort( files, size, sizeof(Files), FileCmp );
		
		key = Explorer( size, folder ) ;
		if ( key == KEY_CTRL_F3 ) break ;	// new file
		if ( key == KEY_CHAR_POWROOT ) break ;	// sdk built in file

		if ( ( key == KEY_CTRL_EXIT ) || ( index == size ) ) {							//return to root
			folder[0] = '\0';	
			index = 0;
			SaveConfig();
		}
		else if( files[index].filesize == -1 ){				//folder
			strcpy( folder,files[index].filename );
			index = 0;
		}
		else{										//file
			strcpy( name,files[index].filename );
			break;
		}
	}

	if( strlen(folder) == 0 )
		sprintf( filename, "\\\\"ROOT"\\%s", name );
	else
		sprintf( filename, "\\\\"ROOT"\\%s\\%s", folder, name );
	
	return key ;
}

int SelectFilefree(char *filename) {
	int i;
	int result=SelectFile(filename);
	SaveFavorites();
	free( files );
	return result;
}


static int ReadFile( char *folder )
{
	char str[FILENAMEMAX];
	FONTCHARACTER find_path[50];
	FONTCHARACTER find_name[50];
	FILE_INFO file_info;
	int find_h;
	int size, i;

	size = 0;
	FilePath( folder, find_path );

/*				Get File Num			*/

	Bfile_FindFirst (find_path, &find_h, find_name, &file_info);
	if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) )
		size++;
	while(Bfile_FindNext(find_h, find_name, &file_info)==0){
		if( file_info.type == DT_DIRECTORY || IsFileNeeded( find_name ) )
			size++;
	}
	Bfile_FindClose(find_h);

	size += (FavoritesMAX + 1) ;

/*				Get Name & Size			*/
	i = FavoritesMAX ;
	files = (Files *)malloc( size*sizeof(Files) );
	memset( files, 0, size*sizeof(Files) );
	Bfile_FindFirst (find_path, &find_h, find_name, &file_info);
	if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) ){
		FontToChar(find_name,str);
		strncpy( files[i].filename, str, FILENAMEMAX);
		files[i].filesize = (file_info.type == DT_DIRECTORY ? -1 : file_info.dsize);
		++i;
	}
	while(Bfile_FindNext(find_h, find_name, &file_info)==0)
	{
		if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) ){
			FontToChar(find_name,str);
			strncpy( files[i].filename, str, FILENAMEMAX);
			files[i].filesize = (file_info.type == DT_DIRECTORY ? -1 : file_info.dsize);
			++i;
		}
	}
	Bfile_FindClose( find_h );
	return size;
}



//--------------------------------------------------------------

unsigned int Explorer( int size, char *folder )
{
	int cont=1;
	int top, redraw;
	int i,j,k,tmp;
	unsigned int key;
	int FavCount=0;
	int StartLine;
	char tmpname[FILENAMEMAX];
	
	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,3);				// set cursor rep

	FavCount=0;
	j=FavoritesMAX-1;
	for( i=FavoritesMAX-1; i>=0; i--){			//	set favorites list
		files[i].filesize=0;
		k=0;
		while ( k < FavoritesMAX ) {	// file matching search
			if ( strcmp( Favoritesfiles[i].filename,  files[k].filename )== 0 ) break; // already favorite exist
			k++;
		}
		if ( k == FavoritesMAX ) { 		//	no favorites list
			k=FavoritesMAX+1;
			while ( k < size ) {	// favorite file exist? search
				if ( strcmp( files[k].filename,  Favoritesfiles[i].filename )== 0 ) break; // not matching
				k++;
			}
			if ( k < size ) { 		//	set Favorites 
				strncpy( files[j].filename, Favoritesfiles[i].filename, FILENAMEMAX);
				files[j].filesize = files[k].filesize;
				j--;
				FavCount++;
			} else {	// not found cancel favorite
				memset( Favoritesfiles[i].filename, 0x00, FILENAMEMAX );
				Favoritesfiles[i].filesize=0;
			}
		}
	}
	files[FavoritesMAX].filesize = 0xFFFF;	// separator
	StartLine=FavoritesMAX - FavCount; if ( FavCount == 0 ) StartLine++;

	top = index ;

	while( cont )
	{
		FavCount=0;
		for( i=0; i<FavoritesMAX; i++){			//	count Favorites list
			if ( files[i].filesize ) FavCount++;
		}
		StartLine=FavoritesMAX - FavCount; if ( FavCount == 0 ) StartLine++;

		Bdisp_AllClr_VRAM();
		Fkey_dispN( 0,"EXE ");
		Fkey_dispR( 1,"EDIT");
		Fkey_dispR( 2,"NEW");
		Fkey_dispR( 3,"REN");
		Fkey_dispR( 4,"DEL");
		Fkey_dispN( 5,"Fav.");
		locate(1, 1);Print((unsigned char*)"Prog List  [        ]");
		locate(13, 1);Print( strlen(folder) ? (unsigned char*)folder : (unsigned char*)"Root");
		if( size < 1 ){
			locate( 8, 4 );
			Print( (unsigned char*)"No Data" );
		}
		else{
			char buf[22],buf2[22];
			if( index < StartLine )
				index = StartLine;
			if( top > index )
				top = index;
			if( index > top + N_LINE - 1 )
				top = index - N_LINE + 1;
			if( top < StartLine )
				top = StartLine;

			for(i = 0;i < N_LINE && i + top < size; ++i ){
				locate( 1, i + 2 );

				if( files[i + top].filesize == 0 ) {
					sprintf( buf, "---------------------");
				} else
				if ( files[i + top].filesize == 0xFFFF ) {
					sprintf( buf, "------Favorites------");
				} else
				if( files[i + top].filesize == -1 ) {
					sprintf( buf, " [%s]", files[i + top].filename );
				} else {
					strncpy( buf2, files[i + top].filename, FILENAMEMAX);
					j=strlen(files[i + top].filename);
					if (j<4) j=4;
					buf2[j-4]='\0';
					sprintf( buf, " %-12s:%6u ", buf2, files[i + top].filesize - 0x38 );
				}
				Print( (unsigned char*)buf );
			}
			Bdisp_AreaReverseVRAM( 0, (index-top+1)*8 , 127, (index-top+2)*8-1 );
			if( top > 0 )
				PrintXY( 120, 8, (unsigned char*)"\xE6\x92", top == index );
			if( top + N_LINE < size  )
				PrintXY( 120, N_LINE*8, (unsigned char*)"\xE6\x93" , top + N_LINE - 1 == index );
		}
		GetKey(&key);
		if ( KEY_CTRL_XTT  == key ) key='A';
		if ( KEY_CHAR_LOG  == key ) key='B';
		if ( KEY_CHAR_LN   == key ) key='C';
		if ( KEY_CHAR_SIN  == key ) key='D';
		if ( KEY_CHAR_COS  == key ) key='E';
		if ( KEY_CHAR_TAN  == key ) key='F';
		if ( KEY_CHAR_FRAC == key ) key='G';
		if ( KEY_CTRL_FD   == key ) key='H';
		if ( KEY_CHAR_LPAR == key ) key='I';
		if ( KEY_CHAR_RPAR == key ) key='J';
		if ( KEY_CHAR_COMMA== key ) key='K';
		if ( KEY_CHAR_STORE== key ) key='L';
		if ( KEY_CHAR_7    == key ) key='M';
		if ( KEY_CHAR_8    == key ) key='N';
		if ( KEY_CHAR_9    == key ) key='O';
		if ( KEY_CHAR_4    == key ) key='P';
		if ( KEY_CHAR_5    == key ) key='Q';
		if ( KEY_CHAR_6    == key ) key='R';
		if ( KEY_CHAR_MULT == key ) key='S';
		if ( KEY_CHAR_DIV  == key ) key='T';
		if ( KEY_CHAR_1    == key ) key='U';
		if ( KEY_CHAR_2    == key ) key='V';
		if ( KEY_CHAR_3    == key ) key='W';
		if ( KEY_CHAR_PLUS == key ) key='X';
		if ( KEY_CHAR_MINUS== key ) key='Y';
		if ( KEY_CHAR_0    == key ) key='Z';
		if ( ( 'A' <= key ) && ( key <= 'Z' ) ) {
			i=FavoritesMAX;
			while ( i<size ) {
				if ( files[i].filesize ==- 1 ) i++;
				else if ( files[i].filename[0]==key ) {
						index = i;
						top = index;
						break;
				} else i++; // folder skip
			}
		}
		switch ( key ) {
			case KEY_CTRL_UP:
				do {
					if( --index < StartLine  )
						index = size - 1;
				} while ( files[index].filesize == 0xFFFF ) ;
				break;
			case KEY_CTRL_DOWN:
				do {
					if( ++index > size - 1 )
						index = StartLine ;
				} while ( files[index].filesize == 0xFFFF ) ;
				break;
			case KEY_CTRL_EXE:
			case KEY_CTRL_F1:	// run
			case KEY_CTRL_F2:	// edit
			case KEY_CTRL_F3:	// New file
			case KEY_CTRL_F4:	// rename file
			case KEY_CTRL_F5:	// delete file
				cont =0 ;
				break;
			case KEY_CTRL_F6:		// ------- Favorites list
				if ( files[index].filesize == 0 ) break;
				i=0;
				while ( i < FavoritesMAX ) {	// file matching search
					if ( strcmp( files[i].filename,  files[index].filename )== 0 ) break; // not matching
					i++;
				}
				if ( i < FavoritesMAX ) { 		//	off Favorites list
					if ( YesNo( "Favorite-Off ?" ) ) {
						files[i].filesize = 0;
						memset( Favoritesfiles[i].filename, 0x00, FILENAMEMAX );
							while ( i > 0 ) {	// space adjust
								files[i].filesize = files[i-1].filesize ;
								files[i-1].filesize = 0;
								strncpy( files[i].filename, files[i-1].filename, FILENAMEMAX);
								memset( Favoritesfiles[i-1].filename, 0x00, FILENAMEMAX );
								i--;
							}
						SaveFavorites();
					}
				} else {						//	add Favorites list
					if ( files[0].filesize ) ErrorMSG( "Favorites over ",FavoritesMAX);
					else
					if ( YesNo( "Favorite-ADD ?" ) ) {
						i=1;
						while ( i < FavoritesMAX ) {	// space adjust
							files[i-1].filesize = files[i].filesize ;
							strncpy( files[i-1].filename, files[i].filename, FILENAMEMAX);
							i++;
						}
						i=FavoritesMAX-1;
						files[i].filesize = files[index].filesize ;
						strncpy( files[i].filename, files[index].filename, FILENAMEMAX);
						index=i;
						SaveFavorites();
					}
				}
				redraw = 1;
				break;
			case KEY_CTRL_EXIT:
				index = size;
				cont =0 ;
				break;
			case KEY_CTRL_VARS:
				SetVar(0);		// A - 
				break;
			case KEY_CTRL_SHIFT:
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_dispN_aA( 3, "Fv.\xE6\x92");
				Fkey_dispN_aA( 4, "Fv.\xE6\x93");
				Fkey_dispN( 5, "ver.");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CHAR_POWROOT:
							cont =0 ;
							break;
					case KEY_CTRL_SETUP:
							selectSetup=SetupG(selectSetup);
							SaveFavorites();
							break;
					case KEY_CTRL_F1:
							selectVar=SetVar(selectVar);		// A - 
							SaveFavorites();
							break;
					case KEY_CTRL_F2:
							selectMatrix=SetMatrix(selectMatrix);		//
							break;
					case KEY_CTRL_F3:
							SetViewWindow();
							SaveFavorites();
							break;
					case KEY_CTRL_F4:	// up
							if ( index <= StartLine ) break;
							strncpy( tmpname, files[index-1].filename, FILENAMEMAX);
							tmp=files[index-1].filesize;
							strncpy( files[index-1].filename, files[index].filename, FILENAMEMAX);
							files[index-1].filesize=files[index].filesize;
							strncpy( files[index].filename, tmpname, FILENAMEMAX);
							files[index].filesize=tmp;
							index--;
							SaveFavorites();
							break;
					case KEY_CTRL_F5:	// down
							if ( FavCount < 1 ) break;
							if ( index >= FavoritesMAX-1 ) break;
							strncpy( tmpname, files[index+1].filename, FILENAMEMAX);
							tmp=files[index+1].filesize;
							strncpy( files[index+1].filename, files[index].filename, FILENAMEMAX);
							files[index+1].filesize=files[index].filesize;
							strncpy( files[index].filename, tmpname, FILENAMEMAX);
							files[index].filesize=tmp;
							index++;
							SaveFavorites();
							break;
					case KEY_CTRL_F6:
							PopUpWin( 6 );
							locate( 3, 2 ); Print( (unsigned char*)"Basic Interpreter" );
							locate( 3, 3 ); Print( (unsigned char*)"&(Basic Compiler)" );
							locate( 3, 4 ); Print( (unsigned char*)"            v0.66" );
							locate( 3, 6 ); Print( (unsigned char*)"     by sentaro21" );
							locate( 3, 7 ); Print( (unsigned char*)"          (c)2015" );
							GetKey(&key);
							redraw = 1;
							break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	

	return key;
}


static int IsFileNeeded( FONTCHARACTER *find_name )
{
	char str[13];
	FontToChar(find_name,str);
	return (strcmp(str + strlen(str) - 4, ".g1m") == 0);
}

static int FileCmp( const void *p1, const void *p2 )
{
	Files *f1 = (Files *)p1;
	Files *f2 = (Files *)p2;

	if( f1->filesize == -1 && f2->filesize == -1 )
		return strcmp( f1->filename + 1, f2->filename + 1);
	else if( f1->filesize == -1 )
		return 1;
	else if( f2->filesize == -1 )
		return -1;
	else
		return strcmp( f1->filename, f2->filename );
}


FONTCHARACTER * CharToFont( const char *cFileName, FONTCHARACTER *fFileName )
{
	int len, i;
	
	for( i = 0, len = strlen( cFileName ); i < len ; ++i )
		fFileName[i] = cFileName[i];
	fFileName[i] = '\0';
	
	return fFileName;
}

char * FontToChar( const FONTCHARACTER *fFileName, char *cFileName )
{
	int i = 0;
	while( (cFileName[i] = fFileName[i]) !=0 )
		++i;
	return cFileName;
}

FONTCHARACTER * FilePath( char *sFolder, FONTCHARACTER *sFont )
{
	char path[50];

	if( strlen(sFolder)==0 )
		sprintf( path, "\\\\"ROOT"\\*" );
	else
		sprintf( path, "\\\\"ROOT"\\%s\\*",sFolder );

	//Convert to FONTCHARACTER
	CharToFont( path, sFont );
	return sFont;
}



//----------------------------------------------------------------------------------------------
/* load file to buffer */
unsigned char * loadFile( const char *name , int editMax)
{
	int handle;
	FONTCHARACTER filename[50];
	int size;
	char *buffer;


	CharToFont( name, filename );
	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 )
	{
		ErrorMSG( "Can't open file", handle);
		return NULL;
	}

	size = Bfile_GetFileSize( handle );

	buffer = ( char *)malloc( size*sizeof(char)+editMax+4 );
	if( buffer == NULL )
	{
		ErrorMSG( "Out of memory",-1);
		return NULL;
	}
	memset( buffer, 0x00,     size*sizeof(char)+editMax+4 );

	Bfile_ReadFile( handle, buffer, size, 0 );
	buffer[size] = '\0';
	strcat( buffer, " " ); // for some reasons about the preprocessor...

	Bfile_CloseFile( handle );
	return (unsigned char *)buffer;
}


/* store bytecode into file */
int storeFile( const char *name, unsigned char* codes, int size )
{
	int handle;
	FONTCHARACTER filename[50];
	int r,s;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */

	CharToFont( name, filename );
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	if( r == 0 ) { //already existed, delete it
		r = Bfile_DeleteFile( filename );
		if( r < 0 ) { ErrorMSG( "Can't delete file", r );	return 1 ; }
	}

	handle = Bfile_CreateFile( filename, size );
	if( handle < 0 ) { ErrorMSG( "Can't create file", handle ); return 1 ; }
	r = Bfile_CloseFile( handle );

	handle = Bfile_OpenFile( filename, _OPENMODE_WRITE );
	if( handle < 0 ) { ErrorMSG( "Can't open file", handle ); return 1 ; }
	Bfile_WriteFile( handle, codes, size );
	r = Bfile_CloseFile( handle );
	if( r ) { ErrorMSG( "Close error", handle ); return 1 ; }

	return 0 ;
}

void SetFullfilenameG1M( char *fname, char *sname ) {
	if( strlen(folder) == 0 )
		sprintf( fname, "\\\\"ROOT"\\%s.g1m", sname );
	else
		sprintf( fname, "\\\\"ROOT"\\%s\\%s.g1m", folder, sname );
}

/* file exist? */
int ExistFile( const char *fname )
{
	int handle;
	FONTCHARACTER filename[50];
	int r,s;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */

	CharToFont( fname, filename );
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	return r; // r==0 existed 
}

/* G1M file exist? */
int ExistG1M( const char *sname )
{
	int handle;
	FONTCHARACTER filename[50];
	int r,s;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */
	char fname[32];

	SetFullfilenameG1M( fname, sname );
	return ExistFile( fname ); // r==0 existed 
}

//----------------------------------------------------------------------------------------------

unsigned int InputStrFilename(int x, int y, int width, unsigned char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;

	buffer[width]='\0';
	csrX=strlen((char*)buffer);
	key=InputStrSub( x, y, width, 0, buffer, width, SPC, rev_mode, FLOAT_OFF, EXP_OFF, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_OFF);
	return ( key );
}

unsigned int InputFilename( char * buffer, char* msg) {		// 
	unsigned int key;
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(3);
	locate(3,3); Print((unsigned char *)msg);
	locate(3,4); Print((unsigned char *)" [        ]");
	key=InputStrFilename( 5, 4, 8, (unsigned char *)buffer, ' ', REV_OFF ) ;
	RestoreDisp(SAVEDISP_PAGE1);
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) return 1;
	return 0; // ok
}


void filenameToBas( unsigned char *filebase, char *filename) {
	unsigned char *nameptr;
	unsigned char c;
	int i;
	nameptr=filebase+0x3C;
	for (i=0;i<8;i++) nameptr[i]='\0';
	i=0;
	c=filename[i];
	while ( c != '\0' ) {
		c=filename[i];
		nameptr[i++]=c;
	}
}

void BasTofilename8( unsigned char *filebase, char *sname) {
	unsigned char *nameptr;
	unsigned char c;
	int i;
	nameptr=filebase+0x3C;
	i=0;
	while (i<8) {
		c = nameptr[i];
		if ((c==' ')) c='~';
		sname[i]=c;
		if ( c=='\0' ) break;
		i++;
	}
	sname[i]='\0';
}

void G1M_header( unsigned char *filebase ,int *size ) {
	(*size) = (*size + 3 ) & 0xFFFFFFFC ; // file size 4byte align adjust
	SetSrcSize( filebase, *size );
	filebase[0x00]=0xAA;			// G1M header set
	filebase[0x01]=0xAC;
	filebase[0x02]=0xBD;
	filebase[0x03]=0xAF;
	filebase[0x04]=0x90;
	filebase[0x05]=0x88;
	filebase[0x06]=0x9A;
	filebase[0x07]=0x8D;
	filebase[0x08]=0xCE;
	filebase[0x09]=0xFF;
	filebase[0x0A]=0xEF;
	filebase[0x0B]=0xFF;
	filebase[0x0C]=0xEF;
	filebase[0x0D]=0xFF;
	filebase[0x0E]=(0xBE-*size)&0xFF;
	filebase[0x0F]=0xFE;
	filebase[0x10]=0xFF;
	filebase[0x11]=0xFF;
	filebase[0x12]=((0xFFFF-*size)>>8)&0xFF;	//
	filebase[0x13]=((0xFFFF-*size))&0xFF;		//
	filebase[0x14]=(0x47-*size)&0xFF;			//
	filebase[0x15]=0x00;
	filebase[0x16]=0x00;
	filebase[0x17]=0x00;
	filebase[0x18]=0x00;
	filebase[0x19]=0x00;
	filebase[0x1A]=0x00;
	filebase[0x1B]=0x00;
	filebase[0x1C]=0x00;
	filebase[0x1D]=0x00;
	filebase[0x1E]=0xFF;
	filebase[0x1F]=0xFE;
}


int LoadProgfile( char *sname ) {
	unsigned char *filebase;
	int fsize,size;
	filebase = loadFile( sname , EditMaxfree );
	if ( filebase == NULL ) return 1 ;

	fsize=0xFFFF-(filebase[0x12]*256+filebase[0x13]);
	size=SrcSize( filebase ) ;
	if ( ( fsize != size ) || ( filebase[0x20] != 'P' )|| ( filebase[0x21] != 'R' ) ){ ErrorMSG( "Not support file", fsize );
		 return 1;
	}
	
	CB_PreProcess( filebase  + 0x56 );
	ProgEntryN=0;						// Main program
	ProgfileAdrs[ProgEntryN]= filebase;
	ProgfileMax[ProgEntryN]= SrcSize( filebase ) +EditMaxfree ;
	ProgfileEdit[ProgEntryN]= 0;
	ProgEntryN++;
	CB_ProgEntry( filebase + 0x56 ) ;		// sub program search
	
	return 0;
}

int SaveG1M( unsigned char * filebase ){
	char fname[32],basname[16];
	int size,i;

	BasTofilename8( filebase, basname);

	SetFullfilenameG1M( fname, basname );

	size = SrcSize(filebase);
	filebase[size+1]=0x00;
	filebase[size+2]=0x00;
	filebase[size+3]=0x00;
	G1M_header( filebase, &size );		// G1M header set
	
	return storeFile( fname, filebase, size );	// 0:ok
}

int SaveProgfile( int progNo ){
	unsigned char *filebase;
	char fname[32],basname[16],msg[32];
	int size,i;
	
	filebase=ProgfileAdrs[progNo];
	BasTofilename8( filebase, basname);

  loop:
	if ( InputFilename( basname, "Save File Name?" ) ) return 1 ;
	filenameToBas( filebase, basname);
	if ( ExistG1M( basname ) ==0 ) if ( YesNo( "Overwrite OK?" ) == 0 ) goto loop;

	return SaveG1M( filebase );
}

int SavePicture( unsigned char *filebase, int pictNo ){
	int handle;
	FONTCHARACTER filename[50];
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	char folder2[FILENAMEMAX];
	int r,s;
	char fname[32],basname[16],msg[32];
	int size,i;
 	unsigned char c,d;
 	c=pictNo/10+'0'; if ( c=='0' ) c=' ';
 	d=pictNo%10+'0';
 	
	for (i=0; i<0x4C; i++) filebase[i]=0x00;	// header clear

	size=0x800+0x4C;
	G1M_header( filebase, &size );	// G1M header set
	
	filebase[0x20]='P';			// G1M Pict header set
	filebase[0x21]='I';
	filebase[0x22]='C';
	filebase[0x23]='T';
	filebase[0x24]='U';
	filebase[0x25]='R';
	filebase[0x26]='E';
	filebase[0x27]=' ';
	filebase[0x28]= c;
	filebase[0x29]= d;
	filebase[0x33]=0x01;
	filebase[0x34]='m';
	filebase[0x35]='a';
	filebase[0x36]='i';
	filebase[0x37]='n';
	filebase[0x3C]='P';
	filebase[0x3D]='I';
	filebase[0x3E]='C';
	filebase[0x3F]='T';
 	if ( c==' ' ) { c=d; d=0; }
 	filebase[0x40]= c;
	filebase[0x41]= d;
	filebase[0x44]=0x07;

	for (i=0;i<FILENAMEMAX;i++) folder2[i]=folder[i];	// backup folder
	folder[0x00]='P';
	folder[0x01]='i';
	folder[0x02]='c';
	folder[0x03]='t';
	folder[0x04]='\0';

	sprintf( fname, "\\\\"ROOT"\\%s", folder );
	CharToFont( fname, filename );
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	if( r != 0 ) {
		handle = Bfile_CreateDirectory( filename );
		if( handle < 0 ) { 
//			ErrorMSG( "Can't create Dir", handle );
			return handle ; }
		r = Bfile_CloseFile( handle );
	}
	
	r = SaveG1M( filebase );
	for (i=0;i<FILENAMEMAX;i++) folder[i]=folder2[i];	// restore folder
	
	return r ;
}

unsigned char * LoadPicture( int pictNo ){
	unsigned char *filebase,*pict;
	char fname[32],pictname[16];
	int i;
 	unsigned char c,d;
 	c=pictNo/10+'0';
 	d=pictNo%10+'0'; if ( c=='0' ) { c=d; d=0; }

	pictname[0]='P';
	pictname[1]='I';
	pictname[2]='C';
	pictname[3]='T';
	pictname[4]= c;
	pictname[5]= d;
	pictname[6]='\0';
	
	sprintf( fname, "\\\\"ROOT"\\%s\\%s.g1m", "Pict", pictname );

	pict = loadFile( fname ,0);					//
	return pict;
}

//----------------------------------------------------------------------------------------------
void DeleteFile(char *sname) {
	FONTCHARACTER filename[50];
	int r;

	CharToFont( sname, filename );

	r = Bfile_DeleteFile( filename );
	if( r < 0 ) { ErrorMSG( "Can't delete file", r );	return ; }
}

void DeleteFileFav(char *sname ) {
	FONTCHARACTER filename[50];
	char fname[32];
	int i,j,r;
	
	CharToFont( sname, filename );

	if ( YesNo( "Delete file?" ) == 0 ) return ;
	
	r = Bfile_DeleteFile( filename );
	if( r < 0 ) { ErrorMSG( "Can't delete file", r );	return ; }

//	i=0;
//	while ( i < FavoritesMAX ) {	// file matching search
//		if ( strcmp( name,  Favoritesfiles[i].filename )== 0 ) break; // not matching
//		i++;
//	}
//	if ( i < FavoritesMAX ) { 		//	delet Favorites 
//		Favoritesfiles[i].filesize = 0;
//	}
}

//----------------------------------------------------------------------------------------------
int RenameFile( char *sname ) {
	unsigned char *filebase;
	char fname[32],basname[16],msg[32];
	int size,i,j;

	if ( LoadProgfile( sname ) ) return 1 ; // error
	
	filebase=ProgfileAdrs[0] ;
	BasTofilename8( filebase, basname);

	if ( InputFilename( basname, "Rename File Name?" ) ) return 1 ; // cancel
	SetFullfilenameG1M( fname, basname );
	if ( strcmp(sname,fname)==0 ) return 0; // no rename
	
	filenameToBas( filebase, basname);

	if ( ExistG1M( basname ) ==0 ) if ( YesNo( "Overwrite OK?" ) == 0 ) return 1 ; // cancel
	
	if ( SaveG1M( filebase ) == 0 ) DeleteFile( sname ) ;
	else return 1;

	i=0;
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( name,  Favoritesfiles[i].filename )== 0 ) break; // not matching
		i++;
	}
	if ( i < FavoritesMAX ) { 		//	rename Favorites 
		for (j=0;j<FILENAMEMAX;j++) fname[j]='\0';
		sprintf(fname, "%s.g1m", basname );
		if ( Favoritesfiles[i].filesize ) strncpy( Favoritesfiles[i].filename, fname, FILENAMEMAX);
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------
//------------------------------------------------------------------------strage memory version
#define ConfigMAX 1000
/*
void SaveConfigS(){
	char fname[]="\\\\"ROOT"\\CBASIC.cnf";
	unsigned char buffer[ConfigMAX];
	unsigned char *sbuf;
	int    *bufint=(int*)buffer;
	double *bufdbl=(double*)buffer;
	int size,i,r;
	
	memset( buffer, 0x00, ConfigMAX );

	buffer[ 0]='C';
	buffer[ 1]='B';
	buffer[ 2]='.';
	buffer[ 3]='c';
	buffer[ 4]='o';
	buffer[ 5]='n';
	buffer[ 6]='f';
	buffer[ 7]='i';
	buffer[ 8]='g';
	buffer[ 9]='0';
	buffer[10]='5';
	buffer[11]='0';
	buffer[12]='\0';
	buffer[13]='\0';
	buffer[14]='\0';
	buffer[15]='\0';
	buffer[16]='C';
	buffer[17]='B';
	buffer[18]='.';
	buffer[19]='c';
	buffer[20]='o';
	buffer[21]='n';
	buffer[22]='f';
	buffer[23]='i';
	buffer[24]='g';
	buffer[25]='0';
	buffer[26]='5';
	buffer[27]='0';
	buffer[28]='\0';
	buffer[29]='\0';
	buffer[30]='\0';
	buffer[31]='\0';

	bufint[ 8]=DrawType;
	bufint[ 9]=Coord;
	bufint[10]=Grid;
	bufint[11]=Axes;
	bufint[12]=Label;
	bufint[13]=Derivative;
	bufint[14]=S_L_Style;
	bufint[15]=Angle;
	bufint[16]=BreakCheck;
	bufint[17]=TimeDsp;
	
	bufint[18]=0;
	bufint[19]=0;
	bufint[20]=0;
	bufint[21]=0;
	bufint[22]=0;
	bufint[23]=0;
	bufint[24]=0;
	bufint[25]=0;
	bufint[26]=0;
	bufint[27]=0;
	bufint[28]=0;
	bufint[29]=0;
	bufint[30]=0;
	bufint[31]=0;

	bufdbl[16]=Xfct;
	bufdbl[17]=Yfct;
	bufdbl[18]=0;
	bufdbl[19]=0;
	for ( i=20; i< 20+26 ; i++ ) bufdbl[i]=REG[i-20];
	for ( i=46; i< 46+11 ; i++ ) bufdbl[i]=REGv[i-46];

	sbuf=buffer+512;

	for ( i=0; i<FavoritesMAX; i++ ) {
		strncpy( (char*)sbuf, Favoritesfiles[i].filename, FILENAMEMAX);
		sbuf+=FILENAMEMAX;
	}

	r=storeFile( fname, buffer, ConfigMAX );	// 0:ok
}

void LoadConfigS(){
	char fname[]="\\\\"ROOT"\\CBASIC.cnf";
	unsigned char *buffer;
	unsigned char *sbuf;
	int    *bufint;
	double *bufdbl;
	int size,i;
	
	if ( ExistFile( fname ) ) return ; // no config file
	
	buffer = loadFile( fname ,0);					//
	bufint=(int*)buffer;
	bufdbl=(double*)buffer;
	
	if ( ( buffer[ 0]=='C' ) &&	// file check
		 ( buffer[ 1]=='B' ) &&
		 ( buffer[ 2]=='.' ) &&
		 ( buffer[ 3]=='c' ) &&
		 ( buffer[ 4]=='o' ) &&
		 ( buffer[ 5]=='n' ) &&
		 ( buffer[ 6]=='f' ) &&
		 ( buffer[ 7]=='i' ) &&
		 ( buffer[ 8]=='g' ) ) {
		
		DrawType  =bufint[ 8];	// load config & memory
		Coord     =bufint[ 9];
		Grid      =bufint[10];
		Axes      =bufint[11];
		Label     =bufint[12];
		Derivative=bufint[13];
		S_L_Style =bufint[14];
		Angle     =bufint[15];
		BreakCheck=bufint[16];
		TimeDsp   =bufint[17];

		Xfct=bufdbl[16];
		Yfct=bufdbl[17];
		for ( i=20; i< 20+26 ; i++ ) REG[i-20] =bufdbl[i];
		for ( i=46; i< 46+11 ; i++ ) REGv[i-46]=bufdbl[i];

		sbuf=buffer+512;

		for ( i=0; i<FavoritesMAX; i++ ) {
			strncpy( Favoritesfiles[i].filename, (char*)sbuf, FILENAMEMAX);
			sbuf+=FILENAMEMAX;
		}
	}
}
*/
//--------------------------------------------------------------

void SaveFavorites(){
	int i;
	for( i=0; i<FavoritesMAX; i++){			//	backup Favorites list
			strncpy( Favoritesfiles[i].filename, files[i].filename, FILENAMEMAX);
			Favoritesfiles[i].filesize = files[i].filesize;
	}
	SaveConfig();
}
//------------------------------------------------------------------------ main memory version
void SaveConfig(){
	const unsigned char fname[]="CBasic";
	unsigned char buffer[ConfigMAX];
	unsigned char *sbuf;
	int    *bufint=(int*)buffer;
	double *bufdbl=(double*)buffer;
	int size,i,r;
	int handle,state;

	handle=Bfile_OpenMainMemory(fname);
	if (handle >= 0) {		// Already Exists
		Bfile_CloseFile(handle);
	}
	
	if (handle==IML_FILEERR_ENTRYNOTFOUND) {
		handle=Bfile_CreateMainMemory(fname);
		if (handle<0) {ErrorMSG("Create Error",handle); return;}
		state=Bfile_CloseFile(handle);
		if (state<0)  {ErrorMSG("Close Error",state); return;}
	}

	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	
	memset( buffer, 0x00, ConfigMAX );
	buffer[ 0]='C';
	buffer[ 1]='B';
	buffer[ 2]='.';
	buffer[ 3]='c';
	buffer[ 4]='o';
	buffer[ 5]='n';
	buffer[ 6]='f';
	buffer[ 7]='i';
	buffer[ 8]='g';
	buffer[ 9]='0';
	buffer[10]='6';
	buffer[11]='0';

	bufint[ 3]=CB_INT;
	bufint[ 4]=DrawType;
	bufint[ 5]=Coord;
	bufint[ 6]=Grid;
	bufint[ 7]=Axes;
	bufint[ 8]=Label;
	bufint[ 9]=Derivative;
	bufint[10]=S_L_Style;
	bufint[11]=Angle;
	bufint[12]=BreakCheck;
	bufint[13]=TimeDsp;
	bufint[14]=MatXYmode;
	bufint[15]=0;

	bufdbl[ 8]=Xfct;
	bufdbl[ 9]=Yfct;
	for ( i=10; i< 10+58 ; i++ ) bufdbl[i]=REG[i-10];
	for ( i=68; i< 68+11 ; i++ ) bufdbl[i]=REGv[i-68];
	for ( i=160; i< 160+58 ; i++ ) bufint[i]=REGINT[i-160];

	sbuf=buffer+220*4;
	
	strncpy( (char*)sbuf, folder, FILENAMEMAX);
	sbuf+=FILENAMEMAX;
	for ( i=0; i<FavoritesMAX; i++ ) {
		strncpy( (char*)sbuf, Favoritesfiles[i].filename, FILENAMEMAX);
		sbuf+=FILENAMEMAX;
	}

	handle=Bfile_OpenMainMemory(fname);
	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	state=Bfile_WriteFile(handle,buffer,ConfigMAX);
	if (state<0)  {ErrorMSG("Write Error",state); return;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return;}
}


void LoadConfig(){
	const unsigned char fname[]="CBasic";
	unsigned char buffer[ConfigMAX];
	unsigned char *sbuf;
	int    *bufint;
	double *bufdbl;
	int size,i;
	int handle,state;

	handle=Bfile_OpenMainMemory(fname);
	if (handle<0)  { // Open Error
		return;
	}
	state=Bfile_ReadFile(handle, buffer, ConfigMAX, 0);
	if (state<0)  {ErrorMSG("Read Error",state); return;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return;}

	bufint=(int*)buffer;
	bufdbl=(double*)buffer;
	
	if ( ( buffer[ 0]=='C' ) &&	// file check
		 ( buffer[ 1]=='B' ) &&
		 ( buffer[ 2]=='.' ) &&
		 ( buffer[ 3]=='c' ) &&
		 ( buffer[ 4]=='o' ) &&
		 ( buffer[ 5]=='n' ) &&
		 ( buffer[ 6]=='f' ) &&
		 ( buffer[ 7]=='i' ) &&
		 ( buffer[ 8]=='g' ) &&
		 ( buffer[ 9]=='0' ) &&
		 ( buffer[10]=='6' ) &&
		 ( buffer[11]=='0' ) ) {
		
		CB_INT    =bufint[ 3];
		DrawType  =bufint[ 4];	// load config & memory
		Coord     =bufint[ 5];
		Grid      =bufint[ 6];
		Axes      =bufint[ 7];
		Label     =bufint[ 8];
		Derivative=bufint[ 9];
		S_L_Style =bufint[10];
		Angle     =bufint[11];
		BreakCheck=bufint[12];
		TimeDsp   =bufint[13];
		MatXYmode =bufint[14];

		Xfct=bufdbl[ 8];
		Yfct=bufdbl[ 9];
		for ( i=10; i< 10+58 ; i++ ) REG[i-10] =bufdbl[i];
		for ( i=68; i< 68+11 ; i++ ) REGv[i-68]=bufdbl[i];
		for ( i=160; i< 160+58 ; i++ ) REGINT[i-160]=bufint[i];

		sbuf=buffer+220*4;

		strncpy( folder, (char*)sbuf, FILENAMEMAX);
		sbuf+=FILENAMEMAX;
		for ( i=0; i<FavoritesMAX; i++ ) {
			strncpy( Favoritesfiles[i].filename, (char*)sbuf, FILENAMEMAX);
			sbuf+=FILENAMEMAX;
		}
	} else {
		Bfile_DeleteMainMemory((const unsigned char*)"CBasic");
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
#define NewMax 2048+0x4C

int NewProg(){
	unsigned char *filebase;
	char fname[32],basname[16],msg[32];
	int size,i;
	
	basname[0]='\0';
	if ( InputFilename( basname, "New File Name?" ) ) return 1 ;
	if ( ExistG1M( basname ) == 0 ) { // existed file
		SetFullfilenameG1M( fname, basname );
		LoadProgfile( fname );
		return 0;
	}
	
	size=NewMax;
	filebase = (unsigned char *)malloc( size*sizeof(char)+4 );
	memset( filebase, 0x77,             size*sizeof(char)+4 );
	if( filebase == NULL ) {
		ErrorMSG( "Out of memory",-1);
		return 1;
	}
	for (i=0; i<0x56+4; i++) filebase[i]=0x00;	// header clear

	size=0x56+1;
	G1M_header( filebase, &size );	// G1M header set
	
	filebase[0x20]='P';			// G1M Basic header set
	filebase[0x21]='R';
	filebase[0x22]='O';
	filebase[0x23]='G';
	filebase[0x24]='R';
	filebase[0x25]='A';
	filebase[0x26]='M';
	filebase[0x33]=0x01;
	filebase[0x34]='s';
	filebase[0x35]='y';
	filebase[0x36]='s';
	filebase[0x37]='t';
	filebase[0x38]='e';
	filebase[0x39]='m';
	filebase[0x3C]=0x30;
	filebase[0x44]=0x01;
	
	filenameToBas( filebase, basname);

	ProgEntryN=0;						// new Main program
	ProgfileAdrs[ProgEntryN]= filebase;
	ProgfileMax[ProgEntryN]= SrcSize( filebase ) +EditMaxfree ;
	ProgfileEdit[ProgEntryN]= 1;

	return 0;
}

//----------------------------------------------------------------------------------------------
void CB_ProgEntry( unsigned char *SRC ) { //	Prog "..." into memory
	unsigned int c=1;
	unsigned char buffer[32]="";
	char filename[32];
	unsigned char *src;
	unsigned char *StackProgSRC;
	int StackProgPtr;
	unsigned int key=0;

//	locate( 1, 1); PrintLine(" ",21);						//
//	sprintf(buffer,"==%-8s==%08X",SRC-0x56+0x3C, SRC-0x56);
//	locate (1, 1); Print( (unsigned char*)buffer );

	ExecPtr=0;
	while ( c!=0 ) {
		c=SRC[ExecPtr++];
		if ( c==0x00 ) { ExecPtr--; break; }
		switch ( c ) {
			case 0x3A:	// <:>
			case 0x0D:	// <CR>
				break;
			case 0xED:	// Prog "..."
				ExecPtr++;	// " skip
				GetQuotOpcode(SRC, buffer, 32);	// Prog name
//				locate( 1, 2); PrintLine(" ",21);						//
//				locate( 1, 2); Print(buffer);							//
//				locate( 1, 3); PrintLine(" ",21); GetKey(&key);			//
				ProgNo = CB_SearchProg( buffer );
				if ( ProgNo < 0 ) { 				// undefined Prog
					SetFullfilenameG1M( filename, (char*)buffer );
					src = loadFile( filename ,EditMaxfree);
//					locate( 1, 3); Print(filename);						//
//					locate( 1, 4); PrintLine(" ",21);					//
//					sprintf(buffer,"ptr=%08X",src);						//
//					locate( 1, 4); Print(buffer); GetKey(&key);			//
					if ( src !=NULL ) {
						ProgfileAdrs[ProgEntryN]= src;
						ProgfileMax[ProgEntryN]= SrcSize( src );
						ProgfileEdit[ProgEntryN]= 0;
						ProgEntryN++;
						if ( ProgEntryN > ProgMax ) { ErrorNo=MemoryERR; ErrorPtr=ExecPtr; } // Memory error
						StackProgPtr = ExecPtr;
						CB_PreProcess( src + 0x56 );
						CB_ProgEntry( src + 0x56  );		//
						ExecPtr = StackProgPtr ;
					}
				}
				break;
			case 0xF7:	// 
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
			case 0xFF:	//
				ExecPtr++;
				break;
			default:
				break;
		}
	}

	SetSrcSize( SRC-0x56 , ExecPtr+0x56+1 );
}

