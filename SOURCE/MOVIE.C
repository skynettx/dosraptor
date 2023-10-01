#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
  
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"

PRIVATE INT back_patch = EMPTY;

/*************************************************************************
MOVIE_BPatch() Plays Sound FX in background for one anim
 *************************************************************************/
VOID
MOVIE_BPatch (
INT soundfx
)
{
   back_patch = soundfx;
   SND_Patch ( back_patch, 127 );
}
  
/*************************************************************************
 MOVIE_ShowFrame () - Shows an animation frame
 *************************************************************************/
VOID
MOVIE_ShowFrame (
BYTE * inpic               // INPUT : pointer to animpic
)
{
   if ( inpic == NUL )
      return;

   ANIM_Render ( inpic );
  
   GFX_MarkUpdate ( 0, 0, 320, 200 );
}
  
/*************************************************************************
 MOVIE_Play () - Playes an Animation
 *************************************************************************/
INT
MOVIE_Play (
FRAME * frame,              // INPUT : pointer to array of frame defs
INT     numplay,            // INPUT : number of times to play
BYTE *  palette             // INPUT : pointer to palette
)
{
   BOOL     flag = TRUE;
   FRAME *  curfld;
   BYTE  *  pic;
   INT      opt = K_OK;
   BYTE     fill;
   INT      hold;
   INT      loop;
  
   memset ( displaybuffer,0, 64000 );
  
   curfld = frame;

   IMS_StartAck();
  
   for (;;)
   {
      pic = GLB_GetItem ( curfld->item );

      if ( flag )
      {
         fill = *pic;
         memset ( displaybuffer, fill, 64000 );
         pic++;
      }

      switch ( curfld->startf )
      {
         default:
            MOVIE_ShowFrame ( pic );
            GFX_WaitUpdate ( curfld->framerate );
            break;

         case M_FADEOUT:
            GFX_FadeOut ( curfld->red, curfld->green, curfld->blue, curfld->startsteps );

         case M_ERASE:
            memset ( displaybuffer, 0, 64000 );
            GFX_MarkUpdate ( 0, 0, 320, 200 );
            GFX_DisplayUpdate();
            break;

         case M_FADEIN:
            if ( flag )
            {
               GFX_FadeOut ( 0, 0, 0, 2 );
               flag = FALSE;
            }
            MOVIE_ShowFrame ( pic );
            GFX_WaitUpdate ( curfld->framerate );
            GFX_FadeIn ( palette, curfld->startsteps );
            break;
      }

      if ( curfld->holdframe )
      {
         for ( loop = 0; loop < curfld->holdframe; loop++ )
         {
            hold = FRAME_COUNT;
            while ( FRAME_COUNT == hold )
            {
               if ( back_patch != EMPTY )
               {
                  if ( !SND_IsPatchPlaying ( back_patch ) )
                     SND_Patch ( back_patch, 127 );
               }
            }
         }
      }
      else
      {
         if ( back_patch != EMPTY )
         {
            if ( !SND_IsPatchPlaying ( back_patch ) )
               SND_Patch ( back_patch, 127 );
         }
      }

      if ( curfld->soundfx != EMPTY )
         SND_Patch ( curfld->soundfx, curfld->fx_xpos );

      switch ( curfld->endf )
      {
         default:
            break;
  
         case M_ERASE:
            memset ( displaybuffer, 0, 64000 );
            GFX_MarkUpdate ( 0, 0, 320, 200 );
            GFX_DisplayUpdate();
            break;

         case M_FADEOUT:
            GFX_FadeOut ( curfld->red, curfld->green, curfld->blue, curfld->endsteps );
            break;

         case M_FADEIN:
            GFX_FadeIn ( palette, curfld->startsteps );
            break;

         case M_PALETTE:
            GFX_SetPalette ( palette, 0 );
            break;
      }

      GLB_FreeItem ( curfld->item );

      if ( curfld->numframes == 0 )
      {
         numplay--;
         if ( numplay == 0 ) break;
         memset ( displaybuffer, 0, 64000 );
         curfld = frame;
         flag = TRUE;
         continue;
      }

      if ( IMS_CheckAck() )
      {
         IMS_StartAck();
         opt = K_SKIPALL;
         break;
      }

      flag = FALSE;

      curfld++;
   }

   if ( opt != K_OK )
   {
      KBD_Clear();
      GFX_FadeOut ( 0, 0, 0, 16 );
      memset ( displaybuffer, 0, 64000 );
      GFX_MarkUpdate( 0, 0, 320, 200 );
      GFX_DisplayUpdate();
      GFX_SetPalette ( palette, 0 );
   }

   if ( back_patch != EMPTY )
   {
      SND_StopPatch ( back_patch );
      back_patch = EMPTY;
   }

   return ( opt );

}
