extern "C" {

#include "prizm.h"
#include "CBP.h"

//------------------------------------------------------------------------- MCS

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

int MCS_SearchDirectory(unsigned char *dir, TMainMemoryDirectoryEntry **pdir, char *dirno);
//int MCS_CreateDirectory( unsigned char*dir, char*dirno );
//int MCS_DeleteDirectory( unsigned char*dir );
int MCS_GetSystemDirectoryInfo( unsigned char*dir, unsigned int*pdir, char*dirno );
int MCS_SearchDirectoryItem( unsigned char*dir, unsigned char*item, char*flags_0, TDirectoryItem*item_ptr, int*data_ptr, int*data_length );



static unsigned char MCSdir_system[]="system";

int MCS_ReadFileList() {	// MSC file list -> files
	TMainMemoryDirectoryEntry *pdir;
	TDirectoryItem *item;
	char dirno;
	char fbuf[32];
	char *cg3m=".g3m";
	int i,j,r;
	int size;

	r = MCS_SearchDirectory( MCSdir_system, &pdir, &dirno );
	if ( r == 0x40 ) { ErrorMSG( "Can't find dir", r); return 0; }
	if ( r != 0    ) { ErrorMSG( "MCS dir error",  r); return 0; }

	item = (TDirectoryItem*)pdir->addr;

	size = pdir->count + FavoritesMAX + 1 ;
	if ( pdir->count == 0 ) return size;	// no file

	memset( files, 0, FILEMAX*sizeof(Files) );
	
	i = FavoritesMAX ; j=0;
	while( i< size-1 ) {
		strncpy( fbuf, (char *)item[j].name ,8 );
		fbuf[8]='\0';
		strcat( fbuf, cg3m ) ;
		strncpy( files[i].filename, fbuf, FILENAMEMAX);
		strncpy( files[i].folder, folder, FOLDERMAX);
		files[i].filesize = item[j].length;
		i++; j++;
		if ( i+1 > FILEMAX ) Abort();
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

char * MCS_LoadG3M( char *fname , int *editMax, int disperror, int *filesize  ) {	// MCS -> g3m file
	TDirectoryItem *item;
	int tmp[4];
	char *dirno;
	char *filebase,*srcbase;
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	int size;
	char basname[16];
	SetBasName( basname, fname);
	r = MCS_SearchDirectoryItem( MCSdir_system, (unsigned char *)basname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	if ( r == 0x30 ) { 
		if ( disperror ) ErrorMSGfile( "Can't find file", (char*)fname, r);
		return NULL; 
	}
	if ( r != 0    ) { 
		if ( disperror ) ErrorMSGfile( "MCS load error",  (char*)fname, r); 
		return NULL; 
	}

	item = (TDirectoryItem*)tmp[0];
	srcbase=(char *)data_ptr+10;
	size=0;
	while ( srcbase[size] ) {
		if ( size >= data_length ) break;
		NextLineGB( srcbase, &size );
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
	G1M_header( filebase, &size );	// G3M header set
	G1M_Basic_header( filebase );	// G3M Basic header set
	basname8ToG1MHeader( filebase, basname);
	(*filesize) = size;
	return filebase;
}

int MCS_SaveG3M( char *filebase ) {	// g3m file -> MCS
	TDirectoryItem *item;
	int tmp[4];
	int i,r;
	char flags_0;
	int  data_ptr;
	int  data_length;
	int size;
	int buffer;
	char basname[16];
	char buf[32],buf2[32];
	int maxspace,currentload,remainingspace;
	MCS_GetState( &maxspace, &currentload, &remainingspace );

	G1MHeaderTobasname8( filebase, basname);
	r = MCS_SearchDirectoryItem( MCSdir_system, (unsigned char *)basname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	item = (TDirectoryItem*)tmp[0];

	size = FixSrcSize(filebase);
	size = ( ( size +10 +3 ) & 0xFFFFFFFC ) ; // file size 4byte align adjust
	SetSrcSize( filebase, size ); 

// sprintf3(buf,"size=%d r=%d",size,r);
// sprintf3(buf,"%X %X",(int)item,flags_0);
// OkMSGstr2(buf,buf2);

	if ( data_length == size ) {
//			MCSOvwDat2( unsigned char*dir, unsigned char*item, int bytes_to_write, void*buffer, int write_offset );
		r = MCSOvwDat2( MCSdir_system, (unsigned char *)basname, data_length, filebase+0x56-10, 0);
	} else {
		if ( size-data_length > remainingspace ) { ErrorMSG( "MCS memory full", -1 ); return -1 ; }
//		r = MCSDelVar2( MCSdir_system, (unsigned char *)basname );
//		r = MCSPutVar2( MCSdir_system, (unsigned char *)basname, size, filebase+0x56-10 );
		r = SaveConfigWriteFileSub( (unsigned char *)filebase+0x56-10, MCSdir_system, (unsigned char *)basname, size );
		FileListUpdate=1;
	}
	if ( r>1 ) { ErrorMSG( "Can't save file", r ); return r ; }
	r = MCS_SearchDirectoryItem( MCSdir_system, (unsigned char *)basname, &flags_0, (TDirectoryItem *)&tmp, &data_ptr, &data_length );
	item = (TDirectoryItem*)tmp[0];
	item->flags[0] = 0xC7;	// basic program
	return r;
}


int MCS_DeleteG3M( char *fname  ) {	// delete g3m
	char basname[16];
	SetBasName( basname, fname);
	return MCSDelVar2( MCSdir_system, (unsigned char *)basname );
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

}
