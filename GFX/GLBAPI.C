/****************************************************************************
* GLBapi  - File loading routines
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Aug, 1992
*
* CONTENTS: glbapi.c glbapi.h
*
* EXTERN MODULES - Main module of your program
*
* NOTES:
*
*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <malloc.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>

// #include <windows.h>
// #include <windowsx.h>
  
#include "exitapi.h"
#include "vmemapi.h"
#include "glbapi.h"
  
#define _SCOTTGAME

PRIVATE CHAR *    serial = "32768GLB";
PRIVATE CHAR      exePath[ _MAX_PATH ];
PRIVATE INT       num_glbs;
PRIVATE KEYFILE   g_key;
PRIVATE CHAR      prefix[5] = "FILE";
PRIVATE BOOL		fVmem = FALSE;

/*
* define file descriptor used to access file.
*/
typedef struct
{
   char     name[ 16 ];
   VM_OWNER vm_mem;
   DWORD    size;
   DWORD    offset;
   DWORD    flags;
	DWORD		lock_cnt;
} ITEMINFO;

typedef struct
{
   char     filepath[ _MAX_PATH ];
   ITEMINFO *item;
   int      items;
   int      handle;
   int      permissions;
} FILEDESC;

typedef struct
{
   WORD     itemnum;
   WORD     filenum;
} ITEM_ID;

typedef union
{
   ITEM_ID  id;
   DWORD    handle;
} ITEM_H;

PRIVATE FILEDESC  filedesc[ MAX_GLB_FILES ];

#define ITF_LOCKED  0x80000000L
#define ITF_ENCODED 0x40000000L

/***************************************************************************
  GLB_EnCrypt - Encrypt Data
 ***************************************************************************/
VOID
GLB_EnCrypt(
CHAR     *key,             // INPUT : Key that will allow Decryption
BYTE     *buffer,          // INPUT : Buffer to Encrypt
size_t   length            // INPUT : Length of Buffer
)
{
   INT   klen = strlen( key );
   INT   prev_byte;
   INT   kidx;
  
   kidx = SEED % klen;
   prev_byte = key[ kidx ];
   while ( length-- )
   {
      prev_byte = ( *buffer + key[ kidx ] + prev_byte ) % 256;
      *buffer++ = prev_byte;
  
      if ( ++kidx >= klen )
         kidx = 0;
   }
}
  
/***************************************************************************
  GLB_DeCrypt - Decrypt Data
 ***************************************************************************/
VOID
GLB_DeCrypt(
CHAR    *key,               // INPUT : Key that will allow Decryption
BYTE    *buffer,            // INPUT : Buffer to Encrypt
size_t  length              // INPUT : Length of Buffer
)
{
   INT   klen = strlen( key );
   INT   prev_byte;
   INT   kidx;
   CHAR  dchr;
  
   kidx = SEED % klen;
   prev_byte = key[ kidx ];
   while ( length-- )
   {
      dchr = ( *buffer - key[ kidx ] - prev_byte ) % 256;
      prev_byte = *buffer;
      *buffer++ = dchr;
  
      if ( ++kidx >= klen )
         kidx = 0;
   }
}

/*------------------------------------------------------------------------
   GLB_FindFile() - Finds a file, opens it, and stores it's path 
 ------------------------------------------------------------------------*/
PRIVATE INT
GLB_FindFile(
INT	return_on_failure,	// INPUT : Don't bomb if file not open
INT	filenum,             // INPUT : file number
INT	permissions				// INPUT : file access permissions
)
{
   char        *routine = "GLB_FindFile";
   char        filename[ _MAX_PATH ];
   int         handle;
   FILEDESC    *fd;
/*
* Scott, the ASSERT function is a macro that will be compiled-out when
* not debugging ( DEBUG not defined ), otherwise it calls EXIT_Assert
* if the expression is false ( which bails out of your program and displays
* the expression, file & line number of the assertion ).  This is a standard
* way of checking variables for correct boundries during development, and
* makes it easy to remove extra logic for doing so by recompiling without
* DEBUG defined.
*/
   ASSERT( filenum >= 0 && filenum < MAX_GLB_FILES );
/*
* create a file name and attempt to open it local first, then if it
* fails use the exe path and try again.
*/
   sprintf( filename, "%s%04u.glb", prefix, filenum );
   if ( ( handle = open( filename, permissions ) ) == -1 )
	{
      sprintf( filename, "%s%s%04u.glb", exePath, prefix, filenum );
   	if ( ( handle = open( filename, permissions ) ) == -1 )
		{
			if ( return_on_failure )
				return -1;

         sprintf( filename, "%s%04u.glb", prefix, filenum );
   	   EXIT_Error ("GLB_FindFile: %s, Error #%d,%s",
								filename, errno, strerror( errno ) );
		}
	}
/*
* Keep file handle
*/
   fd = &filedesc[ filenum ];

   strcpy( fd->filepath, filename );
   fd->permissions = permissions;
   fd->handle = handle;

	return handle;
}
  

  
/*------------------------------------------------------------------------
   GLB_OpenFile() - Opens & Caches file handle
 ------------------------------------------------------------------------*/
PRIVATE INT
GLB_OpenFile(
INT	return_on_failure,	// INPUT : Don't bomb if file not open
INT	filenum,             // INPUT : file number
INT	permissions				// INPUT : file access permissions
)
{
   FILEDESC    *fd;

   ASSERT( filenum >= 0 && filenum < MAX_GLB_FILES );

   fd = &filedesc[ filenum ];

   if ( fd->handle == 0 )
      return GLB_FindFile( return_on_failure, filenum, permissions );
   else if ( fd->permissions != permissions )
   {
      close( fd->handle );
   	if ( ( fd->handle = open( fd->filepath, permissions ) ) == -1 )
		{
			if ( return_on_failure )
				return -1;

   	   EXIT_Error ("GLB_OpenFile: %s, Error #%d,%s",
								fd->filepath, errno, strerror( errno ) );
      }
   }
   else
   {
      lseek( fd->handle, 0L, SEEK_SET );
   }
   return fd->handle;
}
  

/*------------------------------------------------------------------------
   GLB_CloseFiles() - Closes all cached files.
 ------------------------------------------------------------------------*/
PRIVATE VOID
GLB_CloseFiles(
VOID
)
{
	INT		j;

	for ( j = 0; j < MAX_GLB_FILES; j++ )
	{
      if ( filedesc[ j ].handle )
      {
   		close( filedesc[ j ].handle );
         filedesc[ j ].handle = 0;
      }
	}
}


/*------------------------------------------------------------------------
   GLB_NumItems() - Returns number of items in a .GLB file
 ------------------------------------------------------------------------*/
PRIVATE INT
GLB_NumItems (
INT filenum
)
{
   KEYFILE     key;
   INT         handle;

   ASSERT( filenum >= 0 && filenum < num_glbs );

	handle = GLB_OpenFile( 1, filenum, O_RDONLY | O_BINARY );
	if ( handle == -1 )
		return 0;

   lseek( handle, 0L, SEEK_SET );
   if ( ! read( handle,( VOID * ) &key, sizeof( KEYFILE ) ) )
   {
      EXIT_Error( "GLB_NumItems: Read failed!" );
   }
  
#ifdef _SCOTTGAME
   GLB_DeCrypt( serial, ( BYTE * )&key, sizeof( KEYFILE ) );
#endif
  
   return ( ( int ) key.offset );
}

/*--------------------------------------------------------------------------
 GLB_LoadIDT() Loads a item descriptor table from a GLB file.
 --------------------------------------------------------------------------*/
PRIVATE VOID
GLB_LoadIDT(
FILEDESC *fd               // INPUT: file to load
)
{
   int      handle;
   int      j;
	int		k;
	int 		n;
   KEYFILE  key[ 10 ];
   ITEMINFO *ii;

   handle = fd->handle;
   ii = fd->item;

   lseek( handle, sizeof( KEYFILE ), SEEK_SET );
   for ( j = 0; j < fd->items; )
   {
		k = fd->items - j;
		if ( k > ASIZE( key ) )
			k = ASIZE( key );

      read( handle, ( VOID * )key, k * sizeof( KEYFILE ) );
		for ( n = 0; n < k; n++ )
		{
#ifdef _SCOTTGAME
      	GLB_DeCrypt( serial, ( BYTE * ) &key[ n ], sizeof( KEYFILE ) );
#endif
      	if ( key[ n ].opt == GLB_ENCODED )
         	ii->flags |= ITF_ENCODED;

      	ii->size    = key[ n ].filesize;
      	ii->offset  = key[ n ].offset;
      	memcpy( ii->name, key[ n ].name, sizeof( ii->name ) );
      	ii++;
		}
		j += k;
   }
}


/*************************************************************************
   GLB_UseVM - Use virtual memory functions for heap managment.
 *************************************************************************/
PUBLIC VOID
GLB_UseVM(
   void
   )
{
   fVmem = TRUE;
}
  
/*************************************************************************
   GLB_InitSystem() - Starts up .GLB file system
 *************************************************************************/
INT			// Returns number of GLB resources opened.
GLB_InitSystem (
CHAR  *exepath,      // INPUT: Where program was run from
INT   innum,         // INPUT: MAX .GLB FILES TO LOOK FOR
CHAR  *iprefix       // INPUT: FILENAME PREFIX ( NULL for "FILE" )
)
{
	int			opened;
	int			filenum;
   int         num;
	char 			*p;
   FILEDESC    *fd;
/*
* Extract path from program source of execution.
*/
	memset( exePath, 0, sizeof( exePath ) );
  	strcpy( exePath, exepath );
	if ( ( p = strrchr( exePath, '\\' ) ) != NULL )
		*( p + 1 ) = '\0';

   num_glbs = innum;
   ASSERT( num_glbs >= 1 && num_glbs <= MAX_GLB_FILES );
  
   if ( iprefix )
   {
      ASSERT( strlen( iprefix ) < sizeof( prefix ) - 1 );

      strcpy( prefix, iprefix );
      strupr ( prefix );
   }
   memset( filedesc, 0, sizeof( filedesc ) );
/*
* Next, read in header of each file and allocate cache
*/
   opened = 0;
   for ( filenum = 0; filenum < num_glbs; filenum++ )
   {
      fd = &filedesc[ filenum ];
      if ( ( num = GLB_NumItems( filenum ) ) != 0 )
      {
         fd->items = num;
		/*
		 * Note: calloc zeros out all memory that it allocates
		*/
         fd->item = ( ITEMINFO * ) calloc ( num, sizeof( ITEMINFO ) );
         if ( fd->item == NULL )
            EXIT_Error ("GLB_NumItems: memory ( init )");
		/*
		* Load Item Descriptor Table for file
		*/
         GLB_LoadIDT( fd );
			opened++;
      }
   }
   return ( opened );
}


/*************************************************************************
 GLB_Load() Loads a file to a pointer from a .GLB file
 *************************************************************************/
DWORD                         // RETURN : size of item read
GLB_Load(
BYTE *inmem,               // INPUT: memory pointer or NULL
INT  filenum,              // INPUT: file number
INT  itemnum               // INPUT: item in file number
)
{
   int      handle;
   ITEMINFO *ii;

   ASSERT( filenum >= 0 && filenum < num_glbs );

	handle = filedesc[ filenum ].handle;
	if ( handle == 0 )
		return 0;

   ASSERT( itemnum < ( WORD ) filedesc[ filenum ].items );

   ii = filedesc[ filenum ].item;
   ii += itemnum;

	if ( inmem != NULL )
	{
		if ( ii->vm_mem.obj != NULL && inmem != ii->vm_mem.obj )
			memcpy( inmem, ii->vm_mem.obj, ii->size );
		else
		{
			lseek( handle, ii->offset, SEEK_SET );
      	read( handle, ( VOID * ) inmem, ii->size );
#ifdef _SCOTTGAME
      	if ( ii->flags & ITF_ENCODED )
      	{
         	GLB_DeCrypt( serial, inmem, ii->size );
      	}
#endif
		}
	}
   return ii->size;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
 GLB_FetchItem() - Loads item into memory only if free core exists.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum { FI_CACHE, FI_DISCARD, FI_LOCK } FI_MODE;

PRIVATE VOID *
GLB_FetchItem(
DWORD handle,
FI_MODE	mode
)
{
   BYTE     *obj;
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
	{
      EXIT_Error ( "GLB_FetchItem: empty handle." );
      return NULL;
	}

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;

	if ( mode == FI_LOCK )
		ii->flags |= ITF_LOCKED;

   if ( ( obj = ii->vm_mem.obj ) == NULL )
	{
		ii->lock_cnt = 0;
      if ( ii->size == 0 )
         ii->vm_mem.obj = NULL;
      else
      {
         if ( fVmem )
			{
            obj = VM_Malloc( ii->size,
						( ii->flags & ITF_LOCKED ) ? NULL : &ii->vm_mem,
						( mode == FI_CACHE ) ? FALSE : TRUE );
			}
         else
			{
            obj = calloc( ii->size, sizeof( BYTE ) );
			}
			if ( mode == FI_LOCK )
				ii->lock_cnt = 1;
			ii->vm_mem.obj = obj;
			if ( obj != NULL )
			{
	         GLB_Load( obj, itm.id.filenum, itm.id.itemnum );
			}
      }
	}
	else if ( mode == FI_LOCK && fVmem )
	{
		ii->lock_cnt++;
		VM_Lock( obj );
	}
	if ( ii->vm_mem.obj == NULL && mode != FI_CACHE )
	{
      EXIT_Error ( "GLB_FetchItem: failed on %d bytes, mode=%d.", ii->size, mode );
	}
	if ( mode == FI_DISCARD && fVmem )
		VM_Touch( &ii->vm_mem );

	return ii->vm_mem.obj;
}

/***************************************************************************
 GLB_CacheItem() - Loads item into memory only if free core exists.
 ***************************************************************************/
VOID *
GLB_CacheItem(
DWORD handle
)
{
	return GLB_FetchItem( handle, FI_CACHE );
}

/***************************************************************************
 GLB_GetItem() - Loads and allocates memory for a .GLB item
 ***************************************************************************/
BYTE *
GLB_GetItem (
DWORD handle               // INPUT : handle of item
)
{
	return GLB_FetchItem( handle, FI_DISCARD );
}
  
/***************************************************************************
 GLB_LockItem () - Keeps Item From being discarded.
 ***************************************************************************/
VOID *
GLB_LockItem(
DWORD handle
)
{
	return GLB_FetchItem( handle, FI_LOCK );
}

/***************************************************************************
 GLB_UnlockItem () - Allows item to be discarded from memory.
 ***************************************************************************/
VOID
GLB_UnlockItem(
DWORD handle
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return;

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;

	if ( ii->vm_mem.obj != NULL && fVmem )
	{
		if ( ii->lock_cnt )
		{
			ii->lock_cnt--;
			if ( ii->lock_cnt )
				return;
		}
	   ii->flags &= ~ITF_LOCKED;
		VM_Unlock( ii->vm_mem.obj, &ii->vm_mem );
	}
	else
	{
	   ii->flags &= ~ITF_LOCKED;
	}
}

/***************************************************************************
 GLB_IsLabel () - tests to see if ID is a label or an Item
 ***************************************************************************/
BOOL                       // RETURN: TRUE = Label
GLB_IsLabel (
DWORD handle               // INPUT : handle of item
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return FALSE;

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;

	return ( ii->size == 0 ? TRUE : FALSE );
}

/***************************************************************************
 GLB_ReadItem() - Loads Item into user memory for a .GLB item
 ***************************************************************************/
VOID
GLB_ReadItem (
DWORD handle,              // INPUT : handle of item
BYTE  *mem                 // INPUT : pointer to memory
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return;

	ASSERT( mem != NULL );

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;
	if ( mem != NULL )
	{
		GLB_Load( mem, itm.id.filenum, itm.id.itemnum );
	}
}
  
/***************************************************************************
   GLB_GetItemID () - Gets Item ID from the text name
 ***************************************************************************/
DWORD                      // RETURN: Handle
GLB_GetItemID (
CHAR * in_name             // INPUT : pointer to text name
)
{
   ITEMINFO    *ii;
	ITEM_H		itm;
   INT         filenum;
   INT         itemnum;
   INT         maxloop;
  
	ASSERT( in_name != NULL );

	itm.handle = ~0;
	if ( *in_name != ' ' && *in_name != '\0' )
	{
   	for ( filenum = 0; filenum < num_glbs; filenum++ )
   	{
      	maxloop = filedesc[ filenum ].items;
      	ii = filedesc[ filenum ].item;
      	for ( itemnum = 0; itemnum < maxloop; itemnum++ )
      	{
         	if ( stricmp( ii->name, in_name ) == 0 )
         	{
					itm.id.filenum = filenum;
					itm.id.itemnum = itemnum;
					return itm.handle;
         	}
         	ii++;
      	}
   	}
	}
	return itm.handle;
}

#if 0  
/***************************************************************************
 GLB_GetPtr() - Returns a pointer to item ( handle )
 ***************************************************************************/
BYTE *                        // RETURN: pointer to item
GLB_GetPtr (
INT handle                 // INPUT : handle of item
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return NULL;

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;
	return ii->memory;
}
#endif 

/***************************************************************************
 GLB_FreeItem() - Frees memory for items and places items < MAX SIZE
 ***************************************************************************/
VOID
GLB_FreeItem (
DWORD handle               // INPUT : handle of item
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return;

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   if ( itm.id.itemnum >= ( WORD ) filedesc[ itm.id.filenum ].items )
   {
      EXIT_Error( "GLB_FreeItem - item out of range: %d > %d file %d.\n",
            itm.id.itemnum, filedesc[ itm.id.filenum ].items, itm.id.filenum );
   }
//   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;
	if ( ii->vm_mem.obj != NULL )
	{
		ii->flags &= ~ITF_LOCKED;
		if ( fVmem )
			VM_Free( ii->vm_mem.obj );
		else
			free( ii->vm_mem.obj );
		ii->vm_mem.obj = NULL;
	}
}
  
/***************************************************************************
 GLB_FreeAll() - Frees All memory used by GLB items
 ***************************************************************************/
VOID
GLB_FreeAll (
VOID
)
{
	int		filenum;
	int		itemnum;
   ITEMINFO *ii;

	for ( filenum = 0; filenum < num_glbs; filenum++ )
	{
		ii = filedesc[ filenum ].item;
		for ( itemnum = 0; itemnum < filedesc[ filenum ].items; itemnum++ )
		{
			if ( ii->vm_mem.obj && ( ii->flags & ITF_LOCKED ) == 0 )
			{
				if ( fVmem )
					VM_Free( ii->vm_mem.obj );
				else
					free( ii->vm_mem.obj );
				ii->vm_mem.obj = NULL;
			}
			ii++;
		}
	}
}
 
/***************************************************************************
 GLB_ItemSize() - Returns Size of Item
 ***************************************************************************/
INT                        // RETURN: sizeof ITEM
GLB_ItemSize (
DWORD handle               // INPUT : handle of item
)
{
   ITEM_H   itm;
   ITEMINFO *ii;
  
   if ( handle == ~0 )
      return 0;

   itm.handle = handle;

   ASSERT( itm.id.filenum < ( WORD ) num_glbs );
   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

   ii = filedesc[ itm.id.filenum ].item;
   ii += itm.id.itemnum;
	return ii->size;
}
 
/***************************************************************************
   GLB_ReadFile () reads in a normal file
 ***************************************************************************/
INT                            // RETURN: size of record
GLB_ReadFile (
CHAR *name,                 // INPUT : filename
VOID *buffer                // OUTPUT: pointer to buffer or NULL
)
{
	CHAR	  fqp[ _MAX_PATH ];
   INT     handle;
   DWORD   sizerec;

  	if ( access( name, 0 ) == -1 )
	{
		strcpy( fqp, exePath );
		strcat( fqp, name );
		name = fqp;
	}
   if ( ( handle = open ( name,O_RDONLY | O_BINARY ) ) == -1 )
      EXIT_Error ("LoadFile: Open failed!");
  
   sizerec = filelength ( handle );
  
   if ( buffer && sizerec )
   {
      if ( ! read ( handle, buffer, sizerec ) )
      {
         close ( handle );
         EXIT_Error ("GLB_LoadFile: Load failed!");
      }
   }
  
   close (handle);
  
   return ( sizerec );
}
 
/***************************************************************************
   GLB_SaveFile () saves buffer to a normal file ( filename )
 ***************************************************************************/
VOID
GLB_SaveFile (
CHAR *name,                // INPUT : filename
VOID *buffer,              // INPUT : pointer to buffer
DWORD length               // INPUT : length of buffer
)
{
   INT   handle;
  
   if ( ( handle = open ( name, O_BINARY | O_WRONLY | O_CREAT |
      O_TRUNC,S_IREAD | S_IWRITE ) ) == -1 )
      EXIT_Error ("SaveFile: Open failed!");
  
   if ( length )
   {
      if (!write(handle,buffer,length))
      {
         close (handle);
         EXIT_Error ("GLB_SaveFile: Write failed!");
      }
   }
   close (handle);
}
