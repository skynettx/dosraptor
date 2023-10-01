/****************************************************************************
* GFXapi  - Graphic Library for 320x200 256 color vga/mcga
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
*
* CONTENTS: gfxapi.c gfxapi_a.asm gfxapi.h
*
* EXTERN MODULES - Timer Int routines to call GFX_TimeFrameRate at about
*                  the refresh rate of the monitor
*
*---------------------------------------------------------------------------*/

#include <graph.h>  
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <malloc.h>
#include <string.h>
  
#include "gfxapi.h"
#include "exitapi.h"
#include "tsmapi.h"
#include "dpmiapi.h"
  
#define G3D_DIST 200

PUBLIC   volatile INT framecount = 0;
PUBLIC   INT      g_rseed        = 1;
PUBLIC   DWORD    ylookup [ SCREENHEIGHT ];
PUBLIC   BYTE *   displaybuffer  = (BYTE *)0x0000;
PUBLIC   BYTE *   displayscreen  = (BYTE *)0xa0000;
PUBLIC   BOOL     update_start   = FALSE;
PUBLIC   INT      ud_x           = 0;
PUBLIC   INT      ud_y           = 0;
PUBLIC   INT      ud_lx          = 0;
PUBLIC   INT      ud_ly          = 0;
PUBLIC   INT      o_ud_x         = 0;
PUBLIC   INT      o_ud_y         = 0;
PUBLIC   INT      o_ud_lx        = 0;
PUBLIC   INT      o_ud_ly        = 0;
PUBLIC   INT      stable[324];
PUBLIC   INT      tablelen;
PUBLIC   INT      fontspacing    = 1;
PUBLIC   BYTE  *  ltable;
PUBLIC   BYTE  *  dtable;
PUBLIC   BYTE  *  gtable;
PUBLIC   VOID     (*framehook)(VOID (*)(void)) = (VOID (*))0;
PUBLIC   BOOL     retraceflag  =  TRUE;

PUBLIC   INT      G3D_x        =  0;         // input: x position
PUBLIC   INT      G3D_y        =  0;         // input: y position
PUBLIC   INT      G3D_z        =  0;         // input: z position
PUBLIC   INT      G3D_screenx  =  0;         // output: screen x pos
PUBLIC   INT      G3D_screeny  =  0;         // output: screen y pos
PUBLIC   INT      G3D_viewx    =  159;       // user view x pos
PUBLIC   INT      G3D_viewy    =  99;        // user view y pos
PUBLIC   INT      G3D_viewz    =  G3D_DIST;  // user view z pos

PRIVATE  BOOL     gfxdebug     =  FALSE;
PRIVATE  BYTE     tpal1[768];
PRIVATE  BYTE     tpal2[768];

PRIVATE  DWORD    tsm_id;
PRIVATE  INT      start_lookup = 0;
PRIVATE  INT      end_lookup = 255;

// USED TO PASS STUFF TO GFXAPI_A.ASM
PUBLIC   BYTE *   gfx_inmem   = 0;
PUBLIC   INT      gfx_xp      = 0;
PUBLIC   INT      gfx_yp      = 0;
PUBLIC   INT      gfx_lx      = 0;
PUBLIC   INT      gfx_ly      = 0;
PUBLIC   INT      gfx_imga    = 0;

/*==========================================================================
   GFX_TimeFrameRate () - Should be interrupt called at 70 fps
 ==========================================================================*/
TSMCALL INT
GFX_TimeFrameRate (
VOID
)
{
   framecount++;
   return(0);
}

/***************************************************************************
GFX_SetDebug () - Sets Debug mode
 ***************************************************************************/
VOID
GFX_SetDebug (
BOOL flag                  // INPUT : TRUE/FALSE
)
{
   gfxdebug = flag;
}
  
/*************************************************************************
   GFX_ClipLines ()
 *************************************************************************/
INT                        // RETURN: 0 = Off, 1 == No Clip , 2 == Cliped
GFX_ClipLines (
BYTE **  image,            // INOUT : pointer to image or NUL
INT  *   x,                // INOUT : pointer to x pos
INT  *   y,                // INOUT : pointer to y pos
INT  *   lx,               // INOUT : pointer to width
INT  *   ly                // INOUT : pointer to length
)
{
   INT rval = 1;

   if ( *x >= SCREENWIDTH ) return ( 0 );
   if ( *y >= SCREENHEIGHT ) return ( 0 );
   if ( *x + *lx <= 0 ) return ( 0 );
   if ( *y + *ly <= 0 ) return ( 0 );
  
   if ( *y < 0 )
   {
      rval = 2;
      if ( image ) *image += ( -*y * *lx );
      *ly += *y;
      *y = 0;
   }

   if ( *y + *ly > SCREENHEIGHT )
   {
      rval = 2;
      *ly = SCREENHEIGHT - *y;
   }
  
   if ( *x < 0 )
   {
      rval = 2;
      if ( image ) *image += -*x;
      *lx += *x;
      *x = 0;
   }

   if ( *x + *lx > SCREENWIDTH )
   {
      rval = 2;
      *lx = SCREENWIDTH - *x;
   }
  
   return ( rval );
}
  
/**************************************************************************
   GFX_SetVideoMode13() - sets 256 color 320x200 mode
 **************************************************************************/
VOID
GFX_SetVideoMode13(
VOID
)
{
   union REGS      r;
  
   r.w.ax = 0x13;
   int386 (0x10,(const union REGS *)&r,&r);
}
  
/**************************************************************************
   GFX_RestoreMode() - Restores Original video mode
 **************************************************************************/
VOID
GFX_RestoreMode(
VOID
)
{
   union REGS      r;
  
   r.w.ax = 0x03;
   int386 (0x10,(const union REGS *)&r,&r);
}

/**************************************************************************
GFX_SetPalette() - Sets VGA palette
 **************************************************************************/
VOID
GFX_SetPalette (
BYTE * palette,
INT start_pal
)
{
   volatile INT num = 0;

   palette += ( start_pal * 3 );

   if ( retraceflag )
   {
      retrace1:
         num = inp ( 0x3DA );
         if ( num & 8 ) goto retrace1;
      retrace2:
         num = inp ( 0x3DA );
         if ( !( num & 8 ) ) goto retrace2;
   }

   outp ( 0x3C8, start_pal );

   start_pal = ( 256 - start_pal ) * 3;

   while ( start_pal-- )
   {
      outp ( 0x3C9, *palette++ );
   }
}

/**************************************************************************
  GFX_InitSystem() - allocates buffers, makes tables, does not set vmode
 **************************************************************************/
VOID
GFX_InitSystem (
VOID
)
{
   CHAR * err = "GFX_Init() - DosMemAlloc";
   INT   loop;
   DWORD segment;

   if ( _dpmi_dosalloc ( 4000, &segment ) ) EXIT_Error(err);
   displaybuffer = ( BYTE *)( segment<<4 );

   _dpmi_lockregion( displaybuffer, 64000 );
   memset ( displaybuffer, 0, 64000 );
  
	tsm_id = TSM_NewService( GFX_TimeFrameRate, 70, 255, 0 );

   for ( loop = 0; loop < SCREENHEIGHT; loop++ )
      ylookup[loop] = SCREENWIDTH * loop;

   if ( _dpmi_dosalloc ( 32, &segment ) ) EXIT_Error(err);
   ltable = ( BYTE *)( segment<<4 );
   ltable = (BYTE *)(((INT)ltable+255)&~0xff);

   if ( _dpmi_dosalloc ( 32, &segment ) ) EXIT_Error(err);
   dtable = ( BYTE *)( segment<<4 );
   dtable = (BYTE *)(((INT)dtable+255)&~0xff);

   if ( _dpmi_dosalloc ( 32, &segment ) ) EXIT_Error(err);
   gtable = ( BYTE *)( segment<<4 );
   gtable = (BYTE *)(((INT)gtable+255)&~0xff);

   displayscreen = (BYTE *)0xa0000;
}

/**************************************************************************
GFX_InitVideo() - Inits things related to Video, and sets up fade tables
 **************************************************************************/
VOID
GFX_InitVideo (
BYTE * curpal
)
{
   GFX_SetVideoMode13();
   GFX_SetPalette ( curpal, 0 );

   GFX_MakeLightTable ( curpal, ltable, 9 );
   GFX_MakeLightTable ( curpal, dtable, -9 );
   GFX_MakeGreyTable ( curpal, gtable );
}
  
/**************************************************************************
  GFX_EndSystem() - Frees up all resources used by GFX system
 **************************************************************************/
VOID
GFX_EndSystem (
VOID
)
{
	 TSM_DelService( tsm_id );

   memset ( displayscreen, 0, 64000 );

   GFX_RestoreMode();
}
  
/**************************************************************************
  GFX_GetPalette() - Sets 256 color palette
 **************************************************************************/
VOID
GFX_GetPalette (
BYTE * curpal              // OUTPUT : pointer to palette data
)
{
   INT  loop;
  
   outp ( 0x3c7, 0 );

   for ( loop = 0; loop < 768; loop ++ )
      *curpal++ =  inp ( 0x3c9 );
}

/**************************************************************************
 GFX_FadeOut () - Fade Palette out to ( Red, Green , and Blue Value
 **************************************************************************/
VOID
GFX_FadeOut (
   INT red,                // INPUT : red ( 0 - 63 )
   INT green,              // INPUT : green ( 0 - 63 )
   INT blue,               // INPUT : blue ( 0 - 63 )
   INT steps               // INPUT : steps of fade ( 0 - 255 )
   )
{
   volatile INT   loop, i;
   BYTE     pal1[769];
   BYTE     pal2[769];
   BYTE *   optr;
   BYTE *   nptr;
   INT      num;
   INT      delta;

   GFX_GetPalette ( pal1 );
   memcpy ( pal2, pal1, 768 );

   for ( loop = 0; loop < steps; loop++ )
   {
      optr     = pal1;
      nptr     = pal2;

      for ( i = 0; i < 256; i++ )
      {
         num  = *optr++;
         delta = red - num;
         *nptr++ = num + delta * loop / steps;
         
         num  = *optr++;
         delta = green - num;
         *nptr++ = num + delta * loop / steps;

         num  = *optr++;
         delta = blue - num;
         *nptr++ = num + delta * loop / steps;
      }

      GFX_SetPalette ( pal2, 0 );
   }

   nptr = pal2;
   for ( loop = 0; loop < 256; loop++ )
   {
      *nptr++ = red;
      *nptr++ = green;
      *nptr++ = blue;
   }

   GFX_SetPalette ( pal2, 0 );

}

/**************************************************************************
 GFX_FadeIn () - Fades From current palette to new palette
 **************************************************************************/
VOID
GFX_FadeIn (
   BYTE * palette,         // INPUT : palette to fade into
   INT steps               // INPUT : steps of fade ( 0 - 255 )
   )
{
   volatile INT      loop, i;
   BYTE     pal1[768];
   BYTE     pal2[768];
   INT      delta;

   GFX_GetPalette ( pal1 );
   memcpy ( pal2, pal1, 768 );

   for ( loop = 0; loop < steps; loop++ )
   {
      for ( i = 0; i < 768; i++ )
      {
         delta = palette[ i ] - pal1[ i ];
         pal2[ i ] = pal1[ i ] + delta * loop / steps;
      }

      GFX_SetPalette ( pal2, 0 );
   }

   GFX_SetPalette ( palette, 0 );
}

/**************************************************************************
GFX_FadeStart () - Sets up fade for GFX_FadeFrame()
 **************************************************************************/
VOID
GFX_FadeStart (
VOID
)
{
   GFX_GetPalette ( tpal1 );
   memcpy ( tpal2, tpal1, 768 );
}

/**************************************************************************
GFX_FadeFrame () - Fades Individual Frames
 **************************************************************************/
VOID
GFX_FadeFrame (
BYTE * palette,         // INPUT : palette to fade into
INT cur_step,           // INPUT : cur step position
INT steps               // INPUT : total steps of fade ( 0 - 255 )
)
{
   INT      i;
   INT      delta;

   for ( i = 0; i < 768; i++ )
   {
      delta = palette[ i ] - tpal1[ i ];
      tpal2[ i ] = tpal1[ i ] + delta * cur_step / steps;
   }

   GFX_SetPalette ( tpal2, 0 );
}

/**************************************************************************
GFX_SetPalRange() - Sets start and end range for remaping stuff
 **************************************************************************/
VOID
GFX_SetPalRange (
INT start,
INT end 
)
{
   if ( start < end && end < 256 && start >= 0 )
   {
      start_lookup = start;
      end_lookup = end;
   }
}

/**************************************************************************
  GFX_GetRGB() - gets R,G and B values from pallete data
 **************************************************************************/
VOID
GFX_GetRGB (
BYTE * pal,                // INPUT : pointer to palette data
INT  num,                  // INPUT : palette entry
INT  *red,                 // OUTPUT: red value
INT  *green,               // OUTPUT: green value
INT  *blue                 // OUTPUT: blue value
)
{
   *red   = pal[num *3];
   *green = pal[num *3 + 1];
   *blue  = pal[num *3 + 2];
}
  
/**************************************************************************
  GFX_Remap() - Finds the closest color avalable
 **************************************************************************/
INT                            // RETURN: new color number
GFX_Remap (
BYTE * pal,                // INPUT : pointer to palette data
INT  red,                  // INPUT : red  ( 0 - 63 )
INT  green,                // INPUT : green( 0 - 63 )
INT  blue                  // INPUT : blue ( 0 - 63 )
)
{
   INT  pos         = 0;
   INT  loop;
   INT  color[3];
   INT  diff[3];
   INT  low;
   INT  num;
  
   low = ( 256 * 3 ) + 1;
  
   for ( loop = start_lookup; loop < end_lookup+1; loop++ )
   {

      GFX_GetRGB ( pal, loop, &color[0], &color[1], &color[2] );
  
      diff[0] = abs ( color[0] - red );
      diff[1] = abs ( color[1] - green );
      diff[2] = abs ( color[2] - blue );
  
      num = diff [ 0 ] + diff [ 1 ] + diff [ 2 ];
  
      if ( num <= low )
      {
         low = num;
         pos = loop;
      }
   }
   return ( pos );
}
  
/**************************************************************************
  GFX_MakeLightTable() - make a light/dark palette lookup table
 **************************************************************************/
VOID
GFX_MakeLightTable (
BYTE *palette,             // INPUT : pointer to palette data
BYTE *ltable,              // OUTPUT: pointer to lookup table
INT  level                 // INPUT : - 63 to + 63
)
{
   INT  loop;
   INT  red;
   INT  green;
   INT  blue;
   INT  n_red;
   INT  n_green;
   INT  n_blue;
  
   for ( loop = 0; loop < 256; loop++ )
   {
      GFX_GetRGB ( palette, loop, &red, &green, &blue );
  
      if ( red >= 0 ) n_red = red + level;
      else n_red = 0;

      if ( green >= 0 ) n_green = green + level;
      else n_green = 0;

      if ( blue >= 0 ) n_blue = blue + level;
      else n_blue = 0;
  
      if ( level >= 0 )
      {
         if ( n_red > 63 ) n_red = 63;
         if ( n_green > 63 ) n_green = 63;
         if ( n_blue > 63 ) n_blue = 63;
      }
      else
      {
         if ( n_red < 0 ) n_red = 0;
         if ( n_green < 0 ) n_green = 0;
         if ( n_blue < 0 ) n_blue = 0;
      }
  
      ltable[loop] = GFX_Remap ( palette, n_red, n_green, n_blue );
   }
}
  
/**************************************************************************
  GFX_MakeGreyTable() - make a grey palette lookup table
 **************************************************************************/
VOID
GFX_MakeGreyTable (
BYTE *palette,             // INPUT : pointer to palette data
BYTE *ltable               // OUTPUT: pointer to lookup table
)
{
   INT  loop;
   INT  red;
   INT  green;
   INT  blue;
   INT  n_red;
   INT  n_green;
   INT  n_blue;
  
   for ( loop = 0; loop < 256; loop++ )
   {
      GFX_GetRGB ( palette, loop, &red, &green, &blue );
  
      n_red = ( ( red + green + blue ) / 3 );
  
      n_green = n_red;
      n_blue = n_red;
  
      ltable[loop] = GFX_Remap ( palette, n_red, n_green, n_blue );
   }
}
  
/*************************************************************************
   GFX_GetScreen() -     Gets A block of screen memory to CPU memory
 *************************************************************************/
VOID
GFX_GetScreen (
BYTE *outmem,              // OUTPUT: pointer to CPU mem
INT  x,                    // INPUT : x pos
INT  y,                    // INPUT : y pos
INT  lx,                   // INPUT : x length
INT  ly                    // INPUT : y length
)
{
   BYTE * source;
   INT   loop;
  
   if ( GFX_ClipLines ( &outmem, &x, &y, &lx, &ly ) )
   {
      source = displaybuffer + x + ylookup[y];
  
      for ( loop = 0 ;loop < ly; loop++, outmem += lx, source += SCREENWIDTH )
         memcpy ( outmem, source, ( size_t ) lx );
   }
}
  
/*************************************************************************
   GFX_PutTexture() - Repeats a Picture though the area specified
 *************************************************************************/
VOID
GFX_PutTexture (
BYTE *intxt,               // INPUT : color texture
INT  x,                    // INPUT : x pos
INT  y,                    // INPUT : y pos
INT  lx,                   // INPUT : x length
INT  ly                    // INPUT : y length
)
{
   GFX_PIC * h = ( GFX_PIC * )intxt;
   BYTE *   buf;
   INT      loopx;
   INT      loopy;
   INT      xpos;
   INT      ypos;
   INT      new_lx;
   INT      new_ly;
   INT      maxxloop = abs(x) + lx;
   INT      maxyloop = abs(y) + ly;
   INT      x2;
   INT      y2;
  
   x2 = x + lx - 1;
   y2 = y + ly - 1;
  
   if ( x2 >= SCREENWIDTH ) x2 = SCREENWIDTH - 1;
   if ( y2 >= SCREENHEIGHT ) y2 = SCREENHEIGHT - 1;
  
   for ( loopy = y; loopy < maxyloop; loopy += h->height )
   {
      if ( loopy > y2 ) continue;
      if ( loopy + h->height <= 0 ) continue;
      if ( loopy < 0 )
      {
         buf += ( -loopy * h->width );
         new_ly += loopy;
         ypos = 0;
      }
      else
         ypos = loopy;
  
      for ( loopx = x; loopx < maxxloop; loopx += h->width )
      {
         if ( loopx > x2 ) continue;
         if ( loopx + (INT)h->width <= 0 ) continue;
         buf = intxt + sizeof ( GFX_PIC );
  
         new_lx = h->width;
         new_ly = h->height;
  
         if ( loopx < 0 )
         {
            buf += -loopx;
            new_lx += loopx;
            xpos = 0;
         }
         else
            xpos = loopx;
  
         if ( ( xpos + new_lx - 1 ) >= x2 )
            new_lx = ( x2 + 1 ) - xpos;
  
         if ( ( ypos + new_ly - 1 ) >= y2 )
            new_ly =  ( y2 + 1 ) - ypos;
  
         gfx_inmem = buf;
         gfx_xp = xpos;
         gfx_yp = ypos;
         gfx_lx = new_lx;
         gfx_ly = new_ly;
         gfx_imga = ( h->width - new_lx );

         GFX_PutPic ();

         GFX_MarkUpdate ( xpos, ypos, new_lx, new_ly );
      }
   }
}

/*************************************************************************
   GFX_ShadeArea()- lightens or darkens and area of the screen
 *************************************************************************/
VOID
GFX_ShadeArea (
SHADE opt,                 // INPUT : DARK/LIGHT or GREY
INT  x,                    // INPUT : x position
INT  y,                    // INPUT : y position
INT  lx,                   // INPUT : x length
INT  ly                    // INPUT : y length
)
{
   BYTE * buf;
   BYTE * cur_table;
   INT  loop;
  
   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      buf = displaybuffer + x + ylookup[ y ];
  
      switch ( opt )
      {
         case DARK:
            cur_table = dtable;
            break;
  
         case LIGHT:
            cur_table = ltable;
            break;
  
         case GREY:
            cur_table = gtable;
            break;
      }
  
      GFX_MarkUpdate ( x, y, lx, ly );
  
      for ( loop = 0; loop < ly; loop++ )
      {
         GFX_Shade ( buf, lx, cur_table );
         buf += SCREENWIDTH;
      }
   }
}
  
/*************************************************************************
   GFX_ShadeShape()- lightens or darkens and area of the screen
 *************************************************************************/
VOID
GFX_ShadeShape (
SHADE opt,                 // INPUT : DARK/LIGHT or GREY
BYTE  * inmem,             // INPUT : mask 0 = no shade ( GFX format pic )
INT   x,                   // INPUT : x position
INT   y                    // INPUT : y position
)
{
   GFX_PIC * h = ( GFX_PIC * )inmem;
   GFX_SPRITE  *  ah;
   BYTE *         cur_table;
   BYTE *         dest;
   INT            rval;
   INT            ox = x;
   INT            oy = y;
   INT            lx = h->width;
   INT            ly = h->height;
  
   inmem += sizeof ( GFX_PIC );
  
   rval = GFX_ClipLines ( NUL, &ox, &oy, &lx, &ly );
   if ( !rval ) return;
  
   switch ( opt )
   {
      case DARK:
         cur_table = dtable;
         break;
  
      case LIGHT:
         cur_table = ltable;
         break;
  
      case GREY:
         cur_table = gtable;
         break;
   }

   switch ( rval )
   {
      default:
         break;
  
      case 1:
         dest = displaybuffer + ox + ylookup [ oy ];
  
         GFX_ShadeSprite ( dest, inmem, cur_table );
         break;
  
      case 2:
         ah = ( GFX_SPRITE * )inmem;
  
         while ( ah->offset != EMPTY )
         {
            inmem += sizeof ( GFX_SPRITE );
  
            ox = ah->x + x;
            oy = ah->y + y;

            if ( oy > SCREENHEIGHT ) break;
  
            lx = ah->length;
            ly = 1;
  
            if ( GFX_ClipLines ( NUL, &ox, &oy, &lx, &ly ) )
               GFX_Shade ( displaybuffer + ox + ylookup [oy], lx, cur_table );
  
            inmem += ah->length;
  
            ah = ( GFX_SPRITE * )inmem;
         }
         break;
   }
}
  
/*************************************************************************
   GFX_VShadeLine () - Shades a vertical line
 *************************************************************************/
VOID
GFX_VShadeLine (
SHADE opt,                 // INPUT : DARK/LIGHT or GREY
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT ly                     // INPUT : length of line
)
{
   INT lx = 1;
   BYTE * outbuf;
   BYTE * cur_table;
  
   if ( ly < 1 ) return;

   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      switch ( opt )
      {
         case DARK:
            cur_table = dtable;
            break;
  
         case LIGHT:
            cur_table = ltable;
            break;
  
         case GREY:
            cur_table = gtable;
            break;
      }
  
      GFX_MarkUpdate ( x, y, lx, ly );
  
      outbuf = displaybuffer + x + ylookup[y];
  
      while ( ly-- )
      {
         *outbuf = *( cur_table + *outbuf );
         outbuf += SCREENWIDTH;
      }
   }
}
  
/*************************************************************************
   GFX_HShadeLine () Shades a Horizontal Line
 *************************************************************************/
VOID
GFX_HShadeLine (
SHADE opt,                 // INPUT : DARK/LIGHT or GREY
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx                     // INPUT : length of line
)
{
   INT ly = 1;
   BYTE * outbuf;
   BYTE * cur_table;
  
   if ( lx < 1 ) return;

   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      switch ( opt )
      {
         case DARK:
            cur_table = dtable;
            break;
  
         case LIGHT:
            cur_table = ltable;
            break;
  
         case GREY:
            cur_table = gtable;
            break;
      }
  
      GFX_MarkUpdate ( x, y, lx, ly );
  
      outbuf = displaybuffer + x + ylookup[y];
  
      GFX_Shade ( outbuf, lx, cur_table );
   }
}
  
/*************************************************************************
   GFX_LightBox()- Draws a rectangle border with light source
 *************************************************************************/
VOID
GFX_LightBox (
CORNER  opt,               // INPUT : light source
INT  x,                    // INPUT : x position
INT  y,                    // INPUT : y position
INT  lx,                   // INPUT : x length
INT  ly                    // INPUT : y length
)
{
   if ( lx < 1 ) return;
   if ( ly < 1 ) return;

   switch (opt)
   {
      case UPPER_LEFT:
         GFX_HShadeLine ( LIGHT, x , y, lx - 1);
         GFX_VShadeLine ( LIGHT, x, y + 1, ly - 2 );
         GFX_HShadeLine ( DARK, x , y + ly -1, lx );
         GFX_VShadeLine ( DARK, x + lx - 1, y + 1, ly - 2 );
         break;
  
      case UPPER_RIGHT:
      default:
         GFX_HShadeLine ( LIGHT, x + 1, y, lx - 1);
         GFX_VShadeLine ( LIGHT, x + lx - 1, y + 1, ly - 2 );
         GFX_HShadeLine ( DARK, x , y + ly - 1, lx );
         GFX_VShadeLine ( DARK, x , y, ly - 1 );
         break;
  
      case LOWER_LEFT:
         GFX_HShadeLine ( LIGHT, x , y + ly - 1, lx - 1);
         GFX_VShadeLine ( LIGHT, x , y + 1, ly - 2 );
         GFX_HShadeLine ( DARK, x , y , lx );
         GFX_VShadeLine ( DARK, x + lx - 1, y + 1, ly - 1 );
         break;
  
      case LOWER_RIGHT:
         GFX_HShadeLine ( LIGHT, x + 1, y, lx - 1);
         GFX_VShadeLine ( LIGHT, x + lx - 1, y + 1, ly - 2 );
         GFX_HShadeLine ( DARK, x, y, lx );
         GFX_VShadeLine ( DARK, x, y + 1, ly - 2 );
         break;
   }
}
  
/*************************************************************************
   GFX_ColorBox () - sets a rectangular area to color
 *************************************************************************/
VOID
GFX_ColorBox (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : width
INT ly,                    // INPUT : length
INT color                  // INPUT : fill color ( 0 - 255 )
)
{
   BYTE * outbuf;
  
   if ( lx < 1 ) return;
   if ( ly < 1 ) return;

   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      outbuf = displaybuffer + x + ylookup[y];
  
      GFX_MarkUpdate ( x, y, lx, ly );
  
      if ( color < 0 )
      {
         while ( ly-- )
            GFX_HLine ( x, y++, lx, color );
      }
      else
      {
         while (ly--)
         {
            memset ( outbuf, color, (size_t)lx );
            outbuf += SCREENWIDTH;
         }
      }
   }
}
  
/*************************************************************************
   GFX_HLine () - plots a horizontal line in color
 *************************************************************************/
VOID
GFX_HLine (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : width
INT color                  // INPUT : fill color ( 0 - 255 )
)
{
   BYTE * outbuf;
   INT ly = 1;
   INT loop;
  
   if ( lx < 1 ) return;

   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      outbuf = displaybuffer + x + ylookup[y];
  
      GFX_MarkUpdate ( x, y, lx, 1 );
  
      if ( color < 0 )
      {
         for ( loop = 0; loop < lx; loop++, outbuf++ )
            *outbuf ^= (BYTE)(255 + color);
      }
      else
         memset ( outbuf , color, (size_t) lx );
   }
  
}
  
/*************************************************************************
   GFX_VLine () plots a vertical line in color
 *************************************************************************/
VOID
GFX_VLine (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT ly,                    // INPUT : length
INT color                  // INPUT : fill color ( 0 - 255 )
)
{
   BYTE * outbuf;
   INT lx = 1;
  
   if ( ly < 1 ) return;

   if ( GFX_ClipLines ( ( BYTE ** ) 0, &x, &y, &lx, &ly ) )
   {
      outbuf = displaybuffer + x + ylookup[y];
  
      GFX_MarkUpdate ( x, y, 1, ly );
  
      if ( color < 0 )
      {
         while ( ly-- )
         {
            *outbuf ^= (BYTE)(255 + color);
            outbuf += SCREENWIDTH;
         }
      }
      else
      {
         while ( ly-- )
         {
            *outbuf = color;
            outbuf += SCREENWIDTH;
         }
      }
   }
}

/*************************************************************************
   GFX_Line () plots a line in color ( Does no Clipping )
 *************************************************************************/
VOID
GFX_Line (
INT   x,                   // INPUT : x start point
INT   y,                   // INPUT : y start point
INT   x2,                  // INPUT : x2 end point
INT   y2,                  // INPUT : y2 end point
INT   color                // INPUT : color ( 0 - 255 )
)
{
   INT addx = 1;
   INT addy = 1;
   INT delx;
   INT dely;
   INT maxloop;
   INT err;

   delx = x2 - x;
   dely = y2 - y;

   if ( delx < 0 )
   {
      delx = -delx;
      addx = -addx;
   }
   if ( dely < 0 )
   {
      dely = -dely;
      addy = -addy;
   }

   if ( delx >= dely )
   {
      err = -(dely>>1);
      maxloop = delx + 1;
   }
   else
   {
      err = (delx>>1);
      maxloop = dely + 1;
   }

   if ( delx >= dely )
   {
      while ( maxloop )
      {
         if ( x >= 0 && x < 320 && y >=0 && y < 200 )
            *(displaybuffer + x + ylookup[y] ) = ( BYTE ) color;
         maxloop--;
         err += dely;
         x += addx;

         if ( err > 0 )
         {
            y += addy;
            err -= delx;
         }
      }
   }
   else
   {
      while ( maxloop )
      {
         if ( x >= 0 && x < 320 && y >=0 && y < 200 )
            *(displaybuffer + x + ylookup[y] ) = ( BYTE ) color;
         maxloop--;
         err += delx;
         y += addy;
         if ( err > 0 )
         {
            x += addx;
            err -= dely;
         }
      }
   }
}

/*************************************************************************
   GFX_Rectangle () - sets a rectangular border to color
 *************************************************************************/
VOID
GFX_Rectangle (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : width
INT ly,                    // INPUT : length
INT color                  // INPUT : fill color ( 0 - 255 )
)
{
   if ( ly < 1 ) return;
   if ( lx < 1 ) return;

   GFX_HLine ( x, y, lx, color );
   GFX_HLine ( x, y + ly - 1, lx, color );
   GFX_VLine ( x, y + 1, ly - 2, color );
   GFX_VLine ( x + lx - 1, y + 1, ly - 2, color );
}
  
/*************************************************************************
   GFX_ScalePic () - Scales picture optionaly make color 0 see thru
 *************************************************************************/
VOID
GFX_ScalePic (
BYTE * buffin,             // INPUT : pointer to pic data
INT  x,                    // INPUT : x display position
INT  y,                    // INPUT : y display position
INT  new_lx,               // INPUT : new x length
INT  new_ly,               // INPUT : new y length
BOOL see_thru              // INPUT : TRUE = see thru
)
{
   GFX_PIC * h       = (GFX_PIC *)buffin;
   BYTE *   dest     = displaybuffer;
   BYTE *   pic      = buffin + sizeof ( GFX_PIC );
   DWORD    accum_y  = 0;
   DWORD    accum_x  = 0;
   DWORD    addx     = ( ( h->width << 16 ) / new_lx );
   DWORD    addy     = ( ( h->height << 16 ) / new_ly );
   INT      outpos;
  
   if ( h->type == GSPRITE )
   {
      GFX_PutSprite ( buffin, x, y );
      return;
   }

   if ( x < 0 )
   {
      accum_x = addx * (DWORD)-x;
      pic += ( accum_x >> 16 );
      new_lx += x;
      accum_x = accum_x & 0x0000FFFF;
      x = 0;
   }
  
   if ( y < 0 )
   {
      accum_y = addy * (DWORD)-y;
      pic += ( ( accum_y >> 16 ) * h->width );
      new_ly += y;
      accum_y = accum_y & 0x0000FFFF;
      y = 0;
   }
  
   if ( ( x + new_lx ) > (signed)SCREENWIDTH ) new_lx = SCREENWIDTH - x;
   if ( ( y + new_ly ) > (signed)SCREENHEIGHT ) new_ly = SCREENHEIGHT - y;
  
   dest += ( x + ylookup[ y ] );
  
   GFX_MarkUpdate ( x, y, new_lx, new_ly );
  
   tablelen = outpos = new_lx;
   outpos--;
  
   while ( outpos >= 0 )
   {
      *(stable + outpos ) = ( accum_x >> 16 );
      accum_x += addx;
      outpos--;
   }
  
   if ( see_thru )
   {
      while ( new_ly-- > 0 )
      {
         GFX_CScaleLine ( dest, pic + ( accum_y >> 16 ) * h->width  );
         accum_y += addy;
         dest += SCREENWIDTH;
      }
   }
   else
   {
      while ( new_ly-- > 0 )
      {
         GFX_ScaleLine ( dest, pic + ( accum_y >> 16 ) * h->width  );
         accum_y += addy;
         dest += SCREENWIDTH;
      }
   }
}
  
/*************************************************************************
   GFX_MarkUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
VOID
GFX_MarkUpdate (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : x length
INT ly                     // INPUT : y length
)
{
   INT x2 = x + lx - 1;
   INT y2 = y + ly - 1;
   INT ud_x2 = ud_x + ud_lx - 1;
   INT ud_y2 = ud_y + ud_ly - 1;
  
   if ( update_start  )
   {
      if ( x < ud_x )
         ud_x = x;
  
      if ( y < ud_y )
         ud_y = y;
  
      if ( x2 > ud_x2 ) ud_x2 = x2;
      if ( y2 > ud_y2 ) ud_y2 = y2;
   }
   else
   {
      ud_x = x;
      ud_y = y;
      ud_x2 = x + lx - 1;
      ud_y2 = y + ly - 1;
      if ( ( lx + ly) > 0 )
         update_start = TRUE;
   }
  
   if ( ud_x & 3 ) ud_x -= ( ud_x & 3 );
  
   if ( ud_x < 0 )
      ud_x = 0;
  
   if ( ud_y < 0 )
      ud_y = 0;
  
   ud_lx = ud_x2 - ud_x + 1;
   ud_ly = ud_y2 - ud_y + 1;
  
   if ( ud_lx & 3 ) ud_lx += ( 4 - ( ud_lx & 3 ) );
  
   if ( ud_lx + ud_x > SCREENWIDTH ) ud_lx = SCREENWIDTH - ud_x;
   if ( ud_ly + ud_y > SCREENHEIGHT ) ud_ly = SCREENHEIGHT - ud_y;
  
}

/*************************************************************************
   GFX_ForceUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
VOID
GFX_ForceUpdate (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : x length
INT ly                     // INPUT : y length
)
{
   ud_x = o_ud_x = x;
   ud_y = o_ud_y = y;
   ud_lx = o_ud_lx = lx;
   ud_ly = o_ud_ly = ly;
}

/***************************************************************************
   GFX_SetFrameHook () sets function to call before every screen update
 ***************************************************************************/
VOID
GFX_SetFrameHook (
VOID (*func)(VOID (*)(void)) // INPUT : pointer to function
)
{
   framehook = func;
}

/***************************************************************************
 GFX_Delay () - Delay for ( count ) of screen frames ( sec/70 )
 ***************************************************************************/
VOID
GFX_Delay (
INT count                  // INPUT : wait # of frame ticks
)
{
   static INT  hold;
   INT         loop;

   for ( loop = 0; loop < count; loop++ )
   {
      hold = FRAME_COUNT;
      while ( FRAME_COUNT == hold && gfxdebug == FALSE );
   }

}
  
/***************************************************************************
   GFX_WaitUpdate () - Updates screen at specified frame rate
 ***************************************************************************/
VOID
GFX_WaitUpdate (
INT count               // INPUT : frame rate ( MAX = 70 )
)
{
   static INT  hold = 0;
   INT         loop;

   if ( count > 70 )
      count = 70;
   else if ( count < 1 )
      count = 1;

   count = 70/count;

   GFX_MarkUpdate ( o_ud_x, o_ud_y, o_ud_lx, o_ud_ly );

   for ( loop = 0; loop < count; loop++ )
   {
      while ( FRAME_COUNT == hold && gfxdebug == FALSE );
      hold = FRAME_COUNT;
   }

   if ( update_start )
   {
      if ( framehook )
         framehook( ( VOID * )GFX_DisplayScreen );
      else
         GFX_DisplayScreen();
   }
  
   o_ud_x  = ud_x;
   o_ud_y  = ud_y;
   o_ud_lx = ud_lx;
   o_ud_ly = ud_ly;
}

/***************************************************************************
   GFX_DisplayUpdate () - Copys Marked areas to display
 ***************************************************************************/
VOID
GFX_DisplayUpdate (
VOID
)
{
   static INT hold = 0;
  
   while ( FRAME_COUNT == hold && gfxdebug == FALSE );
  
   GFX_MarkUpdate ( o_ud_x, o_ud_y, o_ud_lx, o_ud_ly );
  
   if ( update_start )
   {
      if ( framehook )
         framehook( ( VOID * )GFX_DisplayScreen );
      else
         GFX_DisplayScreen();
   }
  
   o_ud_x = ud_x;
   o_ud_y = ud_y;
   o_ud_lx = ud_lx;
   o_ud_ly = ud_ly;
  
   hold = FRAME_COUNT;
}
  
/***************************************************************************
   GFX_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
VOID
GFX_PutImage (
BYTE * image,              // INPUT : image data
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
BOOL see_thru              // INPUT : true = masked, false = put block
)
{
   GFX_PIC * h = ( GFX_PIC *)image;
  
   gfx_lx = h->width;
   gfx_ly = h->height;

   if ( h->type == GSPRITE )
   {
      GFX_PutSprite ( image, x, y );
   }
   else
   {
      image += sizeof ( GFX_PIC );
  
      if ( GFX_ClipLines ( &image, &x, &y, &gfx_lx, &gfx_ly ) )
      {
         GFX_MarkUpdate ( x, y, gfx_lx, gfx_ly );
  
         gfx_xp = x;
         gfx_yp = y;

         gfx_inmem = image;
         gfx_imga = h->width;

         if ( !see_thru )
         {
            gfx_imga -=  gfx_lx;
            GFX_PutPic ();
         }
         else
            GFX_PutMaskPic();
      }
   }
}

/***************************************************************************
   GFX_PutSprite () -Puts a Sprite into display buffer
 ***************************************************************************/
VOID
   GFX_PutSprite (
   BYTE * inmem,           // INPUT : inmem 
   INT x,                  // INPUT : x pos
   INT y                   // INPUT : y pos
   )
{
   GFX_PIC * h = ( GFX_PIC * )inmem;
   GFX_SPRITE  *  ah;
   BYTE *         dest;
   BYTE *         outline;
   INT            rval;
   INT            ox = x;
   INT            oy = y;
   INT            lx = h->width;
   INT            ly = h->height;
  
   rval = GFX_ClipLines ( NUL, &ox, &oy, &lx, &ly );
   if ( !rval ) return;
  
   inmem += sizeof ( GFX_PIC );

   switch ( rval )
   {
      default:
         break;
  
      case 1:
         dest = displaybuffer + ox + ylookup [ oy ];
         GFX_DrawSprite ( dest, inmem );
         break;
  
      case 2:
         ah = ( GFX_SPRITE * )inmem;

         while ( ah->offset != EMPTY )
         {
            inmem += sizeof ( GFX_SPRITE );
  
            ox = ah->x + x;
            oy = ah->y + y;
  
            if ( oy > SCREENHEIGHT ) break;

            lx = ah->length;
            ly = 1;

            outline = inmem;
  
            if ( GFX_ClipLines ( &outline, &ox, &oy, &lx, &ly ) )
               memcpy ( displaybuffer + ox + ylookup [ oy ], outline, lx );
  
            inmem += ah->length;
  
            ah = ( GFX_SPRITE * )inmem;
         }
         break;
   }
}

/***************************************************************************
   GFX_OverlayImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
VOID
GFX_OverlayImage (
BYTE * baseimage,          // INPUT : base image data
BYTE * overimage,          // INPUT : overlay image data
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
)
{
   GFX_PIC * bh = ( GFX_PIC *)baseimage;
   GFX_PIC * oh = ( GFX_PIC *)overimage;
   INT loop, i, addnum;
   INT x2 = x + oh->width - 1;
   INT y2 = y + oh->height - 1;
  
   if ( x >=0 && y >=0 && x2 < bh->width && y2 < bh->height )
   {
      baseimage += sizeof ( GFX_PIC );
      baseimage += ( x + ( y * bh->width ) );
  
      overimage += sizeof ( GFX_PIC );
  
      addnum = bh->width - oh->width;

      for ( loop = 0; loop < oh->height; loop++ )
      {
         for ( i = 0; i < oh->width; i++, baseimage++, overimage++ )
         {
            if ( i != 255 )
               *baseimage = *overimage;
         }
         baseimage += addnum;
      }
   }
}
  
  
/***************************************************************************
   GFX_StrPixelLen() - Calculates the length of a GFX string
 ***************************************************************************/
INT                           // RETURNS : pixel length
GFX_StrPixelLen (
VOID * infont,             // INPUT : pointer to current font
CHAR * instr,              // INPUT : pointer to string
size_t maxloop             // INPUT : length of string
)
{
   INT loop;
   INT outlen = 0;
  
   for ( loop = 0; loop < maxloop; loop++ )
   {
      outlen += ( (FONT *) infont )->width [ instr [ loop ] ];
      outlen += fontspacing;
   }
  
   return( outlen );
}
  
/*--------------------------------------------------------------------------
   GFX_PutChar () - Draws charater to displaybuffer and clips
 --------------------------------------------------------------------------*/
PRIVATE INT
GFX_PutChar (
INT      x,                // INPUT : x position
INT      y,                // INPUT : y position
BYTE     inchar,           // INPUT : char to print
FONT *   font,             // INPUT : pointer to font
INT      basecolor         // INPUT : font base color
)
{
   BYTE  *  source   = (BYTE *)font + sizeof ( FONT );
   INT      lx       = font->width[inchar];
   INT      ly       = font->height;
   BYTE  *  cdata    = source + font->charofs[ inchar ];
   INT      addx     = lx;
   BYTE  *  dest;
  
   if ( GFX_ClipLines ( &cdata, &x, &y, &lx, &ly ) )
   {
      dest = displaybuffer + x + ylookup [ y ];
  
      GFX_MarkUpdate ( x, y, lx, ly );
  
      addx = addx - lx;
  
      GFX_DrawChar ( dest, cdata, lx, ly, addx, basecolor );

      return ( lx );
   }
  
   return ( 0 );
}
  
/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
INT                           // RETURN: length of print
GFX_Print (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
CHAR *str,                 // INPUT : string to print
VOID * infont,             // INPUT : pointer to font
INT basecolor              // INPUT : basecolor of font
)
{
   FONT  *  font = infont;
   INT      lx = 0;
   INT      cwidth;
   BYTE     ch;
  
   basecolor--;

   if ( strlen ( str ) )
   {
      while ( ( ch = *str++ ) != 0 )
      {
         if ( font->charofs [ ch ] == (SHORT)EMPTY ) continue;
         cwidth = GFX_PutChar ( x, y, ch, font, basecolor );
         lx += ( cwidth + fontspacing );
         x += ( font->width[ch] + fontspacing );
      }
   }
  
   return ( lx );
}

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
VOID
GFX_3D_SetView(
INT   x,                   // INPUT : x position
INT   y,                   // INPUT : y position
INT   z                    // INPUT : z position
)
{
   G3D_viewx = x;
   G3D_viewy = y;
   G3D_viewz = z;
}
  
/*--------------------------------------------------------------------------
   GFX_3DPoint () plots a points in 3D space
 --------------------------------------------------------------------------*/
VOID
GFX_3DPoint (
VOID
)
{
   G3D_x -=G3D_viewx;
   G3D_y -=G3D_viewy;
   G3D_z -=G3D_viewz;
  
   G3D_screenx = ( ( ( ( G3D_DIST << 11 ) * G3D_x ) / G3D_z ) >> 11);
   G3D_screeny = ( ( ( ( G3D_DIST << 11 ) * G3D_y ) / G3D_z ) >> 11);
  
   G3D_screenx += G3D_viewx;
   G3D_screeny += G3D_viewy;
}
  
/***************************************************************************
   GFX_3D_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
VOID
GFX_3D_PutImage (
BYTE * image,              // INPUT : image data
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT z,                     // INPUT : z position ( distance )
BOOL see_thru              // INPUT : true = masked, false = put block
)
{
   GFX_PIC * h = ( GFX_PIC *)image;
   INT new_lx;
   INT new_ly;
   INT x1;
   INT y1;
  
   if ( z == G3D_DIST )
   {
      GFX_MarkUpdate ( x, y, (INT)h->width, (INT)h->height );
      GFX_PutImage ( image, x, y, see_thru );
   }
   else
   {
      G3D_x = x;
      G3D_y = y;
      G3D_z = z;
      GFX_3DPoint();
      x1 = G3D_screenx;
      y1 = G3D_screeny;
  
      G3D_x = x + h->width  - 1;
      G3D_y = y + h->height - 1;
      G3D_z = z;
      GFX_3DPoint();
  
      new_lx = G3D_screenx - x1;
      new_ly = G3D_screeny - y1;
  
      GFX_MarkUpdate ( x1, y1, new_lx, new_ly );
      GFX_ScalePic ( image , x1, y1, new_lx, new_ly, see_thru );
   }
  
}
