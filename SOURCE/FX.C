#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <string.h>
  
#include "raptor.h"
#include "prefapi.h"
  
#include "file0000.inc"

#define     MAX_SFXS  ( END_SFX - START_SFX - 1 ) / 5

PUBLIC      INT   music_volume;
PUBLIC      INT   fx_volume;
PUBLIC      BOOL  fx_flag  = FALSE;
PUBLIC      BOOL  dig_flag = FALSE;
PUBLIC      BOOL  gus_flag = FALSE;

PRIVATE     INT         num_dig           = 0;
PRIVATE     DWORD       cur_song_item     = EMPTY;  
PRIVATE     INT         cur_song_id       = EMPTY;  
PRIVATE     SND_TYPE    snd_type          = SND_NONE;

PRIVATE     BOOL        init_flag         = FALSE;

typedef struct
{
   DWORD          item;    // GLB ITEM;
   INT            pri;     // PRIORITY 0=LOW
   INT            pitch;   // PITCH TO PLAY PATCH
   BOOL           rpflag;  // TRUE = RANDOM PITCHES
   SFX_HANDLE     sid;     // DMX EFFECT ID
   INT            vol;     // VOLUME
   BOOL           gcache;  // CACHE FOR IN GAME USE ?
   BOOL           odig;    // TRUE = ONLY PLAY DIGITAL
}DFX;

typedef enum
{
   M_NONE = 0,// 0
   M_PC,      // 1
   M_ADLIB,   // 2
   M_GUS,     // 3
   M_PAS,     // 4
   M_SB,      // 5
   M_WAVE,    // 6
   M_CANVAS,  // 7
   M_GMIDI,   // 8
   M_AWE,     // 9
   M_LAST     // 10
}SCARD;

char cards[M_LAST][23] = {
   "None",
   "PC Speaker",
   "Adlib",
   "Gravis Ultra Sound",
   "Pro Audio Spectrum",
   "Sound Blaster",
   "WaveBlaster",
   "Roland Sound Canvas",
   "General Midi",
   "Sound Blaster AWE 32"
};

DFX    fxitems [ FX_LAST_SND ];

/***************************************************************************
SND_Setup() - Inits SND System  called after SND_InitSound() and GLB_Init
 ***************************************************************************/
VOID
SND_Setup (
VOID
)
{
   DFX   *  lib;
   INT      loop;

   memset ( fxitems, 0, sizeof ( fxitems ) );

   if ( snd_type == SND_NONE )
   {
      init_flag = FALSE;
      return;
   }

   init_flag = TRUE;

   // MONKEY 1 EFFECT ======================
   lib         = &fxitems [ FX_MON1 ];
   lib->item   = GLB_GetItemID ("MON1_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // MONKEY 2 EFFECT ======================
   lib         = &fxitems [ FX_MON2 ];
   lib->item   = GLB_GetItemID ("MON2_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // MONKEY 3 EFFECT ======================
   lib         = &fxitems [ FX_MON3 ];
   lib->item   = GLB_GetItemID ("MON3_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // MONKEY 4 EFFECT ======================
   lib         = &fxitems [ FX_MON4 ];
   lib->item   = GLB_GetItemID ("MON4_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // MONKEY 5 EFFECT ======================
   lib         = &fxitems [ FX_MON5 ];
   lib->item   = GLB_GetItemID ("MON5_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // MONKEY 6 EFFECT ======================
   lib         = &fxitems [ FX_MON6 ];
   lib->item   = GLB_GetItemID ("MON6_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // DAVE =================================
   lib         = &fxitems [ FX_DAVE ];
   lib->item   = GLB_GetItemID ("DAVE_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // THEME SONG ======================
   lib         = &fxitems [ FX_THEME ];
   lib->item   = GLB_GetItemID ("THEME_FX");
   lib->pri    = 0;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = TRUE;

   // AIR EXPLOSION ======================
   lib         = &fxitems [ FX_AIREXPLO ];
   lib->item   = GLB_GetItemID ("EXPLO_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // AIR EXPLOSION 2 ( BOSS ) ============
   lib         = &fxitems [ FX_AIREXPLO2 ];
   lib->item   = GLB_GetItemID ("EXPLO2_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // PICK UP BONUS ======================
   lib         = &fxitems [ FX_BONUS ];
   lib->item   = GLB_GetItemID ("BONUS_FX");
   lib->pri    = 0;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // SHIP LOSES SOMTHING CRASH ==========
   lib         = &fxitems [ FX_CRASH ];
   lib->item   = GLB_GetItemID ("CRASH_FX");
   lib->pri    = 0;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // DOOR OPENING =======================
   lib         = &fxitems [ FX_DOOR ];
   lib->item   = GLB_GetItemID ("DOOR_FX");
   lib->pri    = 0;
   lib->pitch  = 120;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // FLY BY SOUND =======================
   lib         = &fxitems [ FX_FLYBY ];
   lib->item   = GLB_GetItemID ("FLYBY_FX");
   lib->pri    = 0;
   lib->pitch  = 120;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ENERGY GRAB ========================
   lib         = &fxitems [ FX_EGRAB ];
   lib->item   = GLB_GetItemID ("EGRAB_FX");
   lib->pri    = 2;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 40;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // GROUND EXPLOSION ===================
   lib         = &fxitems [ FX_GEXPLO ];
   lib->item   = GLB_GetItemID ("GEXPLO_FX");
   lib->pri    = 2;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;
          
   // NORM GUN ===========================
   lib         = &fxitems [ FX_GUN ];
   lib->item   = GLB_GetItemID ("GUN_FX");
   lib->pri    = 10;
   lib->pitch  = 125;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 30;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // JET SOUND ==========================
   lib         = &fxitems [ FX_JETSND ];
   lib->item   = GLB_GetItemID ("JETSND_FX");
   lib->pri    = 4;
   lib->pitch  = 120;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 120;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_LASER  ];
   lib->item   = GLB_GetItemID ("LASER_FX");
   lib->pri    = 2;
   lib->pitch  = 120;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 50;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_MISSLE ];
   lib->item   = GLB_GetItemID ("MISSLE_FX");
   lib->pri    = 3;
   lib->pitch  = 120;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 50;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_SWEP ];
   lib->item   = GLB_GetItemID ("SWEP_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_TURRET ];
   lib->item   = GLB_GetItemID ("TURRET_FX");
   lib->pri    = 1;
   lib->pitch  = 128;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 60;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_WARNING ];
   lib->item   = GLB_GetItemID ("WARN_FX");
   lib->pri    = 2;
   lib->pitch  = 128;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 100;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ====================================
   lib         = &fxitems [ FX_BOSS1 ];
   lib->item   = GLB_GetItemID ("BOSS_FX");
   lib->pri    = 1;
   lib->pitch  = 127;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = TRUE;

   // INSIDE JET SOUND ==========================
   lib         = &fxitems [ FX_IJETSND ];
   lib->item   = GLB_GetItemID ("JETSND_FX");
   lib->pri    = 1;
   lib->pitch  = 235;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 45;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // ENEMY JET SOUND ==========================
   lib         = &fxitems [ FX_EJETSND ];
   lib->item   = GLB_GetItemID ("JETSND_FX");
   lib->pri    = 1;
   lib->pitch  = 65;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 120;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // INTRO E HIT ===========================
   lib         = &fxitems [ FX_INTROHIT ];
   lib->item   = GLB_GetItemID ("GUN_FX");
   lib->pri    = 1;
   lib->pitch  = 215;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // INTRO GUN ===========================
   lib         = &fxitems [ FX_INTROGUN ];
   lib->item   = GLB_GetItemID ("GUN_FX");
   lib->pri    = 1;
   lib->pitch  = 110;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = FALSE;
   lib->odig   = FALSE;

   // ENEMY SHOT ==========================
   lib         = &fxitems [ FX_ENEMYSHOT ];
   lib->item   = GLB_GetItemID ("ESHOT_FX");
   lib->pri    = 1;
   lib->pitch  = 100;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 50;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ENEMY LASER ==========================
   lib         = &fxitems [ FX_ENEMYLASER ];
   lib->item   = GLB_GetItemID ("LASER_FX");
   lib->pri    = 1;
   lib->pitch  = 70;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 120;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ENEMY MISSLE =========================
   lib         = &fxitems [ FX_ENEMYMISSLE ];
   lib->item   = GLB_GetItemID ("MISSLE_FX");
   lib->pri    = 2;
   lib->pitch  = 140;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 55;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // ENEMY SHOT ==========================
   lib         = &fxitems [ FX_ENEMYPLASMA ];
   lib->item   = GLB_GetItemID ("ESHOT_FX");
   lib->pri    = 2;
   lib->pitch  = 127;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // SHIELD HIT =========================
   lib         = &fxitems [ FX_SHIT ];
   lib->item   = GLB_GetItemID ("HIT_FX");
   lib->pri    = 1;
   lib->pitch  = 132;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 127;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // SHIP HIT =========================
   lib         = &fxitems [ FX_HIT ];
   lib->item   = GLB_GetItemID ("GUN_FX");
   lib->pri    = 1;
   lib->pitch  = 214;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 100;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // NO_SHOOT ==========================
   lib         = &fxitems [ FX_NOSHOOT ];
   lib->item   = GLB_GetItemID ("ESHOT_FX");
   lib->pri    = 2;
   lib->pitch  = 254;
   lib->rpflag = FALSE;
   lib->sid    = EMPTY;
   lib->vol    = 40;
   lib->gcache = TRUE;
   lib->odig   = FALSE;

   // PULSE CANNON ======================
   lib         = &fxitems [ FX_PULSE ];
   lib->item   = GLB_GetItemID ("ESHOT_FX");
   lib->pri    = 2;
   lib->pitch  = 100;
   lib->rpflag = TRUE;
   lib->sid    = EMPTY;
   lib->vol    = 50;
   lib->gcache = TRUE;
   lib->odig   = TRUE;

   for ( loop = 0; loop < FX_LAST_SND; loop++ )
   {
      lib = &fxitems [ loop ];
      lib->sid = EMPTY;

      if ( lib->item > 0  )
      {
         lib->item += ( DWORD )snd_type;
         GLB_CacheItem ( lib->item );
      }
      else
         lib->item = EMPTY;
   }
}

/***************************************************************************
SND_CacheFX () Caches all FX's
 ***************************************************************************/
VOID
SND_CacheFX (
VOID
)
{
   DFX   *  lib;
   INT      loop;

   for ( loop = 0; loop < FX_LAST_SND; loop++ )
   {
      lib = &fxitems [ loop ];

      if ( lib->item != EMPTY  )
         GLB_CacheItem ( lib->item );
   }
}


/***************************************************************************
SND_CacheGFX () Caches in Game FX's
 ***************************************************************************/
VOID
SND_CacheGFX (
VOID
)
{
   DFX   *  lib;
   INT      loop;

   SND_StopPatches();

   for ( loop = 0; loop < FX_LAST_SND; loop++ )
   {
      lib = &fxitems [ loop ];

      if ( lib->item != EMPTY && lib->gcache )
         GLB_CacheItem ( lib->item );
   }
}

/***************************************************************************
SND_CacheIFX () _ Caches intro and menu FX
 ***************************************************************************/
VOID
SND_CacheIFX (
VOID
)
{
   DFX   *  lib;
   INT      loop;

   for ( loop = 0; loop < FX_LAST_SND; loop++ )
   {
      lib = &fxitems [ loop ];

      if ( lib->item != EMPTY && !lib->gcache )
         GLB_CacheItem ( lib->item );
   }
}

/***************************************************************************
SND_IsPatchPlaying() - Returns TRUE if patch is playing
 ***************************************************************************/
BOOL
SND_IsPatchPlaying (
DEFX   type                  // INPUT : position in fxitems
)
{
   DFX  * curfld = &fxitems [ type ];

   if ( curfld->sid != EMPTY && SFX_Playing ( curfld->sid ) )
      return ( TRUE );

   return ( FALSE );
}

/***************************************************************************
SND_StopPatches () - STops all currently playing patches
 ***************************************************************************/
VOID
SND_StopPatches (
VOID
)
{
   DFX  * curfld;
   INT    loop;

   for ( curfld = fxitems, loop = 0; loop < FX_LAST_SND; loop++, curfld++ )
   {
      if ( curfld->sid != EMPTY )
      {
         SFX_StopPatch ( curfld->sid );
      }
   }

   for ( curfld = fxitems, loop = 0; loop < FX_LAST_SND; loop++, curfld++ )
   {
      if ( curfld->sid != EMPTY )
      {
         GLB_UnlockItem ( curfld->item );
         curfld->sid = EMPTY;
      }
   }
}

/***************************************************************************
SND_StopPatch () - STops Type patch
 ***************************************************************************/
VOID
SND_StopPatch (
DEFX  type                 // INPUT : DFX type patch to play
)
{
   DFX  * curfld = &fxitems [ type ];

   if ( curfld->sid != EMPTY )
   {
      SFX_StopPatch ( curfld->sid );
      GLB_UnlockItem ( curfld->item );
      curfld->sid = EMPTY;
   }
}

/***************************************************************************
SND_FreeFX () - Frees up Fx's
 ***************************************************************************/
VOID
SND_FreeFX (
VOID
)
{
   DFX   *  lib;
   INT      loop;

	SND_StopPatches();

   for ( loop = 0; loop < FX_LAST_SND; loop++ )
   {
      lib = &fxitems [ loop ];

      if ( lib->item > 0  )
         GLB_FreeItem ( lib->item );
   }
}

/***************************************************************************
SND_Patch () - Test patch to see if it will be played by SND_Play
 ***************************************************************************/
VOID
SND_Patch (
DEFX  type,                // INPUT : DFX type patch to play
INT   xpos                 // INPUT : 127=center
)
{
   DFX  * curfld;
   BYTE * patch;
   INT    loop;
   INT    numsnds;
   INT    rnd;
   INT    volume;

   if ( fx_volume < 1 )
      return;

   rnd      = 0;
   numsnds  = 0;

   for ( curfld = fxitems, loop = 0; loop < FX_LAST_SND; loop++, curfld++ )
   {
      if ( curfld->sid == EMPTY ) continue;

      if ( !SFX_Playing ( curfld->sid ) && type != loop )
         SND_StopPatch ( loop );
      else
         numsnds++;
   }

   if ( numsnds > (num_dig+2) )
      return;

   curfld = &fxitems [ type ];

   if ( curfld->odig && !dig_flag )
      return;

   if ( curfld->item != EMPTY )
   {
      if ( curfld->rpflag )
      {
         rnd = random ( 40 );
         rnd -= 20;
      }

      patch = GLB_LockItem ( curfld->item );

      volume = (curfld->vol * fx_volume) / 127;

      curfld->sid = SFX_PlayPatch ( patch, curfld->pitch + rnd, xpos, volume, 0, curfld->pri );
   }

   return;
}

/***************************************************************************
SND_3DPatch () - playes a patch in 3d for player during game play
 ***************************************************************************/
VOID
SND_3DPatch (
DEFX  type,                // INPUT : DFX type patch to play
INT   x,                   // INPUT : x sprite center
INT   y                    // INPUT : y sprite center
)
{
   DFX  * curfld;
   BYTE * patch;
   INT    loop;
   INT    numsnds;
   INT    rnd;
   INT    xpos;
   INT    vol;
   INT    dx;
   INT    dy;
   INT    dist;
   INT    volume;

   if ( fx_volume < 1 )
      return;

   rnd      = 0;
   numsnds  = 0;

   for ( curfld = fxitems, loop = 0; loop < FX_LAST_SND; loop++, curfld++ )
   {
      if ( curfld->sid == EMPTY ) continue;

      if ( !SFX_Playing ( curfld->sid ) )
         SND_StopPatch ( loop );
      else
         numsnds++;
   }

   if ( numsnds > (num_dig+2) )
      return;

   dx = x - player_cx;
   dy = y - player_cy;

   xpos = 127 + dx;

   if ( xpos < 1 )
      xpos = 1;
   else if ( xpos > 255 )
      xpos = 255;

   dx = abs ( dx );
   dy = abs ( dy );

   dist = dx + dy - ( dx < dy ? dx : dy ) / 2;
                                         
   if ( dist < SND_CLOSE )
      vol = 127;
   else if ( dist > SND_FAR )
      vol = 1;
   else vol = 127 - ( ( dist - SND_CLOSE ) * 127 ) / ( SND_FAR-SND_CLOSE );

   curfld = &fxitems [ type ];

   if ( curfld->odig && !dig_flag )
      return;

   if ( curfld->rpflag )
   {
      rnd = random ( 40 );
      rnd -= 20;
   }

   volume = ( vol * fx_volume) / 127;
   volume = ( volume * curfld->vol ) / 127;

   patch = GLB_LockItem ( curfld->item );
   curfld->sid = SFX_PlayPatch ( patch, curfld->pitch + rnd, xpos, volume, 0, curfld->pri );
}


/***************************************************************************
SND_PlaySong() - Plays song associated with song id
 ***************************************************************************/
VOID
SND_PlaySong (
DWORD item,                // INPUT : Song GLB item
BOOL  chainflag,           // INPUT : Chain Song to ItSelf
BOOL  fadeflag             // INPUT : Fade Song Out
)
{
   BYTE * song;

   if ( music_volume < 1 )
      return;

   if ( cur_song_item == item ) return;

   if ( cur_song_id != EMPTY )
   {
      if ( fadeflag )
      {
         if ( MUS_QrySongPlaying ( cur_song_id ) )
            MUS_FadeOutSong ( cur_song_id, 1000 );

         while ( MUS_QrySongPlaying ( cur_song_id ) );
      }
      else
         MUS_StopSong ( cur_song_id );

      MUS_UnregisterSong ( cur_song_id );
      GLB_UnlockItem ( cur_song_item );
      cur_song_item     = EMPTY;
      cur_song_id       = EMPTY;
   }

   if ( item == EMPTY ) return;

   cur_song_item     = item;
   song              = GLB_LockItem ( item );
   cur_song_id       = MUS_RegisterSong ( song );

   if ( cur_song_id != -1 )
   {
      if ( chainflag )
         MUS_ChainSong ( cur_song_id, cur_song_id );

      if ( fadeflag )
         MUS_FadeInSong ( cur_song_id, 1000 );
      else
         MUS_PlaySong ( cur_song_id, 127 );
   }
}

/***************************************************************************
SND_IsSongPlaying () - Is current song playing
 ***************************************************************************/
BOOL
SND_IsSongPlaying (
VOID
)
{

   if ( cur_song_id != EMPTY )
   {
      if ( MUS_QrySongPlaying ( cur_song_id ) )
         return ( TRUE );
   }

   return ( FALSE );
}

/***************************************************************************
SND_FadeOutSong () - Fades current song out and stops playing music
 ***************************************************************************/
VOID
SND_FadeOutSong (
VOID
)
{
   if ( cur_song_id != EMPTY )
   {
      if ( MUS_QrySongPlaying ( cur_song_id ) )
         MUS_FadeOutSong ( cur_song_id, 1000 );

      while ( MUS_QrySongPlaying ( cur_song_id ) );
      MUS_UnregisterSong ( cur_song_id );
      GLB_UnlockItem ( cur_song_item );
   }

   cur_song_item  = EMPTY;
   cur_song_id    = EMPTY;
}

/***************************************************************************
SND_InitSound () - Does bout all i can think of for Music/FX initing
 ***************************************************************************/
VOID
SND_InitSound (
VOID
)
{
   volatile INT initmcard;
   volatile INT initdcard;
   volatile INT cardtype;
   volatile INT midiport;
   INT port;
   INT irq;
   INT dma;
   INT channels;
   BYTE * amem;
   BYTE * gmem;
   INT  rval;

   dig_flag = FALSE;
   amem = NUL;
   gmem = NUL;
   snd_type = SND_NONE;

   music_volume = INI_GetPreferenceLong ( "Music", "Volume", 127 );
   cardtype = INI_GetPreferenceLong ( "Music", "CardType", 0L );
   port     = INI_GetPreferenceHex ( "Music", "BasePort", -1L );
   midiport = INI_GetPreferenceHex ( "Music", "MidiPort", -1L );
   irq      = INI_GetPreferenceLong ( "Music", "Irq", -1L );
   dma      = INI_GetPreferenceLong ( "Music", "Dma", -1L );

   switch ( cardtype )
   {
      default:
      case M_NONE:
         initmcard = 0;
         break;

      case M_PC:
         initmcard = 0;
         break;

      case M_AWE:
         initmcard = AHW_AWE32;
         break;


      case M_SB:
      case M_ADLIB:
      case M_PAS:
         if ( !AL_Detect ( NUL, NUL ) )
            initmcard = AHW_ADLIB;
         else
            initmcard = -1;
         amem = GLB_GetItem ( GENMIDI_OP2 );
         AL_SetCard ( 6, amem );
         GLB_FreeItem ( GENMIDI_OP2 );
         break;

      case M_GUS:
         if  ( !GF1_Detect() )
         {
            gmem = GLB_GetItem ( DMXGUS_INI );
            GF1_SetMap ( gmem, GLB_ItemSize ( DMXGUS_INI ) );
            GLB_FreeItem ( DMXGUS_INI );
            initmcard = AHW_ULTRA_SOUND;
         }
         else
            initmcard = -1;
         break;

      case M_GMIDI:
      case M_CANVAS:
      case M_WAVE:
         if ( !MPU_Detect ( &midiport, NUL ) )
         {
            initmcard = AHW_MPU_401;
            MPU_SetCard ( midiport );
         }
         else
            initmcard = -1;
         break;
   }

   if ( initmcard == EMPTY )
   {
      initmcard = 0;
      printf ("%s Not Found ( Music Disabled )\n", cards [ cardtype ] );
   }
   else printf ("Music Enabled (%s)\n", cards [ cardtype ] );

   fx_volume 	= INI_GetPreferenceLong ( "SoundFX", "Volume", 127 );
   cardtype 	= INI_GetPreferenceLong ( "SoundFX", "CardType", 0L );
   port     	= INI_GetPreferenceHex ( "SoundFX", "BasePort", -1L );
   midiport 	= INI_GetPreferenceHex ( "SoundFX", "MidiPort", -1L );
   irq      	= INI_GetPreferenceLong ( "SoundFX", "Irq", -1L );
   dma      	= INI_GetPreferenceLong ( "SoundFX", "Dma", -1L );
   channels 	= INI_GetPreferenceLong ( "SoundFX", "Channels", 2 );

   switch ( cardtype )
   {
      default:
      case M_NONE:
         initdcard = 0;
         snd_type = SND_NONE;
         break;

      case M_PC:
         initdcard = AHW_PC_SPEAKER;
         snd_type = SND_PC;
         break;

      case M_ADLIB:
         if ( !AL_Detect ( NUL, NUL ) )
         {
            initdcard = AHW_ADLIB;
            snd_type = SND_MIDI;
         }
         else
            initdcard = -1;
         if ( !amem )
         {
            amem = GLB_GetItem ( GENMIDI_OP2 );
            AL_SetCard ( 6, amem );
            GLB_FreeItem ( GENMIDI_OP2 );
         }
         break;

      case M_GUS:
         if  ( !GF1_Detect() )
         {
            dig_flag = TRUE;
            initdcard = AHW_ULTRA_SOUND;
            snd_type = SND_DIGITAL;

            if ( !gmem )
            {
               gmem = GLB_GetItem ( DMXGUS_INI );
               GF1_SetMap ( gmem, GLB_ItemSize ( DMXGUS_INI ) );
               GLB_FreeItem ( DMXGUS_INI );
            }
         }
         else
            initdcard = -1;
         break;

      case M_PAS:
         if ( !MV_Detect() )
         {
            snd_type = SND_DIGITAL;
            dig_flag = TRUE;
            initdcard = AHW_MEDIA_VISION;
         }
         else
            initdcard = -1;
         break;

      case M_SB:
         if ( !SB_Detect ( &port, &irq, &dma, NUL ) )
         {
               dig_flag = TRUE;
               initdcard = AHW_SOUND_BLASTER;
               SB_SetCard ( port, irq, dma );
               snd_type = SND_DIGITAL;
         }
         else
             initdcard = -1;
         break;

      case M_GMIDI:
      case M_CANVAS:
      case M_WAVE:
         if ( !MPU_Detect ( &midiport, NUL ) )
         {
            initdcard = AHW_MPU_401;
            MPU_SetCard ( midiport );
            snd_type = SND_MIDI;
         }
         else
            initdcard = -1;
         break;
   }

   if ( initdcard == EMPTY )
   {
      initdcard = 0;
      printf ("%s Not Found ( Sound FX Disabled )\n", cards [ cardtype ] );
   }
   else
   {
      fx_flag = TRUE;
      printf ("SoundFx Enabled (%s)\n", cards [ cardtype ] );
   }

   rval = DMX_Init ( 70, 20, initmcard, initdcard );

   printf ("DMX_Init() = %d\n", rval );

   if ( rval == 0 )
   {
      dig_flag = FALSE;
      fx_flag  = FALSE;
      cardtype = M_NONE;
   }

   num_dig 	= 0;
   gus_flag = FALSE;

   if ( channels < 1 || channels > 8 )
      channels = 2;

   if ( cardtype == M_SB || cardtype == M_GUS || cardtype == M_PAS )
   {
      num_dig = channels;

      if ( cardtype == M_GUS && num_dig < 2 )
         gus_flag = TRUE;

      WAV_PlayMode ( channels, 11025 );
   }
   else
      num_dig = 1;

   MUS_SetMasterVolume ( music_volume );

}
