#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
  
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"

#define MAX_SHADES 8

PRIVATE BYTE *          stable[ MAX_SHADES ];
PRIVATE BYTE            stmem [ MAX_SHADES ] [ 512 ];

/***************************************************************************
FLAME_Init () - Inits Flame Tables and stuff
 ***************************************************************************/
VOID
FLAME_Init (
VOID
)
{
   INT loop;

   for ( loop = 0; loop < MAX_SHADES; loop++ )
   {
      stable [ loop ] = stmem [ loop ];

      stable [ loop ] = (BYTE *)(((INT)stable[loop]+255)&~0xff);

      GFX_MakeLightTable ( palette, stable[loop], (MAX_SHADES-loop)*2 );
   }
}

/***************************************************************************
FLAME_InitShades () - Inits shading stuff
 ***************************************************************************/
VOID
FLAME_InitShades (
VOID
)
{
   INT loop;

   for ( loop = 0; loop < MAX_SHADES; loop++ )
      GFX_MakeLightTable ( palette, stable[loop], (MAX_SHADES-loop)*2 );
}

/***************************************************************************
FLAME_Up () - Shows Flame shooting upward
 ***************************************************************************/
VOID
FLAME_Up (
INT ix,                    // INPUT : x position
INT iy,                    // INPUT : y position
INT width,                 // INPUT : width of shade
INT frame                  // INPUT : frame
)
{
   INT height [ 2 ] = { 5, 10 };
   BYTE * outbuf;
   INT y;
   INT loop;
   DWORD curs;
   DWORD addx;
   INT   num;

   if ( opt_detail < 1 )   return;

   frame = frame % 2;

   iy -= ( height [ frame ] - 1 );

   if ( GFX_ClipLines ( NUL, &ix, &iy, &width, &height [ frame ] ) )
   {
      y = iy;

      addx  = ( MAX_SHADES << 16 ) / height [ frame ];
      curs  = addx * ( height [ frame ] - 1 );

      for ( loop = 0; loop < height [ frame ]; loop++ )
      {
         outbuf = displaybuffer + ix + ylookup [ y ];

         y++;

         num = ( curs>>16 );

         if ( num >= 8 )
         {
            EXIT_Error ("flame > 8 %u", curs>>16 );
         }

         if ( num < 0 )
         {
            EXIT_Error ("flame < 0");
         }

         GFX_Shade ( outbuf, width, stable [ num ] );

         curs -= addx;
      }
   }
}
  
/***************************************************************************
FLAME_Down () - Shows Flame shooting downward
 ***************************************************************************/
VOID
FLAME_Down (
INT ix,                    // INPUT : x position
INT iy,                    // INPUT : y position
INT width,                 // INPUT : width of shade
INT frame                  // INPUT : frame
)
{
   INT height [ 2 ] = { 8, 12 };
   BYTE * outbuf;
   INT y;
   INT loop;
   DWORD curs;
   DWORD addx;

   frame = frame % 2;

   if ( GFX_ClipLines ( NUL, &ix, &iy, &width, &height [ frame ] ) )
   {
      y = iy;

      curs  = 0;
      addx  = ( MAX_SHADES << 16 ) / height [ frame ];

      for ( loop = 0; loop < height [ frame ]; loop++ )
      {
         outbuf = displaybuffer + ix + ylookup [ y ];

         y++;

         GFX_Shade ( outbuf, width, stable [ ( curs>>16 ) ] );

         curs += addx;
      }
   }
}

