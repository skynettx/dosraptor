/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// Module MUST be compiled with structure allignment set to a maximum
// of 1 byte ( zp1 ).

#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "gusmidi.h"

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

#if defined(__WATCOMC__) && defined(__FLAT__)
   #define LOADDS _loadds
   #define FAR
   typedef void ( __interrupt __far *PRI )();
   #define outportb outp
   #define enable _enable
   #define disable _disable
   #define farfree free
   #define farmalloc malloc
#else
   #define LOADDS
   #define FAR far
   typedef void ( interrupt *PRI )();
#endif

#include "newgf1.h"

#define	BUFFER 2048U     /* size of DMA buffer for patch loading */

#define MAX_MEM_CONFIG   3

#define NUM_PATCHES 256  /* size of patch array (128 perc, 128 melodic) */
#define BIGGEST_NAME 9   /* size of largest patch name */

#define PATCH_LOADED 1   /* patch loading flags */

#define UNUSED_PATCH -1

static struct patch       new_patch[ NUM_PATCHES ];
static unsigned char FAR *patch_waves[ NUM_PATCHES ];

static int   patch_map[ NUM_PATCHES ][ MAX_MEM_CONFIG + 1 ];
static char  program_name[ NUM_PATCHES ][ BIGGEST_NAME ];
static char  patch_flags[ NUM_PATCHES ];

// DEBUG
// static char  config_name[] = "ULTRASND.INI";

static char  config_name[] = "ULTRAMID.INI";
static char  config_dir[ 80 ] =
   {
   '\0'
   };

static char  ultradir[ 80 ];   /* The name of the configuration directory */
static char *hold_buffer;

static int GUSMIDI_Volume = 255;

static unsigned long GUS_TotalMemory;
static int           GUS_MemConfig;

static int GUS_Installed = 0;

int GUS_ErrorCode = GUS_Ok;
int GUS_AuxError  = 0;

#define GUS_SetErrorCode( status ) \
   GUS_ErrorCode   = ( status );


/*---------------------------------------------------------------------
   Function: GUS_ErrorString

   Returns a pointer to the error message associated with an error
   number.  A -1 returns a pointer the current error.
---------------------------------------------------------------------*/

char *GUS_ErrorString
   (
   int ErrorNumber
   )

   {
   char *ErrorString;

   switch( ErrorNumber )
      {
      case GUS_Error :
         ErrorString = GUS_ErrorString( GUS_ErrorCode );
         break;

      case GUS_Ok :
         ErrorString = "Ultrasound music ok.";
         break;

      case GUS_OutOfMemory :
         ErrorString = "Out of memory in GusMidi.";
         break;

      case GUS_OutOfDosMemory :
         ErrorString = "Out of conventional (640K) memory in GusMidi.";
         break;

      case GUS_GF1Error :
         ErrorString = gf1_error_str( GUS_AuxError );
         break;

      case GUS_InvalidIrq :
         ErrorString = "Ultrasound IRQ must be 7 or less.";
         break;

      case GUS_ULTRADIRNotSet :
         ErrorString = "ULTRADIR environment variable not set.";
         break;

      case GUS_MissingConfig :
         ErrorString = "Can't find ULTRASND.INI file.";
         break;

      case GUS_FileError :
         ErrorString = strerror( GUS_AuxError );
         break;

      default :
         ErrorString = "Unknown Ultrasound error code.";
         break;
      }

   return( ErrorString );
   }


/*
int GUS_GetPatchMap
   (
   char *name
   )

   {
   char text[ 80 ];
   char FAR *ud;
   int  index;
   char pname[ 80 ];
   FILE *fp;

   ud = getenv( "ULTRADIR" );
   if ( ud == NULL )
      {
      GUS_SetErrorCode( GUS_ULTRADIRNotSet );
      return( GUS_Error );
      }

   strcpy( ultradir, ud );
   strcat( ultradir, "\\midi\\" );
   strcpy( config_dir, ud );
   strcat( config_dir, "\\" );
   strcpy( text, config_dir );
   strcat( text, name );

   fp = fopen( text, "r" );
   if ( fp == NULL )
      {
      GUS_SetErrorCode( GUS_MissingConfig );
      return( GUS_Error );
      }

   while( 1 )
      {
      if ( fgets( text, 80, fp ) == NULL )
         {
         break;
         }

      if ( strncmp( text, "[Drum Patches]", 13 ) == 0 )
         {
         while( 1 )
            {
            if ( fgets( text, 80, fp ) == NULL )
               {
               break;
               }

            if ( text[ 0 ] == ';' )
               {
               continue;
               }

            if ( text[ 0 ] == '[' )
               {
               break;
               }

            if ( sscanf( text, "%d=%s\n", &index, pname ) != 2 )
               {
               continue;
               }

            strcpy( program_name[ index + 128 ], pname );
            }

         break;
         }
      }

   if ( fseek( fp, 0L, SEEK_SET ) )
      {
      GUS_AuxError = errno;
      GUS_SetErrorCode( GUS_FileError );
      return( GUS_Error );
      }

   while( 1 )
      {
      if ( fgets( text, 80, fp ) == NULL )
         {
         break;
         }

      if ( strncmp( text, "[Melodic Patches]", 17 ) == 0 )
         {
         while( 1 )
            {
            if ( fgets( text, 80, fp ) == 0 )
               {
               break;
               }

            if ( text[ 0 ] == ';' )
               {
               continue;
               }

            if ( text[ 0 ] == '[' )
               {
               break;
               }

            if ( sscanf( text, "%d=%s\n", &index, pname ) != 2 )
               {
               continue;
               }

            strcpy( program_name[ index ], pname );
            }
         break;
         }
      }

   fclose( fp );

   return( GUS_Ok );
   }
*/

int GUS_GetPatchMap
   (
   char *name
   )

   {
   char text[ 80 ];
   char FAR *ud;
   int  index;
   int  ignore;
   FILE *fp;

   for( index = 0; index < NUM_PATCHES; index++ )
      {
      patch_map[ index ][ 0 ] = UNUSED_PATCH;
      patch_map[ index ][ 1 ] = UNUSED_PATCH;
      patch_map[ index ][ 2 ] = UNUSED_PATCH;
      patch_map[ index ][ 3 ] = UNUSED_PATCH;
      program_name[ index ][ 0 ] = 0;
      }

   ud = getenv( "ULTRADIR" );
   if ( ud == NULL )
      {
      GUS_SetErrorCode( GUS_ULTRADIRNotSet );
      return( GUS_Error );
      }

   strcpy( ultradir, ud );
   strcat( ultradir, "\\midi\\" );
   strcpy( config_dir, ud );
   strcat( config_dir, "\\midi\\" );
   strcpy( text, ultradir );
   strcat( text, name );

   fp = fopen( text, "r" );
   if ( fp == NULL )
      {
      GUS_SetErrorCode( GUS_MissingConfig );
      return( GUS_Error );
      }

   while( 1 )
      {
      if ( fgets( text, 80, fp ) == NULL )
         {
         break;
         }

      if ( text[ 0 ] == '#' )
         {
         continue;
         }

      if ( sscanf( text, "%d", &index ) != 1 )
         {
         continue;
         }

      sscanf( text, "%d, %d, %d, %d, %d, %s\n", &ignore,
         &patch_map[ index ][ 0 ], &patch_map[ index ][ 1 ],
         &patch_map[ index ][ 2 ], &patch_map[ index ][ 3 ],
         program_name[ index ] );
      }

   fclose( fp );

   return( GUS_Ok );
   }

int GUSMIDI_UnloadPatch
   (
   int prognum
   )

   {
   int  prog;

   prog = patch_map[ prognum ][ GUS_MemConfig ];

   if ( patch_flags[ prog ] & PATCH_LOADED )
      {
      disable();

      gf1_unload_patch( &new_patch[ prog ] );
      if ( patch_waves[ prog ] != NULL )
         {
         free( patch_waves[ prog ] );
         patch_waves[ prog ] = NULL;
         }

      /* just in case sequence is still playing */
      new_patch[ prog ].nlayers = 0;
      patch_flags[ prog ] &= ~PATCH_LOADED;

      enable();
      }

   return( GUS_Ok );
   }

int GUSMIDI_LoadPatch
   (
   int prognum
   )

   {
   int  prog;
   char text[ 80 ];
   int  ret;
   unsigned char FAR *wave_buff;
   struct patchinfo  patchi;

   prog = patch_map[ prognum ][ GUS_MemConfig ];

   if ( ( patch_flags[ prog ] & PATCH_LOADED ) || ( prog == UNUSED_PATCH ) )
      {
      return( GUS_Ok );
      }

   if ( !program_name[ prog ][ 0 ] )
      {
      return( GUS_Ok );
      }

   strcpy( text, ultradir );
   strcat( text, program_name[ prog ] );
   strcat( text, ".pat" );

   ret = gf1_get_patch_info( text, &patchi );
   if ( ret != OK )
      {
      GUS_AuxError = ret;
      GUS_SetErrorCode( GUS_GF1Error );
      return( GUS_Error );
      }

   wave_buff = ( unsigned char FAR * )malloc( patchi.header.wave_forms *
      sizeof( struct wave_struct ) );

   if ( wave_buff == NULL )
      {
      GUS_SetErrorCode( GUS_OutOfMemory );
      return( GUS_Error );
      }

   ret = gf1_load_patch( text, &patchi, &new_patch[ prog ], hold_buffer,
      BUFFER, ( unsigned char FAR * )wave_buff, PATCH_LOAD_8_BIT );

   if ( ret != OK )
      {
      free( wave_buff );
      GUS_AuxError = ret;
      GUS_SetErrorCode( GUS_GF1Error );
      return( GUS_Error );
      }

   patch_waves[ prog ]  = wave_buff;
   patch_flags[ prog ] |= PATCH_LOADED;

   return( GUS_Ok );
   }

void *D32DosMemAlloc
   (
   unsigned size
   )

   {
   #if defined(__WATCOMC__) && defined(__FLAT__)
      union REGS r;

      r.x.eax = 0x0100;              /* DPMI allocate DOS memory */
      r.x.ebx = ( size + 15 ) >> 4;  /* Number of paragraphs requested */
      int386( 0x31, &r, &r );
      if ( r.x.cflag )
         {
         return( NULL );   /* Failed */
         }

      return( ( void * )( ( r.x.eax & 0xFFFF ) << 4 ) );
   #else
      return( ( void * )( malloc( size ) ) );
   #endif
   }

void GUSMIDI_ProgramChange
   (
   int channel,
   int prognum
   )

   {
   int  prog;

   prog = patch_map[ prognum ][ GUS_MemConfig ];

   if ( patch_flags[ prog ] & PATCH_LOADED )
      {
      gf1_midi_change_program( &new_patch[ prog ], channel );
      }
   else
      {
      gf1_midi_change_program( NULL, channel );
      }
   }

void GUSMIDI_NoteOn
   (
   int chan,
   int note,
   int velocity
   )

   {
   int prog;

   if ( chan == 9 )
      {
      prog = patch_map[ note + 128 ][ GUS_MemConfig ];

      if ( patch_flags[ prog ] & PATCH_LOADED )
         {
         gf1_midi_note_on( &new_patch[ note + 128 ], 1,
            note, velocity, 9 );
         }
      }
   else
      {
      gf1_midi_note_on( 0L, 1, note, velocity, chan );
      }
   }

#pragma warn -par

void GUSMIDI_NoteOff
   (
   int chan,
   int note,
   int velocity
   )

   {
   gf1_midi_note_off( note, chan );
   }

#pragma warn .par

void GUSMIDI_ControlChange
   (
   int channel,
   int number,
   int value
   )

   {
   gf1_midi_parameter( channel, number, value );
   }

void GUSMIDI_PitchBend
   (
   int channel,
   int lsb,
   int msb
   )

   {
   gf1_midi_pitch_bend( channel, lsb, msb );
   }

void GUSMIDI_ReleasePatches
   (
   void
   )

   {
   int i;

   for( i = 0; i < 256; i++ )
      {
      GUSMIDI_UnloadPatch( i );
      }
   }

void GUSMIDI_SetVolume
   (
   int volume
   )

   {
   volume = max( 0, volume );
   volume = min( volume, 255 );

   GUSMIDI_Volume = volume;
   gf1_midi_master_volume( volume >> 1 ); /* range = 0 to 127 */
   }

int GUSMIDI_GetVolume
   (
   void
   )

   {
   return( GUSMIDI_Volume );
   }

#pragma on (check_stack);

int GUS_Init
   (
   void
   )

   {
   struct load_os os;
   int ret;

   if ( GUS_Installed > 0 )
      {
      GUS_Installed++;
      return( GUS_Ok );
      }

   GUS_SetErrorCode( GUS_Ok );

   GUS_Installed = 0;

   GetUltraCfg( &os );

   #if defined(__WATCOMC__) && defined(__FLAT__)
      if ( os.forced_gf1_irq > 7 )
         {
         GUS_SetErrorCode( GUS_InvalidIrq );
         return( GUS_Error );
         }
   #endif

   os.voices = 24;
   ret = gf1_load_os( &os );
   if ( ret )
      {
      GUS_AuxError = ret;
      GUS_SetErrorCode( GUS_GF1Error );
      return( GUS_Error );
      }

   GUS_Installed = 1;
   return( GUS_Ok );
   }

void GUS_Shutdown
   (
   void
   )

   {
   GUS_SetErrorCode( GUS_Ok );

   if ( GUS_Installed > 0 )
      {
      //printf( "GUS_Installed--;\n" );
      GUS_Installed--;
      if ( GUS_Installed == 0 )
         {
         //printf( "gf1_unload_os();\n" );
         gf1_unload_os();
         }
      }
   //printf( "Gus shutdown exit\n" );
   }

int GUSMIDI_Init
   (
   void
   )

   {
   int ret;
   int i;
   int startmem;

   ret = GUS_Init();
   if ( ret != GUS_Ok )
      {
      return( ret );
      }

   GUS_TotalMemory = gf1_mem_avail();
   GUS_MemConfig   = ( GUS_TotalMemory - 1 ) >> 18;
//   //printf( "Totalmem = %d\n", GUS_TotalMemory );
//   //printf( "Config # = %d\n", GUS_MemConfig );
//   getch();

   if ( GUS_MemConfig < 0 )
      {
      GUS_MemConfig = 0;
      }

   if ( GUS_MemConfig > MAX_MEM_CONFIG )
      {
      GUS_MemConfig = MAX_MEM_CONFIG;
      }

   hold_buffer = D32DosMemAlloc( BUFFER );
   if ( hold_buffer == NULL )
      {
      gf1_unload_os();
      GUS_SetErrorCode( GUS_OutOfDosMemory );
      return( GUS_Error );
      }

   for( i = 0; i < NUM_PATCHES; i++ )
      {
      program_name[ i ][ 0 ] = '\0';
      patch_waves[ i ]       = NULL;
      patch_flags[ i ]       = 0;
      }

   GUSMIDI_SetVolume( 255 );

   /*
   ** GUS_GetPatchMap reads a list of patches and the
   ** associated program numbers out of a configuration file.
   */
   ret = GUS_GetPatchMap( config_name );
   if ( ret != GUS_Ok )
      {
      return( ret );
      }

   startmem = gf1_mem_avail();
   for( i = 0; i < NUM_PATCHES; i++ )
      {
      ret = GUSMIDI_LoadPatch( i );
//      //printf( "%d, %d\n", i, gf1_mem_avail() );

      if ( ret != GUS_Ok )
         {
//         //printf( "Error!\n" );
//         getch();
//      GUS_SetErrorCode( GUS_OutOfDRAM );
//      return( GUS_Error );
         return( ret );
         }
      }

//   //printf( "mem available %d\n", gf1_mem_avail() );
//   getch();
   return( GUS_Ok );
   }

void GUSMIDI_Shutdown
   (
   void
   )

   {
   //printf( "Release Patches\n" );
   GUSMIDI_ReleasePatches();
   //printf( "Shutdown\n" );
   GUS_Shutdown();
   //printf( "Return\n" );
   }
