#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
  
#include "file0000.inc"
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"
  
FRAME frm[40];
  
/***************************************************************************
INTRO_City () - Shows City with planes flying into it
 ***************************************************************************/
BOOL
INTRO_City (
VOID
)
{
   INT loop;
   INT maxframes  = 30;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 8;
      cur->numframes       = framecnt--;
      cur->item            = CHASE_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur->fx_vol          = 0;
      cur->fx_xpos         = 127;

      if ( loop == 4 )
      {
         cur->soundfx = FX_FLYBY;
         cur->fx_xpos = 210;
      }

      if ( loop == 9 )
      {
         cur->soundfx = FX_FLYBY;
         cur->fx_xpos = 100;
      }
   }
  
   cur    = frm;
  
   cur->startf = M_FADEIN;
   cur->startsteps = 128;
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );

   SND_StopPatches();
  
   return ( FALSE );
}
  
/***************************************************************************
INTRO_Side1 () - Show Side OF Player ship going thru city
 ***************************************************************************/
BOOL
INTRO_Side1 (
VOID
)
{
   INT loop;
   INT maxframes  = 20;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   MOVIE_BPatch ( FX_JETSND );

   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 18;
      cur->numframes       = framecnt--;
      cur->item            = SHIPSD1_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->fx_vol          = 127;
      cur->fx_xpos         = 127;
      cur->soundfx         = EMPTY;
   }
  
   if  ( MOVIE_Play ( frm, 2, palette ) == K_SKIPALL )
      return ( TRUE );
  
   return ( FALSE );
}
  
/***************************************************************************
INTRO_Pilot () - Shows Pilots Face with lights moving thru
 ***************************************************************************/
BOOL
INTRO_Pilot (
VOID
)
{
   INT loop;
   INT maxframes  = 21;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   MOVIE_BPatch ( FX_IJETSND );

   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 10;
      cur->numframes       = framecnt--;
      cur->item            = PILOT_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur->fx_vol          = 127;
      cur->fx_xpos         = 127;
   }
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
  
   return ( FALSE );
}
  
/***************************************************************************
INTRO_Explosion () - Bad Guy Blowing UP
 ***************************************************************************/
BOOL
INTRO_Explosion (
VOID
)
{
   INT loop;
   INT maxframes  = 22;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   MOVIE_BPatch ( FX_EJETSND );

   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 12;
      cur->numframes       = framecnt--;
      cur->item            = EXPLO_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur->fx_xpos         = 127;
      cur->fx_vol          = 127;

      if ( loop >= 2 && loop < 10 )
      {
         cur->soundfx         = FX_INTROHIT;
         cur->fx_xpos         = 110 + random ( 40 );
      }

      if ( loop >= 8 )
         if ( loop & 1 )
            cur->soundfx         = FX_AIREXPLO;
   }
  
   cur--;
   cur->soundfx   = FX_AIREXPLO;
   cur->endf      = M_FADEOUT;
   cur->red       = 63;
   cur->green     = 28;
   cur->blue      = 3;
   cur->endsteps  = 60;
   cur++;
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );

   return ( FALSE );
}
  
/***************************************************************************
INTRO_Side2 () - Plaer Side flying thru city Shooting
 ***************************************************************************/
BOOL
INTRO_Side2 (
VOID
)
{
   INT loop;
   INT maxframes  = 20;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
   INT opt;
  
   MOVIE_BPatch ( FX_JETSND );

   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 18;
      cur->numframes       = framecnt--;
      cur->item            = SHIPSD1_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur->fx_vol          = 50;
      cur->fx_xpos         = 127;
      cur->soundfx         = EMPTY;
   }
  
   opt = MOVIE_Play ( frm, 1, palette );
  
   if ( opt == K_SKIPALL )
      return ( TRUE );
  
   if ( opt == K_NEXTFRAME )
      return ( FALSE );
  
   framecnt = maxframes - 1;
   cur      = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 18;
      cur->numframes       = framecnt--;
      cur->item            = SHIPSD2_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur->fx_xpos         = 127;
      if ( loop > 1 )
         cur->soundfx      = FX_INTROGUN;
   }
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );

   SND_StopPatches();
  
   return ( FALSE );
}
  
  
/***************************************************************************
INTRO_Base() - Base Landing at sunset
 ***************************************************************************/
BOOL
INTRO_Base (
VOID
)
{
   INT loop;
   INT maxframes  = 30;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   if ( !GAME2 )
      return ( TRUE );

   for ( loop = 0; loop < maxframes; loop++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 10;
      cur->numframes       = framecnt--;
      cur->item            = BASE_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur++;
   }
  
   cur    = frm;
  
   cur->startf = M_FADEIN;
   cur->startsteps = 128;
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
  
   return ( FALSE );
}
  
/***************************************************************************
INTRO_Landing () - Ship Landing on Base
 ***************************************************************************/
BOOL
INTRO_Landing (
VOID
)
{
   INT loop;
   INT maxframes  = 33;
   INT framecnt   = maxframes - 1;
   FRAME * cur    = frm;
  
   for ( loop = 0; loop < maxframes; loop++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 10;
      cur->numframes       = framecnt--;
      cur->item            = LANDING_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
      cur++;
   }
  
   cur--;
   cur->startf      = M_FADEOUT;
   cur->startsteps  = 64;
   cur->endf        = M_PALETTE;
   cur->endsteps    = 64;
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
  
   return ( FALSE );
}
  
/***************************************************************************
INTRO_Death2 () - Ground Death Scene
 ***************************************************************************/
BOOL
INTRO_Death2 (
VOID
)
{
   INT      loop;
   INT      maxframes  = 6;
   INT      framecnt   = maxframes - 1;
   FRAME *  cur        = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 3;
      cur->numframes       = framecnt--;
      cur->item            = SDEATH_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
   }
  
   if  ( MOVIE_Play ( frm, 8, palette ) == K_SKIPALL )
      return ( TRUE );
 
   GFX_FadeOut ( 0, 0, 0, 100 );

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();

   return ( FALSE );
}

/***************************************************************************
INTRO_Death1 () - Air Death Scene
 ***************************************************************************/
BOOL
INTRO_Death1 (
VOID
)
{
   INT      loop;
   INT      maxframes  = 30;
   INT      framecnt   = maxframes - 1;
   FRAME *  cur        = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 11;
      cur->numframes       = framecnt--;
      cur->item            = DOWN_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;
   }
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
 
   return ( FALSE );
}

/***************************************************************************
INTRO_Death () - Death Scene
 ***************************************************************************/
BOOL
INTRO_Death (
VOID
)
{

   if ( INTRO_Death1() ) return ( TRUE );
   INTRO_Death2();
   return ( FALSE );
}

/***************************************************************************
INTRO_Game1End () - Game 1 Victory
 ***************************************************************************/
BOOL
INTRO_Game1End (
VOID
)
{
   INT      loop;
   INT      maxframes  = 5;
   INT      framecnt   = maxframes - 1;
   FRAME *  cur        = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 4;
      cur->numframes       = framecnt--;
      cur->item            = GAME1END_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;

   }
  
   if  ( MOVIE_Play ( frm, 8, palette ) == K_SKIPALL )
      return ( TRUE );
 
   GFX_FadeOut ( 0, 0, 0, 120 );

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();

   GFX_SetPalette ( palette, 0 );

   return ( FALSE );
}

/***************************************************************************
INTRO_Game2End () - Game 1 Victory
 ***************************************************************************/
BOOL
INTRO_Game2End (
VOID
)
{
   INT      loop;
   INT      maxframes  = 25;
   INT      framecnt   = maxframes - 1;
   FRAME *  cur        = frm;
  
   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 4;
      cur->numframes       = framecnt--;
      cur->item            = GAME2END_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;

      if ( loop == 22 )
      {
         cur->soundfx = FX_AIREXPLO;
         cur->fx_xpos = 127;
      }

      if ( loop == 24 )
      {
         cur->soundfx = FX_AIREXPLO;
         cur->fx_xpos = 127;
      }

   }
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
 
   GFX_FadeOut ( 0, 0, 0, 120 );

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();

   GFX_SetPalette ( palette, 0 );

   return ( FALSE );
}
  
/***************************************************************************
INTRO_Game3End () - Game 1 Victory
 ***************************************************************************/
BOOL
INTRO_Game3End (
VOID
)
{
   INT      loop;
   INT      maxframes  = 39;
   INT      framecnt   = maxframes - 1;
   FRAME *  cur        = frm;
  
   MOVIE_BPatch ( FX_JETSND );

   for ( loop = 0; loop < maxframes; loop++, cur++ )
   {
      cur->holdframe       = 0;
      cur->opt             = M_ANIM;
      cur->framerate       = 8;
      cur->numframes       = framecnt--;
      cur->item            = GAME3END_AGX + loop;
      cur->startf          = M_NORM;
      cur->startsteps      = 0;
      cur->endf            = M_NORM;
      cur->endsteps        = 0;
      cur->red             = 0;
      cur->green           = 0;
      cur->blue            = 0;
      cur->songid          = EMPTY;
      cur->songopt         = S_PLAY;
      cur->songstep        = 0;
      cur->soundfx         = EMPTY;

      if ( loop == 30 )
      {
         cur->soundfx = FX_AIREXPLO;
         cur->fx_xpos = 100;
      }

      if ( loop == 32 )
      {
         cur->soundfx = FX_AIREXPLO;
         cur->fx_xpos = 100;
      }

   }
  
   if  ( MOVIE_Play ( frm, 1, palette ) == K_SKIPALL )
      return ( TRUE );
 
   GFX_FadeOut ( 0, 0, 0, 120 );

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();

   GFX_SetPalette ( palette, 0 );

   return ( FALSE );
}

/***************************************************************************
INTRO_EndGame() - Ends the current game anims
 ***************************************************************************/
VOID
INTRO_EndGame (
INT game
)
{
   IMS_StartAck();

   if ( !gameflag[game] )
      return;

   if ( game > 2 )
      game = 2;

   while ( IMS_IsAck() );
   IMS_StartAck();

   switch ( game )
   {
      default:
      case 0:
         INTRO_Game1End();
         break;

      case 1:
         INTRO_Game2End();
         break;

      case 2:
         INTRO_Game3End();
         break;
   }

   while ( IMS_IsAck() );
   IMS_StartAck();
   INTRO_Base();

   while ( IMS_IsAck() );
   IMS_StartAck();
   INTRO_Landing();

   while ( IMS_IsAck() );
   IMS_StartAck();
   WIN_WinGame ( game );

   if ( game < 1 )
      WIN_Order();
}

/***************************************************************************
INTRO_Taiwan (
 ***************************************************************************/
VOID
INTRO_Taiwan (
VOID
)
{
   volatile INT    loop;
   BYTE * pal1;
   BYTE * pic1;
   INT    local_cnt = FRAME_COUNT;

   FRAME_COUNT = 0;

   pic1 = GLB_GetItem ( TAIWARN_PIC );
   pal1 = GLB_GetItem ( TAIPAL_DAT );

   GFX_FadeOut ( 0, 0, 0, 5 );

   GFX_PutImage ( pic1, 0, 0, FALSE );
   GFX_DisplayUpdate();

   GFX_FadeIn ( pal1, 64 );

   for ( loop = 0; loop < 45; loop++ )
   {
      local_cnt = FRAME_COUNT;
      if ( IMS_IsAck() ) break;
      while ( FRAME_COUNT - local_cnt < 4 );
   }

   GFX_FadeOut ( 0, 0, 0, 63 );

   GLB_FreeItem ( TAIPAL_DAT );
   GLB_FreeItem ( TAIWARN_PIC );
}

/***************************************************************************
INTRO_Credits() - Credits Screen
 ***************************************************************************/
BOOL
INTRO_Credits (
VOID
)
{
   volatile INT    loop;
   BYTE * pal1;
   BYTE * pal2;
   BYTE * pic1;
   BYTE * pic2;
   INT    local_cnt = FRAME_COUNT;

   FRAME_COUNT = 0;

   pic1 = GLB_GetItem ( APOGEE_PIC );
   pal1 = GLB_GetItem ( POGPAL_DAT );

   GFX_FadeOut ( 0, 0, 0, 5 );

   GFX_PutImage ( pic1, 0, 0, FALSE );
   GFX_DisplayUpdate();

   GFX_FadeIn ( pal1, 64 );

   if ( ( bday_num != EMPTY ) && dig_flag )
      SND_Patch ( FX_THEME, 127 );
   else
      SND_PlaySong ( APOGEE_MUS, FALSE, FALSE );

   for ( loop = 0; loop < 30; loop++ )
   {
      local_cnt = FRAME_COUNT;
      if ( IMS_IsAck() ) break;
      while ( FRAME_COUNT - local_cnt < 4 );
   }

   if ( ( bday_num != EMPTY ) && dig_flag )
      while ( SND_IsPatchPlaying ( FX_THEME ) );
   else
      while ( SND_IsSongPlaying() )
      {
         if ( IMS_IsAck() ) break;
      }

   GFX_FadeOut ( 0, 0, 0, 63 );

   GLB_FreeItem ( POGPAL_DAT );
   GLB_FreeItem ( APOGEE_PIC );

   SND_PlaySong ( EMPTY, FALSE, TRUE );

   memset ( displayscreen, 0, 64000 );
   memset ( displaybuffer, 0, 64000 );

   pic2 = GLB_GetItem ( CYGNUS_PIC );
   pal2 = GLB_GetItem ( CYGPAL_DAT );

   GFX_PutImage ( pic2, 0, 0, FALSE );
   GFX_DisplayUpdate();

   GFX_FadeIn ( pal1, 64 );

   GLB_CacheItem ( RINTRO_MUS );

   for ( loop = 0; loop < 65; loop++ )
   {
      if ( IMS_IsAck() && loop > 0 ) break;

      if ( loop == 1 || loop == 40 )
         SND_Patch ( FX_BOSS1, 127 );

      if ( loop == 45 )
         SND_PlaySong ( RINTRO_MUS, TRUE, TRUE );

      if ( KBD_Key ( SC_ESC ) ) break;
      local_cnt = FRAME_COUNT;
      while ( FRAME_COUNT - local_cnt < 3 );
   }

   GFX_FadeOut ( 0, 0, 0, 63 );

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();

   GFX_SetPalette ( palette, 0 );

   GLB_FreeItem ( CYGPAL_DAT );
   GLB_FreeItem ( CYGNUS_PIC );

   IMS_StartAck();

   return ( FALSE );
}
  
/***************************************************************************
INTRO_BaseLanding() - BaseLanding PLays all needed MOVES
 ***************************************************************************/
VOID
INTRO_BaseLanding (
VOID
)
{
   if ( !GAME2 )
      return;

   if ( INTRO_Base() ) return;
   if ( INTRO_Landing() ) return;
}
  
/***************************************************************************
INTRO_PlayMain() - Plays Main Intro
 ***************************************************************************/
BOOL
INTRO_PlayMain (
VOID
)
{
   if ( INTRO_City() ) return ( TRUE );
   if ( INTRO_Side1() ) return ( TRUE );
   if ( INTRO_Pilot() ) return ( TRUE );
   if ( INTRO_Side2() ) return ( TRUE );
   if ( INTRO_Explosion() ) return ( TRUE );

   return ( FALSE );
}
