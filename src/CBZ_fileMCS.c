#include "CB.h"

//------------------------------------------------------------------------- MCS
const unsigned char MCSdir_system[]="system";
	
int MCS_ReadFileList() {	// MSC file list -> files
	TMainMemoryDirectoryEntry *pMCSdir_system;
	TDirectoryItem *item;
	char dirno;
	char fbuf[32];
	char *cg1m=".g1m";
	int i,j,r;
	int size;

	r = MCS_SearchDirectory( MCSdir_system, &pMCSdir_system, &dirno );
	if ( r == 0x40 ) { ErrorMSG( "Can't find MCS", r); return 0; }
	if ( r != 0    ) { ErrorMSG( "MCS error",  r); return 0; }

	item = pMCSdir_system->addr;

	size = pMCSdir_system->count + FavoritesMAX + 1 ;
//	if ( pMCSdir_system->count == 0 ) return size;	// no file

  loop:
	HiddenRAM_freeProg(HiddenRAM_Top);
	files = (Files *)HiddenRAM_mallocProg( size*sizeof(Files) );
	if ( files == NULL ) {	// Abort();
		MSG2("Clear Memory !!  ","File Reading.....");
		Bdisp_PutDisp_DD();
		DeleteMatrix( -1 );
		goto loop;
	}
	memset( files, 0, size*sizeof(Files) );
	
	i = FavoritesMAX ; j=0;
	while( i< size-1 ) {
		strncpy( fbuf, (char *)item[j].name ,8 );
		fbuf[8]='\0';
		strcat( fbuf, cg1m ) ;
		strncpy( files[i].filename, fbuf, FILENAMEMAX);
		strncpy( files[i].folder, folder, FOLDERMAX);
		files[i].filesize = item[j].length;
		i++; j++;
	}
	return size;
}


void ShortName2basname( char *basname, char *sname ){
	int i;
	i = strlen(sname)-4;
	strncpy( basname, sname, i );
	basname[i]='\0';
}
void SetBasName( char *basname, char *fname ){
	char sname[32];
	SetShortName( sname, fname);
	ShortName2basname( basname, sname );
}

char * MCS_LoadG1M( char *fname , int *editMax, int disperror, int *filesize  ) {	// MCS -> g1m file
	TDirectoryItem *item;
	int tmp[4];
	char *filebase,*srcbase;
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	int size;
	int offset;
	int data;
	char basname[16];
	SetBasName( basname, fname);
	r = MCS_SearchDirectoryItem( MCSdir_system, (unsigned char *)basname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	if ( r == 0x30 ) { 
//		ErrorMSGfile( "Can't find file", (char*)fname, r);
		return NULL; 
	}
	if ( r != 0    ) { 
//		ErrorMSGfile( "MCS load error",  (char*)fname, r); 
		return NULL; 
	}

	item = (TDirectoryItem*)tmp[0];
	srcbase=(char *)data_ptr+10;
	size=0;
	while ( srcbase[size] ) {
		if ( size >= data_length ) break;
		NextLine( srcbase, &size );
	}

  loop:
	filebase = ( char *)HiddenRAM_mallocProg( size*sizeof(char)+(*editMax) +0x56 +4 );
	if( filebase == NULL ) {
		(*editMax)/=2; if ( (*editMax)>=16 ) goto loop;
		if ( disperror ) ErrorMSGfile( "Can't load file", (char*)basname, 0 );
		CB_Error(NotEnoughMemoryERR); 
		return NULL;
//		Abort();
	}
	memset( filebase+0x56, 0x00,     size*sizeof(char)+(*editMax)+4 );
	memcpy( filebase+0x56-10, (char*)data_ptr, data_length );
	size += 0x56;
	G1M_header( filebase, &size );	// G1M header set
	G1M_Basic_header( filebase );	// G1M Basic header set
	basname8ToG1MHeader( filebase, basname);
	(*filesize) = size;
	return filebase;
}

int MCS_Save( char *filebase, char *dirname, char *itemname, int size, int flag0 ) {	// file -> MCS
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	int buffer;
	char basname[16];

	r = MCS_SearchDirectoryItem( (unsigned char *)dirname, (unsigned char *)itemname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	item = (TDirectoryItem*)tmp[0];

	if ( data_length == size ) {
		r = MCS_OverwriteData( (unsigned char *)dirname, (unsigned char *)itemname, 0, data_length, filebase+0x56-10 );
	} else {
		if ( size-data_length > MCS_Free() ) { 
			if ( flag0==0xC7 ) ErrorMSG( "MCS memory full", -1 ); 
			return -1 ; 
		}
		r = MCS_DeleteItem( (unsigned char *)dirname, (unsigned char *)itemname );
		r = MCSPutVar2( (unsigned char *)dirname, (unsigned char *)itemname, size, filebase+0x56-10 );
		FileListUpdate=1;
	}
	if ( r ) { 
		if ( flag0==0xC7 ) ErrorMSG( "Can't save file", r ); 
		return r ;
	}
	r = MCS_SearchDirectoryItem( (unsigned char *)dirname, (unsigned char *)itemname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	item = (TDirectoryItem*)tmp[0];
	if ( flag0==0xC7 ) item->flags[0] = flag0;	// basic program
	return r;
}

int MCS_SaveG1M( char *filebase ) {	// g1m file -> MCS
	int size,r;
	char basname[16];

	G1MHeaderTobasname8( filebase, basname);
	size = FixSrcSize(filebase);
	size = ( ( size +10 +3 ) & 0xFFFFFFFC ) ; // file size 4byte align adjust
	SetSrcSize( filebase, size ); 
	r = MCS_Save( filebase, (char*)MCSdir_system, basname, size, 0xC7 ) ;	// g1m file -> MCS
	return r;
}


int MCS_DeleteG1M( char *fname  ) {	// delete g1m
	char basname[16];
	SetBasName( basname, fname);
	return MCS_DeleteItem( MCSdir_system, (unsigned char *)basname );
}

int MCS_ExistFile( char *fname  ) {	// exist?
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	char basname[16];
	SetBasName( basname, fname);
	r = MCS_SearchDirectoryItem( MCSdir_system, (unsigned char *)basname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	return r; // r==0 existed 
}

/*
char * MCS_LoadClip( int *length ) {
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	r = MCS_SearchDirectoryItem( (unsigned char *)MCSdir_REV2, (unsigned char *)MCSitem_clip, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, & data_length );
	if ( r != 0    ) return NULL;
	*length = data_length;
	return (char*)data_ptr;
}
*/
int MCS_Free(){
	int maxspace,currentload,remainingspace;
	MCS_GetState( &maxspace, &currentload, &remainingspace );
	return remainingspace;
}

char * MCS_LoadCAPT( char *pictname ) {
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	r = MCS_SearchDirectoryItem( (unsigned char *)"@REV2", (unsigned char *)pictname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, & data_length );
	if ( r != 0    ) return NULL;
	return (char*)data_ptr+10-4-2-0x50;
}
char * MCS_LoadPICT( char *pictname, int *length ) {
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	r = MCS_SearchDirectoryItem( (unsigned char *)"main", (unsigned char *)pictname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, & data_length );
	if ( r != 0    ) return NULL;
	*length = data_length;
	return (char*)data_ptr-0x4C;
}

int MCS_SaveCAPT( char *pictname, char *filebase  ) {
	return  MCS_Save( filebase, "@REV2", pictname, 1024, 0x00 ) ;	// CAPT file -> MCS
}

int MCS_SavePICT( char *pictname, char *filebase  ) {
	return  MCS_Save( filebase, "main", pictname, 2048, 0x00 ) ;	// PICT file -> MCS
}
