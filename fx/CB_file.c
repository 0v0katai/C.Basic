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
#include "CBI_interpreter.h"
#include "CB_Eval.h"
#include "CBI_Eval.h"
#include "CB_Matrix.h"
#include "CB_setup.h"
#include "CB_TextConv.h"

#include "CB_test.h"

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
static char rename[FILENAMEMAX] = "";
Files Favoritesfiles[FavoritesMAX];
char	FavoritesSize=-1;
char	FileListUpdate=1;

unsigned int SelectFile (char *filename)
{
	unsigned int key;

	Bdisp_AllClr_DDVRAM();
	while( 1 ){
		if ( FileListUpdate  ) {
			size = ReadFile( folder );
			qsort( files, size, sizeof(Files), FileCmp );
		}
		key = Explorer( size, folder ) ;
		if ( key == KEY_CTRL_F3 ) break ;	// new file
		if ( key == KEY_CHAR_POWROOT ) break ;	// sdk built in file

		if ( key == KEY_CTRL_EXIT ) {			//to top of list
			index = 0;
			SaveConfig();
		} else
		if ( ( key == KEY_CTRL_QUIT ) || ( index == size ) ) {			//return to root
			if( strlen(folder) ) FileListUpdate = 1 ; // folder to root
			folder[0] = '\0';	
			index = 0;
			SaveConfig();
		} else
		if( files[index].filesize == -1 ){				//folder
			strcpy( folder,files[index].filename );
			index = 0;
			FileListUpdate = 1 ;
		} else {										//file
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

void FileListfree() {
	if ( FileListUpdate ) {
		if ( files != NULL ) free( files );
	}
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
void FavoritesFunc( int *index ) {
	char tmpname[FILENAMEMAX];
	int i;
	if ( files[(*index)].filesize == 0 ) return;
	i=0;
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( files[i].filename,  files[(*index)].filename )== 0 ) break; // not matching
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
			files[i].filesize = files[(*index)].filesize ;
			strncpy( files[i].filename, files[(*index)].filename, FILENAMEMAX);
			(*index)=i;
			SaveFavorites();
		}
	}
}
void FavoritesUp( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	strncpy( tmpname, files[(*index)-1].filename, FILENAMEMAX);
	tmp=files[(*index)-1].filesize;
	strncpy( files[(*index)-1].filename, files[(*index)].filename, FILENAMEMAX);
	files[(*index)-1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX);
	files[(*index)].filesize=tmp;
	(*index)--;
	SaveFavorites();
}
void FavoritesDown( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	strncpy( tmpname, files[(*index)+1].filename, FILENAMEMAX);
	tmp=files[(*index)+1].filesize;
	strncpy( files[(*index)+1].filename, files[(*index)].filename, FILENAMEMAX);
	files[(*index)+1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX);
	files[(*index)].filesize=tmp;
	(*index)++;
	SaveFavorites();
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
	int filemode=0;
	
	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep

	top = index ;
	
	if ( FileListUpdate ) {
		FavCount=0;
		j=FavoritesMAX-1;
		for( i=FavoritesMAX-1; i>=0; i--){		//	set favorites list
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
		files[FavoritesMAX].filesize = 0xFFFE;	// separator
		
		if ( ( rename[0] != '\0' ) && ( index > FavCount) ){
			for ( k=FavCount; k<size; k++ ) {
				if ( files[k].filesize != -1 ) {
					if ( strncmp( files[k].filename,  rename,  strlen(files[k].filename)-4 ) == 0 ) index=k; // rename name matching
				}
			}
		}
	}
	FileListUpdate = 0 ;
	rename[0] = '\0';

	while( cont )
	{
		FavCount=0;
		for( i=0; i<FavoritesMAX; i++){			//	count Favorites list
			if ( files[i].filesize ) FavCount++;
		}
		StartLine=FavoritesMAX - FavCount; if ( FavCount == 0 ) StartLine++;

		Bdisp_AllClr_VRAM();
		switch ( filemode ) {
			case 0:
				Fkey_dispN( 0,"EXE ");
				Fkey_dispR( 1,"EDIT");
				Fkey_dispR( 2,"NEW");
				Fkey_dispR( 3,"COPY");
				Fkey_dispR( 4,"DEL");
				Fkey_DISPN( 5," \xE6\x9E ");
				break;
			case 1:
				Fkey_dispN( 0,"Text");
				Fkey_dispR( 1,"REN");
//				Fkey_Clear( 2 );
//				Fkey_Clear( 3 );
//				Fkey_Clear( 4 );
				Fkey_dispN( 2, "Fav.");
				Fkey_dispN_aA( 3, "Fv.\xE6\x92");
				Fkey_dispN_aA( 4, "Fv.\xE6\x93");
				Fkey_DISPN( 5," \xE6\x9E ");
				break;
		}
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
				if ( files[i + top].filesize == 0xFFFE ) {
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
				if ( files[i].filesize == -1 ) i++;
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
				} while ( files[index].filesize == 0xFFFE ) ;
				break;
			case KEY_CTRL_DOWN:
				do {
					if( ++index > size - 1 )
						index = StartLine ;
				} while ( files[index].filesize == 0xFFFE ) ;
				break;
			case KEY_CTRL_EXE:
			case KEY_CTRL_F1:	// run
				switch ( filemode ) {
					case 0:
						key=FileCMD_RUN;
						break;
					case 1:
						key=FileCMD_TEXT;
						break;
				}
				cont =0 ;
				break;
			case KEY_CTRL_F2:	// edit
				switch ( filemode ) {
					case 0:
						key=FileCMD_EDIT;
						break;
					case 1:
						key=FileCMD_RENAME;
						break;
				}
				cont =0 ;
				break;
			case KEY_CTRL_F3:	// New file
				switch ( filemode ) {
					case 0:
						key=FileCMD_NEW;
						cont =0 ;
						break;
					case 1:
						FavoritesFunc( &index );
						break;
				}
				break;
			case KEY_CTRL_F4:	// Copy file
				switch ( filemode ) {
					case 0:
						key=FileCMD_COPY;
						cont =0 ;
						break;
					case 1:
						if ( index <= StartLine ) break;
						if ( index >= FavoritesMAX-1 ) break;
						FavoritesUp( &index );
						break;
				}
				break;
			case KEY_CTRL_F5:	// delete file
				switch ( filemode ) {
					case 0:
						key=FileCMD_DEL;
						cont =0 ;
						break;
					case 1:
						if ( FavCount < 1 ) break;
						if ( index >= FavoritesMAX-1 ) break;
						FavoritesDown( &index );
						break;
				}
				break;
			case KEY_CTRL_OPTN:		// ------- Favorites list
				filemode = 0 ;
				FavoritesFunc( &index );
				break;
			case KEY_CTRL_F6:		// ------- change function
				filemode = 1-filemode ;
				break;
			
			case KEY_CTRL_EXIT:
				index = size;
				cont =0 ;
				break;
			case KEY_CTRL_VARS:
				filemode = 0 ;
				SetVar(0);		// A - 
				break;
			case KEY_CTRL_SHIFT:
				filemode = 0 ;
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_dispN_aA( 3, "Fv.\xE6\x92");
				Fkey_dispN_aA( 4, "Fv.\xE6\x93");
				Fkey_dispN( 5, "Fav.");
				GetKey(&key);
				switch (key) {
					case KEY_CHAR_POWROOT:
							key=FileCMD_Prog;
					case KEY_CTRL_EXIT:
					case KEY_CTRL_QUIT:
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
					case KEY_CTRL_PAGEUP:	// up
					case KEY_CTRL_F4:	// up
							if ( index <= StartLine ) break;
							if ( index >= FavoritesMAX-1 ) break;
							FavoritesUp( &index );
							break;
					case KEY_CTRL_PAGEDOWN:	// down
					case KEY_CTRL_F5:	// down
							if ( FavCount < 1 ) break;
							if ( index >= FavoritesMAX-1 ) break;
							FavoritesDown( &index );
							break;
					case KEY_CTRL_F6:
//							VerDisp();
//							redraw = 1;
							FavoritesFunc( &index );
							break;
					case KEY_CHAR_ASIN:
							if ( CB_INTDefault ) CBint_test(); else CB_test();
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
	
	SaveFavorites();

	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
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

void SetShortName( char *sname, char *name) {
	int c,i;
	int ptr=0;
	c=name[ptr++];
	while( c=='\\' ) c=name[ptr++];	//
	while( c!='\\' ) c=name[ptr++];	// ROOT skip
	if ( strchr(name+ptr,'\\') != NULL ) { name=strchr(name+ptr,'\\'); ptr=0;}
	c=name[ptr];
	i=0;
	while ( c ) {
		c=name[ptr++];
		sname[i++]=c;
	}
	name[i]='\0';
}

void ErrorMSGfile( char *buffer, char *name){
	char sname[32];
	SetShortName( sname, name);
	ErrorMSGstr( buffer, sname);
}

//----------------------------------------------------------------------------------------------
/* load file to buffer */
char * loadFile( const char *name , int editMax)
{
	int handle;
	FONTCHARACTER filename[50];
	int size;
	char *buffer;


	CharToFont( name, filename );
	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 )
	{
		ErrorMSGfile( "Can't find file", name);
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
	return buffer;
}


/* store bytecode into file */
int storeFile( const char *name, unsigned char* codes, int size )
{
	int handle;
	FONTCHARACTER filename[50];
	int r,s;
	int freespace[2];

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */

	CharToFont( name, filename );
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	if( r == 0 ) { //already existed, delete it
		r=Bfile_GetMediaFree(DEVICE_STORAGE,freespace);
		if ( r != 0 ) { ErrorMSG( "SMEM error", r );	return 1 ; }
		if ( freespace[0]==0 ) freespace[0]=freespace[1];
		if ( freespace[0] < size+256 ) { ErrorMSG( "Not enough SMEM", freespace[0] ); return 1 ; }
		r = Bfile_DeleteFile( filename );
		if( r < 0 ) { ErrorMSG( "Can't delete file", r );	return 1 ; }
	}
	FileListUpdate=1;
	handle = Bfile_CreateFile( filename, size );
	if( handle < 0 ) { ErrorMSGfile( "Can't create file", name ); return 1 ; }
	r = Bfile_CloseFile( handle );

	handle = Bfile_OpenFile( filename, _OPENMODE_WRITE );
	if( handle < 0 ) { ErrorMSG( "Can't open file", handle ); return 1 ; }
	Bfile_WriteFile( handle, codes, size );
	r = Bfile_CloseFile( handle );
	if( r ) { ErrorMSG( "Close error", handle ); return 1 ; }

	return 0 ;
}

void SetFullfilenameExt( char *fname, char *sname, char *extname ) {
	if( strlen(folder) == 0 )
		sprintf( fname, "\\\\"ROOT"\\%s.%s", sname, extname );
	else
		sprintf( fname, "\\\\"ROOT"\\%s\\%s.%s", folder, sname, extname );
}

int GetFileSize( const char *fname ) {
	int handle;
	FONTCHARACTER filename[50];
	int r,s;
	int size;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */

	CharToFont( fname, filename );

	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) { //	Can't find file
		return 0;
	}
	size = Bfile_GetFileSize( handle );
	r = Bfile_CloseFile( handle );
	return size;
/*
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	if ( r == 0 ) {	// existed
		handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
		size = Bfile_GetFileSize( handle );
		r = Bfile_CloseFile( handle );
		return size;
	} else return 0;
*/
}

/* file exist? */
int ExistFile( const char *fname ) {
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
int ExistG1M( const char *sname ) {
	int handle;
	FONTCHARACTER filename[50];
	int r,s;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	/* end */
	char fname[32];

	SetFullfilenameExt( fname, sname, "g1m" );
	return ExistFile( fname ); // r==0 existed 
}

//----------------------------------------------------------------------------------------------

unsigned int InputStrFilename(int x, int y, int width, char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;

	buffer[width]='\0';
//	csrX=strlen((char*)buffer);
	key=InputStrSub( x, y, width, 0, buffer, width, SPC, rev_mode, FLOAT_OFF, EXP_OFF, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_OFF);
	return ( key );
}

int InputFilename( char * buffer, char* msg) {		// 
	unsigned int key;
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(3);
	locate(3,3); Print((unsigned char *)msg);
	locate(3,4); Print((unsigned char *)" [        ]");
	key=InputStrFilename( 5, 4, 8, buffer, ' ', REV_OFF ) ;
	RestoreDisp(SAVEDISP_PAGE1);
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) return 1;
	return 0; // ok
}


void filename8ToSname( char *filebase, char *sname) {
	char *nameptr;
	int c;
	int i;
	nameptr=filebase+0x3C;
	for (i=0;i<8;i++) nameptr[i]='\0';
	i=0;
	c=sname[i];
	while ( c != '\0' ) {
		c=sname[i];
		if (i<8) nameptr[i]=c;
		i++;
	}
}

void SnameTofilename8( char *filebase, char *sname) {
	char *nameptr;
	int c;
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

void G1M_header( char *filebase ,int *size ) {
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

void G1M_Basic_header( char *filebase ) {
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
	filebase[0x44]=0x01;	// basic
}


int LoadProgfile( char *name, int searchProg ) {
	char *filebase;
	int fsize,size;
	int plus=0;
	if ( searchProg ) plus=EditMaxfree;
	filebase = loadFile( name , plus );
	if ( filebase == NULL ) { ErrorMSGfile( "Not enough memory", name ); return 1 ; }

	fsize=0xFFFF-((filebase[0x12]&0xFF)*256+(filebase[0x13]&0xFF));
	size=SrcSize( filebase ) ;
	if ( ( fsize != size ) || ( filebase[0x20] != 'P' )|| ( filebase[0x21] != 'R' ) ){ ErrorMSG( "Not support file", fsize );
		 return 1;
	}

	ProgEntryN=0;						// Main program
	ProgfileAdrs[ProgEntryN]= filebase;
	ProgfileMax[ProgEntryN]= SrcSize( filebase ) +EditMaxfree ;
	ProgfileEdit[ProgEntryN]= 0;
	ProgEntryN++;
	ErrorNo=0;
	if ( searchProg ) CB_ProgEntry( filebase + 0x56 ) ;		// sub program search
	if ( ErrorNo ) return ErrorNo; // error
	ProgNo=0;
	ExecPtr=0;

	return 0;
}

int SaveG1M( char *filebase ){
	char fname[32],basname[16];
	int size,i;

	SnameTofilename8( filebase, basname);

	SetFullfilenameExt( fname, basname, "g1m" );

	size = SrcSize(filebase);
	filebase[size+1]=0x00;
	filebase[size+2]=0x00;
	filebase[size+3]=0x00;
	G1M_header( filebase, &size );		// G1M header set
	
	return storeFile( fname, (unsigned char*)filebase, size );	// 0:ok
}

int SaveProgfile( int progNo ){
	char *filebase;
	char fname[32],basname[16];
	int size,i;
	
	filebase=ProgfileAdrs[progNo];
	SnameTofilename8( filebase, basname);
	G1M_Basic_header( filebase );	// G1M Basic header set

  loop:
	if ( InputFilename( basname, "Save File Name?" ) ) return 1 ;
	filename8ToSname( filebase, basname);
	if ( ExistG1M( basname ) ==0 ) if ( YesNo( "Overwrite OK?" ) == 0 ) goto loop;

	strncpy( rename, basname, FILENAMEMAX);
	return SaveG1M( filebase );
}

int SavePicture( char *filebase, int pictNo ){
	int handle;
	FONTCHARACTER filename[50];
	FONTCHARACTER buffer[50];
	FILE_INFO info;
	char folder2[FILENAMEMAX];
	int r,s;
	char fname[32],basname[16];
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
	filebase[0x44]=0x07;	// Picture

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

char * LoadPicture( int pictNo ){
	char *filebase,*pict;
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
	FileListUpdate=1;
}

void DeleteFileFav(char *sname ) {
	FONTCHARACTER filename[50];
	char fname[32];
	int i,j,r;
	
	if ( YesNo( "Delete file?" ) == 0 ) return ;

	DeleteFile( sname );

}

//----------------------------------------------------------------------------------------------
int RenameFile( char *sname ) {
	char *filebase;
	char fname[32],basname[16];
	int size,i,j;

	if ( LoadProgfile( sname, 0) ) return 1 ; // error

	filebase = ProgfileAdrs[0];
	SnameTofilename8( filebase, basname);

	if ( InputFilename( basname, "Rename File Name?" ) ) return 1 ; // cancel
	SetFullfilenameExt( fname, basname, "g1m" );
	if ( strcmp(sname,fname)==0 ) return 0; // no rename
	
	filename8ToSname( filebase, basname);

	if ( ExistG1M( basname ) ==0 ) if ( YesNo( "Overwrite OK?" ) == 0 ) return 1 ; // cancel
	
	if ( SaveG1M( filebase ) == 0 ) DeleteFile( sname ) ;
	else return 1;

	strncpy( rename, basname, FILENAMEMAX);
	
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
int CopyFile( char *sname ) {
	char *filebase;
	char fname[32],basname[16];
	int size,i,j;

	if ( LoadProgfile( sname, 0) ) return 1 ; // error

	filebase = ProgfileAdrs[0];
	SnameTofilename8( filebase, basname);

	if ( InputFilename( basname, "Copy File Name?" ) ) return 1 ; // cancel
	SetFullfilenameExt( fname, basname, "g1m" );
	if ( strcmp(sname,fname)==0 ) return 0; // no rename
	
	filename8ToSname( filebase, basname);

	if ( ExistG1M( basname ) ==0 ) if ( YesNo( "Overwrite OK?" ) == 0 ) return 1 ; // cancel
	
	if ( SaveG1M( filebase ) ) return 1;

	strncpy( rename, basname, FILENAMEMAX);
	
	return 0;
}

//----------------------------------------------------------------------------------------------
#define ConfigMAX 1000
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
	short  *bufshort=(short*)buffer;
	int    *bufint =(int*)buffer;
	double *bufdbl =(double*)buffer;
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

	bufshort[ 7]=CB_INTDefault;		bufshort[ 6]=0;
	bufshort[ 9]=DrawType;			bufshort[ 8]=0;
	bufshort[11]=Coord;				bufshort[10]=0;
	bufshort[13]=Grid;				bufshort[12]=0;
	bufshort[15]=Axes;				bufshort[14]=0;
	bufshort[17]=Label;				bufshort[16]=0;
	bufshort[19]=Derivative;		bufshort[18]=0;
	bufshort[21]=S_L_Style;			bufshort[20]=0;
	bufshort[23]=Angle;				bufshort[22]=0;
	bufshort[25]=BreakCheck;		bufshort[24]=0;
	bufshort[27]=TimeDsp;			bufshort[26]=0;
	bufshort[29]=MatXYmode;			bufshort[28]=0;
	bufshort[31]=PictMode;			bufshort[30]=CheckIfEnd;

	bufdbl[ 8]=Xfct;
	bufdbl[ 9]=Yfct;
	for ( i= 10; i<  10+26 ; i++ ) bufdbl[i]=REG[i-10];
	for ( i= 42; i<  42+26 ; i++ ) bufdbl[i]=REGsmall[i-42];
	for ( i= 68; i<  68+11 ; i++ ) bufdbl[i]=REGv[i-68];
	for ( i=160; i< 160+26 ; i++ ) bufint[i]=REGINT[i-160];
	for ( i=192; i< 192+26 ; i++ ) bufint[i]=REGINTsmall[i-192];

	bufshort[218*2  ]=(short)KeyRepeatFirstCount;
	bufshort[218*2+2]=(short)KeyRepeatNextCount;
	
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
	short  *bufshort;
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

	bufshort=(short*)buffer;
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
									// load config & memory
		CB_INTDefault =bufshort[ 7];		
		DrawType      =bufshort[ 9];        
		Coord         =bufshort[11];        
		Grid          =bufshort[13];        
		Axes          =bufshort[15];        
		Label         =bufshort[17];        
		Derivative    =bufshort[19];        
		S_L_Style     =bufshort[21];        
		Angle         =bufshort[23];        
		BreakCheck    =bufshort[25];        
		TimeDsp       =bufshort[27];        
		MatXYmode     =bufshort[29];        
		PictMode      =bufshort[31];        CheckIfEnd    =bufshort[30];

		Xfct=bufdbl[ 8];
		Yfct=bufdbl[ 9];
		for ( i= 10; i<  10+26 ; i++ ) REG[i-10]   =bufdbl[i];
		for ( i= 42; i<  42+26 ; i++ ) REGsmall[i-42] =bufdbl[i];
		for ( i= 68; i<  68+11 ; i++ ) REGv[i-68]=bufdbl[i];
		for ( i=160; i< 160+26 ; i++ ) REGINT[i-160]=bufint[i];
		for ( i=192; i< 192+26 ; i++ ) REGINTsmall[i-192]=bufint[i];

		KeyRepeatFirstCount=bufshort[218*2  ]; if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount=20;
		KeyRepeatNextCount =bufshort[218*2+2]; if ( KeyRepeatNextCount  < 1 ) KeyRepeatNextCount =5;

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
int NewProg(){
	char *filebase;
	char fname[32],basname[16];
	int size,i;
	
	basname[0]='\0';
	if ( InputFilename( basname, "New File Name?" ) ) return 1 ;
	if ( ExistG1M( basname ) == 0 ) { // existed file
		SetFullfilenameExt( fname, basname, "g1m" );
		LoadProgfile( fname, 1);
		return 0;
	}
	
	size=NewMax;
	filebase = (char *)malloc( size*sizeof(char)+4 );
	memset( filebase, 0x77,             size*sizeof(char)+4 );
	if( filebase == NULL ) {
		ErrorMSG( "Out of memory",-1);
		return 1;
	}
	for (i=0; i<0x56+4; i++) filebase[i]=0x00;	// header clear

	size=0x56+1;
	G1M_header( filebase, &size );	// G1M header set
	G1M_Basic_header( filebase );	// G1M Basic header set
	
	filename8ToSname( filebase, basname);

	ProgEntryN=0;						// new Main program
	ProgfileAdrs[ProgEntryN]= filebase;
	ProgfileMax[ProgEntryN]= SrcSize( filebase ) + NewMax ;
	ProgfileEdit[ProgEntryN]= 1;
	ProgNo=0;
	ExecPtr=0;
	return 0;
}

//----------------------------------------------------------------------------------------------
void ConvertToText( char *sname ){
	char *filebase;
	char fname[32],basname[16];
	int size,i,j;
	char *text;
	int textptr;
	int textsize;

	if ( LoadProgfile( sname, 0) ) return ; // error

	filebase = ProgfileAdrs[0];
	SnameTofilename8( filebase, basname);

	size=SrcSize( filebase ) *3/2 ;
	text = (char *)malloc( size*sizeof(char) +4 );
	if ( text == NULL )  { CB_ErrMsg(MemoryERR); return ; }
	memset( text, 0x00,   size );
	
	textsize=OpcodeToText( filebase+0x56, text, size );
	if ( textsize<0 ) { CB_ErrMsg(MemoryERR); goto retend; }
	
	filename8ToSname( filebase, basname);
	SetFullfilenameExt( fname, basname, "txt" );
	
	if ( ExistFile( fname ) == 0 ) { // ==0 existed 
		if ( YesNo( "Overwrite OK?" ) == 0 ) goto retend; // cancel
		Bdisp_PutDisp_DD();
	}
	storeFile( fname, (unsigned char*)text, textsize );
	FileListUpdate=0;	// 
	ErrorMSGfile( "Convert Complete!", fname);
  retend:
	free(text);	//
}
//----------------------------------------------------------------------------------------------

void SetFullfilenameBin( char *fname, char *sname ) {
	char extname[4];
	char *cptr;
	int i;
	extname[0]='\0';
	extname[1]='\0';
	extname[2]='\0';
	extname[3]='\0';
	cptr=strstr(sname,".");
	if ( cptr==NULL ) {
		extname[0]='b';
		extname[1]='i';
		extname[2]='n';
	} else {
		strncpy( extname, cptr+1, 3 );
		*cptr='\0';
	}
	if( strlen(folder) == 0 )
		sprintf( fname, "\\\\"ROOT"\\%s.%s", sname, extname );
	else
		sprintf( fname, "\\\\"ROOT"\\%s\\%s.%s", folder, sname, extname );
}

int CB_IsExist( char *SRC ) {	//	IsExist("TEST")		//  no exist: return 0     exist: return filesize
	char fname[32],sname[16];
	int c,i,matsize;
	char* FilePtr;
	int r;

	c =SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	GetLocateStr(SRC, sname,12);
	c =SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	SetFullfilenameBin( fname, sname );
	
	return GetFileSize( fname );
}

void CB_Save( char *SRC ) { //	Save "TEST",Mat A[1,3] [,Q] etc
	char fname[32],sname[16];
	int c,i,matsize;
	char* FilePtr;
	int reg,dimA,dimB;
	int check=0;

	c =SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	GetLocateStr(SRC, sname,12);
	SetFullfilenameBin( fname, sname );
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	FilePtr=MatrixPtr( reg, dimA, dimB );
	matsize=MatrixSize( reg, MatArySizeA[reg], MatArySizeB[reg] ) - MatrixSize( reg, dimA+1, dimB+1 ) + MatrixSize( reg, 1, 1 );

	c =SRC[ExecPtr];
	if ( c == ',' ) {
		c =SRC[++ExecPtr];
		if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
		ExecPtr++;
	}
	if ( ExistFile( fname ) == 0 ) { // ==0 existed 
		if ( check ) if ( YesNo( "Overwrite OK?" ) == 0 ) return  ; // cancel
		Bdisp_PutDisp_DD();
	}
	if ( storeFile( fname, (unsigned char*)FilePtr, matsize )==0 ) return ;	// 0:ok
	CB_Error(FileERR);
}


void CB_Load( char *SRC ) { //	Load ("TEST" [, Ptr])->Mat A[1,3] 
	char fname[32],sname[16];
	int c,i,matsize;
	char* FilePtr;
	int ptr=0;
	int reg,dimA,dimB;

	int handle;
	FONTCHARACTER filename[50];
	int size;

	c =SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	GetLocateStr(SRC, sname,12);
	SetFullfilenameBin( fname, sname );
	c =SRC[ExecPtr];
	if ( c == ',' ) {
		ExecPtr++;
		ptr=CB_EvalInt( SRC );
		if ( ptr < 0 ) { CB_Error(RangeERR); return; }	// Range error
	}
	c =SRC[ExecPtr];
	if ( c != ')' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	FilePtr=MatrixPtr( reg, dimA, dimB );	
	matsize=MatrixSize( reg, MatArySizeA[reg], MatArySizeB[reg] ) - MatrixSize( reg, dimA+1, dimB+1 ) + MatrixSize( reg, 1, 1 );

	CharToFont( fname, filename );
	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) {
		ErrorMSGfile( "Can't find file", fname);
		CB_Error(FileERR); return ;
	}
	
	size = Bfile_GetFileSize( handle ) -(ptr) ;

	if ( size > matsize ) size=matsize;

	Bfile_ReadFile( handle, FilePtr, size, ptr );
	Bfile_CloseFile( handle );
	if ( FilePtr == NULL ) { ErrorMSGfile( "Load Error", fname ); CB_Error(FileERR); return ; }

}

//----------------------------------------------------------------------------------------------
void CB_Local( char *SRC ) {
	int c=1,i,j,st,en;
	i=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		c=SRC[ExecPtr];
		if ( ( 'a' <= c ) && ( c <='z' ) ) {
			st=c-'a';
			c=SRC[++ExecPtr];
			if ( c == 0x7E ) {		// '~'
				c=SRC[++ExecPtr];
				if ( ( 'a' <= c ) && ( c <='z' ) ) {
					ExecPtr++;
					en=c-'a';
					if ( en<st ) { CB_Error(SyntaxERR); return; }	// Syntax error
					c=SRC[++ExecPtr];
					if ( i+st > 25 ) st=25-i;
					if ( i+en > 25 ) en=25-i;
					for ( j=st; j<=en; j++) ProgLocalVar[ProgEntryN-1][i+j] = st+j ;
				}
			} else ProgLocalVar[ProgEntryN-1][i] = st;	// local var set
		} 
		i++;
		c=SRC[ExecPtr];
		if ( c != ',' ) break; 	// 
		ExecPtr++;
		if ( i > 26 ) { CB_Error(TooMuchData); break; }	// too much error
	}
	ProgLocalN[ProgEntryN-1] = i;
}

void CB_ProgEntry( char *SRC ) { //	Prog "..." into memory
	int c=1;
	char buffer[32]="";
	char filename[32];
	char *src;
	char *StackProgSRC;
	int StackProgPtr;
	unsigned int key=0;
	int srcPrg;

//	locate( 1, 1); PrintLine(" ",21);						//
//	sprintf(buffer,"==%-8s==%08X",SRC-0x56+0x3C, SRC-0x56);
//	locate (1, 1); Print( (unsigned char*)buffer );

	if ( ErrorNo ) return ;
	ProgNo=ProgEntryN-1;
	ExecPtr=0;
	while ( c!=0 ) {
		c=SRC[ExecPtr++];
		if ( c==0x00 ) { ExecPtr--; break; }
		switch ( c ) {
			case 0x3A:	// <:>
			case 0x0D:	// <CR>
				break;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0xFFFFFFED:	// Prog "..."
				ExecPtr++;	// " skip
				GetQuotOpcode(SRC, buffer, 32);	// Prog name
//				locate( 1, 2); PrintLine(" ",21);						//
//				locate( 1, 2); Print(buffer);							//
//				locate( 1, 3); PrintLine(" ",21); GetKey(&key);			//
				srcPrg = CB_SearchProg( buffer );
				if ( srcPrg < 0 ) { 				// undefined Prog
					SetFullfilenameExt( filename, (char*)buffer, "g1m" );
					src = loadFile( filename ,EditMaxfree);
//					locate( 1, 3); Print(filename);						//
//					locate( 1, 4); PrintLine(" ",21);					//
//					sprintf(buffer,"ptr=%08X",src);						//
//					locate( 1, 4); Print(buffer); GetKey(&key);			//
					if ( src == NULL ) { CB_Error(NotfoundProgERR); return ; }  // Not found Prog
					else {
						ProgfileAdrs[ProgEntryN]= src;
						ProgfileMax[ProgEntryN]= SrcSize( src ) +EditMaxfree ;
						ProgfileEdit[ProgEntryN]= 0;
						ProgEntryN++;
						if ( ProgEntryN > ProgMax ) { CB_Error(TooManyProgERR); CB_ErrMsg(ErrorNo); return ; } // Memory error
						StackProgPtr = ExecPtr;
						CB_ProgEntry( src + 0x56  );		//
						ExecPtr = StackProgPtr ;
					}
				}
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr++] == 0xFFFFFFF1 ) CB_Local(SRC);	// Local var set
				break;
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	//
				ExecPtr++;
				break;
			default:
				break;
		}
	}

	SetSrcSize( SRC-0x56 , ExecPtr+0x56+1 );
}

