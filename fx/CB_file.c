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
#include "CB_Str.h"

#include "CB_test.h"

//-------------------------------------------------------------- source code refer to (WSC) file.c
//---------------------------------------------------------------------------------------------

#define FILENAMEMAX 13
#define FOLDERMAX 9
#define N_LINE 6

#define FOLDER_FLAG -1
#define FOLDER_SEPALATOR -2

typedef struct{
	char filename[FILENAMEMAX];
	char folder[FOLDERMAX];
	short filesize;
}Files;

#define FavoritesMAX 7

static Files *files = NULL;
static int index = 0;

static int ReadFile( char *folder );
static int IsFileNeeded( FONTCHARACTER *FileName );
static FONTCHARACTER *FilePath( char *sFolder, FONTCHARACTER *sFont );
unsigned int Explorer( int size, char *folder );
static int FileCmp( const void *p1, const void *p2 );

static int size=0;
static char folder[FILENAMEMAX] = "", tmpfolder[FILENAMEMAX] = "", name[FILENAMEMAX];
static char renamename[FILENAMEMAX] = "";
static char renamefolder[FOLDERMAX] = "";
Files Favoritesfiles[FavoritesMAX];
char	FileListUpdate=1;
int redrawsubfolder=0;
int recentsize=0;

unsigned int SelectFile (char *filename)
{
	unsigned int key;

	Bdisp_AllClr_DDVRAM();
	while( 1 ){
		if ( FileListUpdate  ) {
			MSG1("File Reading.....");
//			if ( ( UseHiddenRAM == 0 ) && ( files != NULL ) ) {
//				free( files );	// *file free
//			}
 			size = ReadFile( folder );
			qsort( files, size, sizeof(Files), FileCmp );
		}
		key = Explorer( size, folder ) ;
		if ( key == FileCMD_NEW  ) break ;	// new file
		if ( key == FileCMD_Prog ) break ;	// sdk built in file

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
		if( files[index].filesize == FOLDER_FLAG ){				//folder
			strcpy( folder, files[index].filename );
			index = 0;
			FileListUpdate = 1 ;
			redrawsubfolder= 0 ;
		} else {										//file
			strcpy( name,   files[index].filename );
			if ( strcmp( files[index].folder  ,  folder ) != 0 ) FileListUpdate=1;
			strcpy( folder, files[index].folder );
			break;
		}
	}

	if( strlen(folder) == 0 )
		sprintf( filename, "\\\\"ROOT"\\%s", name );
	else
		sprintf( filename, "\\\\"ROOT"\\%s\\%s", folder, name );
	
	return key ;
}

//void FileListfree() {
//	if ( FileListUpdate ) {
//		if ( UseHiddenRAM == 0 ) if ( files != NULL ) free( files );
//	}
//}

void Abort(){		// abort program
	unsigned int key;
	MSG2("Not enough Memory","Please Restart");
	while (1) GetKey(&key);
}

static int ReadFile( char *folder )
{
	char str[FILENAMEMAX];
	FONTCHARACTER find_path[50];
	FONTCHARACTER find_name[50];
	FILE_INFO file_info;
	int find_h;
	int size, i, r;

	size = FavoritesMAX + 1 ;
	FilePath( folder, find_path );

/*				Get File Num			*/

	r = Bfile_FindFirst (find_path, &find_h, find_name, &file_info);
	if ( r == 0 ) {
		if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) )
			size++;
		while(Bfile_FindNext(find_h, find_name, &file_info)==0){
			if( file_info.type == DT_DIRECTORY || IsFileNeeded( find_name ) )
				size++;
		}
		Bfile_FindClose(find_h);
	}
	
/*				Get Name & Size			*/
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		files = (Files *)HiddenRAM();
	} else {
		if ( recentsize < size ) {
			if ( files != NULL ) free(files);
			 files = (Files *)malloc( size*sizeof(Files) );
			if ( files == NULL ) Abort();
			recentsize = size;
		}
	}
	memset( files, 0, size*sizeof(Files) );
	
	i = FavoritesMAX ;
	r =	Bfile_FindFirst (find_path, &find_h, find_name, &file_info);
	if ( r == 0 ) {
		if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) ){
			FontToChar(find_name, str);
			strncpy( files[i].filename, str, FILENAMEMAX);
			strncpy( files[i].folder, folder, FOLDERMAX);
			files[i].filesize = (file_info.type == DT_DIRECTORY ? FOLDER_FLAG : file_info.dsize);
			++i;
		}
		while(Bfile_FindNext(find_h, find_name, &file_info)==0)
		{
			if( file_info.type == DT_DIRECTORY ||  IsFileNeeded( find_name ) ){
				FontToChar(find_name,str);
				strncpy( files[i].filename, str, FILENAMEMAX);
				strncpy( files[i].folder, folder, FOLDERMAX);
				files[i].filesize = (file_info.type == DT_DIRECTORY ? FOLDER_FLAG : file_info.dsize);
				++i;
			}
		}
		Bfile_FindClose( find_h );
	}

	return size;
}



//--------------------------------------------------------------
void DeleteFavorites( int i ) {	// i:index
	while ( i >= 0 ) {	// space adjust
		files[i].filesize = 0;
		memset( files[i].filename, 0x00, FILENAMEMAX +FOLDERMAX );
		if ( i ) {
			files[i].filesize = files[i-1].filesize ;
			strncpy( files[i].filename, files[i-1].filename, FILENAMEMAX );
			strncpy( files[i].folder,   files[i-1].folder,   FOLDERMAX );
		}
		i--;
	}
	SaveFavorites();
}

int FavoritesFunc( int index ) {
	char tmpname[FILENAMEMAX];
	int i=(index);
	if ( files[i].filesize == 0 ) {
		if ( i < FavoritesMAX ) DeleteFavorites( i ) ;
		return;
	}
	i=0;
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( files[i].filename,  files[(index)].filename )== 0 ) 
		if ( strcmp( files[i].folder,    files[(index)].folder   )== 0 ) break; // not matching
		i++;
	}
	if ( i < FavoritesMAX ) { 		//	off Favorites list
		if ( YesNo( "Favorite-Off ?" ) ) {
			DeleteFavorites( i ) ;
		}
	} else {						//	add Favorites list
		if ( files[0].filesize ) ErrorMSGstr1( "Full Favorites" );
		else
		if ( YesNo( "Favorite-ADD ?" ) ) {
			i=1;
			while ( i < FavoritesMAX ) {	// space adjust
				files[i-1].filesize = files[i].filesize ;
				strncpy( files[i-1].filename, files[i].filename, FILENAMEMAX );
				strncpy( files[i-1].folder,   files[i].folder,   FOLDERMAX );
				i++;
			}
			i=FavoritesMAX-1;
			files[i].filesize = files[(index)].filesize ;
			strncpy( files[i].filename, files[(index)].filename, FILENAMEMAX );
			strncpy( files[i].folder,   files[(index)].folder,   FOLDERMAX );
			(index)=i;
			SaveFavorites();
		}
	}
	return index;
}
void FavoritesUp( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)-1].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)-1].folder,   FOLDERMAX );
	tmp=files[(*index)-1].filesize;
	strncpy( files[(*index)-1].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)-1].folder,   files[(*index)].folder,   FOLDERMAX );
	files[(*index)-1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
	files[(*index)].filesize=tmp;
	(*index)--;
	SaveFavorites();
}
void FavoritesDown( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)+1].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)+1].folder,   FOLDERMAX );
	tmp=files[(*index)+1].filesize;
	strncpy( files[(*index)+1].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)+1].folder,   files[(*index)].folder,   FOLDERMAX );
	files[(*index)+1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
	files[(*index)].filesize=tmp;
	(*index)++;
	SaveFavorites();
}

//--------------------------------------------------------------

unsigned int Explorer( int size, char *folder )
{
	int cont=1;
	int top;
	int i,j,k,s,tmp;
	unsigned int key;
	int FavCount=0;
	int StartLine;
	int filemode=0;
	char *str;
	int nofile=0;
	int Isfolder=0;
	char buffer[32];
	
	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep

	top = index ;
	
	if ( FileListUpdate ) {
		for( i=0; i<FavoritesMAX; i++){			//	set  Favorites list
			strncpy( files[i].filename, Favoritesfiles[i].filename, FILENAMEMAX );
			strncpy( files[i].folder,   Favoritesfiles[i].folder,   FOLDERMAX );
			files[i].filesize = Favoritesfiles[i].filesize ;
		}
		files[FavoritesMAX].filesize = FOLDER_SEPALATOR;	// separator
		
		if ( renamename[0] != '\0' ) {
			if ( index > FavoritesMAX ) s=FavoritesMAX; else s=0;
			for ( k=s; k<size; k++ ) {
				if ( files[k].filesize != FOLDER_FLAG ) {
					if ( strncmp( files[k].filename,  renamename  ,  strlen(renamename  ) ) == 0 )
					if ( strncmp( files[k].folder  ,  renamefolder,  strlen(renamefolder) ) == 0 ) { index=k; break; } // rename name matching
				}
			}
		}
	}
	FileListUpdate = 0 ;	// 1:update
	renamename[0] = '\0';
	renamefolder[0] = '\0';

	while( cont )
	{
		FavCount=0;
		for( i=0; i<FavoritesMAX; i++){			//	count Favorites list
			if ( files[i].filesize ) FavCount++;
		}
		StartLine=FavoritesMAX - FavCount; 
		if ( FavCount == 0 ) StartLine++; 
		else {
			if ( redrawsubfolder==0 ) if ( strlen(folder) ) { index=FavoritesMAX +1; top=index-1; redrawsubfolder=1; }
		}

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
		locate(1, 1);Print((unsigned char*)"File List  [        ]");
		locate(13,1);Print( strlen(folder) ? (unsigned char*)folder : (unsigned char*)"/");	// root
		if( size < 1+FavoritesMAX+1 ){
			locate( 8, 4 );
			Print( (unsigned char*)"No Data" );
			nofile=1;
		}
		else{
			char buf[22],buf2[22];
			relist:
			if( index < StartLine )
				index = StartLine;
			if( top > index )
				top = index;
			if( index > top + N_LINE - 1 )
				top = index - N_LINE + 1;
			if( top < StartLine )
				top = StartLine;

			for(i = 0;i < N_LINE && i + top < size; ++i ){

				if( files[i + top].filesize == 0 ) {
					sprintf( buf, "---------------------");
				} else
				if ( files[i + top].filesize == FOLDER_SEPALATOR ) {
					sprintf( buf, "------Favorites------");
				} else
				if( files[i + top].filesize == FOLDER_FLAG ) {
					sprintf( buf, " [%s]", files[i + top].filename );
				} else {
					strncpy( buf2, files[i + top].filename, FILENAMEMAX );
					j=strlen(files[i + top].filename); if (j<4) j=4;
					k=files[i + top].filesize;
					if ( buf2[j-3]=='g' ) {
						buf2[j-4]='\0';
						k -= 0x38;	// file size adjust G1M
					}
					sprintf( buf, " %-12s :%5u ", buf2, k );

					if ( strcmp( files[i+top].folder, folder ) != 0 )
					if ( i+top < FavoritesMAX ) {
						j=strlen(files[i + top].folder) ;
						if (j>5) j=5;
						if ( j ) strncpy( buf2, files[i + top].folder, j); 
						buf2[j++]='/'; buf2[j]=0;
						strncpy( buf, files[i + top].filename, FILENAMEMAX );
						j=strlen(files[i + top].filename); if (j<4) j=4;
						if ( buf[j-3]=='g' ) buf[j-4]='\0';
						strcat(buf2,buf); buf2[14]=0;
						sprintf( buf, "%-14s:%5u ", buf2, k );
					}						
				}
				locate( 1, i + 2 ); Print( (unsigned char*)buf );
			}

			Bdisp_AreaReverseVRAM( 0, (index-top+1)*8 , 127, (index-top+2)*8-1 );
			if( top > 0 )
				PrintXY( 120, 8, (unsigned char*)"\xE6\x92", top == index );
			if( top + N_LINE < size  )
				PrintXY( 120, N_LINE*8, (unsigned char*)"\xE6\x93" , top + N_LINE - 1 == index );
		}

		Isfolder= ( files[index].filesize == FOLDER_FLAG ) ;
		
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
				if ( files[i].filesize == FOLDER_FLAG ) i++;
				else if ( files[i].filename[0]==key ) {
						index = i;
						top = index;
						break;
				} else i++; // folder skip
			}
		}
		switch ( key ) {
			case KEY_CTRL_UP:
				if ( nofile ) break;
				do {
					if( --index < StartLine  )
						index = size - 1;
				} while ( files[index].filesize == FOLDER_SEPALATOR ) ;
				break;
			case KEY_CTRL_DOWN:
				if ( nofile ) break;
				do {
					if( ++index > size - 1 )
						index = StartLine ;
				} while ( files[index].filesize == FOLDER_SEPALATOR ) ;
				break;
			case KEY_CTRL_EXE:
				if ( filemode != 0 ) break;
			case KEY_CTRL_F1:	// run
				if ( nofile ) break;
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
				if ( Isfolder ) break;
				if ( nofile ) break;
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
				if ( Isfolder ) break;
				switch ( filemode ) {
					case 0:
						key=FileCMD_NEW;
						cont =0 ;
						break;
					case 1:
						index = FavoritesFunc( index );
//						FavoritesFunc( index );
						break;
				}
				break;
			case KEY_CTRL_F4:	// Copy file
				if ( Isfolder ) break;
				if ( nofile ) break;
				switch ( filemode ) {
					case 0:
						key=FileCMD_COPY;
						cont =0 ;
						break;
					case 1:
						if ( index <= StartLine ) break;
						if ( index > FavoritesMAX-1 ) break;
						FavoritesUp( &index );
						break;
				}
				break;
			case KEY_CTRL_F5:	// delete file
				if ( Isfolder ) break;
				if ( nofile ) break;
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
				if ( nofile ) break;
				filemode = 0 ;
				index = FavoritesFunc( index );
//				FavoritesFunc( index );
				break;
			case KEY_CTRL_F6:		// ------- change function
				filemode = 1-filemode ;
				break;
			
			case KEY_CTRL_EXIT:
				if ( ( nofile ) || ( index == StartLine ) ) key=KEY_CTRL_QUIT;
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
				Fkey_dispN( 3, "Pass");
				Fkey_Clear( 4 );
//				sprintf( buffer, "[%d]", size-FavoritesMAX-1); PrintMini(14*6+3, 7*8+2, (unsigned char*)buffer , MINI_OVER);  // number of file 
//				Fkey_dispN_aA( 3, "Fv.\xE6\x92");
//				Fkey_dispN_aA( 4, "Fv.\xE6\x93");
				Fkey_dispN( 5, "Debg");
				GetKey(&key);
				switch (key) {
					case KEY_CHAR_POWROOT:
							key=FileCMD_Prog;
//					case KEY_CTRL_EXIT:
					case KEY_CTRL_QUIT:
							FileListUpdate = 1 ; // 
							cont = 0 ;
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
					case KEY_CTRL_F4:	//
							key=FileCMD_PASS;
							cont = 0 ;
							break;
//					case KEY_CTRL_F5:	//
					case KEY_CTRL_F6:
							if ( Isfolder ) break;
							if ( nofile ) break;
							key=FileCMD_DebugRUN;
							cont =0 ;
							break;
					case KEY_CTRL_PAGEUP:	// up
							if ( nofile ) break;
							if ( index <= StartLine ) break;
							if ( index > FavoritesMAX-1 ) break;
							FavoritesUp( &index );
							break;
					case KEY_CTRL_PAGEDOWN:	// down
							if ( nofile ) break;
							if ( FavCount < 1 ) break;
							if ( index >= FavoritesMAX-1 ) break;
							FavoritesDown( &index );
							break;
					case KEY_CHAR_ASIN:
							if ( CB_INTDefault ) CBint_test(); else CB_test();
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
	return ( (strcmp(str + strlen(str) - 4, ".g1m") == 0) || (strcmp(str + strlen(str) - 4, ".txt") == 0) );
}

static int FileCmp( const void *p1, const void *p2 )
{
	Files *f1 = (Files *)p1;
	Files *f2 = (Files *)p2;

	if( f1->filesize == FOLDER_FLAG && f2->filesize == FOLDER_FLAG )
		return strcmp( f1->filename + 1, f2->filename + 1);
	else if( f1->filesize == FOLDER_FLAG )
		return 1;
	else if( f2->filesize == FOLDER_FLAG )
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

void SetShortName( char *sname, char *name) {	// fullpath name -> short name
	int c,i;
	int ptr=0;
	c=name[ptr++];
	while( c=='\\' ) c=name[ptr++];	//
	while( c!='\\' ) c=name[ptr++];	// ROOT skip
	if ( strchr(name+ptr,'\\') != NULL ) { name=strchr(name+ptr,'\\')+1; ptr=0;}
	c=name[ptr];
	i=0;
	while ( c ) {
		c=name[ptr++];
		sname[i++]=c;
	}
	sname[i]='\0';
}

void ErrorMSGfile( char *buffer, char *name, int err){
	char sname[32];
	char buf[32];
	SetShortName( sname, name);
	if ( err ) {
		sprintf(buf, "%s (%d)", sname, err);
		ErrorMSGstr( buffer, buf);
	} else {
		ErrorMSGstr( buffer, sname);
	}
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
		ErrorMSGfile( "Can't find file", name, handle);
		CB_Error(FileERR); 
		return NULL;
	}

	size = Bfile_GetFileSize( handle );

	buffer = ( char *)malloc( size*sizeof(char)+editMax+4 );
	if( buffer == NULL ) Abort();
	memset( buffer, 0x00,     size*sizeof(char)+editMax+4 );

	Bfile_ReadFile( handle, buffer, size, 0 );
	buffer[size] = '\0';
//	strcat( buffer, " " ); // for some reasons about the preprocessor...

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
	if( handle < 0 ) { ErrorMSGfile( "Can't create file", name, handle ); return 1 ; }
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
int YesNoOverwrite(){
	return ( YesNo( "Overwrite OK?" ) ==0 ) ;
}

unsigned int InputStrFilename(int x, int y, int width, int maxLen, char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;

	buffer[width]='\0';
	csrX=strlen((char*)buffer);
	key=InputStrSub( x, y, width, csrX, buffer, maxLen, SPC, rev_mode, FLOAT_OFF, EXP_OFF, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	return ( key );
}

void Getfolder( char *sname ) ;

int InputFilename( char * buffer, char* msg) {		// 
	unsigned int key;
//	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(3);
	locate(3,3); Print((unsigned char *)msg);
	locate(3,4); Print((unsigned char *)" [             ]");
	Fkey_Clear( 0 );
	Fkey_Clear( 1 );
	Fkey_Clear( 2 );
	Fkey_DISPN( 5, " / ");
	key=InputStrFilename( 5, 4, 12, 18, buffer, ' ', REV_OFF ) ;
//	RestoreDisp(SAVEDISP_PAGE1);
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) return 1;
	Getfolder( buffer );
	return 0; // ok
}


void basname8ToG1MHeader( char *filebase, char *basname) {	// abcd -> header
	char *nameptr;
	int c;
	int i;
	nameptr=filebase+0x3C;
	for (i=0;i<8;i++) nameptr[i]='\0';
	i=0;
	c=basname[i];
	do {
		if (i<8) nameptr[i]=c;
		i++;
		c=basname[i];
	} while ( ( c!='\0' ) ) ;
//	} while ( ( c!='\0' ) ||  ( c!='.' ) ) ;
}

void G1MHeaderTobasname8( char *filebase, char *basname) {	// header -> abcd
	char *nameptr;
	int c;
	int i;
	nameptr=filebase+0x3C;
	i=0;
	while (i<8) {
		c = nameptr[i];
		if ((c==' ')) c='~';
		basname[i]=c;
		if ( c=='\0' ) break;
		i++;
	}
	basname[i]='\0';
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
	filebase[0x3A]=0x00;
	filebase[0x3B]=0x00;
	filebase[0x44]=0x01;	// basic
	filebase[0x55]=0x00;	// pass flag clear(C.basic)
}


//----------------------------------------------------------------------------------------------

void InputPassPrintbasnamefile( char *basname, char *msg) {		// print basname
	char buffer[16];
	Bdisp_AllClr_VRAM();
	locate(1,1); Print((unsigned char *)msg);
	sprintf(buffer,"[%-8s]",basname);
	locate(1,2); Print((unsigned char *)buffer);
}

int InputPassname( int y, char* inputpassname, char *msg) {		// password input
	unsigned int key;
	locate(1,y  ); Print((unsigned char *)msg);
	locate(1,y+1); Print((unsigned char *)"[        ]");
	Fkey_Clear( 0 );
	Fkey_Clear( 1 );
	Fkey_Clear( 2 );
	key=InputStrFilename( 2, y+1, 8, 8, inputpassname, ' ', REV_OFF ) ;
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) return 1;
	return 0; // ok
}

int CheckPassWordmsg( char *filebase, char *msg ){	// 1:cancel  0:Ok  -1:no pass
	char passname[9];
	char inputpassname[9];
	char basname[9];
	
	memset( passname, 0, 9);
	strncpy( passname, filebase+0x4C, 8);	// Get password
	G1MHeaderTobasname8( filebase, basname);
	
	if ( strlen(passname) ) {
		memset( inputpassname, 0, 9);
		mismatchloop:
		InputPassPrintbasnamefile( basname, "Program Name");
		if ( InputPassname( 3, inputpassname, msg ) ) return 1; //cancel
		if ( strncmp(passname, inputpassname, 8 )==0 ) goto passOk;	// ok
		else {	// mismatch
			ErrorMSGstr1(" Mismatch");
			goto mismatchloop;
		}
	} 
	passOk:
	filebase[0x55]=0x01;	// pass ok (C.basic)
	return 0;	// ok
}
int CheckPassWord( char *filebase ){	// 1:cancel  0:Ok  -1:no pass
	CheckPassWordmsg( filebase, "Password?" );
}

int SetPassWord( int y, char *filebase, char *basname ,char* msg){	// 1:no password   0:Ok
	char inputpassname[9];
	memset( inputpassname, 0, 9);
	if ( InputPassname( y, inputpassname, msg ) ) return 1; //cancel
	strncpy( filebase+0x4C, inputpassname, 8);	// Set password
	return 0; // ok
}

int InputFilenamePassname( char *filebase, char *basname, char* msg) {		// 
	unsigned int key;
	char buffer[16];
	Bdisp_AllClr_VRAM();
	locate(1,1); Print((unsigned char *)msg);
	locate(1,2); Print((unsigned char *)"[        ]");
	Fkey_dispN( 0, "Pass");
	Fkey_Clear( 1 );
	Fkey_Clear( 2 );
	key=InputStrFilename( 2, 2, 8, 8, basname, ' ', REV_OFF ) ;
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) return 1;
	if (key==KEY_CTRL_F1) {	// password input
		if ( SetPassWord( 3, filebase, basname, "Password?" ) ) return 1 ; // cancel
	}
	filebase[0x55]=0x01;	// pass ok (C.basic)
	return 0; // ok
}
/*
void NewPassWord( char *fname ){	// New Password command
	char *filebase;
	int fsize,size;
	char sname[16],basname[16];

	SetShortName( sname, fname);
	if ( strcmp( sname + strlen(sname) - 4, ".g1m") != 0 ) return ;	// not g1mfile
	filebase = loadFile( fname , 0 );
//	if ( filebase == NULL ) return ;
	if ( CheckG1M( filebase ) ) goto exit; // not support g1m
	G1MHeaderTobasname8( filebase, basname);
	InputPassPrintbasnamefile( basname, "Program Name");
	if ( filebase[0x4C] == 0 ) { // new password
		if ( SetPassWord( 3, filebase, basname, "Set Password" ) ) goto exit; // cancel
	} else { // unlock password
		if ( CheckPassWordmsg( filebase , "Unlock password" ) ) goto exit; // cancel
		memset( filebase+0x4C, 0, 8);	// clear password
	}
	if ( SaveBasG1M( filebase ) ==0 ) ErrorMSGstr1("    Complete!");
	exit:
	free( filebase );
}
*/
void NewPassWord( char *fname ){	// New Password command
	char *filebase;
	int fsize,size;
	char sname[16],basname[16];

	SetShortName( sname, fname);
	if ( strcmp( sname + strlen(sname) - 4, ".g1m") != 0 ) return ;	// not g1mfile
	if ( LoadProgfile( fname, 0 ) ) return ; // error
	filebase = ProgfileAdrs[0];
	if ( CheckG1M( filebase ) ) return ; // not support g1m
	G1MHeaderTobasname8( filebase, basname);
	InputPassPrintbasnamefile( basname, "Program Name");
	if ( filebase[0x4C] == 0 ) { // new password
		if ( SetPassWord( 3, filebase, basname, "Set Password" ) ) return ; // cancel
	} else { // unlock password
		if ( CheckPassWordmsg( filebase , "Unlock password" ) ) return ; // cancel
		memset( filebase+0x4C, 0, 8);	// clear password
	}
	if ( SaveBasG1M( filebase ) ==0 ) ErrorMSGstr1("    Complete!");
}

//----------------------------------------------------------------------------------------------
void ConvertToOpcode( char *filebase, char *sname, int editsize){
	int size,i,j;
	char *text;
	int textptr;
	int codesize;
	int textsize;

	MSG1("Text Converting..");
	textsize=strlen(filebase);
	memcpy( filebase+editsize, filebase, textsize);
	memset( filebase, 0, editsize );
	codesize=TextToOpcode( filebase, filebase+editsize, textsize+editsize );
	size=codesize+0x56+1;
	G1M_header( filebase, &size );	// G1M header set
	G1M_Basic_header( filebase );	// G1M Basic header set
	basname8ToG1MHeader( filebase, sname);
}

int CheckG1M( char *filebase ){	// Check G1M Basic file
	int	fsize = 0xFFFF-((filebase[0x12]&0xFF)*256+(filebase[0x13]&0xFF));
	int size  = SrcSize( filebase ) ;
	if ( ( fsize != size ) || ( filebase[0x20] != 'P' )|| ( filebase[0x21] != 'R' ) ){ ErrorMSG( "Not support file", fsize );
		 return 1; // error
	} 
	return 0; // ok
}

int LoadProgfile( char *fname, int editsize ) {
	char *filebase;
	int fsize,size;
	char sname[16],basname[16];
	int textsize;
	int progsize;

	SetShortName( sname, fname);
	if ( strcmp( sname + strlen(sname) - 4, ".txt") == 0 ) {	// text file
			filebase = loadFile( fname , editsize + EditMaxfree);
//			if ( filebase == NULL ) return 1;
			textsize=strlen(filebase);
			if ( editsize ) ConvertToOpcode( filebase, sname, editsize + EditMaxfree);		// text file -> G1M file
			progsize = textsize +  editsize ;
	} else {	// G1M file
			filebase = loadFile( fname , editsize );
//			if ( filebase == NULL ) return 1;
			if ( CheckG1M( filebase ) ) { free( filebase ); return 1; } // not support g1m
			progsize = SrcSize( filebase ) + editsize ;
	}

	ProgEntryN=0;						// Main program
	ProgfileAdrs[0]= filebase;
	ProgfileMax[0]= progsize;
	ProgfileEdit[0]= 0;
	ProgNo=0;
	ExecPtr=0;
	strncpy( filebase+0x3C-8, folder, 8);		// set folder to header
	
	return 0; // ok
}

int SaveG1M( char *filebase ){
	char fname[32],basname[16];
	int size,i;

	G1MHeaderTobasname8( filebase, basname);

	SetFullfilenameExt( fname, basname, "g1m" );

	size = SrcSize(filebase);
	filebase[size+1]=0x00;
	filebase[size+2]=0x00;
	filebase[size+3]=0x00;
	G1M_header( filebase, &size );		// G1M header set
	
	return storeFile( fname, (unsigned char*)filebase, size );	// 0:ok
}

int SaveBasG1M( char *filebase ){
	G1M_Basic_header( filebase );	// G1M Basic header set
	return SaveG1M( filebase );
}	

int SaveProgfile( int progNo ){
	char *filebase;
	char fname[32],sname[16],basname[32];
	int size,i,r;
	
	filebase=ProgfileAdrs[progNo];
	G1MHeaderTobasname8( filebase, basname);

  loop:
	if ( InputFilename( basname, "Save File Name?" ) ) return 1 ;
//	if ( InputFilenamePass( filebase, basname, "Save File name?") ) return 1 ; // cancel
	if ( ExistG1M( basname ) ==0 ) if ( YesNoOverwrite() ) goto loop;
	basname8ToG1MHeader( filebase, basname);

	sprintf(sname, "%s.g1m", basname );
	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);
	
	strncpy( tmpfolder, folder, FOLDERMAX );
	strncpy( folder, filebase+0x3C-8, 8 );
	r=SaveBasG1M( filebase );
	
	i=0;
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( Favoritesfiles[i].filename,  sname )== 0 ) 
		if ( strcmp( Favoritesfiles[i].folder  ,  folder)== 0 ) break; // already favorite exist
		i++;
	}
	if ( i < FavoritesMAX ) { 		//	exist favorites list
		Favoritesfiles[i].filesize = SrcSize(filebase);
	}

	strncpy( folder, tmpfolder, FOLDERMAX );
	return r;
}

int NewProg(){
	char *filebase;
	char fname[32],basname[32];
	int size,i;

	size=NewMax;
	filebase = (char *)malloc( size*sizeof(char)+4 );
	if( filebase == NULL ) {
		CB_ErrMsg(NotEnoughMemoryERR);
		return 1;
	}
	memset( filebase, 0x77,             size*sizeof(char)+4 );
	for (i=0; i<0x56+4; i++) filebase[i]=0x00;	// header clear

	size=0x56+1;
	G1M_header( filebase, &size );	// G1M header set
	G1M_Basic_header( filebase );	// G1M Basic header set
	
	basname[0]='\0';
//	if ( InputFilename( basname, "New File Name?" ) ) return 1 ;
	if ( InputFilenamePassname( filebase, basname, "New Program Name?") ) return 1 ; // cancel
	if ( ExistG1M( basname ) == 0 ) { // existed file
		SetFullfilenameExt( fname, basname, "g1m" );
		LoadProgfile( fname, EditMaxfree );
		return 0;
	}
	basname8ToG1MHeader( filebase, basname);

	ProgEntryN=0;						// new Main program
	ProgfileAdrs[0]= filebase;
	ProgfileMax[0]= SrcSize( filebase ) + NewMax ;
	ProgfileEdit[0]= 1;
	ProgNo=0;
	ExecPtr=0;
	strncpy( filebase+0x3C-8, folder, 8);		// set folder to header
	return 0;	// ok
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

void DeleteFile(char *fname) {
	FONTCHARACTER filename[50];
	int r;

	CharToFont( fname, filename );

	r = Bfile_DeleteFile( filename );
	if( r < 0 ) { ErrorMSGfile( "Can't delete file", fname, r ); return ; }
	FileListUpdate=1;
}

void DeleteFileFav(char *fname ) {
	char sname[16];
	int i;
	
	if ( YesNo( "Delete file?" ) == 0 ) return ;
	DeleteFile( fname );
	
	SetShortName( sname, fname);		//
	i=0;
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( Favoritesfiles[i].filename,  sname )== 0 ) 
		if ( strcmp( Favoritesfiles[i].folder  ,  folder)== 0 ) break; // already favorite exist
		i++;
	}
	if ( i < FavoritesMAX ) { 		//	exist favorites list
		DeleteFavorites( i ) ;
	}

}

//----------------------------------------------------------------------------------------------
int RenameCopyFile( char *fname ,int select ) {	// select:0 rename  select:1 copy
	char *filebase;
	char name[32],sname[16],sname2[16],basname[32];
	int size,i,j;
	char msg[2][18]={"Rename File Name?" ,"Copy File Name?" };

	if ( LoadProgfile( fname, 0 ) ) return 1 ; // error
	filebase = ProgfileAdrs[0];
	SetShortName( sname, fname);
	if ( strcmp( sname + strlen(sname) - 4, ".txt") == 0 ) {	// text file
		strncpy( basname, sname, strlen(sname)-4);
		basname[strlen(sname)-4]='\0';
		if ( InputFilename( basname, msg[select] ) ) return 1 ; // cancel
		SetFullfilenameExt( name, basname, "txt" );
		if ( strcmp(name,fname)==0 ) return 0; // no rename
		if ( ExistFile( name ) == 0 ) if ( YesNoOverwrite() ) return 1 ; // cancel
		if ( storeFile( name, (unsigned char*)filebase, strlen(filebase) )==0 ) {
			if ( select == 0 ) DeleteFile( fname ) ;	// (rename) delete original file
		} else return 1;
		
	} else {	// G1M file
		G1MHeaderTobasname8( filebase, basname);
		if ( CheckG1M( filebase ) ) return 1; // not support g1m
		if ( InputFilename( basname, msg[select] ) ) return 1 ; // cancel
		SetFullfilenameExt( name, basname, "g1m" );
		if ( strcmp(name,fname)==0 ) return 0; // no rename
		basname8ToG1MHeader( filebase, basname);
		if ( ExistG1M( basname ) ==0 ) if ( YesNoOverwrite() ) return 1 ; // cancel
		if ( SaveBasG1M( filebase ) == 0 ) { 
			if ( select == 0 ) DeleteFile( fname ) ;	// (rename) delete original file
		} else return 1;
	}

//	for (j=0;j<FILENAMEMAX;j++) sname[j]='\0';
	SetShortName( sname, name);		// rename name
	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);
	
	if ( select ) return 0;	// (copy)

	i=0;
	SetShortName( sname2, fname);	// orignal name
	while ( i < FavoritesMAX ) {	// file matching search
		if ( strcmp( folder,  Favoritesfiles[i].folder )== 0 ) 
		if ( strcmp( sname2,  Favoritesfiles[i].filename )== 0 ) break; // not matching
		i++;
	}
	if ( i < FavoritesMAX ) { 		//	rename Favorites 
		strncpy( Favoritesfiles[i].filename, sname,  FILENAMEMAX );	// rename name
		strncpy( Favoritesfiles[i].folder,   folder,   FOLDERMAX );
	}
	
	return 0;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
#define ConfigMAX 1080
//--------------------------------------------------------------

void SaveFavorites(){
	int i;
	for( i=0; i<FavoritesMAX; i++){			//	backup Favorites list
		if ( files[i].filesize == 0 ) {
			memset(  Favoritesfiles[i].filename, 0x00, FILENAMEMAX +FOLDERMAX );
			Favoritesfiles[i].filesize = 0;
		} else {
			strncpy( Favoritesfiles[i].filename, files[i].filename, FILENAMEMAX );
			strncpy( Favoritesfiles[i].folder,   files[i].folder,   FOLDERMAX );
			Favoritesfiles[i].filesize = files[i].filesize;
		}
	}
	SaveConfig();
}
//------------------------------------------------------------------------ main memory version
void SaveConfig(){
	const unsigned char fname[]="CBasic1";
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
	buffer[ 9]='9';
	buffer[10]='9';
	buffer[11]='n';

	bufshort[ 7]=CB_INTDefault;		bufshort[ 6]=UseHiddenRAM;
	bufshort[ 9]=DrawType;			bufshort[ 8]=RefreshCtrl;
	bufshort[11]=Coord;				bufshort[10]=Refreshtime;
	bufshort[13]=Grid;				bufshort[12]=ENG;
	bufshort[15]=Axes;				bufshort[14]=CB_Round.MODE;
	bufshort[17]=Label;				bufshort[16]=CB_Round.DIGIT-1;
	bufshort[19]=Derivative;		bufshort[18]=0;
	bufshort[21]=S_L_Style;			bufshort[20]=0;
	bufshort[23]=Angle;				bufshort[22]=0;
	bufshort[25]=BreakCheck;		bufshort[24]=0;
	bufshort[27]=TimeDsp;			bufshort[26]=PageUpDownNum;
	bufshort[29]=MatXYmode;			bufshort[28]=1-MatBaseDefault;
	bufshort[31]=PictMode;			bufshort[30]=CheckIfEnd;

	bufdbl[ 8]=Xfct;
	bufdbl[ 9]=Yfct;
	for ( i= 10; i<  10+26 ; i++ ) bufdbl[i]=REG[i-10];
	for ( i= 42; i<  42+26 ; i++ ) bufdbl[i]=REGsmall[i-42];
	for ( i= 68; i<  68+11 ; i++ ) bufdbl[i]=REGv[i-68];
	for ( i=160; i< 160+26 ; i++ ) bufint[i]=REGINT[i-160];
	for ( i=192; i< 192+26 ; i++ ) bufint[i]=REGINTsmall[i-192];

	bufshort[218*2]=(short)KeyRepeatFirstCount;
	bufshort[219*2]=(short)KeyRepeatNextCount;
	
	for ( i=0; i<FavoritesMAX; i++ ) {
		bufint[i+220]=Favoritesfiles[i].filesize;
	}
	sbuf=buffer+(220+FavoritesMAX)*4;

	strncpy( (char*)sbuf, folder, FILENAMEMAX);
	sbuf+=FILENAMEMAX;
	for ( i=0; i<FavoritesMAX; i++ ) {
		strncpy( (char*)sbuf, Favoritesfiles[i].filename, FILENAMEMAX );
		sbuf+=FILENAMEMAX;
		strncpy( (char*)sbuf, Favoritesfiles[i].folder,   FOLDERMAX );
		sbuf+=FOLDERMAX;
	}

	handle=Bfile_OpenMainMemory(fname);
	if (handle<0) {ErrorMSG("Open Error",handle); return;}
	state=Bfile_WriteFile(handle,buffer,ConfigMAX);
	if (state<0)  {ErrorMSG("Write Error",state); return;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return;}
}


void LoadConfig(){
	const unsigned char fname[]="CBasic1";
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
		 ( buffer[ 9]=='9' ) &&
		 ( buffer[10]=='9' ) &&
		 ( buffer[11]=='n' ) ) {
									// load config & memory
		CB_INTDefault =bufshort[ 7];		UseHiddenRAM  =bufshort[6];
		DrawType      =bufshort[ 9];        RefreshCtrl   =bufshort[8];
		Coord         =bufshort[11];        Refreshtime   =bufshort[10];
		Grid          =bufshort[13];        ENG           =bufshort[12];
		Axes          =bufshort[15];        CB_Round.MODE =bufshort[14];
		Label         =bufshort[17];        CB_Round.DIGIT=bufshort[16]+1;
		Derivative    =bufshort[19];        
		S_L_Style     =bufshort[21];        
		Angle         =bufshort[23];        
		BreakCheck    =bufshort[25];        
		TimeDsp       =bufshort[27];        PageUpDownNum =bufshort[26]; if ( PageUpDownNum < 1 ) PageUpDownNum = PageUpDownNumDefault;
		MatXYmode     =bufshort[29];        MatBaseDefault=1-bufshort[28];
		PictMode      =bufshort[31];        CheckIfEnd    =bufshort[30];

		Xfct=bufdbl[ 8];
		Yfct=bufdbl[ 9];
		for ( i= 10; i<  10+26 ; i++ ) REG[i-10]   =bufdbl[i];
		for ( i= 42; i<  42+26 ; i++ ) REGsmall[i-42] =bufdbl[i];
		for ( i= 68; i<  68+11 ; i++ ) REGv[i-68]=bufdbl[i];
		for ( i=160; i< 160+26 ; i++ ) REGINT[i-160]=bufint[i];
		for ( i=192; i< 192+26 ; i++ ) REGINTsmall[i-192]=bufint[i];

		KeyRepeatFirstCount=bufshort[218*2]; if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount = 20;
		KeyRepeatNextCount =bufshort[219*2]; if ( KeyRepeatNextCount  < 1 ) KeyRepeatNextCount  =  5;

		for ( i=0; i<FavoritesMAX; i++ ) {
			Favoritesfiles[i].filesize=bufint[i+220];
		}
		sbuf=buffer+(220+FavoritesMAX)*4;

		strncpy( folder, (char*)sbuf, FILENAMEMAX);
		sbuf+=FILENAMEMAX;
		for ( i=0; i<FavoritesMAX; i++ ) {
			strncpy( Favoritesfiles[i].filename, (char*)sbuf, FILENAMEMAX );
			sbuf+=FILENAMEMAX;
			strncpy( Favoritesfiles[i].folder,   (char*)sbuf, FOLDERMAX );
			sbuf+=FOLDERMAX;
		}
	} else {
		Bfile_DeleteMainMemory(fname);
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void Setfoldername16( char *folder16, char *sname ) {
	char *cptr;
	int i=0,j,def=1;
	if ( ( sname[i]== 0x5C ) || ( sname[i]== '/' ) ) { do sname[i]=sname[++i]; while ( sname[i] ); def=0; }
	for (i=0;i<17+4;i++) folder16[i]=0;
	cptr=strstr(sname,"\\");
	if ( cptr==NULL ) { i=0; }	// current folder 
	else { def=0;		// sub folder
		i=cptr-sname; if ( i>8 ) i=8;
		strncpy( folder16, sname, i );
		i++;
		goto exit;
	}
	cptr=strstr(sname,"/");
	if ( cptr==NULL ) { i=0; }	// current folder 
	else { def=0; 		// sub folder
		i=cptr-sname; if ( i>8 ) i=8;
		strncpy( folder16, sname, i );
		i++;
	}
	exit:
	if ( def ) strncpy( folder16, folder, FOLDERMAX );
	if ( cptr != NULL ) { i=cptr-sname+1; }
	for (j=8;j<16+4;j++) {
		folder16[j]=sname[i]; if ( sname[i]==0 ) break;
		i++;
	}
}
void Getfolder( char *sname ) {
	char folder16[21];
	char *cptr;
	int i=0,j;
	strncpy( tmpfolder, folder, FOLDERMAX );
	Setfoldername16( folder16, sname ) ;
	strncpy( folder, folder16, FOLDERMAX-1 );
	strncpy( sname, folder16+8, 9+4 );
}

void Restorefolder() {
	strncpy( folder, tmpfolder, FOLDERMAX );
}


void SetFullfilenameBin( char *fname, char *sname ) {
	char extname[4]={0,0,0,0};
	char *cptr;
	int i;
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
	CB_GetLocateStr(SRC, sname,22);
//	c =SRC[ExecPtr];
//	if ( c == ')' ) ExecPtr++;
	Getfolder( sname );
	SetFullfilenameBin( fname, sname );
	r=GetFileSize( fname );
	Restorefolder();
	return r;
}

char * CB_SaveLoadOprand( char *SRC , int *matsize ) {
	int reg,dimA,dimB,base;
	char *ptr;
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	ptr = (char *)MatrixPtr( reg, dimA, dimB );
	base = 1-MatAry[reg].Base;
	*matsize = MatrixSize( reg, MatAry[reg].SizeA, MatAry[reg].SizeB ) - MatrixSize( reg, dimA+base, dimB+base ) + MatrixSize( reg, 1, 1 );
	return ptr;
}

void CB_Save( char *SRC ) { //	Save "TEST",Mat A[1,3] [,Q] etc
	char fname[32],sname[16];
	int c,i,matsize;
	char* FilePtr;
	int check=0;
	
	c =SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	CB_GetLocateStr(SRC, sname,22);
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	FilePtr = CB_SaveLoadOprand( SRC, &matsize);
	if ( ErrorNo ) return; // error

	c =SRC[ExecPtr];
	if ( c == ',' ) {
		c =SRC[++ExecPtr];
		if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
		ExecPtr++;
	}
	Getfolder( sname );
	SetFullfilenameBin( fname, sname );
	if ( ExistFile( fname ) == 0 ) { // ==0 existed 
		if ( check ) if ( YesNoOverwrite() ) goto exit; // cancel
		Bdisp_PutDisp_DD();
	}
	if ( storeFile( fname, (unsigned char*)FilePtr, matsize )!=0 ) CB_Error(FileERR);
	exit:
	Restorefolder();
}

void CB_Load( char *SRC ) { //	Load ("TEST" [, Ptr])->Mat A[1,3] 
	char fname[32],sname[16];
	int c,i,matsize;
	char* FilePtr;
	int ptr=0;

	int handle;
	FONTCHARACTER filename[50];
	int size;

	c =SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	CB_GetLocateStr(SRC, sname,22);
	c =SRC[ExecPtr];
	if ( c == ',' ) {
		ExecPtr++;
		ptr=CB_EvalInt( SRC );
		if ( ptr < 0 ) { CB_Error(RangeERR); return; }	// Range error
	}
	c =SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	c =SRC[ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	FilePtr = CB_SaveLoadOprand( SRC, &matsize);
	if ( ErrorNo ) return ; // error

	Getfolder( sname );
	SetFullfilenameBin( fname, sname );
	CharToFont( fname, filename );
	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) {
		ErrorMSGfile( "Can't Load file", fname, handle);
		CB_Error(FileERR); goto exit; ;
	}
	
	size = Bfile_GetFileSize( handle ) -(ptr) ;

	if ( size > matsize ) size=matsize;

	Bfile_ReadFile( handle, FilePtr, size, ptr );
	Bfile_CloseFile( handle );
	if ( FilePtr == NULL ) { ErrorMSGfile( "Load Error", fname, 0 ); CB_Error(FileERR); }
	exit:
	Restorefolder();
}
//----------------------------------------------------------------------------------------------
void ConvertToText( char *fname ){
	char *filebase;
	char sname[16],basname[16];
	int size,i,j;
	char *text;
	int textptr;
	int textsize;
	int buffersize;

	MSG1("Wait a moment...");
//	Bdisp_PutDisp_DD_DrawBusy();
	SetShortName( sname, fname);
	if ( strcmp( sname + strlen(sname) - 4, ".txt") == 0 ) {	// text file -> G1M
		if ( LoadProgfile( fname, EditMaxfree ) ) return ; // error
		filebase = ProgfileAdrs[0];
		G1MHeaderTobasname8( filebase, basname);
		SetFullfilenameExt( fname, basname, "g1m" );
		basname8ToG1MHeader( filebase, basname);
		if ( ExistG1M( basname ) ==0 ) if ( YesNoOverwrite() ) return  ; // cancel	
		if ( SaveBasG1M( filebase ) ) return ;
		
	} else {	// G1M file -> Text
		buffersize=files[index].filesize /2;	// buffersize 50% up
		if ( buffersize < EditMaxfree ) buffersize=EditMaxfree;
		if ( LoadProgfile( fname, buffersize ) ) return ; // error
		filebase = ProgfileAdrs[0];
		G1MHeaderTobasname8( filebase, basname);
		if ( CheckPassWord( filebase ) ) return ;	// password error
		
		size = SrcSize( filebase ) + buffersize ;
		memcpy( filebase+buffersize, filebase, SrcSize( filebase ) );
		text = filebase;
		textsize=OpcodeToText( filebase+buffersize+0x56, text, size );
		if ( textsize<0 ) { CB_ErrMsg(MemoryERR); return ; } // error
		
		SetFullfilenameExt( fname, basname, "txt" );
		if ( ExistFile( fname ) == 0 ) { // ==0 existed 
			if ( YesNoOverwrite() ) { return ; } // cancel
			Bdisp_PutDisp_DD();
		}
		storeFile( fname, (unsigned char*)text, textsize );
	}

	ErrorMSGfile( "Convert Complete!", fname, 0);
	SetShortName( sname, fname);
	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// C.Basic Pre process
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_Local( char *SRC ) {
	int c=1,i;
	i=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		c=SRC[ExecPtr];
		if ( ( 'a' <= c ) && ( c <='z' ) ) {
			ExecPtr++;
			ProgLocalVar[ProgEntryN-1][i] = c-'a';	// local var set
		} 
		i++;
		c=SRC[ExecPtr];
		if ( c != ',' ) break; 	// 
		ExecPtr++;
		if ( i > ArgcMAX ) { CB_Error(TooMuchData); break; }	// too much error
	}
	ProgLocalN[ProgEntryN-1] = i;
}

//----------------------------------------------------------------------------------------------
int PP_Search_IfEnd( char *SRC ){
	int c,i;
	int PP_ptr;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) { 			// If
					PP_ptr=ExecPtr-2;
					i=PP_Search_IfEnd(SRC) ;
					if ( ErrorNo ) return;					
					if ( i != 1  ) { ExecPtr=PP_ptr; CB_Error(IfWithoutIfEndERR); CB_ErrMsg(ErrorNo); return 0; } // not IfEnd error 
					break;
				} else
				if ( c == 0x03 ) return 1 ;	// IfEnd
				break ;
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

void CB_PreProcess( char *SRC ) { //	If..IfEnd Check
	int c=1,i;
	int execptr=ExecPtr;

	ErrorNo=0;
	ExecPtr=0;
	if ( CheckIfEnd ) PP_Search_IfEnd(SRC);
	if ( ErrorNo ) return;
	ExecPtr=execptr;
}
//----------------------------------------------------------------------------------------------
void CB_ProgEntry( char *SRC ) { //	Prog "..." into memory
	int c=1;
	char buffer[32]="",folder16[21],bufferb[32];
	char filename[32];
	char *filebase;
	char *StackProgSRC;
	int StackProgPtr;
	unsigned int key=0;
	int srcPrg;
	int progno=ProgNo;

//	locate( 1, 1); PrintLine(" ",21);						//
//	sprintf(buffer,"==%-8s==%08X",SRC-0x56+0x3C, SRC-0x56);
//	locate (1, 1); Print( (unsigned char*)buffer );

	if ( ErrorNo ) { return ; }
	ProgNo=ProgEntryN-1;
	CB_PreProcess( SRC ) ;
	if ( ErrorNo ) { return ; }
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
				CB_GetQuotOpcode(SRC, buffer, 32);	// Prog name
//				locate( 1, 2); PrintLine(" ",21);						//
//				locate( 1, 2); Print(buffer);							//
//				locate( 1, 3); PrintLine(" ",21); GetKey(&key);			//
				strcpy( bufferb, buffer);
				Setfoldername16( folder16, bufferb );
				srcPrg = CB_SearchProg( folder16 );
				if ( srcPrg < 0 ) { 				// undefined Prog
					Getfolder( buffer );
					SetFullfilenameExt( filename, (char*)buffer, "g1m" );
					filebase = loadFile( filename ,EditMaxProg );
					strncpy( filebase+0x3C-8, folder16, 8);		// set folder to header
					Restorefolder();
//					locate( 1, 3); Print(filename);						//
//					locate( 1, 4); PrintLine(" ",21);					//
//					sprintf(buffer,"ptr=%08X",filebase);						//
//					locate( 1, 4); Print(buffer); GetKey(&key);			//
					if ( ErrorNo ) {
						ErrorPtr=ExecPtr;
						ProgNo=progno;
						ErrorProg=ProgNo;
						if ( filebase != NULL ) free( filebase );
						return;
					}
					if ( filebase != NULL ) {
						ProgfileAdrs[ProgEntryN]= filebase;
						ProgfileMax[ProgEntryN]= SrcSize( filebase ) +EditMaxProg ;
						ProgfileEdit[ProgEntryN]= 0;
						ProgEntryN++;
						if ( ProgEntryN > ProgMax ) { CB_Error(TooManyProgERR); CB_ErrMsg(ErrorNo); return ; } // Too Many Prog error
						StackProgPtr = ExecPtr;
						CB_ProgEntry( filebase + 0x56  );		//
						if ( ErrorNo == IfWithoutIfEndERR ) return ;
						ExecPtr = StackProgPtr ;
//						if ( ErrorNo ) { 
//							ErrorPtr=StackProgPtr;
//							ProgNo=progno;
//							ErrorProg=ProgNo;
//							return ;
//						}
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

//---------------------------------------------------------------------------------------------- align dummy
int fileObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
/*
void FavoritesDowndummy( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)+1].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)+1].folder,   FOLDERMAX );
	tmp=files[(*index)+1].filesize;
	strncpy( files[(*index)+1].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)+1].folder,   files[(*index)].folder,   FOLDERMAX );
	files[(*index)+1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
	files[(*index)].filesize=tmp;
	(*index)++;
	SaveFavorites();
}

void FavoritesDowndummy2( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)+1].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)+1].folder,   FOLDERMAX );
	tmp=files[(*index)+1].filesize;
	strncpy( files[(*index)+1].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)+1].folder,   files[(*index)].folder,   FOLDERMAX );
	files[(*index)+1].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
	files[(*index)].filesize=tmp;
	(*index)++;
	SaveFavorites();
}

void FavoritesDowndummy3( int *index ) {
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)+1].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)+1].folder,   FOLDERMAX );
	tmp=files[(*index)+1].filesize;
	strncpy( files[(*index)+1].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)+1].folder,   files[(*index)].folder,   FOLDERMAX );
//	files[(*index)+1].filesize=files[(*index)].filesize;
//	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
//	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
//	files[(*index)].filesize=tmp;
//	(*index)++;
//	SaveFavorites();
}
*/
