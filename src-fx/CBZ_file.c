#include "CB.h"

//-------------------------------------------------------------- source code refer to (WSC) file.c
//---------------------------------------------------------------------------------------------
#define	FONTCHARACTER_MAX 0x10A/2

Files *files = NULL;
int index = 0;

static int ReadFile( char *folder );
static int IsFileNeeded( FONTCHARACTER *FileName );
static FONTCHARACTER *FilePath( char *sFolder, FONTCHARACTER *sFont );
unsigned int Explorer( int size, char *folder );
static int FileCmp( const void *p1, const void *p2 );

static int size=0;
char folder[FOLDERMAX] = "", tmpfolder[FOLDERMAX] = "";	//, name[FILENAMEMAX] = "";
static char renamename[FILENAMEMAX] = "";
static char renamefolder[FOLDERMAX] = "";
static Files Favoritesfiles[FavoritesMAX];
char FileListUpdate=1;
char StorageMode=0;						// 0:Storage memory   1:SD
char redrawsubfolder=0;
//int recentsize=0;
char ForceG1Msave=0;		//    1: force g1m save
char AutoSaveMode=0;		//    1: Auto save ( not pop up )
const char root[][5]={"fls0","crd0","fls0","crd0"};
char root2[root2_MAX]="";	// "\\SAVE-F"
char root3[root2_MAX]="";	// "\\SAVE-F"
char textmodeExt[4];
char textmode=0;

int SetRoot2( char* SRC ) {
	char sname[root2_MAX];
	int c;
	c = SRC[g_exec_ptr++];
	if ( c == '/' ) {
		if ( SRC[g_exec_ptr] == '"' ) {
			CB_GetLocateStr(SRC, sname, root2_MAX-1); if ( g_error_type ) goto exit ;	// error
			root2[0] = '\\';
			strncpy( root2+1, sname, root2_MAX-1 );
			root2[root2_MAX-1] = '\0';
		} else root2[0] = '\0';
	} else
	if ( c=='.' ) RestoreRoot2() ;
	else g_exec_ptr--;
  exit:
	return SRC[g_exec_ptr] ;
}

void StoreRoot2(){
	strncpy( root3, root2, root2_MAX);
}
void RestoreRoot2(){
	strncpy( root2, root3, root2_MAX);
}

void SetFullfilenameNoExtNoFolder( char *filename, char *sname ) {
	sprintf( filename, "\\\\%s\\%s", root[StorageMode], sname);
}
void SetFullfilenameNoExtFolder( char *filename, char *dir, char *sname ) {
	sprintf( filename, "\\\\%s\\%s\\%s", root[StorageMode], dir, sname);
}
void SetFullfilenameNoExtsub( char *filename, char *dir, char *sname ) {
//	if ( ( strlen(dir) == 0 ) || ( StorageMode & 2 ) )
	if ( ( strlen(dir) == 0 ) )
		SetFullfilenameNoExtNoFolder( filename, sname ) ;
	else
		SetFullfilenameNoExtFolder( filename, dir, sname ) ;
}
void SetFullfilenameNoExt( char *filename, char *sname ) {
	SetFullfilenameNoExtsub( filename, folder, sname ) ;
}

void SetFullfilenameExtNoFolder( char *filename, char *sname, char *extname ) {
	sprintf( filename, "\\\\%s\\%s.%s", root[StorageMode], sname, extname );
}
void SetFullfilenameExtFolder( char *filename, char *dir, char *sname, char *extname ) {
	sprintf( filename, "\\\\%s\\%s\\%s.%s", root[StorageMode], dir, sname, extname );
}
void SetFullfilenameExtsub( char *filename, char *dir, char *sname, char *extname ) {
//	if ( ( strlen(dir) == 0 ) || ( StorageMode & 2 ) )
	if ( ( strlen(dir) == 0 ) )
		SetFullfilenameExtNoFolder( filename, sname, extname );
	else
		SetFullfilenameExtFolder( filename, dir, sname, extname );
}
void SetFullfilenameExt( char *filename, char *sname, char *extname ) {
	SetFullfilenameExtsub( filename, folder, sname, extname ) ;
}

unsigned int SelectFile (char *filename)
{
	unsigned int key;
	int i;
	char name[32]="";
	Bdisp_AllClr_DDVRAM();
	while( 1 ){
		if ( FileListUpdate  ) {
			if ( EnableExtFont ) {
				MSG2(VerMSG,"Font Reading.....");
				ReadExtFont();
//				LoadExtFontAnk(   3, "", -1 );			// FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6
//				LoadExtFontKana(  3, "", -1 );			// FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
//				LoadExtFontGaiji( 3, "", -1 );			// FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
			} else {
				ClearExtFontflag();
			}
			MSG2(VerMSG,"File Reading.....");
			Bdisp_PutDisp_DD();
			if ( StorageMode & 2 ) folder[0]='\0';
			if ( StorageMode & 2 ) size = MCS_ReadFileList() ;
			else                   size = ReadFile( folder );
			qsort( files+(FavoritesMAX), size-FavoritesMAX-1, sizeof(Files), FileCmp );
			memcpy2( files+FavoritesMAX+1, files+(FavoritesMAX), sizeof(Files)*(size-(FavoritesMAX+1)) );
		}
		key = Explorer( size, folder ) ;
		if ( key == FileCMD_NEW  ) break ;	// new file
		if ( key == FileCMD_MKDIR ) break ;	// Make Directory
		if ( key == FileCMD_DELDIR ) break ;	// Delete Directory
		if ( key == FileCMD_RENDIR ) break ;	// Rename Directory

		if ( key == KEY_CTRL_EXIT ) {			//to top of list
			index = 0;
		} else
		if ( ( key == KEY_CTRL_QUIT ) || ( index == size ) ) {			//return to root
			if ( !( StorageMode & 2 ) ) {
				if( strlen(folder) ) FileListUpdate = 1 ; // folder to root
				folder[0] = '\0';
			}
			index = 0;
		} else
		if( files[index].filesize == FOLDER_FLAG ){				//folder
			strcpy( folder, files[index].filename );
			index = 0;
			FileListUpdate = 1 ;
			redrawsubfolder= 0 ;
		} else {										//file
			strcpy( name,   files[index].filename );
			if ( !( StorageMode & 2 ) ) {
				if ( strcmp( files[index].folder  ,  folder ) != 0 ) FileListUpdate=1;
				strcpy( folder, files[index].folder );
			}
			break;
		}
		SaveConfig();
	}

	SetFullfilenameNoExt( filename, name );
	return key ;
}


//--------------------------------------------------------------
void Abort(){		// abort program
	unsigned int key;
	MSG2("Not enough Memory","Please Restart");
	while (1) GetKey(&key);
}
void ToLower( char *str ){
	while( (*str) != '\0' ) {
		if ( ( 'A' <= *str ) && ( *str <= 'Z' ) ) *str +=('z'-'Z');
		str++;
	}
}

void GetExtName( char *sname, char *ext ){	// sname -> sname.ext
	char *cptr;
	int i;
	cptr=(char*)strstr(sname,".");
	if ( cptr!=NULL ) {
		i=strlen(sname);
		while ( sname[i]!='.' ) i--;
		cptr=sname+i;
		strncpy( ext, cptr+1, 3 );
		*cptr='\0';
	} else ext[0]='\0';
	ext[3]='\0';
	ToLower( ext );
}

static int IsFileNeeded( FONTCHARACTER *find_name )
{
	char buffer[128];
	char ext[5];
	FontToChar(find_name, buffer);
	if ( buffer[0]=='.' ) return 0;	//
	GetExtName( buffer, ext );
	if( strlen(folder) ) return 1;	// all of sub folder
	return ( (strcmp(ext, "g3m") == 0) || (strcmp(ext, "g1m") == 0) || (strcmp(ext, "txt") == 0) || (strcmp(ext, "bmp") == 0) || (strcmp(ext, "csv") == 0) || (strcmp(ext, "bin") == 0) );
}

static int FileCmp( const void *p1, const void *p2 )
{
	Files *f1 = (Files *)p1;
	Files *f2 = (Files *)p2;
	char sname1[FILENAMEMAX+1],sname2[FILENAMEMAX+1];
	char ext1[4],ext2[4];
	int i;

	if( f1->filesize == FOLDER_FLAG && f2->filesize == FOLDER_FLAG )
		return strcmp( f1->filename, f2->filename );
	else if( f1->filesize == FOLDER_FLAG )
		return 1;
	else if( f2->filesize == FOLDER_FLAG )
		return -1;
	else {
		strncpy( sname1, f1->filename, FILENAMEMAX );
		strncpy( sname2, f2->filename, FILENAMEMAX );
		GetExtName( sname1, ext1 );
		GetExtName( sname2, ext2 );
		i = strcmp( sname1, sname2 );
		if  ( i ) return i;
		return strcmp( ext1, ext2 );
	}
}


void SetShortName( char *sname, char *filename) {	// fullpath filename -> short name
	int c,i;
	int ptr;
	ptr=strlen(filename);
	c=filename[--ptr];
	while ( c!='\\' ) {
		c=filename[--ptr];	//
		if ( ptr<0 ) break;
	}
	i=0;
	while ( c ) {
		c=filename[++ptr];
		sname[i++]=c;
	}
//	sname[i]='\0';
}

FONTCHARACTER * CharToFont( const char *cFileName, FONTCHARACTER *fFileName )
{
	int len, i;
	char fname[128],sname[64];
	strcpy( fname, cFileName );
	if ( ( fname[0] == '\\' ) && ( fname[1] == '\\' ) ) memcpy( sname, fname+7, 64-1 );
	else SetShortName( sname, fname );
	sprintf( fname, "\\\\%s%s\\%s", root[StorageMode], root2, sname);
	for( i = 0, len = strlen( fname ); i < len ; ++i )
		fFileName[i] = fname[i];
	fFileName[i] = '\0';

	return fFileName;
}

char * FontToChar( const FONTCHARACTER *fFileName, char *cFileName )
{
	int i = 0, len;
	char fname[128],sname[64];
	while( ( fname[i] = fFileName[i]) !=0 )
		++i;
	len = strlen(root2);
	if ( ( len != 0 ) && ( ( fname[0] == '\\' ) && ( fname[1] == '\\' ) ) && ( strncmp( fname+6, root2, len ) == 0 ) ) {
		SetShortName( sname, fname );
		SetFullfilenameNoExt( fname, sname );
	}

	strcpy( cFileName, fname );
	return cFileName;
}

FONTCHARACTER * FilePath( char *sFolder, FONTCHARACTER *sFont )
{
	char path[128];

	SetFullfilenameNoExtsub( path, sFolder, "*.*" );

	//Convert to FONTCHARACTER
	CharToFont( path, sFont );
	return sFont;
}

void ErrorMSGfile( char *buffer, char *filename, int err){
	char sname[32];
	char buf[32];
	SetShortName( sname, filename);
	if ( err ) {
		sprintf(buf, "%s (%d)", sname, err);
		ErrorMSGstr( buffer, buf);
	} else {
		ErrorMSGstr( buffer, sname);
	}
}

//--------------------------------------------------------------
static int ReadFile( char *folder )
{
	char str[64];
	FONTCHARACTER find_path[FONTCHARACTER_MAX];
	FONTCHARACTER find_name[FONTCHARACTER_MAX];
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
//	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
//		files = (Files *)HiddenRAM();
//	} else {
//		if ( recentsize < size ) {
		  loop:
			HiddenRAM_freeProg(HiddenRAM_Top);
			files = (Files *)HiddenRAM_mallocProg( size*sizeof(Files) );
			if ( files == NULL ) {	// Abort();
				MSG2("Clear Memory !!  ","File Reading.....");
				Bdisp_PutDisp_DD();
				DeleteMatrix( -1 );
				goto loop;
			}
//			recentsize = size;
//		}
//	}
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
				if ( ( i & 0x7 ) == 0 ) {
					sprintf( str, "%3d", i );
					CB_Print( 17, 4,  (unsigned char *)str);
					Bdisp_PutDisp_DD();
				}
				++i;
			}
		}
		Bfile_FindClose( find_h );
	}

	return size;
}


FONTCHARACTER * FilePath2( char *sFolder, char *ext, FONTCHARACTER *sFont )
{
	char path[128];

	SetFullfilenameExtsub( path, sFolder, "*", ext );

	//Convert to FONTCHARACTER
	CharToFont( path, sFont );
	return sFont;
}
static int IsFileNeeded2( FONTCHARACTER *find_name, char *ext )
{
	char str[64];
	char ext2[8];
	FontToChar(find_name,str);
	ToLower(ext);
	GetExtName( str, ext2 );
	return ( (strcmp( ext, ext2 ) == 0)  );
}
static int ReadFile2( char *folder, char *ext, int maxsize )
{
	char str[64];
	FONTCHARACTER find_path[FONTCHARACTER_MAX];
	FONTCHARACTER find_name[FONTCHARACTER_MAX];
	FILE_INFO file_info;
	int find_h;
	int size, i, r;

//	FileListUpdate=1;
//	memset( files, 0, maxsize*sizeof(Files) );
	FilePath2( folder, ext, find_path );
	i = 0 ;
	r =	Bfile_FindFirst(find_path, &find_h, find_name, &file_info);
	while ( r == 0 ) {
		if( ( ext[0] == '*' ) || ( ( file_info.type != DT_DIRECTORY ) && ( IsFileNeeded2( find_name, ext ) ) ) ){
			FontToChar(find_name, str);
			strncpy( files[i].filename, str, FILENAMEMAX);
			strncpy( files[i].folder, folder, FOLDERMAX);
			files[i].filesize = (file_info.type == DT_DIRECTORY ? FOLDER_FLAG : file_info.dsize);
			++i;
			if ( i+1 > maxsize ) goto exit;
		}
		r = Bfile_FindNext(find_h, find_name, &file_info);
	}
  exit:
	Bfile_FindClose( find_h );

	size=i;
	qsort( files, size, sizeof(Files), FileCmp );

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
	CopyFilesToFavorites();
}

int FavoritesFunc( int index ) {
	char tmpname[FILENAMEMAX];
	int i=(index);
	if ( files[i].filesize == 0 ) {
		if ( i < FavoritesMAX ) DeleteFavorites( i ) ;
		return 0;
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
			CopyFilesToFavorites();
		}
	}
	return index;
}
void FavoritesUpDown( int *index, int updw ) {	// up:-1	down:1
	int tmp;
	char tmpname[FILENAMEMAX];
	char tmpfolder[FOLDERMAX];
	strncpy( tmpname,   files[(*index)+updw].filename, FILENAMEMAX );
	strncpy( tmpfolder, files[(*index)+updw].folder,   FOLDERMAX );
	tmp=files[(*index)+updw].filesize;
	strncpy( files[(*index)+updw].filename, files[(*index)].filename, FILENAMEMAX );
	strncpy( files[(*index)+updw].folder,   files[(*index)].folder,   FOLDERMAX );
	files[(*index)+updw].filesize=files[(*index)].filesize;
	strncpy( files[(*index)].filename, tmpname, FILENAMEMAX );
	strncpy( files[(*index)].folder, tmpfolder, FOLDERMAX );
	files[(*index)].filesize=tmp;
	(*index)=(*index)+updw;
	CopyFilesToFavorites();
}

//--------------------------------------------------------------

int GetMediaFree( unsigned int *high, unsigned int *low) {
	int freespace[2];
	(*high)=0;
	switch ( StorageMode ) {
		case 0:		// Storage memory
			if ( Bfile_GetMediaFree(DEVICE_STORAGE,freespace) != 0 ) Abort() ;
			if ( IsSH3 ) {
				(*low) =freespace[0];
			} else {
				(*high)=freespace[0];
				(*low) =freespace[1];
			}
			break;
		case 1:		// SD
			if ( Bfile_GetMediaFree(DEVICE_SD_CARD,freespace) != 0 ) Abort() ;
			if ( IsSH3 ) {
				(*low) =freespace[0];
			} else {
				(*high)=freespace[0];
				(*low) =freespace[1];
			}
			break;
		case 2:		// main memory
			(*low) =MCS_Free();
			break;
	}
	return (*low);
}
void GetMediaFreeStr10( char *buffer ) {
	unsigned int high,low;
	unsigned int k;
	GetMediaFree( &high, &low);
	k=low/0x400+ high*0x400000;
	if ( k < 1000*1024 ) {
		sprintf(buffer,"%10d",low );
//	} else
//	if ( k < 1000*1024*1024 ) {
//		sprintf(buffer,"%10dK",k);
	} else {
		sprintf(buffer,"%9.3fM",(double)k/1024.0);
	}
}

int GetMemFree() {
	int k;
	if ( MaxMemMode ) {
		k = HiddenRAM_MatTopPtr - HiddenRAM_Top ;
	} else {
		k = HiddenRAM_MatTopPtr - HiddenRAM_ProgNextPtr ;
	}
	return k;
}
void GetMemFreeStr10( char *buffer ) {
	sprintf(buffer,"%7d bytes free ",GetMemFree() );
}

int CheckSD(){	// SD model  return : 1
	int freespace[2];
	if ( Bfile_GetMediaFree(DEVICE_SD_CARD,freespace) != 0 ) return 0;
	return 1 ;
}


//----------------------------------------------------------------------------------------------
void check_basname( char *basname ) {
	int c=basname[7]&0xFF;
	if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) basname[7]='\0';
}

int Check_Favorite( char*folder, char *sname ) {
	int i=0;
	char buf1[32];
	char buf2[32];
	strcpy( buf1, sname );
	if ( ( StorageMode & 2 ) ==0 ) ToLower( buf1 );	// non MCS mode
	while ( i < FavoritesMAX ) {	// file matching search
		strcpy( buf2, Favoritesfiles[i].filename );
		if ( ( StorageMode & 2 ) ==0 ) ToLower( buf2 );	// non MCS mode
		if ( strcmp( buf1, buf2 )== 0 )
		if ( strcmp( Favoritesfiles[i].folder  ,  folder)== 0 ) break; // already favorite exist
		i++;
	}
	return i;
}

void Check_Favorite_size( char*folder, char *oldsname, char *newsname, int size ) {
	int i=Check_Favorite( folder, oldsname);	// file matching search
	if ( i < FavoritesMAX ) { 		//	exist favorites list
		if ( size ) Favoritesfiles[i].filesize = size;
		strncpy( Favoritesfiles[i].filename, newsname,  FILENAMEMAX );	// rename name
		strncpy( Favoritesfiles[i].folder,   folder,    FOLDERMAX );
	}
}

unsigned int Explorer( int size, char *folder )
{
	int cont=1;
	int top;
	int i,j,k,s,tmp;
	unsigned int key;
	int FavCount=0;
	int StartLine;
	int filemode=0;
	int nofile=0;
	int Isfolder=0;
	char buffer[32];
	char buffer2[32];
	char buffer3[32];
	char fname[64];
	char ext[5];
	char search[10]="", *search2;
	int ContinuousSelect=0;
	int CursorStyle;
	int alphalock ;
	char alphalock_bk ;
	int searchmode=0;
	int csrX=0;
	int n_line=N_LINE;
	int yk;
	int miniflag=(EditFontSize & 0x07);

	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep
	KeyRecover();

	if ( index > size - 1 ) index = size - 1;
	if ( size == FavoritesMAX+1 ) index = 0;
//	top = index ;
	top = 0;

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

	GetMediaFreeStr10(buffer2);

	textmode = 0 ;
	
//	lowercase = 0;
 alpha_start:
	CursorStyle=Cursor_GetFlashStyle();
	if (CursorStyle<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 				Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
	apply_alphalock();
	alphalock = 1;
	Cursor_SetFlashMode(0); 		// cursor flashing off

	while( cont && (FileListUpdate==0) )
	{
		if ( miniflag ) { n_line=8; yk=18/3; } else { n_line=6; yk=24/3; }
		if ( StorageMode >= 2 ) memset( folder, 0, FOLDERMAX );
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
		
		if ( searchmode ) filemode=0; 
		switch ( filemode ) {
			case 0:
				Fkey_Icon( FKeyNo1, 388 );	//	Fkey_dispN( FKeyNo1,"EXE ");
				Fkey_Icon( FKeyNo2, 389 );	//	Fkey_dispR( FKeyNo2,"EDIT");
				Fkey_Icon( FKeyNo3, 390 );	//	Fkey_dispR( FKeyNo3,"NEW");
				Fkey_Icon( FKeyNo4, 909 );	//	Fkey_dispR( FKeyNo4,"COPY");
				Fkey_Icon( FKeyNo5,  56 );	//	Fkey_dispR( FKeyNo5,"DEL");
				Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
				break;
			case 1:
				strcpy(buffer,files[index].filename);
				GetExtName( buffer, ext );
				if ( (strcmp(ext, "g1m") == 0 ) ) Fkey_dispN( FKeyNo1,">txt"); else Fkey_dispN( FKeyNo1,">g1m");
				Fkey_Icon( FKeyNo2, 392 );	//	Fkey_dispR( FKeyNo2,"REN");
//				FkeyClear( FKeyNo3 );
//				FkeyClear( FKeyNo4 );
//				FkeyClear( FKeyNo5 );
				Fkey_dispN( FKeyNo3, "Fav.");
				Fkey_dispN_aA( 3, "Fv.\xE6\x92");
				Fkey_dispN_aA( 4, "Fv.\xE6\x93");
				Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
				break;
			case 2:
				FkeyClearAll();
				Fkey_Icon( FKeyNo1, 910 );	//	Fkey_dispR( FKeyNo3,"MkDir");
				Fkey_Icon( FKeyNo2, 944 );	//	Fkey_dispN( FKeyNo1,"RnDir");
				if ( StorageMode != 1 ) Fkey_dispN( FKeyNo3,"\xE6\x91SD");
				if ( StorageMode != 0 ) Fkey_dispN( FKeyNo4,"\xE6\x91SMEM");
				if ( StorageMode <  2 ) Fkey_dispN( FKeyNo5,"\xE6\x91Main");
//				Fkey_Icon( FKeyNo5,  56 );	//	Fkey_dispR( FKeyNo5,"DELA");
				Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
				break;
		}
//		locate(1, 1);Print((unsigned char*)"File List  [        ]");
//		locate(13,1);Print( strlen(folder) ? (unsigned char*)folder : (unsigned char*)"/");	// root
		locate(1, 1);Print((unsigned char*)"[        ]");
		if ( StorageMode & 2 ) {
			locate(2,1);Print((unsigned char*)"Main Mem");
		} else {
			if ( root2[0] ) PrintMini(13, 1, (unsigned char*)(root2+1), MINI_OVER);
			sprintf( buffer, "%-8s", folder );
			locate(2,1);Print( strlen(folder) ? (unsigned char*)buffer : (unsigned char*)"/");	// root
		}
		if ( searchmode ) {
			locate(12, 1);Print((unsigned char*)"[        ]");
			locate(13,1);Print( (unsigned char*)search );	// search string
			if ( lowercase  ) Fkey_dispN_aA( FKeyNo4, "A<>a"); else Fkey_dispN_Aa( FKeyNo4, "A<>a");
			Fkey_Icon( FKeyNo5, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
//			Fkey_Icon( FKeyNo6, 402 );	//	Fkey_DISPN( FKeyNo6, " / ");
			if ( StorageMode < 2 ) {
				if ( search[0]=='/' ) Fkey_dispN( FKeyNo6, "\xE6\x91\x46ile"); else Fkey_dispN( FKeyNo6, "\xE6\x91 /");
			} else {
				FkeyClear( FKeyNo6 );
			}
		} else {
			PrintMini(10*6+1, 1, (unsigned char*)buffer2, MINI_OVER);  // free area
			sprintf(buffer, "(%d)", size-FavoritesMAX-1);
			PrintMini(18*6  , 1, (unsigned char*)buffer , MINI_OVER);  // number of file
		}
		if( size < 2+FavoritesMAX-FavCount ){
			locate( 8, 4 );
			Print( (unsigned char*)"No Data" );
			nofile=1;
		}
		else{
			char buf[22],buf2[22];
			relist:
			if ( index == FavoritesMAX ) index++;
			if( index < StartLine )
				index = StartLine;
			if( top > index )
				top = index;
			if( index > top + n_line - 1 )
				top = index - n_line + 1 ;
			if( top < StartLine )
				top = StartLine;
			if( size - StartLine <= n_line )
				top = StartLine;

			for(i = 0;i < n_line && i + top < size; ++i ){

				if( files[i + top].filesize == 0 ) {
					sprintf( buf, "---------------------");
					goto dsp1;
				} else
				if ( files[i + top].filesize == FOLDER_SEPALATOR ) {
				  	if ( miniflag ) {
						sprintf( buf, "----------Favorites----------");
					} else {
						sprintf( buf, "------Favorites------");
					}
					goto dsp1;
				} else
				if( files[i + top].filesize == FOLDER_FLAG ) {
					sprintf( buf, " [%s]", files[i + top].filename );
				  dsp1:
				  	if ( miniflag ) {
						PrintMini(    2, (i+1)*yk+2, (unsigned char*)buf, MINI_OVER ) ;
					} else {
						locate(  1, i + 2 ); Print( (unsigned char*)buf );
					}
				} else {
					strncpy( buf2, files[i + top].filename, FILENAMEMAX );
					j=strlenOp(files[i + top].filename); if (j<4) j=4;
					k=files[i + top].filesize;
					if ( buf2[j-3]=='g' ) {
//						k -= 0x38;	// file size adjust G1M
						if ( StorageMode & 2 ) {	// MCS mode
							buf2[j-4]='\0';
							k = (k+21) & 0xFFFFFFFC;	// file size adjust G1M
						}
					}
					sprintf( buf, " %-12s ", buf2 );

					if ( strcmp( files[i+top].folder, folder ) != 0 )
					if ( i+top < FavoritesMAX ) {
						j=strlenOp(files[i + top].folder) ;
						if (j>5) j=5;
						if ( j ) strncpy( buf2, files[i + top].folder, j);
						buf2[j++]='/'; buf2[j]=0;
						strncpy( buf, files[i + top].filename, FILENAMEMAX );
						j=strlenOp(files[i + top].filename); if (j<4) j=4;
						if ( buf[j-3]=='g' ) buf[j-4]='\0';
						strcat(buf2,buf); buf2[14]=0;
						sprintf( buf, "%-14s ", buf2 );
					}
				  	if ( miniflag ) {
						PrintMini(    2, (i+1)*yk+2, (unsigned char*)buf, MINI_OVER ) ;
					} else {
						locate(  1, i + 2 ); Print( (unsigned char*)buf );
					}
				  	if ( miniflag ) {
						sprintf( buf, ":%8u ", k );
						PrintMini(    (22-1)*12/3, (i+1)*yk+2, (unsigned char*)buf, MINI_OVER ) ;
					} else {
						sprintf( buf, ":%5u ", k );
						locate( 15, i + 2 ); Print( (unsigned char*)buf );
					}
				}
			}

			Bdisp_AreaReverseVRAM( 0, (index-top+1)*yk+(miniflag!=0) , 127-8*miniflag, (index-top+2)*yk+1-(miniflag==0)*2 );
//			Bdisp_AreaReverseVRAM( 0, (index-top+1)*8 , 127, (index-top+2)*8-1 );
			if( top > StartLine ) {
				if ( miniflag ) {
					PrintMini( 121, 8, (unsigned char*)"\xE6\x92", MINI_OVER ) ;
				} else {
					PrintXY( 120, 8, (unsigned char*)"\xE6\x92", top == index );
				}
			}
			if( top + n_line < size  ) {
				if ( miniflag ) {
					PrintMini( 121, n_line*6+2, (unsigned char*)"\xE6\x93", MINI_OVER ) ;
				} else {
					PrintXY( 120, n_line*8, (unsigned char*)"\xE6\x93" , top + n_line - 1 == index );
				}
			}
		}

		Isfolder= ( files[index].filesize == FOLDER_FLAG ) ;

		if ( searchmode ) {
			locate(13+csrX,1);
			Cursor_SetFlashMode(1);			// cursor flashing on
			CursorStyle=Cursor_GetFlashStyle();
			if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
			if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
			if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
			if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
		}
		if ( ContinuousSelect ) ContinuousSelect=0; else GetKey(&key);
		Cursor_SetFlashMode(0); 		// cursor flashing off
		
		if ( KeyCheckPMINUS() ) {
			key = '@';
		}

		switch ( key ) {
			case KEY_CTRL_ALPHA:
				alphalock = 0 ;
				key = 0;
				break;
					
			case KEY_CTRL_EXIT:
				if ( ( searchmode==0 ) && ( ( nofile ) || ( index == StartLine ) ) ) {
					key=KEY_CTRL_QUIT;
					cont =0 ;
					break;
				}
				if ( searchmode==0 ) index = 0;
				searchmode=0;
			case KEY_CTRL_AC:
				search[0]='\0';
				csrX=0;
				if  ( alphalock ) goto alpha_start;
				key = 0;
				break;

			case KEY_CTRL_LEFT:
				if ( searchmode == 0 ) break;
				PrevOpcode( search, &csrX );
				break;
				
			case KEY_CTRL_RIGHT:
				if ( searchmode == 0 ) break;
				if ( search[csrX] != 0x00 )	NextOpcode( search, &csrX );
				break;

			case KEY_CTRL_DEL:
				if (searchmode ) {
					if ( CursorStyle < 0x6 ) {		// insert mode
						PrevOpcode( search, &csrX );
					}
					DeleteOpcode1( search, 8, &csrX );
				}
				break;
			
			case KEY_CTRL_UP:
				if ( nofile ) break;
				do {
					if( --index < StartLine  )
						index = size - 1;
				} while ( files[index].filesize == FOLDER_SEPALATOR ) ;
				key = 0;
				break;
			case KEY_CTRL_DOWN:
				if ( nofile ) break;
				do {
					if( ++index > size - 1 )
						index = StartLine ;
				} while ( files[index].filesize == FOLDER_SEPALATOR ) ;
				key = 0;
				break;
			case KEY_CTRL_EXE:
//				if ( filemode != 0 ) break;
				strcpy(buffer,files[index].filename);
				if ( buffer[0] == 0 ) break;
				GetExtName( buffer, ext );
				if ( (strcmp(ext, "bmp") == 0 ) ) {
					SetFullfilenameExt( fname, buffer, ext );
					if ( LoadProgfile( fname, 0, 0, 1 ) ) return 1 ; // error
					break;
				}
				key=FileCMD_RUN;
				cont =0 ;
				break;
			case KEY_CTRL_F1:	// run
				if ( nofile ) break;
				switch ( filemode ) {
					case 0:
						key=FileCMD_RUN_F1;
						cont =0 ;
						break;
					case 1:
						key=FileCMD_TEXT;
						cont =0 ;
						break;
					case 2:
						if ( StorageMode & 2 ) break;
					  	if ( MakeDirectory() == 0 )	cont =0 ;	// ok
						key=FileCMD_MKDIR;
						break;
				}
				break;
			case KEY_CTRL_F2:	// edit
				if ( nofile ) break;
				switch ( filemode ) {
					case 0:
					  edit:
						if ( Isfolder ) break;
						key=FileCMD_EDIT;
						cont =0 ;
						break;
					case 1:		// rename
						if ( Isfolder ) goto rendir;
						key=FileCMD_RENAME;
						cont =0 ;
						break;
					case 2:
					  rendir:
//						if ( Is35E2 == 0 ) break;
						if ( StorageMode & 2 ) break;
						if ( Isfolder == 0 ) break;
					  	if ( RenameDirectorys(files[index].filename) == 0 ) cont =0 ;	// ok
						key=FileCMD_RENDIR;
						break;
				}
				break;
			case KEY_CTRL_F3:	// New file
				switch ( filemode ) {
					case 0:
						key=FileCMD_NEW;
						if ( Isfolder ) {
//							strcpy( folder, files[index].filename );
							index = 0;
						}
						cont =0 ;
						break;
					case 1:
						if ( Isfolder ) break;
						index = FavoritesFunc( index );
//						FavoritesFunc( index );
						break;
					case 2:	// ->SD
						if ( StorageMode == 1 ) break;
						ChangeStorageMode( CheckSD() ) ; // SD mode
						FileListUpdate = 1;
						cont=0;
						break;
				}
				break;
			case KEY_CTRL_F4:	// Copy file
				if ( searchmode ) {
					lowercase=1-lowercase;
					if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
					key = 0;
					break;
				} 
				switch ( filemode ) {
					case 0:
						if ( Isfolder ) break;
						if ( nofile ) break;
						key=FileCMD_COPY;
						cont =0 ;
						break;
					case 1:
						goto fav_up;
						break;
					case 2:	// ->Storage
						if ( StorageMode == 0 ) break;
						ChangeStorageMode( 0 );	// -> storage memory
						FileListUpdate = 1;
						cont=0;
						break;
				}
				break;
			case KEY_CTRL_F5:	// delete file
				if ( searchmode ) {
					key=SelectChar( &ContinuousSelect);
					if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
					break;
				}
				switch ( filemode ) {
					case 0:
						if ( nofile ) break;
						if ( Isfolder )	{
							if ( DeleteFolder(files[index].filename, 1) == 0 ) cont =0 ;	// ok
							key=FileCMD_DELDIR;
						} else	{
							key=FileCMD_DEL;
							cont =0 ;
						}
						break;
					case 1:
						goto fav_dw;
						break;
					case 2:	// ->MCS
						if ( StorageMode >= 2 ) break;
						ChangeStorageMode( StorageMode | 2 );	// -> main memory
						FileListUpdate = 1;
						cont=0;
						break;
				}
				break;
			case KEY_CTRL_F6:		// ------- change function
				if ( StorageMode < 2 ) {
					if ( searchmode ) {
						key='/';
						break;
					}
				} 
				filemode ++;
				if ( filemode >2 ) filemode=0;
				break;

				
			case KEY_CTRL_OPTN:		// ------- Favorites list
				if ( nofile ) break;
				filemode = 0 ;
				index = FavoritesFunc( index );
//				FavoritesFunc( index );
				break;

			case KEY_CTRL_VARS:
				filemode = 0 ;
				SetVar(0);		// A -
				break;

//			case KEY_CHAR_SQUARE:
			case KEY_CHAR_ROOT:
//			case KEY_CHAR_VALR:
			  textmodejp:
				strcpy(buffer,files[index].filename);
				if ( buffer[0] == 0 ) break;
				GetExtName( buffer, textmodeExt );
				textmode = 1 ;
				goto edit;
				break;

			case KEY_CTRL_SHIFT:
				filemode = 0 ;
				Fkey_Icon( FKeyNo1, 877 );	//	Fkey_dispN( FKeyNo1, "Var");
				Fkey_Icon( FKeyNo2, 286 );	//	Fkey_dispN( FKeyNo2, "Mat");
				Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "VWIN");
				Fkey_Icon( FKeyNo4, 406 );	//	Fkey_dispN( FKeyNo4, "Pass");
				if ( StorageMode==3 ) Fkey_dispN( FKeyNo5,"\xE6\x91SD");
				else
				if ( StorageMode==2 ) Fkey_dispN( FKeyNo5,"\xE6\x91SMEM");
				else				  Fkey_dispN( FKeyNo5,"\xE6\x91Main");
				Fkey_dispN( FKeyNo6, "Debg");
				GetMemFreeStr10(buffer3);
				if ( searchmode == 0 ) PrintMini(10*6+1, 1, (unsigned char*)buffer3, MINI_OVER);  // free mem area
				alphalock_bk = alphalock;
				alphalock = 0 ;
				if ( searchmode ) {
					locate(13+i,1);
					Cursor_SetFlashMode(1);			// cursor flashing on
				}
				GetKey(&key);
				Cursor_SetFlashMode(0); 		// cursor flashing off
				switch (key) {
					case KEY_CTRL_SHIFT:
							key = 0;
							break;
					case KEY_CTRL_ALPHA:
							alphalock = 1 ;
							key = 0;
							break;
					case KEY_CTRL_QUIT:
						update:
							FileListUpdate = 1 ; //
							cont = 0 ;
							break;
					case KEY_CTRL_SETUP:
							i = StorageMode ;
							selectSetup=SetupG(selectSetup, 0);
							miniflag=(EditFontSize & 0x07);
							if ( ( FileListUpdate ) || ( i != StorageMode ) ) { key = KEY_CTRL_EXIT; goto update; }
							break;
					case KEY_CTRL_F1:
							selectVar=SetVar(selectVar);		// A -
							break;
					case KEY_CTRL_F2:
							selectMatrix=SetMatrix(selectMatrix);		//
							break;
					case KEY_CTRL_F3:
							SetViewWindow();
							break;
					case KEY_CTRL_F4:	//
							key=FileCMD_PASS;
							cont = 0 ;
							break;
					case KEY_CTRL_F5:	//
						  ChangeStorageMode:
							if ( StorageMode == 0 ) {
								ChangeStorageMode( 2 );	// -> main memory
							} else
							if ( StorageMode == 1 ) {
								ChangeStorageMode( 3 );	// -> main memory
							} else {
								ChangeStorageMode( StorageMode & 1 );	// -> storage/SD memory
							}
							FileListUpdate = 1;
							cont=0;
							break;
					case KEY_CTRL_F6:
							if ( Isfolder ) break;
							if ( nofile ) break;
							key=FileCMD_DebugRUN;
							cont =0 ;
							break;
					case KEY_CTRL_PAGEUP:	// up
					  fav_up:
							if ( nofile ) break;
							if ( index <= StartLine ) break;
							if ( index > FavoritesMAX-1 ) break;
							FavoritesUpDown( &index, -1 );
							break;
					case KEY_CTRL_PAGEDOWN:	// down
					  fav_dw:
							if ( nofile ) break;
							if ( FavCount < 1 ) break;
							if ( index >= FavoritesMAX-1 ) break;
							FavoritesUpDown( &index, 1 );
							break;

					case KEY_CHAR_ROOT:
							goto textmodejp;
					case KEY_CTRL_CATALOG:
							Bdisp_AllClr_VRAM();
							locate(1,1); Print((unsigned char*)"=== File mode Help ===");
							locate(1,2); Print((unsigned char*)"[OPTN]: Favorite");
							locate(1,3); Print((unsigned char*)"[VARS]: variabl list");
							locate(1,4); Print((unsigned char*)"[(-)]: '@'");
							locate(1,5); Print((unsigned char*)"[ALPHA]+[ \x90\xE5\xC2 ] : ~");
							locate(1,6); Print((unsigned char*)"[ALPHA]+[ ^  ] : '");
							locate(5,7); Print((unsigned char*)"Press:[EXIT]");
							ExitKey();
					default:
						break;
				}
				break;
			default:
				break;
		}

		if ( KEY_CHAR_DIV  == key ) key='/'*( StorageMode < 2 );
		if ( KEY_CHAR_PLUS == key ) key='+';
		if ( KEY_CHAR_MINUS== key ) key='-';
//		if ( KEY_CHAR_DP   == key ) key='.';
		if ( KEY_CHAR_SQUARE  == key ) key='~';
		if ( KEY_CHAR_VALR    == key ) key='~';
		if ( KEY_CHAR_POW     == key ) key='^';
		if ( KEY_CHAR_THETA   == key ) key=0x27;
		if ( KEY_CHAR_EXP  == key ) key=0x0C;
		if ( KEY_CHAR_PMINUS  == key ) key='@';
		if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');

		if ( key == '/' ) {
			if ( searchmode ) {
				if (search[0]=='/') {
					for (i=0; i<8; i++ ) search[i]=search[i+1];
					csrX--;
					goto fileSRC;
				} else {
					if ( strlen(search) ) {
						for (i=7; i>0; i-- ) search[i]=search[i-1];
						search[8]='\0';
						search[0]='/';
						csrX++;
						if ( search[1]==0 ) goto foldertop; else goto fileSRC;
					}
				}
			}
		}
		
		if ( ( 0x20 <= key ) && ( key <= 0x7E ) ) {
			if ( CursorStyle < 0x6 ) {		// insert mode
				i=InsertOpcode1( search, 8, csrX, key );
			} else {					// overwrite mode
				if ( search[csrX] != 0x00 ) DeleteOpcode1( search, 8, &csrX);
				i=InsertOpcode1( search, 8, csrX, key );
			}
			if ( i==0 ) NextOpcode( search, &csrX );
		}

		if ( ( 0x0C == key ) ) {
		  foldertop:
			i=FavoritesMAX;
			while ( i<size ) {
				if ( files[i].filesize == FOLDER_FLAG ) {
						index = i;
						top = index;
					break;
				}
				i++;
			}
		} else
		if ( ( 0x20 < key ) && ( key < 0x7E ) &&( strlen(search) ) ) {
			searchmode=1;
		  fileSRC:
			j=strlen(search);
			if ( search[0]=='/' ) {;	// folder search
				j--; if ( j==0 ) goto foldertop;
			}
			i=FavoritesMAX;
			while ( i<size ) {
				if ( search[0]=='/' ) {
					if ( files[i].filesize != FOLDER_FLAG ) i++;
					else {		// folder search
						k=0;
						search2=search+1;
						while ( k<=j ) {
							if ( files[i].filename[k] != search2[k] ) { k=(k>=j); break; }
							k++;
						}
						if ( k>0 ) {
							index = i;
							top = index;
							break;
						} else i++; // FavoritesMAX skip
					}
				} else {		// file search
					if ( files[i].filesize == FOLDER_FLAG ) i++;
					else {
						k=0;
						while ( k<=j ) {
							if ( files[i].filename[k] != search[k] ) { k=(k>=j); break; }
							k++;
						}
						if ( k>0 ) {
							index = i;
							top = index;
							break;
						} else i++; // FavoritesMAX skip
					}
				}
			}
		}

		SaveFavorites();
	}

	if ( alphalock ) { i=key; perform_key(KEY_CTRL_ALPHA); key=i; }
	

	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
	return key;
}



//----------------------------------------------------------------------------------------------
int alreadyExistedFile( FONTCHARACTER *filename, FONTCHARACTER *buffer ){
	int handle;
	FONTCHARACTER filename2[FONTCHARACTER_MAX];
	int r,s;
	/* disable, just for call "Bfile_FindFirst" */
	FILE_INFO info;
	/* end */
	memcpy( (char*)filename2, (char*)filename, 50*2);
	r = Bfile_FindFirst( filename2, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
//	if( r == 0 ) { //already existed, delete it
//		s=0; if ( folder != NULL ) s=strlen(folder)*2+2;
//		memcpy( (char*)filename+14+s, (char*)buffer, (50-8)*2-s );	// replace filename
//	}
	return r;	// 0:already existed
}

int renameExistedFile( char *basname, char *ext ){
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int r,s;
	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	/* end */
	char fname[64];

	SetFullfilenameExt( fname, basname, ext );
	if ( StorageMode & 2 ) {
		SetShortName( basname, fname );
		return 1;	// MCS
	}
	CharToFont( fname, filename );
	r = alreadyExistedFile( filename, buffer );
	if ( r ) return r ;
	FontToChar( buffer, basname );
//	SetShortName( basname, fname );
//	GetExtName( basname, ext );
	return r;	// 0:already existed
}
int renameExistedfname( char *fname ){
	int handle;
	int r,s;
	char sname[64];
	char basname[32];
	char ext[8];

	SetShortName( basname, fname );
	GetExtName( basname, ext );
	r = renameExistedFile( basname, ext ) ;
	if ( r ) return r;

	GetExtName( basname, ext );
	SetFullfilenameExt( fname, basname, ext );
	return r;	// 0:already existed
}

/* load file to buffer */
char * loadFile( const char *name, int *editMax, int disperror, int *filesize )
{
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int size;
	char *buffer;
	char fname[64];

	strcpy( fname, name );
	renameExistedfname( fname );
	CharToFont( fname, filename );

	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) {
		if ( disperror ) ErrorMSGfile( "Can't find file", name, handle);
		CB_Error(FileNotFound);
		return NULL;
	}

	size = Bfile_GetFileSize( handle );
  loop:
	buffer = ( char *)HiddenRAM_mallocProg( size*sizeof(char)+(*editMax)+4 );
	if( buffer == NULL ) {
		(*editMax)/=2; if ( (*editMax)>=16 ) goto loop;
		if ( disperror ) ErrorMSGfile( "Can't load file", (char*)name, handle);
		CB_Error(NotEnoughMemory);
		return NULL;
//		Abort();
	}
	memset( buffer, 0x00,     size*sizeof(char)+(*editMax)+4 );

	(*filesize) = size;

	Bfile_ReadFile( handle, buffer, size, 0 );
	buffer[size] = '\0';
//	strcat( buffer, " " ); // for some reasons about the preprocessor...

	Bfile_CloseFile( handle );
	return buffer;
}

int deleteFile( FONTCHARACTER *filename, int size ){
	int handle;
	FONTCHARACTER filename2[FONTCHARACTER_MAX];
	int r,s;
	unsigned int freehigh,freelow;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	/* end */
	char fname[64];

	FontToChar( filename, fname );
	renameExistedfname( fname );
	CharToFont( fname, filename );
	r = alreadyExistedFile( filename, buffer );
	if( r == 0 ) { //already existed, delete it
		GetMediaFree(&freehigh,&freelow);
		if ( ( freehigh == 0 ) && ( freelow < size+256 ) ){ ErrorMSG( "Not enough MEM", freelow ); return 2 ; }
		r = Bfile_DeleteFile( filename );
		if( r < 0 ) { ErrorMSG( "Can't delete file", r );	return 1 ; }
	}
	return 0;	// ok
}

int GetFileSize( const char *fname ) {
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int r,s;
	int size;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
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

/* store bytecode into file */
int storeFile( const char *name, unsigned char* codes, int size )
{
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	FONTCHARACTER filename2[FONTCHARACTER_MAX];
	int r,s,fsize;
	unsigned int freehigh,freelow;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	/* end */

	CharToFont( name, filename );
	CharToFont( name, filename2 );

	FileListUpdate=1;
	fsize = GetFileSize( name );
	if ( ( fsize > size ) || ( fsize==0 ) || ( Is35E2==0 ) ) {	// delete & create file
		r = deleteFile( filename2, size ) ;
		if ( r ) return r;
		handle = Bfile_CreateFile( filename, size );
		if( handle < 0 ) { ErrorMSGfile( "Can't create file", (char*)name, handle ); return 1 ; }
		r = Bfile_CloseFile( handle );
	}

	handle = Bfile_OpenFile( filename, _OPENMODE_WRITE );
	if( handle < 0 ) { ErrorMSG( "Can't open file", handle ); return 1 ; }
	Bfile_WriteFile( handle, codes, size );
	r = Bfile_CloseFile( handle );
	if( r ) { ErrorMSG( "Close error", handle ); return 1 ; }

	return 0 ;
}


int ExistFile( char *fname, int replace ) {
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	char sname[32];
	int r,s;

	/* disable, just for call "Bfile_FindFirst" */
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	/* end */

	CharToFont( fname, filename );
	r = alreadyExistedFile( filename, buffer );	// replace? filename
	if ( ( replace==0 ) || ( r ) ) return r;	// not exist
	FontToChar( buffer, sname );	// replace fname
	SetFullfilenameNoExt( fname, sname );
	return r; // r==0 existed
}

/* G1M file exist? */
int ExistG1M( const char *sname ) {
	char fname[32];
	char ext[8];
	int r;
	SetFullfilenameExt( fname, sname, "g1m" );
	if ( StorageMode & 2 ) return MCS_ExistFile( fname );
	r = ExistFile( fname, 1 ); // r==0 existed
	if ( r==0 ) {
		SetShortName( sname, fname);	// replace sname
		GetExtName( sname, ext );
	}
	return r;
}
int ExistG1Mext( const char *sname, char *ext ) {
	char fname[32];
	int r;
	SetFullfilenameExt( fname, sname, ext );
	if ( ext[0] == 'M' ) return MCS_ExistFile( fname );
	r = ExistFile( fname, 1 ); // r==0 existed
	if ( r==0 ) {
		SetShortName( sname, fname);	// replace sname
		GetExtName( sname, ext );
	}
	return r;
}

//----------------------------------------------------------------------------------------------
int YesNoOverwrite(){
	return ( YesNo( "Overwrite OK?" ) ==0 ) ;
}
int YesNoOverwritefile( char *filename){
	char sname[64];
	SetShortName( sname, filename);
	return YesNo2(sname,"Overwrite OK?") == 0 ;
}

unsigned int InputStrFilename(int x, int y, int width, int maxLen, char* buffer ) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key=0;
	int i;
//	char srcstr[]=".";
//	int ptr=1;
	char *cptr;

	buffer[width]='\0';
	csrX=strlenOp((char*)buffer);
//	i=StrSrc( buffer, srcstr, &ptr, 22 ); if ( i ) csrX=ptr-;
	cptr=(char*)strstr((char*)buffer,".");
	if ( cptr!=NULL ) {
		csrX=(int)cptr-(int)buffer;
	}
	do {
		key=InputStrSub_status( x, y, width, csrX, buffer, maxLen, " ", REV_OFF, FLOAT_OFF, EXP_OFF, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	} while ( (key!=KEY_CTRL_EXIT)&&(key!=KEY_CTRL_QUIT)&&(buffer[0]=='\0') ) ;
	for( i=0; i<strlenOp((char*)buffer); i++ ) {	// kana cancel
		if ( buffer[i]==0xFFFFFFFF ) {
			buffer[i]=0x7E;
			if ( buffer[i+1] != '\0' ) buffer[i+1]=0x7E;
		}
	}
	return ( key );
}

void Getfolder( char *sname ) ;

int InputFilenameG1MorG3M( char *buffer, char* pmsg, char *ext ) {		//
	int key;
	char msg1[32];
	char msg2[32];
	SaveDisp(SAVEDISP_PAGE2);
	PopUpWin(5);
	apply_alphalock();
  loop:
  	if ( ext[0]=='M' ) {
		sprintf( msg1, "%s to       ", pmsg );
		sprintf( msg2, "     Main Memory." );
		if ( StorageMode & 1 ) 	Fkey_dispN( FKeyNo1, ">SD");
		else 					Fkey_dispN( FKeyNo1, ">SMM");
	} else {
		sprintf( msg1, "%s as %s to", pmsg, ext);
		if ( StorageMode & 1 ) 	sprintf( msg2, "  SD      Memory." );
		else 					sprintf( msg2, "  Storage Memory." );
		Fkey_dispN( FKeyNo1, ">MCS");
//		Fkey_dispN( FKeyNo2, "g1m");
	}
	locate(3,2); Print((unsigned char *)msg1);
	locate(3,3); Print((unsigned char *)msg2);
	locate(3,5); Print((unsigned char *)"File Name?");
	locate(3,6); Print((unsigned char *)" [             ]");
//	Fkey_dispN( FKeyNo1, "Main");
//	Fkey_dispN( FKeyNo2, "g1m");
	FkeyClear( FKeyNo2 );
	FkeyClear( FKeyNo3 );
	Fkey_Icon( FKeyNo6, 402 );	//	Fkey_DISPN( FKeyNo6, " / ");
	key=InputStrFilename( 5, 6, 12, 18, buffer ) ;
	if ( ext[0]=='M' ) {
		if (key==KEY_CTRL_F1) { strcpy( ext, "g1m" ); goto loop; }	// ->S.MEM
		if ( (key==KEY_CTRL_F2) || (key==KEY_CTRL_F3) ) goto loop;
	} else {
		if (key==KEY_CTRL_F1) { strcpy( ext, "MCS" ); goto loop; }	// MCS
		if (key==KEY_CTRL_F2) { strcpy( ext, "g1m" ); goto loop; }	// g1m
	}
	RestoreDisp(SAVEDISP_PAGE2);
	if (key==KEY_CTRL_AC) return 1;
	if ( (key==KEY_CTRL_EXIT)||(key==KEY_CTRL_QUIT) ) { WaitKeyEXIT(); return 1; }
	Getfolder( buffer );
	return 0; // ok
}

int InputFilename( char * buffer, char* msg) {		//
	unsigned int key;
//	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(3);
	locate(3,3); Print((unsigned char *)msg);
	locate(3,4); Print((unsigned char *)" [             ]");
	FkeyClear( FKeyNo1 );
	FkeyClear( FKeyNo2 );
	FkeyClear( FKeyNo3 );
	Fkey_Icon( FKeyNo6, 402 );	//	Fkey_DISPN( FKeyNo6, " / ");
	do {
		key=InputStrFilename( 5, 4, 12, 18, buffer ) ;
	} while ( (key==KEY_CTRL_F1)||(key==KEY_CTRL_F2)||(key==KEY_CTRL_F3) );
//	RestoreDisp(SAVEDISP_PAGE1);
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) { WaitKeyEXIT(); return 1; }
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
	check_basname( nameptr );	// check end of multibyte character
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
	filebase[0x10]=((0xFFFFFFFF-*size)>>24)&0xFF;	//
	filebase[0x11]=((0xFFFFFFFF-*size)>>16)&0xFF;	//
	filebase[0x12]=((0xFFFFFFFF-*size)>>8)&0xFF;	//
	filebase[0x13]=((0xFFFFFFFF-*size))&0xFF;		//
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
	char buffer[32];
	Bdisp_AllClr_VRAM();
	locate(1,1); Print((unsigned char *)msg);
	sprintf(buffer,"[%-8s]",basname);
	locate(1,2); Print((unsigned char *)buffer);
}

int InputPassname( int y, char* inputpassname, char *msg) {		// password input
	unsigned int key;
	locate(1,y  ); Print((unsigned char *)msg);
	locate(1,y+1); Print((unsigned char *)"[        ]");
	FkeyClear( FKeyNo1 );
	FkeyClear( FKeyNo2 );
	FkeyClear( FKeyNo3 );
	key=InputStrFilename( 2, y+1, 8, 8, inputpassname ) ;
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) { WaitKeyEXIT(); return 1; }
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
	char buffer[32];
	Bdisp_AllClr_VRAM();
	locate(1,1); Print((unsigned char *)msg);
	locate(1,2); Print((unsigned char *)"[        ]");
	Fkey_Icon( FKeyNo1, 406 );	//	Fkey_dispN( FKeyNo1, "Pass");
	FkeyClear( FKeyNo2 );
	FkeyClear( FKeyNo3 );
	key=InputStrFilename( 2, 2, 8, 8, basname ) ;
	if (key==KEY_CTRL_AC) return 1;
	if (key==KEY_CTRL_EXIT) { WaitKeyEXIT(); return 1; }
	if (key==KEY_CTRL_F1) {	// password input
		if ( SetPassWord( 3, filebase, basname, "Password?" ) ) return 1 ; // cancel
	}
	filebase[0x55]=0x01;	// pass ok (C.basic)
	return 0; // ok
}

void NewPassWord( char *fname ){	// New Password command
	char *filebase;
	int fsize,size;
	char sname[32],basname[32],ext[8];

	SetShortName( sname, fname);
	GetExtName( sname, ext );
	if ( strcmp( ext, "g1m") != 0 ) return ;	// not g1mfile
	if ( LoadProgfile( fname, 0, 0, 1 ) ) return ; // error
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
//----------------------------------------------------------------------------------------------
void ConvertToOpcode( char *filebase, char *sname, int editsize){
	int size,i,j;
	char *text;
	int textptr;
	int codesize;
	int textsize;

	ProgressBarPopUp("Text Converting..", sname );
	textsize=strlen(filebase);
	memcpy( filebase+editsize, filebase, textsize);
	memset( filebase, 0, editsize );
	codesize=TextToOpcode( filebase, filebase+editsize, textsize+editsize );
	size=codesize+0x56+1;
	G1M_header( filebase, &size );	// G1M header set
	G1M_Basic_header( filebase );	// G1M Basic header set
	basname8ToG1MHeader( filebase, sname);
	MSGpop();
}

int CheckG1M( char *filebase ){	// Check G1M Basic file
	unsigned int key;
	int fsize,size;
	if ( ( filebase[0x00] == 'B' ) && ( filebase[0x01] == 'M' ) ) {	// bmp ?
		Bdisp_AllClr_VRAM();
		if ( DecodeBmp2Vram( filebase, 0, 0 ) ==0 ) return 1; // not bmp
		GetKey( &key );
		return 1;
	}
	fsize = 0xFFFFFFFF-(((filebase[0x10]&0xFF)<<24)+((filebase[0x11]&0xFF)<<16)+((filebase[0x12]&0xFF)<<8)+(filebase[0x13]&0xFF));
	size  = SrcSize( filebase ) ;
	if ( ( fsize != size ) || ( filebase[0x20] != 'P' ) || ( filebase[0x21] != 'R' ) ) {
		if ( ( filebase[0x20] == 'P' ) && ( filebase[0x21] == 'I' ) && ( filebase[0x22] == 'C' ) ) {
			Bdisp_AllClr_VRAM();
			if ( size>0 ) {
				RclPictOr( filebase );
				memcpy( PictAry[0], filebase+0x4C, size );	// Pict display
			}
			GetKey( &key );
			return 1;
		} else
		if ( ( filebase[0x20] == 'C' ) && ( filebase[0x21] == 'A' ) && ( filebase[0x22] == 'P' ) ) {
			memcpy( PictAry[0], filebase+0x50, 1024 );	// Capt display
			GetKey( &key );
			return 1;
		} else {
			ErrorMSG( "Not support file", fsize );
			return 1; // error
		}
	}
	return 0; // ok
}
int LoadProgfile( char *fname, int prgNo, int editsize, int disperror ) {
	char *filebase;
	int fsize,edsize;
	char sname[32],basname[32];
	int textsize;
	int maxsize;
	char CurrentFileMode;
	char ext[8];

	SetShortName( sname, fname);
	SetShortName( basname, fname);
	GetExtName( basname, ext );
//	renameExistedFile( basname, ext );	// replace? filename
//	GetExtName( basname, ext );
	SetFullfilenameExt( fname, basname, ext );

	edsize = editsize;
	if ( StorageMode & 2 ) {	// MCS mode
			filebase = MCS_LoadG1M( fname , &edsize, disperror, &fsize );	// hidden ram
			goto g1mload;
	} else
	if ( ( textmode ) || ( strcmp( ext, "csv") == 0 ) ) {	// text edit mode
			strncpy( textmodeExt, ext, 4);
			textmode = 1;
			goto textload;
	} else
	if ( strcmp( ext, "txt") == 0 ) {	// text file
		  textload:
			edsize += EditMaxfree;
			filebase = loadFile( fname , &edsize, disperror, &fsize );	// hidden ram
			if ( filebase == NULL ) return 1;
			textsize=strlen(filebase);
			if ( editsize ) {
				ConvertToOpcode( filebase, basname, editsize + EditMaxfree);		// text file -> G1M file
				strncpy( filebase+0x3C-8, folder, 8);		// set folder to G1M header
			}
			CurrentFileMode=1;		// text load save mode
	} else {	// G1M file
			filebase = loadFile( fname , &edsize, disperror, &fsize );	// hidden ram
		  g1mload:
			if ( filebase == NULL ) { CB_Error(FileNotFound); return 1; }
			if ( CheckG1M( filebase ) ) { HiddenRAM_freeProg( filebase ); return 1; } // not support g1m
			strncpy( filebase+0x3C-8, folder, 8);		// set folder to G1M header
			CurrentFileMode=0;		// g1m load save mode
	}

	ProgEntryN=prgNo;						// set program no
	ProgfileAdrs[prgNo]= filebase;
	ProgfileMax[prgNo]=  fsize + edsize;
	if ( ProgfileMax[prgNo] > EDITMAX ) ProgfileMax[prgNo] = EDITMAX;
	ProgfileEdit[prgNo]= 0;
	ProgfileMode[prgNo]= CurrentFileMode;
	g_current_prog=prgNo;
//	ExecPtr=0;
	if ( editsize ) return ( CB_PreProcessIndent( filebase, prgNo ) ) ;	// 0:ok
	return 0; // ok
}

int SaveG1Msub( char *filebase, int bin ){
	char fname[64],basname[32];
	int size,i;

	G1MHeaderTobasname8( filebase, basname);

	SetFullfilenameExt( fname, basname, "g1m" );

	if ( bin )	size = SrcSize(filebase)+0x56;
	else		size = FixSrcSize(filebase)+0x56;
	G1M_header( filebase, &size );		// G1M header set

	return storeFile( fname, (unsigned char*)filebase, size );	// 0:ok
}
int SaveG1Mbin( char *filebase ){
	return SaveG1Msub( filebase, 1 );	// 0:ok
}
int SaveG1M( char *filebase ){
	return SaveG1Msub( filebase, 0 );	// 0:ok
}

int SaveBasG1M( char *filebase ){
	G1M_Basic_header( filebase );	// G1M Basic header set
	if ( StorageMode & 2 ) return MCS_SaveG1M( filebase );	// g1m file -> MCS
	else return SaveG1M( filebase );
}
int SaveBasG1MorG3M( char *filebase, char *ext ){		// save g1m or g3m
	G1M_Basic_header( filebase );	// G1M Basic header set
	if ( ext[0]=='M' ) return MCS_SaveG1M( filebase );	// g3m file -> MCS
	else return SaveG1M( filebase ) ;
}

void SetExt( char *ext ) {
	if ( StorageMode & 2 ) strcpy( ext, "MCS" ); else strcpy( ext, "g1m" );
}

int SaveProgfile( int progNo ){
	char *filebase;
	char fname[64],fname2[64],sname[32],basname[32],basname2[32];
	int size,i,r=1;
	char *text;
	int textptr;
	int textsize;
	int buffersize;
	char CurrentFileMode=ProgfileMode[progNo];
	char ext[8],ext2[8];
	char buffer[32];

	filebase=ProgfileAdrs[progNo];
	CB_PostProcessIndentRemove( filebase );
	G1MHeaderTobasname8( filebase, basname);

	if ( textmode ) {
		sprintf( buffer, "Save %s Name?", textmodeExt );
		if ( InputFilename( basname, buffer ) ) return 1 ;
		SetFullfilenameExt( fname, basname, textmodeExt );
		strcpy( fname2, fname) ;
		text = filebase +0x56;
		textsize = SrcSize(filebase) -0x56 ;
		goto savetext;
	} else
	if ( ( CurrentFileMode == 0 ) || ( ForceG1Msave == 1 ) ) {	// g1m save
	  loop:
	  	SetExt( ext );	// set ext ( g1m,MCS )
	  	SetExt( ext2 );	// set ext ( g1m,MCS )
		if ( AutoSaveMode == 0 ) {
			if ( InputFilenameG1MorG3M( basname, "Save", ext ) ) if ( CurrentFileMode == 1 ) goto loop2; else return 1 ;
			strcpy( basname2, basname) ;
			if ( ext[0]=='M' ) ChangeStorageMode( StorageMode | 2); else ChangeStorageMode( StorageMode & 0xFD);
			if ( ExistG1Mext( basname2, ext ) ==0 ) if ( YesNoOverwrite() ) goto loop;
		}
		basname8ToG1MHeader( filebase, basname);
		sprintf(sname, "%s.g1m", basname );
		r=SaveBasG1MorG3M( filebase, ext );	// S.mem / MCS
	  	FileListUpdate = 1;
	} else
	if ( CurrentFileMode == 1 ) {	// text save
	  loop2:
		if ( AutoSaveMode == 0 ) {
			if ( InputFilename( basname, "Save Text Name?" ) ) return 1 ;
		}
		SetFullfilenameExt( fname, basname, "txt" );
		strcpy( fname2, fname) ;

		if ( AutoSaveMode == 0 ) {
			if ( ExistFile( fname2, 0 ) ==0 ) if ( YesNoOverwrite() ) goto loop2;
		}
		size = ProgfileMax[progNo] ;
		buffersize = size-SrcSize( filebase );	// buffersize (free space)
		memcpy( filebase+buffersize, filebase, SrcSize( filebase ) );
		text = filebase;
		textsize=OpcodeToText( filebase+buffersize+0x56, text, size );
		if ( textsize<0 ) { CB_ErrMsg(MemoryERR); return 1 ; } // error
	  savetext:
		storeFile( fname, (unsigned char*)text, textsize );
		SetShortName( sname, fname);
		r=0;
	}

	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);

	Check_Favorite_size( folder, sname, sname, SrcSize(filebase) ) ;	// favorite filesize update?

	return r;
}

int NewProg(){
	char *filebase;
	char sname[32],fname[64],basname[32];
	int size,i;

	size=NewMax;
	filebase = ( char *)HiddenRAM_mallocProg( size*sizeof(char)+4 );
	if( filebase == NULL ) {
		CB_ErrMsg(NotEnoughMemory);
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
		LoadProgfile( fname, 0, EditMaxfree, 1 );
		return 0;
	}
	basname8ToG1MHeader( filebase, basname);

	ProgEntryN=0;						// new Main program
	ProgfileAdrs[0]= filebase;
	ProgfileMax[0]= SrcSize( filebase ) + NewMax ;
	if ( ProgfileMax[0] > EDITMAX ) ProgfileMax[0] = EDITMAX;
	ProgfileEdit[0]= 1;
	ProgfileMode[0]= 0;	// g1m default
	g_current_prog=0;
	g_exec_ptr=0;
	strncpy( filebase+0x3C-8, folder, 8);		// set folder to header

	return 0;	// ok
}

//----------------------------------------------------------------------------------------------
void SetPICTname( int pictNo, char *pictname ){
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
	pictname[7]='\0';
}
void SetCAPTname( int pictNo, char *pictname ){
	SetPICTname( pictNo, pictname );
	pictname[0]='C';
	pictname[1]='A';
	pictname[2]='P';
}

int SavePicture( char *filebase, int pictNo ){
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	char folder2[FILENAMEMAX];
	int r,s;
	char fname[64],basname[32];
	char pictname[32];
	int size,i;
 	unsigned char c,d;
 	c=pictNo/10+'0'; if ( c=='0' ) c=' ';
 	d=pictNo%10+'0';

	SetPICTname( pictNo, pictname );
	if ( PictMode == 3 )  return  MCS_SavePICT( pictname, filebase );

	for (i=0; i<0x4C; i++) filebase[i]=0x00;	// header clear

//	size=0x800+0x4C;	// 2KB Pict
	size=0x400+0x4C;	// 1KB Pict
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
	folder[0x01]='I';
	folder[0x02]='C';
	folder[0x03]='T';
	folder[0x04]='\0';

	r = CreateDirectorySub( folder, 0 );
	if ( r < 0 ) return r;

	r = SaveG1Mbin( filebase );
	for (i=0;i<FILENAMEMAX;i++) folder[i]=folder2[i];	// restore folder

	return r ;
}

int SaveCapture( char *filebase, int pictNo ){
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FILE_INFO info;
	char folder2[FILENAMEMAX];
	int r,s;
	char fname[64],basname[32];
	int size,i;
 	unsigned char c,d;
 	c=pictNo/10+'0'; if ( c=='0' ) c=' ';
 	d=pictNo%10+'0';

	SetPICTname( pictNo, basname );
	return MCS_SaveCAPT( basname, filebase );
}

char * Load1st2nd( char *basname, char *dir2nd, char *ext ){
	char *fileptr;
	char fname[32];
	int edsize=0;
	int fsize;

	SetFullfilenameExt( fname, basname, ext );

	fileptr = loadFile( fname, &edsize, 0, &fsize );					// no hidden load
	HiddenRAM_freeProg( fileptr );
	if ( fileptr  != NULL ) return fileptr;

	g_error_type=0;	// error cancel
	SetFullfilenameExtFolder( fname, dir2nd, basname, ext ) ;	// folder 2nd
	edsize = 0;
	fileptr = loadFile( fname, &edsize, 0, &fsize );					// no hidden load
	HiddenRAM_freeProg( fileptr );
	return fileptr ;
}

char * LoadPicture( int pictNo ){
	char pictname[32];
	char *pict;
	SetPICTname( pictNo, pictname );
	return Load1st2nd( pictname, "PICT", "g1m" );
}
char * LoadPictureMCS( int pictNo, int *length ){
	char pictname[32];
	char *pict;
	SetPICTname( pictNo, pictname );
	return pict = MCS_LoadPICT( pictname, &(*length) );
}
char * LoadCapture( int pictNo ){
	char pictname[32];
	char *pict;
	SetCAPTname( pictNo, pictname );
	if ( PictMode == 3 ) pict = MCS_LoadCAPT( pictname );
	else				 pict = Load1st2nd( pictname, "CAPT", "g1m" );
	return pict;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void DeleteFile(char *fname) {
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int r;

	if ( StorageMode & 2 ) {
		r = MCS_DeleteG1M( fname  );
		if( r ) { ErrorMSGfile( "Can't delete MCS", fname, r ); return ; }
	} else {
		CharToFont( fname, filename );
		r = deleteFile( filename, 0 ) ;
	}
	FileListUpdate=1;
}

void DeleteFileFav(char *fname, int yesno ) {
	char sname[32];
	int i;

	SetShortName( sname, fname);		//
	if ( yesno ) if ( YesNo2( sname,"Delete file?" ) == 0 ) return ;
	DeleteFile( fname );

	i=Check_Favorite( folder, sname);	// file matching search
	if ( i < FavoritesMAX ) { 		//	exist favorites list
		DeleteFavorites( i ) ;
	}

}


void DeleteDirectory(char *fname) {
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int r;

//	r=strlen(fname)-1;
//	if ( fname[r] == '\\' ) fname[r]='\0';
	CharToFont( fname, filename );

	r = Bfile_DeleteDirectory( filename );
	if( r < 0 ) { ErrorMSGfile( "Can't delete fldr", fname, r ); return ; }
	FileListUpdate=1;
}

int DeleteFolder(char *foldername, int yesno ) {	// delete folder
	char fname[32];
	int i;

	if ( yesno ) if ( YesNo2( foldername,"Delete folder?" ) == 0 ) return 1 ; // cancel
	SetFullfilenameNoExtNoFolder( fname, foldername ) ;
	DeleteDirectory( fname );
	folder[0]='\0';
	return 0;	// ok
}

//----------------------------------------------------------------------------------------------
int CreateDirectorySub( char *newfolder, int dsperror ){
	int r,s,handle;
	FILE_INFO info;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	char fname[64];

	SetFullfilenameNoExtNoFolder( fname, newfolder ) ;
	CharToFont( fname, filename );
	r = Bfile_FindFirst( filename, &handle, buffer, &info );
	s = Bfile_FindClose( handle );
	if( r != 0 ) {
		handle = Bfile_CreateDirectory( filename );
		if( handle < 0 ) {
			if ( dsperror ) ErrorMSGfile( "Can't make folder", newfolder, r );
			return handle ;
		}
		r = Bfile_CloseFile( handle );
		r = 0;
	}
	return r;
}

int MakeDirectory(){
	char name[32];
	int	r;
	if( strlen(folder) != 0 ) return 1; // The directory can be created under only the route directory.
	Bdisp_AllClr_VRAM();
	name[0]='\0';
	if ( InputPassname( 1, name, "Folder Name" ) ) return 1; //cancel
	r = CreateDirectorySub( name, 1 );
	FileListUpdate = 1 ;
	index=0;
	strcpy( folder, name );
	if ( r != 0  ) return 1;
	return 0;	// ok
}

//----------------------------------------------------------------------------------------------
const char RenameCopy_msg[2][18]={"Rename File Name?" ,"Copy File Name?" };

int RenameCopyFile( char *fname ,int select ) {	// select:0 rename  select:1 copy
	char *filebase;
	char name[64],name2[64],sname[32],oldsname[32],basname[32];
	int size,i,j;
	int bmp=0;
	char ext[8],ext2[8];
	char *str;

	SetShortName( oldsname, fname);
	SetShortName( basname, fname);
	GetExtName( basname, ext );
	if ( strcmp( ext, "g1m") == 0 ) {	// g1m file
	// G1M file
		if ( LoadProgfile( fname, 0, 0, 1 ) ) return 1 ; // error
		filebase = ProgfileAdrs[0];
	  	SetExt( ext  );	// set ext ( g1m,MCS )
	  	SetExt( ext2 );	// set ext ( g1m,MCS )
		G1MHeaderTobasname8( filebase, basname);
		if ( CheckG1M( filebase ) ) return 1; // not support g1m
		if ( select == 0 ) {
			if ( InputFilename( basname, RenameCopy_msg[select] ) ) return 1 ; // cancel
		} else {
			if ( InputFilenameG1MorG3M( basname, "Copy", ext ) ) return 1 ; // cancel
		}
		if ( ext[0]=='M' ) {	// MCS
			ChangeStorageMode( StorageMode | 2 );
		} else ChangeStorageMode( StorageMode & 0xFD );
		SetFullfilenameExt( name, basname, ext );
		if ( ( strcmp( name, fname)==0 ) && ( strcmp(ext,ext2)==0 ) ) return 0; // no rename
		basname8ToG1MHeader( filebase, basname);
		if ( ExistG1Mext( basname, ext ) ==0 ) {
			FkeyClearAll();
			if ( YesNoOverwritefile(name) ) return 1 ; // cancel
			DeleteFile( fname ) ;	// (rename) delete original file
		} else {
			if ( select==0 ) DeleteFile( fname ) ;	// (rename) delete original file
		}
		if ( SaveBasG1MorG3M( filebase, ext ) == 0 ) {
		} else return 1;
	  	FileListUpdate = 1;
		if ( ext[0]=='M' ) SetFullfilenameExt( name, basname, "g1m" );	// MCS
	} else {
		filebase=CB_LoadSub( basname, 0, &size, ext ) ;
		if ( filebase ==NULL ) return 1 ; // error
		if ( InputFilename( basname, RenameCopy_msg[select] ) ) return 1 ; // cancel
		GetExtName( basname, ext );
		SetFullfilenameExt( name, basname, ext );
		if ( strcmp(name,fname)==0 ) return 0; // no copy
		strcpy( name2, name );
		if ( ExistFile( name2, 1 ) == 0 ) {
			FkeyClearAll();
			if ( YesNoOverwritefile(name) ) return 1 ; // cancel
			DeleteFile( name2 ) ;	// (rename) delete original file
		} else {
			if ( select==0 ) DeleteFile( fname ) ;	// (rename) delete original file
		}
		FileListUpdate = 1 ;
		if ( storeFile( name, (unsigned char*)filebase, size ) ) return 1;	// 1:error  0:ok
	}

//	for (j=0;j<FILENAMEMAX;j++) sname[j]='\0';
	SetShortName( sname, name);		// rename name
	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);

	if ( select == 0 ) Check_Favorite_size( folder, oldsname, sname, 0 ) ;	// favorite rename?

	return 0;
}

int RenameDirectorys( char * foldername ){
	char newname[32];
	char oldfolder[32];
	char newfolder[32];
	FONTCHARACTER buffer[FONTCHARACTER_MAX];
	FONTCHARACTER buffer2[FONTCHARACTER_MAX];
	char sname[32];
	int	r;
	newname[0]='\0';
	strcpy( newname, foldername );
	if ( InputFilename( newname, "Rename Folder ?" ) ) return 1 ; // cancel
	newname[8]='\0';
	SetFullfilenameNoExtNoFolder( oldfolder,  foldername );
	SetFullfilenameNoExtNoFolder( newfolder,  newname    );
	CharToFont( oldfolder, buffer );
	CharToFont( newfolder, buffer2 );
	r=Bfile_RenameEntry( buffer, buffer2 );
	if( r < 0 ) { ErrorMSGfile( "Can't rename folder", newname, r ); return 1 ; }
	FileListUpdate = 1 ;
//	SetShortName( sname, newfolder);		// rename name
//	strncpy( renamename, sname, FILENAMEMAX);
//	strncpy( renamefolder, folder, FOLDERMAX);
	return 0;	// ok
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void Setfoldername16( char *folder16, char *sname ) {
	char *cptr;
	int i=0,j,def=1;
	if ( ( sname[i]== '\\' ) || ( sname[i]== '/' ) ) { do sname[i]=sname[++i]; while ( sname[i] ); def=0; }
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

void SetFullfilenameBin( char *fname, char *sname, char *ext ) {
	char sname2[32];
	char ext2[8];
	char *cptr;
	int i;
	strncpy( sname2, sname, 31 ); sname2[31]='\0';
	GetExtName( sname2, ext2 );
	if ( ext2[0]=='\0' ) strncpy( ext2, ext, 3 ); ext2[3]='\0';
	SetFullfilenameExt( fname, sname2, ext2 );
}

int CB_FileList( char *ext ) {
	int i,n;
	int reg;
	int dimA,dimB,base=1,element=8;
	char fname[33];
	Files file2[256];
	Files *filesbk=files;
	files=file2;
	n=ReadFile2( folder, ext, 255 );
	if ( n<1 ) { files=filesbk; return 0; }	// no file
	dimA=n;
	dimB=32;
	NewMatListAns( dimA, dimB, base, element );
	memset( fname, 0, 32 );
	if ( g_error_type ) return 0;	// error
	for ( i=0; i<n; i++ ) {
		if( strlen(folder) == 0 )
			sprintf( fname, "%s", files[i].filename );
		else
			sprintf( fname, "/%s/%s", folder, files[i].filename );
		strncpy( MatrixPtr( CB_MatListAnsreg, i+1, 1 ), fname, 32 );
	}
	dspflag=4;	// List ans
	files=filesbk;
	return n;
}
int CB_IsExist( char *SRC , int calcflag ) {	//	IsExist("TEST")		//  no exist: return 0     exist: return filesize
								//	IsExist("*.bmp") -> MatAns
	char fname[32],sname[32];
	int c,i,matsize;
	char* FilePtr;
	int r;
	char ext[8];
	char *cptr;

	CB_GetLocateStr(SRC, sname,22); if ( g_error_type ) return 0;	// error
	if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
	Getfolder( sname );
	if ( ( sname[0]=='*' ) && ( sname[1]=='.' ) ) {
		ext[0]=sname[2];
		ext[1]=sname[3];
		ext[2]=sname[4];
		ext[3]='\0';
//		if ( calcflag ) {
		r=CB_FileList( ext );
		if ( r>0 ) dspflag=4;	// List ans
//		} else {
//			r=1;
//			dspflag=4;	// List ans
//		}
	} else {
		SetFullfilenameBin( fname, sname, "bin" );
		r=GetFileSize( fname );
	}
	Restorefolder();
	return r;
}

char * CB_SaveLoadOprand( char *SRC , int *reg, int *matsize ) {
	int dimA,dimB,base;
	char *ptr;
	MatrixOprand( SRC, &(*reg), &dimA, &dimB );
	ptr = (char *)MatrixPtr( (*reg), dimA, dimB );
	base = 1-MatAry[(*reg)].Base;
	*matsize = MatrixSize( (*reg), MatAry[(*reg)].SizeA, MatAry[(*reg)].SizeB ) - MatrixSize( (*reg), dimA+base, dimB+base ) + MatrixSize( (*reg), 1, 1 );
	return ptr;
}

void CB_SaveSub( char *sname, char* FilePtr, int size, int check, char* extname ) {
	char sname2[32];
	char fname[32];
	char fname2[32];
	int storageMode = StorageMode;

	StorageMode &= 1 ;	// Forth Strage / SD Mode
	Getfolder( sname );
	SetFullfilenameBin( fname, sname, extname );
	strcpy( fname2, fname );
	if ( ExistFile( fname2, 1 ) == 0 ) { // ==0 existed
		if ( check ) {
			if ( YesNoOverwritefile(fname2) ) goto exit; // cancel
			Bdisp_PutDisp_DD();
		}
		DeleteFileFav( fname2 , 0 );	// delete file
	}
	if ( storeFile( fname, (unsigned char*)FilePtr, size )!=0 ) CB_Error(FileERR);
	SetShortName( sname2, fname);	//
	Check_Favorite_size( folder, sname2, sname2, size ) ;	// favorite filesize update?
	exit:
	Restorefolder();
	StorageMode = storageMode;
}
void CB_Save( char *SRC ) { //	Save "TEST",Mat A[1,3] [,Q] etc
	char fname[64],sname[32];
	int c,i,reg,matsize;
	char* FilePtr;
	int check=0;

//	c =SRC[ExecPtr];
//	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	CB_GetLocateStr(SRC, sname,22); if ( g_error_type ) return ;	// error
	c =SRC[g_exec_ptr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
//	FilePtr = CB_SaveLoadOprand( SRC, &reg, &matsize);
	FilePtr = (char *)VarPtrLength( SRC, &matsize, &c, 0);
	if ( c==SERIAL_STRING ) matsize--;
	if ( g_error_type ) return; // error

	c =SRC[g_exec_ptr];
	if ( c == ',' ) {
		c =SRC[++g_exec_ptr];
		if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
		g_exec_ptr++;
	}

	CB_SaveSub( sname, FilePtr, matsize, check, "bin" );
}

char * CB_LoadSub( char *sname, int ptr, int *size, char* extname ) {	// load to new buffer
	char fname[32];
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	char *buffer;
	char ext[8];

	Getfolder( sname );
	GetExtName( sname, ext );
	if ( ext[0] == '\0' ) strcpy( ext, extname );
	renameExistedFile( sname, ext );	// replace? filename
	SetFullfilenameNoExt( fname, sname );
	CharToFont( fname, filename );

	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) {
		CB_Error(FileNotFound);  goto exit;
	}

	(*size) = Bfile_GetFileSize( handle ) -(ptr) ;

	buffer = ( char *)HiddenRAM_mallocProg( (*size)*sizeof(char)+4 );
	if( buffer == NULL ) { CB_Error(NotEnoughMemory); goto exit; }	// Not enough memory error
	memset( buffer, 0x00,     (*size)*sizeof(char)+4 );

	Bfile_ReadFile( handle, buffer, (*size), ptr );
	Bfile_CloseFile( handle );
	if ( buffer == NULL ) { CB_Error(FileERR); }
	exit:
	Restorefolder();
	return buffer;
}
void CB_LoadSubBuffer( char *buffer, char *sname, int ptr, int size, char* extname, int type ) {	// load to buffer
	char fname[32];
	int handle;
	FONTCHARACTER filename[FONTCHARACTER_MAX];
	int fsize;
	char ext[8];

	Getfolder( sname );
	GetExtName( sname, ext );
	if ( ext[0] == '\0' ) strcpy( ext, extname );
	renameExistedFile( sname, ext );	// replace? filename
	SetFullfilenameNoExt( fname, sname );
	CharToFont( fname, filename );

	handle = Bfile_OpenFile( filename, _OPENMODE_READ_SHARE );
	if( handle < 0 ) {
		CB_Error(FileNotFound);  goto exit;
	}
	fsize = Bfile_GetFileSize( handle ) -(ptr) ;
	if ( fsize > size ) fsize=size;
	if ( type == SERIAL_STRING ) memset( buffer, 0, size);
	Bfile_ReadFile( handle, buffer, fsize, ptr );
	Bfile_CloseFile( handle );
	if ( buffer == NULL ) { CB_Error(FileERR); }
	exit:
	Restorefolder();
}
void CB_Load( char *SRC ) { //	Load ("TEST" [, Ptr])->Mat A[1,3]
	char fname[64],sname[32];
	int c,i,reg,matsize;
	char* FilePtr;
	char* matptr;
	int ptr=0,size;
	int type;

//	c =SRC[ExecPtr];
//	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
	CB_GetLocateStr(SRC, sname,22); if ( g_error_type ) return ;	// error
	c =SRC[g_exec_ptr];
	if ( c == ',' ) {
		g_exec_ptr++;
		ptr=CB_EvalInt( SRC );
		if ( ptr < 0 ) { CB_Error(RangeERR); return; }	// Range error
	}
	c =SRC[g_exec_ptr];
	if ( c == ')' ) g_exec_ptr++;
	c =SRC[g_exec_ptr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
//	matptr = CB_SaveLoadOprand( SRC, &reg, &matsize );
	matptr = (char *)VarPtrLength( SRC, &matsize, &type, 1);
	if ( g_error_type ) return ; // error

	CB_LoadSubBuffer( matptr, sname, ptr, matsize, "bin", type ) ;
	if ( g_error_type ) return ; // error
}

void CB_Delete( char *SRC ) {	// Delete "ABC.bin"[,1]
	char fname[32],sname[32];
	char ext[8];
	int c;
	int yesno=0;

	CB_GetLocateStr(SRC, sname, 22); if ( g_error_type ) return ;	// error
	c =SRC[g_exec_ptr];
	if ( c == ',' ) {
		g_exec_ptr++;
		yesno = CB_EvalInt( SRC );
	}
	if ( g_error_type ) return ; // error

	Getfolder( sname );
	GetExtName( sname, ext );
	if ( ext[0] == '\0' ) strcpy( ext, "bin" );
	renameExistedFile( sname, ext );	// replace? filename
	SetFullfilenameNoExt( fname, sname );
	DeleteFileFav( fname , yesno );	// with yesno
	Restorefolder();
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void ConvertToText( char *fname ){
	char *filebase;
	char sname[32],basname[32];
	int size,i,j;
	char *text;
	int textptr;
	int textsize;
	int buffersize;
	char ext[8],ext2[8];

	SetShortName( sname, fname);
	GetExtName( sname, ext );
	if ( strcmp( ext, "txt") == 0 ) {	// text file -> G1M
	  	SetExt( ext );	// set ext ( g1m,g3m,MCS )
		if ( LoadProgfile( fname, 0, EditMaxfree, 1 ) ) return ; // error
		filebase = ProgfileAdrs[0];
		G1MHeaderTobasname8( filebase, basname);
		if ( InputFilenameG1MorG3M( basname, "Convert", ext ) ) return ; // cancel
		if ( ext[0]=='M' ) ChangeStorageMode( StorageMode |  2); else ChangeStorageMode( StorageMode & 0xFD );
		SetFullfilenameExt( fname, basname, (char*)ext );
		basname8ToG1MHeader( filebase, basname);
		if ( ExistG1Mext( basname, ext ) ==0 ) if ( YesNoOverwritefile(fname) ) return ; // cancel
		if ( SaveBasG1MorG3M( filebase, ext ) ) return ;
	  	FileListUpdate = 1;
		if ( ext[0]=='M' ) SetFullfilenameExt( fname, basname, "g1m" );

	} else {	// G1M file -> Text
		MSG1("Wait a moment...");
		buffersize=files[index].filesize /1;	// buffersize 100% up
		if ( buffersize < EditMaxfree ) buffersize=EditMaxfree;
		if ( LoadProgfile( fname, 0, buffersize, 1 ) ) return ; // error
		filebase = ProgfileAdrs[0];
		G1MHeaderTobasname8( filebase, basname);
		if ( CheckPassWord( filebase ) ) return ;	// password error

		size = SrcSize( filebase ) + buffersize ;
		memcpy( filebase+buffersize, filebase, SrcSize( filebase ) );
		text = filebase;
		textsize=OpcodeToText( filebase+buffersize+0x56, text, size );
		if ( textsize<0 ) { CB_ErrMsg(MemoryERR); return ; } // error

		if ( StorageMode == 2 ) ChangeStorageMode( StorageMode & 0xFD );	// MCS->text
		SetFullfilenameExt( fname, basname, "txt" );
		if ( ExistFile( fname, 0 ) == 0 ) { // ==0 existed
			if ( YesNoOverwritefile(fname) ) { return ; } // cancel
			Bdisp_PutDisp_DD();
		}
		storeFile( fname, (unsigned char*)text, textsize );
		FileListUpdate = 1;
	}

	ErrorMSGfile( "Convert Complete!", fname, 0);
	SetShortName( sname, fname);
	strncpy( renamename, sname, FILENAMEMAX);
	strncpy( renamefolder, folder, FOLDERMAX);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
#define ConfigMAX  1080
#define ConfigMAX2  512
#define ConfigMAX3  588+32
//--------------------------------------------------------------

void CopyFilesToFavorites(){
	int i;
	for( i=0; i<FavoritesMAX; i++){			//	backup Favorites list
		memset(  Favoritesfiles[i].filename, 0x00, sizeof(Files) );
		if ( files[i].filesize != 0 ) {
			strncpy( Favoritesfiles[i].filename, files[i].filename, FILENAMEMAX );
			strncpy( Favoritesfiles[i].folder,   files[i].folder,   FOLDERMAX );
			Favoritesfiles[i].filesize = files[i].filesize;
		}
	}
}

void SaveFavorites(){
	int i;
	if ( MaxMemMode==0 ) CopyFilesToFavorites();
	SaveConfig();
}

void CopyFavoritesToFiles(){
	int i;
	if ( MaxMemMode==0 ){
		for( i=0; i<FavoritesMAX; i++){			//	restore Favorites list
			memset(  files[i].filename, 0x00, sizeof(Files) );
			if ( Favoritesfiles[i].filesize != 0 ) {
				strncpy( files[i].filename, Favoritesfiles[i].filename, FILENAMEMAX );
				strncpy( files[i].folder,   Favoritesfiles[i].folder,   FOLDERMAX );
				files[i].filesize = Favoritesfiles[i].filesize;
			}
		}
	}
}

int SaveConfigWriteFile( unsigned char *buffer, const unsigned char *fname, int size ) {
	int state,handle;
	handle=Bfile_OpenMainMemory(fname);
	if (handle >= 0) {		// Already Exists
		Bfile_CloseFile(handle);
	}
	if (handle==IML_FILEERR_ENTRYNOTFOUND) {
		handle=Bfile_CreateMainMemory(fname);
		if (handle<0) {ErrorMSG("Create Error",handle); return handle; }
		state=Bfile_CloseFile(handle);
		if (state<0)  {ErrorMSG("Close Error",state); return handle; }
	}
	if (handle<0) {ErrorMSG("Open Error",handle); return handle; }

	handle=Bfile_OpenMainMemory(fname);
	if (handle<0) {ErrorMSG("Open Error",handle); return handle; }
	state=Bfile_WriteFile(handle,buffer,size);
	if (state<0)  {ErrorMSG("Write Error",state); return state; }
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return state; }
	return state;
}
int LoadConfigReadFile( unsigned char *buffer, const unsigned char *fname, int size ) {
	int state,handle;
	handle=Bfile_OpenMainMemory(fname);
	if (handle<0)  { // Open Error
		return -1;
	}
	state=Bfile_ReadFile(handle, buffer, size, 0);
	if (state<0)  {ErrorMSG("Read Error",state); return -1;}
	state=Bfile_CloseFile(handle);
	if (state<0)  {ErrorMSG("Close Error",state); return -1;}
	return 0;
}

//------------------------------------------------------------------------ save to main memory
void SaveConfig1(){
	const unsigned char fname[]="CBasic1";
	unsigned char buffer[ConfigMAX];
	unsigned char *sbuf;
	short  *bufshort=(short*)buffer;
	int    *bufint =(int*)buffer;
	double *bufdbl =(double*)buffer;
	int size,i,r;

	memset( buffer, 0x00, ConfigMAX );
	strcpy( (char *)buffer, (const char *)"CB.config99n" );

/*
typedef struct {
	char  reserve01;		char  UseHiddenRAM;
	char  reserve02;		char  CB_INTDefault;
	short RefreshCtrl;
	char  reserve03;		char  DrawType;
	short Refreshtime;
	char  reserve04;		char  Coord;
	char  reserve05;		char  ENG;
	char  reserve06;		char  Grid;
	char  reserve07;		char  RoundMODE;
	char  reserve08;		char  Axes;
	char  reserve09;		char  RoundDIGIT;
	char  reserve10;		char  Label;
	short DefaultWaitcount;
	char  reserve11;		char  Derivative;
	char  reserve12;		char  CommandInputMethod;
	char  reserve13;		char  S_L_Style;
	char  reserve14;		char  ForceG1Msave;
	char  reserve15;		char  Angle;
	char  reserve16;		char  StorageMode;
	char  reserve17;		char  BreakCheckDefault;
	short PageUpDownNum;
	char  reserve18;		char  TimeDsp;
	char  reserve19;		char  MatBaseDefault;
	char  reserve20;		char  MatXYmode;
	char  reserve21;		char  CheckIfEnd;
	char  reserve22;		char  PictMode;
} setupdata;

	buffer[ 12+1]=UseHiddenRAM;			buffer[ 14+1]=CB_INTDefault;
	bufshort[ 16/2]=RefreshCtrl;		buffer[ 18+1]=DrawType;
	bufshort[ 20/2]=Refreshtime;		buffer[ 22+1]=Coord;
	buffer[ 24+1]=ENG;					buffer[ 26+1]=Grid;
	buffer[ 28+1]=CB_Round.MODE;		buffer[ 30+1]=Axes;
	buffer[ 32+1]=CB_Round.DIGIT-1;		buffer[ 34+1]=Label;
	bufshort[ 36/2]=DefaultWaitcount;	buffer[ 38+1]=Derivative;
	buffer[ 40+1]=CommandInputMethod;	buffer[ 42+1]=S_L_Style;
	buffer[ 44+1]=ForceG1Msave;			buffer[ 46+1]=Angle;
	buffer[ 48+1]=StorageMode;			buffer[ 50+1]=BreakCheckDefault;
	bufshort[ 52/2]=PageUpDownNum;		buffer[ 54+1]=TimeDsp;
	buffer[ 56+1]=1-MatBaseDefault;		buffer[ 58+1]=MatXYmode;
	buffer[ 60+1]=CheckIfEnd;			buffer[ 62+1]=PictMode;

	bufshort[ 7]=CB_INTDefault;		bufshort[ 6]=UseHiddenRAM;
	bufshort[ 9]=DrawType;			bufshort[ 8]=RefreshCtrl;
	bufshort[11]=Coord;				bufshort[10]=Refreshtime;
	bufshort[13]=Grid;				bufshort[12]=ENG;
	bufshort[15]=Axes;				bufshort[14]=CB_Round.MODE;
	bufshort[17]=Label;				bufshort[16]=CB_Round.DIGIT-1;
	bufshort[19]=Derivative;		bufshort[18]=DefaultWaitcount;
	bufshort[21]=S_L_Style;			bufshort[20]=CommandInputMethod;
	bufshort[23]=Angle;				bufshort[22]=ForceG1Msave;
	bufshort[25]=BreakCheckDefault;		bufshort[24]=StorageMode;
	bufshort[27]=TimeDsp;			bufshort[26]=PageUpDownNum;
	bufshort[29]=MatXYmode;			bufshort[28]=1-MatBaseDefault;
	bufshort[31]=PictMode;			bufshort[30]=CheckIfEnd;
*/

	buffer[ 12] =ExtendPict;		buffer[12+1]=UseHiddenRAM;		buffer[ 14] =ExtendList;		buffer[14+1]=CB_INTDefault;
	bufshort[ 8]=RefreshCtrl;										buffer[ 18] =EditListChar;		buffer[18+1]=DrawType;
	buffer[ 22] =ForceReturnMode;	buffer[22+1]=Coord;				buffer[ 20]=CB_fx5800P;			buffer[20+1]=Refreshtime;
	buffer[ 24] =CB_RecoverSetup;	buffer[24+1]=ENG;				buffer[ 26] =EditExtFont;		buffer[26+1]=Grid;
	bufshort[15]=Axes;												bufshort[14]=CB_Round.MODE;
	bufshort[17]=Label;												buffer[ 32] =MaxMemMode;		buffer[32+1]=CB_Round.DIGIT-1;
	buffer[ 38] = 0;				buffer[38+1]=Derivative;		bufshort[18]=DefaultWaitcount;
	buffer[ 42] =ExitDebugModeCheck;buffer[42+1]=S_L_Style;			bufshort[20]=CommandInputMethod;
	buffer[ 46] =ComplexMode;		buffer[46+1]=Angle;				buffer[ 44]=EnableExtFont;		buffer[44+1]=ForceG1Msave;
	bufshort[25]=BreakCheckDefault;									bufshort[24]=StorageMode;
	buffer[ 54] =CB_EditIndent;		buffer[54+1]=TimeDsp;			bufshort[26]=PageUpDownNum;
	bufshort[29]=MatXYmode;											bufshort[28]=1-MatBaseDefault;
	bufshort[31]=PictMode;											buffer[ 60]=CB_HelpOn;			buffer[60+1]=CheckIfEnd;

	bufdbl[ 8]=Xfct;
	bufdbl[ 9]=Yfct;
	for ( i= 10; i<  10+58 ; i++ ) bufdbl[i]=REG[i-10].real;
	for ( i= 68; i<  68+11 ; i++ ) bufdbl[i]=REGv[i-68];
	for ( i=160; i< 160+58 ; i++ ) bufint[i]=REGINT[i-160];

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

	buffer[1075]= AutoSaveMode;
	buffer[1076]= EditTopLine;
	buffer[1077]= EditFontSize;
	buffer[1078]= DisableDebugMode;
	buffer[1079]= 0;

	SaveConfigWriteFile( buffer, fname, ConfigMAX ) ;
}

void SaveConfig2(){
	const unsigned char fname[]="CBasic2";
	unsigned char buffer[ConfigMAX2];
	unsigned char *sbuf;
	short  *bufshort=(short*)buffer;
	int    *bufint =(int*)buffer;
	double *bufdbl =(double*)buffer;
	int size,i,r;

	memset( buffer, 0x00, ConfigMAX2 );
	strcpy( (char *)buffer, (const char *)"CB.config180" );
	strncpy( (char *)buffer+16, root2, root2_MAX);	// default

	for ( i= 6; i<  6+58 ; i++ ) bufdbl[i]=REG[i-6].imag;

	SaveConfigWriteFile( buffer, fname, ConfigMAX2 ) ;
}


void InitConfig3(){
	const unsigned char fname[]="CBasic3";
	unsigned char buffer[ConfigMAX3];
	unsigned char *sbuf;
	short  *bufshort=(short*)buffer;
	int    *bufint =(int*)buffer;
	double *bufdbl =(double*)buffer;
	int size,i,r;

	memset( buffer, 0x00, ConfigMAX3 );
	SaveConfigWriteFile( buffer, fname, ConfigMAX3 ) ;
}

void SaveConfig(){
	SaveConfig1();
	SaveConfig2();
}

#define FavoritesSIZE	sizeof(Files)*FavoritesMAX

void ChangeFavorites( int oldStorageMode, int newStorageMode ){	// old <> new favorite
	unsigned char fname[]="CBasic3";
	unsigned char buffer[ConfigMAX3+FavoritesMAX*4*3];
	unsigned char *sptr,*dptr;
	int size,i,r;

	if  ( LoadConfigReadFile( buffer, fname, ConfigMAX3 ) < 0 ) {
		InitConfig3();
		if  ( LoadConfigReadFile( buffer, fname, ConfigMAX3 ) < 0 ) return ;
	}

	switch ( oldStorageMode ) {
		case 0:	// storage
			memcpy( buffer, folder, 9 );
			memcpy( buffer+32+FavoritesSIZE*0, Favoritesfiles, FavoritesSIZE );		// Favorites -> storage(0)
			switch ( newStorageMode ) {
				case 1:	// SD
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*1, FavoritesSIZE );	// SD(1)     -> Favorites
					memcpy( folder, buffer+10, 9 );
					break;
				case 2:	// MCS(storage)
				case 3:	// MCS(SD)
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*2, FavoritesSIZE );	// MCS(2)     -> Favorites
					memset( folder, 0, 9 );
					break;
			}
			break;
		case 1:	// SD
			memcpy( buffer+10, folder, 9 );
			memcpy( buffer+32+FavoritesSIZE*1, Favoritesfiles, FavoritesSIZE );		// Favorites -> SD(1)
			switch ( newStorageMode ) {
				case 0:	// storage
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*0, FavoritesSIZE );	// storage(0 -> Favorites
					memcpy( folder, buffer, 9 );
					break;
				case 2:	// MCS(storage)
				case 3:	// MCS(SD)
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*2, FavoritesSIZE );	// MCS(2)     -> Favorites
					memset( folder, 0, 9 );
					break;
			}
			break;
		case 2:	// MCS(storage)
		case 3:	// MCS(SD)
			memcpy( buffer+32+FavoritesSIZE*2, Favoritesfiles, FavoritesSIZE );		// Favorites -> MCS(2)
			switch ( newStorageMode ) {
				case 0:	// storage
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*0, FavoritesSIZE );	// storage(0 -> Favorites
					memcpy( folder, buffer, 9 );
					index = 8;
					if ( MaxMemMode==0 ) strcpy( files[index].folder, folder );
					break;
				case 1:	// SD
					memcpy( Favoritesfiles, buffer+32+FavoritesSIZE*1, FavoritesSIZE );	// SD(1)     -> Favorites
					memcpy( folder, buffer+10, 9 );
					index = 8;
					if ( MaxMemMode==0 ) strcpy( files[index].folder, folder );
					break;
			}
			break;

	}

	SaveConfigWriteFile( buffer, fname, ConfigMAX3 ) ;
	CopyFavoritesToFiles();
}

void ChangeStorageMode( int newMode ) {
	ChangeFavorites( StorageMode, newMode );	// old -> new favorites
	StorageMode = newMode;
	FileListUpdate = 1;
}

//------------------------------------------------------------------------ load from main memory
void LoadConfig1data( int n ) {	// config data ->List Ans
	unsigned char fname[8] ="CBasic1";
	unsigned char buffer[ConfigMAX];
	int fsize;
	switch ( n ) {
		case 1:
			fsize=ConfigMAX;
			break;
		case 2:
			fsize=ConfigMAX2;
			fname[6]='2';
			break;
		case 3:
			fsize=ConfigMAX3;
			fname[6]='3';
			break;
		default:
			return ;
	}
	if  ( LoadConfigReadFile( buffer, fname, fsize ) < 0 ) { CB_Error(FileNotFound); return ; }
	
	NewMatListAns( fsize, 1, 0, 8 );		// List Ans
	if ( g_error_type ) return ;
	memcpy( MatAry[CB_MatListAnsreg].Adrs,  buffer , fsize );
	dspflag=4; 	// List Ans;
}

void LoadConfig1(){
	const unsigned char fname[]="CBasic1";
	unsigned char buffer[ConfigMAX];
	unsigned char *sbuf;
	short  *bufshort;
	int    *bufint;
	double *bufdbl;
	int size,i;
	int handle,state;

	if  ( LoadConfigReadFile( buffer, fname, ConfigMAX ) < 0 ) return;
	
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
		ExtendPict    =buffer[ 12];			UseHiddenRAM  =buffer[12+1];	ExtendList     =buffer[ 14];		CB_INTDefault=buffer[14+1];
		RefreshCtrl   =bufshort[8];											EditListChar   =buffer[ 18];		DrawType     =buffer[18+1];
		ForceReturnMode=buffer[22];			Coord         =buffer[22+1];	CB_fx5800P     =buffer[ 20];		Refreshtime   =buffer[20+1];
		CB_RecoverSetup=buffer[24];			ENG           =buffer[24+1];	EditExtFont    =buffer[ 26];		Grid         =buffer[26+1];
		Axes          =bufshort[15];        								CB_Round.MODE  =bufshort[14];
		Label         =bufshort[17];        								MaxMemMode     =buffer[ 32];		CB_Round.DIGIT=buffer[32+1]+1;
											Derivative    =buffer[38+1];	DefaultWaitcount=bufshort[18];
		ExitDebugModeCheck=buffer[42];		S_L_Style     =buffer[42+1];	CommandInputMethod=bufshort[20];
		ComplexMode   =buffer[46];			Angle         =buffer[46+1];	EnableExtFont  =buffer[ 44];		ForceG1Msave   =buffer[44+1];
		BreakCheckDefault=bufshort[25];        								StorageMode    =bufshort[24];
		CB_EditIndent =buffer[54];			TimeDsp       =buffer[54+1];	PageUpDownNum =bufshort[26]; if ( PageUpDownNum < 1 ) PageUpDownNum = PageUpDownNumDefault;
		MatXYmode     =bufshort[29];        								MatBaseDefault=1-bufshort[28];
		PictMode      =bufshort[31];        								CB_HelpOn      =buffer[ 60];		CheckIfEnd    =buffer[60+1];

		Xfct=bufdbl[ 8];
		Yfct=bufdbl[ 9];
		for ( i= 10; i<  10+58 ; i++ ) REG[i-10].real =bufdbl[i];
		for ( i= 68; i<  68+11 ; i++ ) REGv[i-68]     =bufdbl[i];
		for ( i=160; i< 160+58 ; i++ ) REGINT[i-160]  =bufint[i];

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

		AutoSaveMode =buffer[1075];
		EditTopLine  =buffer[1076];
		EditFontSize =buffer[1077];
		DisableDebugMode =buffer[1078];

	} else {
		Bfile_DeleteMainMemory(fname);
	}
}

void LoadConfig2(){
	const unsigned char fname[]="CBasic2";
	unsigned char buffer[ConfigMAX2];
	unsigned char *sbuf;
	short  *bufshort;
	int    *bufint;
	double *bufdbl;
	int size,i;
	int handle,state;
	char folder[32]="CBasic";		// default C.Basic working folder

	if  ( LoadConfigReadFile( buffer, fname, ConfigMAX2 ) < 0 ) {
		if ( Is35E2 ) {	
			CreateDirectorySub( folder, 1 );		// creat default C.Basic folder for III model
			root2[0]='\\';
			strcpy( root2+1, folder );				// current folder
		}
		return;
	}

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
		 ( buffer[ 9]=='1' ) &&
		 ( buffer[10]=='8' ) &&
		 ( buffer[11]=='0' ) ) {
									// load config & memory
		strncpy( root2, (char *)buffer+16, root2_MAX);	// load default folder
		for ( i= 6; i<  6+58 ; i++ ) REG[i-6].imag =bufdbl[i];	//

	} else {
		Bfile_DeleteMainMemory(fname);
	}
}

void LoadConfig(){
	LoadConfig1();
	LoadConfig2();
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// C.Basic Pre process
//----------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------/Indent

void PP_Search_CR_SPACE_Skip_quot(char *SRC, int *ptr){
	int c,flag=0;
	c=SRC[(*ptr)-2];
	if ( CheckQuotCR( SRC, (*ptr) ) ) flag=1;	// "   "
	while (1){
		c=SRC[(*ptr)++];
		switch ( c ) {
			case 0x0D:	// <CR>
				if ( flag ) break;
			case 0x00:	// <EOF>
			case 0x22:	// "
				return ;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
			case 0xFFFFFFF7:	//
				c=SRC[(*ptr)++];
				break;
		}
	}
}
void PP_Search_CR_SPACE_Skip_comment(char *SRC, int *ptr){
	int c;
	while (1){
		c=SRC[(*ptr)++];
		switch ( c ) {
			case 0x00:	// <EOF>
			case 0x0D:	// <CR>
				return ;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
			case 0xFFFFFFF7:	//
				c=SRC[(*ptr)++];
				break;
		}
	}
}

int PP_Search_CR_SPACE(char *SRC ){
	int c;
	int ptr=0;
	if ( SRC[ptr] == ' ' ) return 1;
	while (1){
		c=SRC[ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return 0;
			case 0x22:	// "
				PP_Search_CR_SPACE_Skip_quot(SRC, &ptr);
				break ;
			case 0x27:	// '
			case 0x13:  // =>
				PP_Search_CR_SPACE_Skip_comment(SRC, &ptr);
				break ;
			case 0x0C:	// <Disps>
			case 0x0D:	// <CR>
				if ( SRC[ptr] == ' ' ) return 1;
				break;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
			case 0xFFFFFFF7:	//
				c=SRC[ptr++];
				break;
		}
	}
	return 0;
}

int PP_Indent_Skip_quot(char *SRC, char *dest, int *sptr, int *dptr){
	int c,flag=0,d=0;
	c=SRC[(*sptr)-2];
	if ( CheckQuotCR( SRC, (*sptr) ) ) flag=1;	// "   "
	if ( SRC[(*sptr)-1] == 0x27 ) { d=1; flag=0; }
	while (1){
		c=SRC[(*sptr)++];
		dest[(*dptr)++]=c;
		switch ( c ) {
			case 0x0D:	// <CR>
				if ( d ) return 0;
				if ( flag ) break;
				return c;
			case 0x22:	// "
				if ( d ) break;
			case 0x00:	// <EOF>
				return 0;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
			case 0xFFFFFFF7:	//
				c=SRC[(*sptr)++];
				dest[(*dptr)++]=c;
				break;
		}
	}
	return 0;
}

int CB_PreProcessIndent( char *filebase, int progno ) { //
	char *dest,*SRC;
	int ptr;
	int c,i,j;
	int size,maxsize,newsize;
	int editMax;
	int sptr=0,dptr=0;
	int indent = 0;
	int editIndent=(CB_EditIndent&0x07);
	int switch_nest=0;
	int switch_indent[16];
	for( i=0;i<16;i++) switch_indent[i]=0;

	if ( editIndent == 0 ) return 0;
	if ( PP_Search_CR_SPACE( filebase+0x56 ) ) return 0;	// already exist indent

	size = SrcSize( filebase ) ;
	editMax = ProgfileMax[progno]-size;
	maxsize = HiddenRAM_MatTopPtr - filebase -16;
	HiddenRAM_freeProg( filebase );
	filebase = (char*)HiddenRAM_mallocProg( maxsize );
	if ( filebase == NULL ) { CB_Error(NotEnoughMemory); CB_ErrMsg(g_error_type); return 1; } //
	memcpy2( filebase+maxsize-size, filebase, size+1 );

	SRC  = filebase+maxsize-size +0x56;
	dest = filebase +0x56;


	while ( sptr < size ){
		if ( dptr >= maxsize ) {
			CB_Error(NotEnoughMemory); CB_ErrMsg(g_error_type); return 1; } //
		c=SRC[sptr++];
		dest[dptr++]=c;
		switch ( c ) {
			case 0x00:	// <EOF>
				goto exit;
			case 0x22:	// "
				if ( PP_Indent_Skip_quot(SRC, dest, &sptr, &dptr) == 0 ) break;
			case 0x0C:	// <Disps>
			case 0x0D:	// <CR>
				i = 0 ;
				while ( i < indent ) {
					dest[dptr++]=' ';
					i++;
				}
				break;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
				c=SRC[sptr++];
				dest[dptr++]=c;
				break;
			case 0xFFFFFFF7:	//
				c=SRC[sptr++];
				dest[dptr++]=c;
				switch (c) {
					case 0x00:	// If
					case 0x04:	// For
					case 0x08:	// While
					case 0x0A:	// Do
						indent += editIndent;
						break;
					case 0x01:	// Then
						if ( SRC[sptr] == ':' ) SRC[sptr]=0x0D;
						break;
					case 0x0FFFFFFEA:	// Switch
						indent += editIndent;
						if ( switch_nest<16 ) switch_nest++;
						switch_indent[switch_nest]=indent;
						break;
					case 0x0FFFFFFEB:	// Case
					case 0x0FFFFFFEC:	// Default
						if ( switch_indent[switch_nest]==0 ) break;
						if ( dest[dptr-3]==' ' ) {
							indent = switch_indent[switch_nest];
							j = editIndent;
							goto jindent;
						}
						break;
					case 0x0FFFFFFED:	// SwitchEnd
						j = 0;
						if ( dest[dptr-3]==' ' ) {
							indent = switch_indent[switch_nest]-editIndent;
							if ( switch_nest>1 ) switch_nest--;
						  jindent:
							dptr -= 3;
							while ( dest[--dptr] == ' ') ;
							dptr++;
							i = 1 ;
							while ( i <= indent ) {
								dest[dptr++]=' ';
								i++;
							}
							dest[dptr++]=0xF7;
							dest[dptr++]=c;
						}
						indent += j;
						break;
					case 0x03:	// IfEnd
					case 0x07:	// Next
					case 0x09:	// WhileEnd
					case 0x0B:	// LpWhile
						indent -= editIndent;
						if ( indent < 0 ) indent =0 ;
					case 0x02:	// Else
					case 0x0F:	// ElseIf
						if ( dest[dptr-3]==' ' ) {
							i = 1 ;
							dptr -= 3;
							do {
								if ( i >= editIndent ) break;
								dptr--;
								i++;
							} while ( dest[dptr] == ' ' ) ;
							dest[dptr++]=0xF7;
							dest[dptr++]=c;
						}
						break;
				}
				break;
			default:
				break;
		}
	}
  exit:
	dest[dptr]='\0';
	newsize = dptr +0x56;
	HiddenRAM_freeProg( filebase );
  loop:
	filebase = (char*)HiddenRAM_mallocProg( newsize +editMax);
	if ( filebase == NULL ) {
		editMax/=2; if ( editMax>=16 ) goto loop;
		{ CB_Error(NotEnoughMemory); CB_ErrMsg(g_error_type); return 1; } //
	}
	G1M_header( filebase, &newsize );	// G1M header set
	ProgfileMax[progno] = newsize +editMax;
	return 0;
}

void CB_PostProcessIndentRemove( char *filebase ) { //
	char *dest,*SRC;
	int ptr;
	int c,i;
	int size,maxsize;
	int buffersize;
	int sptr=0,dptr=0;
	int indent = 0;

	if ( ( CB_EditIndent&0x08 ) == 0 ) return ;

	size = SrcSize( filebase ) ;

	SRC  = filebase +0x56;
	dest = filebase +0x56;

	while ( sptr < size ){
		c=SRC[sptr++];
		dest[dptr++]=c;
		switch ( c ) {
			case 0x00:	// <EOF>
				goto exit;
			case 0x13:  // =>
			case 0x22:	// "
			case 0x27:	// '
				PP_Indent_Skip_quot(SRC, dest, &sptr, &dptr);
				break;
			case ' ':	// <SPC>
				dptr--;
				while ( SRC[sptr] == ' ' ) sptr++;
				break;
			case 0x0000007F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFFF:	//
			case 0xFFFFFFF7:	//
				c=SRC[sptr++];
				dest[dptr++]=c;
				break;
			default:
				break;
		}
	}
  exit:
	dest[dptr]='\0';
	size=dptr+0x56;
	G1M_header( filebase, &size );	// G1M header set
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_Local( char *SRC ) {
	int c=1,i,reg;
	i=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		reg=RegVarAliasEx(SRC);
		if ( reg>=0 ) {
			ProgLocalVar[ProgEntryN][i] = reg;	// local var set
		}
		i++;
		c=SRC[g_exec_ptr];
		if ( c != ',' ) break; 	//
		g_exec_ptr++;
		if ( i >= ArgcMAX ) { CB_Error(TooMuchData); break; }	// too much error
	}
	ProgLocalN[ProgEntryN] = i;
}

//----------------------------------------------------------------------------------------------
int PP_Search_IfEnd( char *SRC ){
	int c,i;
	int PP_ptr;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	//
				c=SRC[g_exec_ptr++];
				if ( c == 0x00 ) { 			// If
					PP_ptr=g_exec_ptr-2;
					i=PP_Search_IfEnd(SRC) ;
					if ( g_error_type ) return;
					if ( i != 1  ) { g_exec_ptr=PP_ptr; CB_Error(MissingIfEnd); CB_ErrMsg(g_error_type); return 0; } // not IfEnd error
					break;
				} else
				if ( c == 0x03 ) return 1 ;	// IfEnd
				break ;
			case 0x0000007F:	//
			case 0xFFFFFFF9:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFFF:	//
				g_exec_ptr++;
				break;
		}
	}
	return 0;
}

void PP_ReplaceCode( char *SRC ){
	int c,i;
//	while (1){
//		c=SRC[ExecPtr++];
//		switch ( c ) {
//			case 0x00:	// <EOF>
//				ExecPtr--;
//				return ;
//			case 0x0000007F:	//
//			case 0xFFFFFFE5:	//
//			case 0xFFFFFFE6:	//
//			case 0xFFFFFFE7:	//
//			case 0xFFFFFFFF:	//
//				ExecPtr++;
//				break;
//			case 0xFFFFFFF7:	//
//				c=SRC[ExecPtr++];
//				if ( c==0x3F ) SRC[ExecPtr-1]=0x3E;	// DotGet(  F73F -> F73E
//				else
//				if ( c==0x4F ) SRC[ExecPtr-1]=0x3D;	// DotTrim(  F74F -> F73D
//				break;
//			case 0xFFFFFFF9:	//
//				c=SRC[ExecPtr++];
//				if ( c==0x4B ) {
//					SRC[ExecPtr-2]=0xFFFFFFF7; SRC[ExecPtr-1]=0x3B; }	// DotPut(  F94B -> F73B
//				else
//				if ( c==0x3E ) SRC[ExecPtr-1]=0x43;	// Sprintf(  F93E -> F943
//				break;
//		}
//	}
	return ;
}

void CB_PreProcess( char *SRC ) { //	If..IfEnd Check
	int c=1,i;
	int execptr=g_exec_ptr;

	if ( textmode ) return ;
	g_exec_ptr=0;
//	PP_ReplaceCode( SRC );
	g_error_type=0;
	g_exec_ptr=0;
	if ( CheckIfEnd ) PP_Search_IfEnd(SRC);
	if ( g_error_type ) return;
	g_exec_ptr=execptr;
}

//----------------------------------------------------------------------------------------------
/*
void CB_ProgEntry( char *SRC ) { //	Prog "..." into memory
	int c=1,r;
	char buffer[32]="",folder16[21];
	char fname[64],filename[64];
	char sname[32],basname[32];
	char ext[8];
	char *filebase;
	char *StackProgSRC;
	int StackProgPtr;
	unsigned int key=0;
	int srcPrg;
	int progno=ProgNo;

//	locate( 1, 1); PrintLine(" ",21);						//
//	sprintf(buffer,"==%-8s==%08X",SRC-0x56+0x3C, SRC-0x56);
//	locate (1, 1); Print( (unsigned char*)buffer );

	if ( textmode ) return ;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFED:	// Prog "..."
				c =SRC[ExecPtr];
				if ( c != 0x22 ) break;
				ExecPtr++;	// " skip
				CB_GetQuotOpcode(SRC, buffer, 32);	// Prog name
				strcpy( basname, buffer);
				Setfoldername16( folder16, basname );
				srcPrg = CB_SearchProg( folder16 );
				if ( srcPrg < 0 ) { 				// undefined Prog
					MSG2("Prog Loading.....",buffer);
					Getfolder( buffer );
					SetFullfilenameExt( filename, buffer, "g1m" ) ;		// g1m 1st reading
					r=LoadProgfile( filename, ProgEntryN, EditMaxProg, 0 ) ;
					if ( r ) {
						ErrorNo=0;	// clear error
						SetFullfilenameExt( filename, buffer, "txt" ) ;	// retry 2nd text file
						r=LoadProgfile( filename, ProgEntryN, EditMaxProg, 0 ) ;
					}
					Restorefolder();
					if ( ErrorNo ) {	// Can't find Prog
						goto err;
					}
					if ( r== 0 ) {	// Prog load Ok
						filebase=ProgfileAdrs[ProgEntryN];
						strncpy( filebase+0x3C, basname, 8);		// set filename to g1m/g3m header
						ProgEntryN++;
						if ( ProgEntryN > ProgMax ) { CB_Error(TooManyProg); CB_ErrMsg(ErrorNo); return ; } // Too Many Prog error
						StackProgPtr = ExecPtr;
						CB_ProgEntry( filebase + 0x56  );		//
						if ( ErrorNo == MissingIfEnd ) return ;
						ExecPtr = StackProgPtr ;
//						if ( ErrorNo ) {
//							g_error_ptr=StackProgPtr;
//							ProgNo=progno;
//							g_error_prog=ProgNo;
//							return ;
//						}
					}
				}
				break;
			case 0xFFFFFFF7:	//
				if ( SRC[ExecPtr++] == 0xFFFFFFF1 ) CB_Local(SRC);	// Local var set
				break;
			case 0xFFFFFFF9:	//
				if ( SRC[ExecPtr++] == 0x0F ) CB_AliasVar(SRC);	// Alias var set
				break;
			case 0x7F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFFF:	//
				ExecPtr++;
				break;
			default:
				break;
		}
		if ( ErrorNo ) {	// error
		 err:
			g_error_ptr=ExecPtr;
			ProgNo=progno;
			g_error_prog=ProgNo;
			CB_ErrMsg(ErrorNo);
			return;
		}
	}

	SetSrcSize( SRC-0x56 , ExecPtr+0x56+1 );
}
*/
void CB_GetAliasLocalProg( char *SRC ) { //	Preprocess Alias/Local
	int c=1;
	g_exec_ptr=0;
	while ( c!=0 ) {
		c=SRC[g_exec_ptr++];
		if ( c==0x00 ) { g_exec_ptr--; break; }
		switch ( c ) {
			case 0x3A:	// <:>
			case 0x0D:	// <CR>
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	//
				if ( SRC[g_exec_ptr++] == 0xFFFFFFF1 ) CB_Local(SRC);	// Local var set
				break;
			case 0xFFFFFFF9:	//
				if ( SRC[g_exec_ptr++] == 0x0F ) CB_AliasVar(SRC);	// Alias var set
				g_error_type=0;
				break;
			case 0x7F:	//
			case 0xFFFFFFE5:	//
			case 0xFFFFFFE6:	//
			case 0xFFFFFFE7:	//
			case 0xFFFFFFFF:	//
				g_exec_ptr++;
				break;
			default:
				break;
		}
	}
}

int CB_GetProgEntry( char *SRC, char *buffer ) { //	Prog "..." into memory
	int i,c,r;
	char folder16[21];
	char filename[64];
	char sname[32],basname[32];
	char ext[8];
	char *filebase;
	int ExecPtr_bk=g_exec_ptr;
	int progEntryN;
	int ProgEntryN_bk = ProgEntryN;
	char *SRC_bk = SRC;

	for (i=0; i<=ProgMax; i++) {			// memory free
		if ( ProgfileAdrs[i] == NULL ) break;		// Prog
	}
	ProgEntryN = i;
	if ( ProgEntryN >= ProgMax ) { CB_Error(TooManyProg); return -1; } // Too Many Prog error

	HelpText = NULL;	// help buffer cancel

	strcpy( basname, buffer);
	Setfoldername16( folder16, basname );
//	MSG2("Prog Loading.....",buffer);
	Getfolder( folder16 );
	SetFullfilenameExt( filename, buffer, "g1m" ) ;		// g1m 1st reading
	r=LoadProgfile( filename, ProgEntryN, EditMaxProg, 0 ) ;
	if ( r ) {
		g_error_type=0;	// clear error
		SetFullfilenameExt( filename, buffer, "txt" ) ;	// retry 2nd text file
		r=LoadProgfile( filename, ProgEntryN, EditMaxProg, 0 ) ;
	}
	Restorefolder();
	if ( ( g_error_type ) || r ) { CB_Error(GoERR); return -1; }	// Can't find Prog

	filebase=ProgfileAdrs[ProgEntryN];
	strncpy( filebase+0x3C, basname, 8);		// set filename to g1m/g3m header

	SRC = filebase +0x56;
	CB_GetAliasLocalProg( SRC ) ; //	Preprocess Alias/Local
	SetSrcSize( SRC-0x56 , g_exec_ptr+0x56+1 );
	g_exec_ptr    = ExecPtr_bk;
	r = ProgEntryN ;
	ProgEntryN = ProgEntryN_bk;
	return r;
}

//--------------------------------------------------------------------------------------------- battery
// refer to SuperH-based fx calculators (version 20) by SimonLothar

int GetMainBatteryVoltagePointer(){
	unsigned int ea;
	unsigned int j;
    ea = *(unsigned int*)0x8001007C;
    ea = ea + 0x049C*4;
    ea = *(unsigned int*)( ea );
    while ( *(unsigned short*)( ea ) != 0xE464 ){
      ea = ea + 2;
    };
    ea = ea + 2;
    j = *(unsigned char*)( ea + 1 );
    j = 4*j;
    j = ( ea + j + 4 ) & 0xFFFFFFFC;
    return *(unsigned int*)( j );
}
// battery == 1 : main (ADC channel C)
// battery != 1 : possibly the backup battery (ADC channel B)
// firstlevel: warning level
// secondlevel: shutdown level

int GetBatteryStatus( int battery, int*firstlevel, int*secondlevel ){
	int (*iGBS)( int ) = 0; // declare an int function pointer
	int*battconfig = (int*)0x80000334;
	*firstlevel = 0x17F;
	*secondlevel = 0x16D;

	iGBS = (int(*)(int))GetMainBatteryVoltagePointer();

	switch ( OS_Version() ){
	    case 102 : break;
	    case 103 : break;
	    case 104 :
	    case 105 :
	        if ( *battconfig==0x43444844 ){
	            *firstlevel = 0x1A5;
	            *secondlevel = 0x191;
	        };
	        break;
	    case 110 :
	    case 111 :
	        *firstlevel = 0x2AA;
	        *secondlevel = 0x288;
	        break;
	    case 200 :
	        if ( *battconfig==0x43444844 ){
	            *firstlevel = 0x1A5;
	            *secondlevel = 0x191;
	        };
	        break;
	};
	if (CPU_check()==2){	// IsSlim
	    *firstlevel = 0x2AA;
	    *secondlevel = 0x288;
	}

	if (iGBS!=0) return (*iGBS)( battery );
	else return 0;
}


int GetMainBatteryVoltage( int battery ) {
	int firstlevel, secondlevel;
	if ( IsEmu ) return 500;
	return GetBatteryStatus( battery, &firstlevel, &secondlevel );
}

int CB_BatteryStatus( char *SRC ){
	int r = GetMainBatteryVoltage( 1 ) ;
	switch  ( CPU_check() ) {
//		case 0:	// SH4A
//			break;
		case 1: // SH3
			if ( IsHiddenRAM == 0 ) r = r*111/100;	// fx-9860G
			else					r = r*101/100;	// fx-9860GII
			break;
		case 2: // Slim
			r = r* 30/100;
			break;
		default:
			break;
	}
	return r;
}

//----------------------------------------------------------------------------------------------
int Emu_check() {
//	int i,t,s=RTC_GetTicks();
//	for(i=0;i<140;i++){
//		Bdisp_PutDisp_DD();
//	}
//	t=RTC_GetTicks()-s;
//	if ( t<6) IsEmu=1;
	if ( *(int*)0x8000FFD0 == 0 ) IsEmu=1;	// is emulator
}

void WaitKeyAC(){
	while ( KeyScanDown(KEYSC_AC) ) ;
}
void WaitKeyEXIT(){
	while ( KeyScanDown(KEYSC_EXIT) ) ;
}
void WaitKeyF1(){
	while ( KeyScanDown(KEYSC_F1) ) ;
}
void WaitKeyEXE(){
	while ( KeyScanDown(KEYSC_EXE) ) ;
}

//---------------------------------------------------------------------------------------------- align dummy
int fileObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4u( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4v( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4w( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4x( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4y( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4z( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4A( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4B( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4C( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4D( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4E( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4F( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4G( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4H( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4I( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4J( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4K( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4L( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4M( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4N( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4O( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4P( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4Q( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4R( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4S( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4T( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4U( unsigned int n ){ return n; }	// align +4byte
int fileObjectAlign4V( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4W( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4X( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4Y( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4Z( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4AA( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4BB( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4CC( unsigned int n ){ return n; }	// align +4byte
//int fileObjectAlign4DD( unsigned int n ){ return n; }	// align +4byte
void FavoritesDowndummy( int *index ) {
	unsigned short tmp;
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
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy3( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy4( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy5( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy6( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
/*
void FavoritesDowndummy7( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy8( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummy9( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummyA( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
void FavoritesDowndummyB( int *index ) {
	unsigned short tmp;
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
	(*index)++;
	files[(*index)].filesize=tmp;
	SaveFavorites();
}
*/
