// STIKING DISTANCE =========================
// 7:30
// 9:45
// MALICE ===================================
// 10:35
// THREE MUSKETEERS =========================
// 10:45
// MRS DF ====================================
// 10:00
// 10:25
// PERFECT WORLD ============================
// 10:35 
// CARLITOS WAY =============================
// 10:25
//  game 2 - 8  game 3 - 3
// PET DETECTIVE ===========================
// 7:30 
//

//================================================

// #define TESTMODE 1

#include "regver.h"

//================================================


#define MEM_KEEP   ( 16 * 1024 )
#define MIN_MEMREQ ( 480 * 1024 )
#define MAX_HMEM   ( 4096 * 1024 )
#define REQ_HMEM   ( 128 * 1024 )
#define REQ_LMEM   ( 500 * 1024 )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <conio.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <ctype.h>
#include <malloc.h>

#include "raptor.h"
#include "prefapi.h"

#include "hangar.inc"
  
#include "file0000.inc"
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"

BYTE * palette;
BYTE * cursor_pic;
BYTE   gpal[768];

PUBLIC INT           g_flash;
PUBLIC DWORD         curship[16];
PUBLIC DWORD         lship[8];
PUBLIC DWORD         dship[8];
PUBLIC DWORD         fship[8];
PUBLIC INT           o_engine [8]   = { 0, 1, 2, 3, 2, 1, 0 };
PUBLIC INT           o_gun1 [8]     = { 1, 3, 5, 6, 5, 3, 1 };
PUBLIC INT           o_gun2 [8]     = { 1, 3, 6, 9, 6, 3, 2 };
PUBLIC INT           o_gun3 [8]     = { 2, 6, 8, 11, 8, 6, 2 };
 
PUBLIC INT           gl_cnt         = 0;
PUBLIC BOOL          end_wave       = FALSE;  
PUBLIC PLAYEROBJ     plr;
PUBLIC BOOL          gameflag[4] = { FALSE, FALSE, FALSE, FALSE };
PUBLIC INT           player_cx      = PLAYERINITX;
PUBLIC INT           player_cy      = PLAYERINITY;
PUBLIC INT           playerx        = PLAYERINITX;
PUBLIC INT           playery        = PLAYERINITY;
PUBLIC INT           playerbasepic  = 3;
PUBLIC INT           playerpic      = 4;
PUBLIC INT           cur_game       = 0;
PUBLIC INT           game_wave[4]   = {0,0,0,0};
PUBLIC MAZELEVEL *   ml;
PUBLIC INT           fadecnt        = 0;
PUBLIC BOOL          startfadeflag  = FALSE;
PUBLIC BOOL          fadeflag       = FALSE;
PUBLIC BOOL          start_end_fade = FALSE;
PUBLIC BOOL          end_fadeflag   = FALSE;
PUBLIC FLATS *       flatlib [ 4 ];
PUBLIC BOOL          debugflag      = FALSE;
PUBLIC BOOL          testflag       = FALSE;
PUBLIC INT           g_oldshield    = EMPTY;
PUBLIC INT           g_oldsuper     = EMPTY;
PUBLIC BYTE *        numbers[11];
PUBLIC INT           curplr_diff    = DIFF_2;

PUBLIC INT          startendwave   = EMPTY;
PUBLIC BOOL         draw_player;
PUBLIC BOOL         bday_flag      = FALSE;
PUBLIC INT          bday_num       = EMPTY;

//==============================================

PUBLIC  CHAR         gdmodestr[] = "CASTLE";
PUBLIC  BOOL         lowmem_flag = FALSE;
PUBLIC  BOOL         reg_flag    = FALSE;
PUBLIC  BOOL         tai_flag    = FALSE;
PUBLIC  BOOL         godmode     = FALSE;

#define ROTPAL_START 240 
#define FADE_FRAMES  20
INT shakes[ FADE_FRAMES ] = { -4,4,-3,3,-2,2,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,0,0 };
#define MAX_GLOW 20
INT glowtable [ 20 ] = { 0,1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1 };
  
CHAR flatnames [4][14] = {
   "FLATSG1_ITM",
   "FLATSG2_ITM",
   "FLATSG3_ITM",
   "FLATSG4_ITM"
};
  
PRIVATE CHAR * copyright = "RAPTOR 'Call of the Shadows' Copyright 1994 Cygnus Studios Inc.";

PRIVATE BYTE * g_lowmem;
PRIVATE BYTE * g_highmem;
PUBLIC INT demo_flag = DEMO_OFF;

typedef struct
{
   INT      month;
   INT      day;
   INT      year;
   CHAR *   name;
}BDAY;

#define MAX_BDAY 6

BDAY bday [ MAX_BDAY ];

#define MONTH     2
#define DAY       20
#define YEAR      1994
#define WLENGTH   12

VOID
RAP_Bday (
VOID
)
{
   struct dosdate_t date;
   INT loop;

   bday_flag = FALSE;
   bday_num  = EMPTY;

   _dos_getdate ( &date );

   for ( loop = 0; loop < MAX_BDAY; loop++ )
   {
      if ( bday [ loop ]. day == 0 ) continue;

      if ( date.month == bday [ loop ].month &&
           date.day == bday [ loop ].day
         && date.year >= bday [ loop ].year )
      {
         bday_num = loop;
         if ( loop == 0 )
            bday_flag = TRUE;
         break;
      }
   }
}


BOOL
RAP_IsDate (
VOID
)
{
   struct dosdate_t date;
   INT comp1;
   INT comp2;
   INT num;

   _dos_getdate ( &date );

   if ( date.year != YEAR )
      return ( FALSE );

   comp1 = date.month;
   comp1 = comp1 << 5;
   comp1 += date.day;

   comp2 = MONTH;
   comp2 = comp2 << 5;
   comp2 += DAY;

   num = comp1 - comp2;

   if ( num >= 0 && num <= WLENGTH )
      return ( TRUE );

   return ( FALSE );
}

VOID
RAP_PrintVmem (
CHAR * desc
)
{
	DWORD	 largest;
	DWORD	 totalfree;
	DWORD	 totallocked;
	DWORD	 totalused;
	DWORD	 discarded;

	VM_GetCoreInfo ( &largest, &totalfree, &totallocked, &totalused, &discarded );
	printf( "\n%s\nVM CORE INFO\nLargest Block: %7d\nAmount Locked: %7d\n    Discarded: %7d\n  Amount Free: %7d\n  Amount Used: %7d\n        Total: %7d\n",
		desc, largest, totallocked, discarded, totalfree, totalused, totalused + totalfree );
}

/*==========================================================================
InitScreen (
 ==========================================================================*/
VOID
InitScreen (
VOID
)
{
   union  REGS   regs;
   BYTE * scradr = (BYTE *)0xB8000;
   INT    loop;
   BYTE   color;
   INT    port = 0x3c8;
   CHAR  *  msg = " RAPTOR: Call Of The Shadows V1.2                        (c)1994 Cygnus Studios";

   regs.w.ax = 0x3;
   int386(0x10,(const union REGS *) &regs, &regs);

   regs.w.ax = 0x200;
   regs.h.bh = 0;
   regs.h.dl = 0;
   regs.h.dh = 0;
   int386(0x10,(const union REGS *) &regs, &regs);

   color = 0;
   color = ( 1<<4 ) + 14;

   outp ( port, 1 );
   port++;
   outp ( port, 1 );
   outp ( port, 5 );
   outp ( port, 16 );

   for ( loop =0; loop < 160; loop++ )
   {
      if ( ( loop & 1 ) )
         *(scradr + loop) = color;
      else
      {
         *(scradr + loop) = *msg;
         msg++;
      }
   }

   printf ("\n");
}

/*==========================================================================
   ShutDown () Shut Down function called by EXIT_xxx funtions
 ==========================================================================*/
SPECIAL VOID
ShutDown (
INT   errcode
)
{
   union    REGS   regs;
   volatile BYTE * scradr = (VOID *)0xB8000;
   volatile BYTE * mem;
   volatile int    loop;
   volatile int    cnt  = 0;
   volatile int    i;

   if ( !errcode && !godmode )
      WIN_Order();

   GLB_FreeAll();

   IPT_DeInit();
   DMX_DeInit();
   TSM_Remove();

   GFX_EndSystem();

   if ( !errcode )
   {
      regs.w.ax = 0x200;
      regs.h.bh = 0;
      regs.h.dl = 0;
      regs.h.dh = 22;
      int386( 0x10, (const union REGS *) &regs, &regs);

      #ifndef LCR_VERSION
      if ( reg_flag )
         mem = GLB_LockItem ( LASTSCR2_TXT );
      else
         mem = GLB_LockItem ( LASTSCR1_TXT );
      #else
         mem = GLB_LockItem ( LASTSCR3_TXT );
      #endif

      for ( loop = 0; loop < (4000-(160*2)); loop++ )
      {
         *scradr = *mem;
         scradr++;
         mem++;
         for (i=0;i<5;i++)
         {
            cnt = cnt + (INT)mem / (INT)scradr - cnt;
            cnt++;
         }
      }

      if ( reg_flag )
         GLB_FreeItem ( LASTSCR2_TXT );
      else
         GLB_FreeItem ( LASTSCR1_TXT );
   }

   PTR_End();
   KBD_End();
   SWD_End();

   free ( g_highmem );
}

VOID
RAP_ClearSides (
VOID
)
{
   GFX_ColorBox ( 0, 0, MAP_LEFT, 200, 0 );
   GFX_ColorBox ( 320-MAP_LEFT, 0, MAP_LEFT, 200, 0 );
}

/***************************************************************************
RAP_GetShipPic () - Loads Correct Ship Pics for Light/Dark Waves
 ***************************************************************************/
VOID
RAP_GetShipPic (
VOID
)
{
   INT   loop;
   BOOL  lightflag = TRUE;

   // GAME 2 wave 8
   if ( cur_game == 1 && game_wave [ cur_game ] == 7 )
      lightflag = FALSE;

   // GAME 3 wave 3
   if ( cur_game == 2 && game_wave [ cur_game ] == 2 )
      lightflag = FALSE;

   for ( loop = 0; loop < 7; loop++ )
   {
      if ( lightflag )
      {
         curship [ loop ]     = lship [ loop ];
         curship [ loop + 7 ] = lship [ loop ];
      }
      else
      {
         curship [ loop ]     = dship [ loop ];
         curship [ loop + 7 ] = fship [ loop ];
      }
   }

   for ( loop = 0; loop < 14; loop++ )
      GLB_CacheItem ( curship [ loop ] );
}
  
/***************************************************************************
   Rot_Color () - Rotates color in palette
 ***************************************************************************/
VOID
Rot_Color (
BYTE *gpal,
INT snum,
INT len
)
{
   short pos, maxloop;
   char h1[3];
  
   pos = snum*3;
   maxloop=len*3;
  
   memcpy ( h1, gpal + pos, 3 );
   memcpy ( gpal + pos, gpal + pos + 3, maxloop );
   memcpy ( gpal + pos + maxloop - 3, h1, 3 );
}
  
/***************************************************************************
   InitMobj() - Inits an object to be moved
 ***************************************************************************/
VOID
InitMobj(
MOVEOBJ * cur              // INPUT : pointer to MOVEOBJ
)
{
   cur->done = FALSE;
   cur->addx = 1;
   cur->addy = 1;
  
   cur->delx = cur->x2 - cur->x;
   cur->dely = cur->y2 - cur->y;
  
   if ( cur->delx < 0 )
   {
      cur->delx = -cur->delx;
      cur->addx = -cur->addx;
   }
   if ( cur->dely < 0 )
   {
      cur->dely = -cur->dely;
      cur->addy = -cur->addy;
   }
  
   if ( cur->delx >= cur->dely )
   {
      cur->err = -(cur->dely>>1);
      cur->maxloop = cur->delx + 1;
   }
   else
   {
      cur->err = (cur->delx>>1);
      cur->maxloop = cur->dely + 1;
   }
}
  
/***************************************************************************
   MoveMobj() - gets next postion for an Object
 ***************************************************************************/
VOID
MoveMobj(
MOVEOBJ * cur              // INPUT : pointer to MOVEOBJ
)
{
   if ( cur->maxloop == 0 )
   {
      cur->done = TRUE;
      return;
   }
  
   if ( cur->delx >= cur->dely )
   {
      cur->x += cur->addx;
      cur->err += cur->dely;
  
      if ( cur->err > 0 )
      {
         cur->y += cur->addy;
         cur->err -= cur->delx;
      }
   }
   else
   {
      cur->y += cur->addy;
      cur->err += cur->delx;
  
      if ( cur->err > 0 )
      {
         cur->x += cur->addx;
         cur->err -= cur->dely;
      }
   }
  
   cur->maxloop--;
}

/***************************************************************************
   MoveSobj() - gets next postion for an Object at speed
 ***************************************************************************/
INT
MoveSobj(
MOVEOBJ * cur,             // INPUT : pointer to MOVEOBJ
INT       speed            // INPUT : speed to plot at
)
{
   if ( speed == 0 ) return ( 0 );

   if ( cur->delx >= cur->dely )
   {
      while ( speed )
      {
         speed--;
         cur->maxloop--;
         cur->x += cur->addx;
         cur->err += cur->dely;
  
         if ( cur->err > 0 )
         {
            cur->y += cur->addy;
            cur->err -= cur->delx;
         }
      }
   }
   else
   {
      while ( speed )
      {
         speed--;
         cur->maxloop--;
         cur->y += cur->addy;
         cur->err += cur->delx;
  
         if ( cur->err > 0 )
         {
            cur->x += cur->addx;
            cur->err -= cur->dely;
         }
      }
   }
  
   if ( cur->maxloop < 1 )
      cur->done = TRUE;

   return ( speed );
}

/***************************************************************************
RAP_PrintNum()
 ***************************************************************************/
VOID
RAP_PrintNum(
INT x,
INT y,
CHAR * str
)
{
   INT       maxloop;
   INT       num;

   maxloop = strlen ( str );

   GFX_PutSprite ( numbers[10], x, y );
   x += 9;

   while ( maxloop-- )
   {
      num = (INT)((*str)-'0');

      if ( num < 11 && num >=0 )
         GFX_PutSprite ( numbers[num], x ,y );

      x += 8;
      str++;
   }
}

/***************************************************************************
RAP_DisplayShieldLevel (
 ***************************************************************************/
VOID
RAP_DisplayShieldLevel (
INT xpos,
INT level
)
{
   INT    loop;
   BYTE * outbuf;
   DWORD  addx;
   DWORD  curs;

   outbuf = displayscreen + ylookup[199];
   outbuf += xpos;

   addx = ( SHIELD_COLOR_RUN << 16 ) / MAX_SHIELD;
   curs = 0;

   for ( loop = 0; loop < MAX_SHIELD; loop++ )
   {
      if ( loop < level )
         memset ( outbuf, 74 - ( curs >> 16 ) ,4 );
      else
         memset ( outbuf, 0, 4 );

      curs += addx;

      outbuf -= 640;
   }
}

/***************************************************************************
RAP_DisplayStats()
 ***************************************************************************/
VOID
RAP_DisplayStats (
VOID
)
{
   static      INT    damage = EMPTY;
   static      BOOL   blinkflag = TRUE;
   CHAR        temp [ 21 ];
   INT         shield;
   INT         super;
   INT         loop;
   INT         x;
   INT         y;
   BYTE  *     pic;
   GFX_PIC *   h;

   // == DISPLAY SUPER SHIELD ========================
   super = OBJS_GetAmt ( S_SUPER_SHIELD );
   if ( g_oldsuper != super )
   {
      RAP_DisplayShieldLevel ( MAP_LEFT - 8, super );
      g_oldsuper = super;
   }

   // == DISPLAY NORM SHIELD ========================
   shield = OBJS_GetAmt ( S_ENERGY );
   if ( g_oldshield != shield )
   {
      RAP_DisplayShieldLevel ( MAP_RIGHT + 4, shield );
   }

   if ( shield <= 0 && !godmode )
   {
      // BLOW UP SHIP IF ! IN GOD MODE ===================

      ANIMS_StartAnim ( A_MED_AIR_EXPLO, playerx + random (32), playery + random (32) );
      ANIMS_StartAnim ( A_SMALL_AIR_EXPLO, playerx + random (32), playery + random (32) );

      if ( startendwave > END_EXPLODE )
      {
         if ( random(2) == 0 )
            SND_Patch ( FX_AIREXPLO, 30 );
         else
            SND_Patch ( FX_AIREXPLO, 225 );
      }

      if ( startendwave == EMPTY )
         startendwave = END_DURATION;

      if ( startendwave == END_EXPLODE )
      {
         draw_player = FALSE;
         SND_Patch ( FX_AIREXPLO, 127 );
         SND_Patch ( FX_AIREXPLO2, 127 );
         ANIMS_StartAnim ( A_LARGE_AIR_EXPLO, player_cx, player_cy );
         for ( loop = 0; loop < ( PLAYERWIDTH * PLAYERHEIGHT ) / 2; loop++ )
         {
            x = playerx - (PLAYERWIDTH/2)  + random ( PLAYERWIDTH*2 );
            y = playery - (PLAYERHEIGHT/2) + random ( PLAYERHEIGHT*2 );
            if ( loop & 1)
               ANIMS_StartAnim ( A_LARGE_AIR_EXPLO, x, y );
            else
               ANIMS_StartAAnim ( A_MED_AIR_EXPLO2, x, y );
         }
         SND_Patch ( FX_AIREXPLO2, 127 );
      }
   }

   // IF END OF WAVE FLY SHIP OFF SCREEN ===================

   if ( startendwave != EMPTY && shield > 0 )
   {
      if ( startendwave == END_FLYOFF )
      {
         IPT_PauseControl ( TRUE );
         SND_Patch ( FX_FLYBY, 127 );
      }

      if ( startendwave < END_FLYOFF )
      {
         x = 0;
         if ( playerx < ( 160 - 8 ) )
            x = 8;
         else if ( playerx > ( 160 + 8 ) )
            x = -8;

         IPT_FMovePlayer ( x, -4 );
      }
   }

   if ( shield <= SHIELD_LOW && !godmode )
   {
      if ( ( gl_cnt % 8 ) == 0 )
      {
         blinkflag ^= TRUE;
         if ( blinkflag )
         {
            if ( damage )
               damage--;
         }
      }

      if ( shield < g_oldshield && super < 1 )
      {
         if ( OBJS_LoseObj() )
         {
            SND_Patch ( FX_CRASH, 127 );
            damage = 2;
         }
      }

      if ( blinkflag )
      {
         if ( damage )
         {
            pic = GLB_GetItem ( WEPDEST_PIC );
            h = ( GFX_PIC * )pic;
            GFX_PutSprite ( pic, ( ( 320 - h->width ) >> 1 ), MAP_BOTTOM - 9 );
         }

         if ( startendwave == EMPTY )
            SND_Patch ( FX_WARNING, 127 );
         pic = GLB_GetItem ( SHLDLOW_PIC );
         h = ( GFX_PIC * )pic;
         GFX_PutSprite ( pic, ( ( 320 - h->width ) >> 1 ), MAP_BOTTOM );
      }
   }

   g_oldshield = shield;

   OBJS_DisplayStats();

   sprintf ( temp, "%08u", plr.score );
   RAP_PrintNum ( 119, MAP_TOP, temp );

   if ( demo_mode == DEMO_RECORD )
      DEMO_DisplayStats();

   if ( debugflag )
   {
      sprintf ( temp, "%02u", plr.diff [ cur_game ] );
      RAP_PrintNum ( 18, MAP_TOP, temp );

      x = MAP_LEFT + 16;
      for ( loop = 0; loop < 16; loop++ )
      {
         GFX_ColorBox ( x, 0, 8, 8, 240 + loop );
         x+=8;
      }
   }
}

VOID
RAP_PaletteStuff (
VOID
)
{
   static   INT wblink = 0;
   static   INT glow1 = 0;
   static   INT glow2 = 8;
   static   cnt = 0;
   static   palcnt = 0;
   static   blink = 0;
   INT      offset;
   INT      num;
   BYTE *   pal1;
   BYTE *   pal2;

   if ( cnt & 1 )
   {
      // == COLOR 240 - 244 ======== WATER
      if ( palcnt % 4 )
         Rot_Color ( gpal, 240, 5 );

      // == COLOR 245 - 249 ======== FIRE
      if ( palcnt % 2 )
         Rot_Color ( gpal, 245, 5 );

      // == COLOR 250 ======== GLOWING FIRE 1
      offset =  ( 250 * 3 );
      pal1 = gpal + offset;
      pal2 = palette + offset;

      num = glowtable [ glow1 ];
      glow1++;
      glow1 = glow1 % MAX_GLOW;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;
      pal1++;
      pal2++;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;
      pal1++;
      pal2++;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;

      // == COLOR 251 ======== GLOWING FIRE 2
      offset =  ( 251 * 3 );
      pal1 = gpal + offset;
      pal2 = palette + offset;

      num = glowtable [ glow2 ];
      glow2++;
      glow2 = glow2 % MAX_GLOW;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;
      pal1++;
      pal2++;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;
      pal1++;
      pal2++;

      *pal1 = *pal2 - (BYTE)num;
      if ( *pal2 < num ) *pal1 = 0;

      // == COLOR 252 & 253 ======== BLINKING REG AND GREEN
      if ( palcnt % 2 )
      {
         offset =  ( 252 * 3 );
         pal1 = gpal + offset;
         pal2 = palette + offset;

         if ( blink & 1 )
         {
            memcpy ( pal1, pal2, 3 );
            pal1+=3;
            pal2+=3;
            memset ( pal1, 0, 3 );
         }
         else
         {
            memset ( pal1, 0, 3 );
            pal1+=3;
            pal2+=3;
            memcpy ( pal1, pal2, 3 );
         }

         blink++;
      }

      // == COLOR 254 ======== BLINKING BLUE
      offset =  ( 254 * 3 );
      if ( random ( 3 ) == 0 )
      {
         pal1 = gpal + offset;
         pal2 = palette + offset;
         memcpy ( pal1, pal2, 3 );
      }
      else
      {
         pal1 = gpal + offset;
         memset ( pal1, 0, 3 );
      }

      // == COLOR 255 ======== BLINKING WHITE

      offset =  ( 255 * 3 );
      if ( wblink < 2 )
      {
         pal1 = gpal + offset;
         pal2 = palette + offset;
         memcpy ( pal1, pal2, 3 );
      }
      else
      {
         pal1 = gpal + offset;
         memset ( pal1, 0, 3 );
      }

      wblink++;
      wblink = wblink %6;

      GFX_SetPalette ( gpal, 240 );
      palcnt++;
   }

   cnt++;
}

/***************************************************************************
Do_Game () - The main game thing this is it dude
 ***************************************************************************/
BOOL           // TRUE=Aborted, FALSE = timeout
Do_Game (
VOID
)
{
   volatile INT  local_cnt;
   BOOL          b2_flag      = FALSE;
   BOOL          b3_flag      = FALSE;
   BOOL          init_flag    = TRUE;
   BOOL          rval         = FALSE;
   INT           start_score  = 0;

   draw_player = TRUE;

   srand ( ( 1024 * game_wave [ cur_game ] ) );

   fadeflag = FALSE;
   end_fadeflag = FALSE;
   KBD_Clear();
   IMS_StartAck();
   BUT_1 = FALSE;
   BUT_2 = FALSE;
   BUT_3 = FALSE;
   BUT_4 = FALSE;
  
   playerx = PLAYERINITX;
   playery = PLAYERINITY;
   PTR_SetPos ( playerx, playery );
   IPT_Start();

   RAP_GetShipPic();
   BONUS_Clear();
   ANIMS_Clear();
   SHOTS_Clear();
   ESHOT_Clear();

   memcpy ( gpal, palette, 768 );
   GFX_FadeOut ( 0, 0, 0, 2 );
  
   memset ( displaybuffer, 0, 64000 );
   memset ( displayscreen, 0, 64000 );

   local_cnt = FRAME_COUNT;
  
   g_flash = 0;
   g_oldsuper     = EMPTY;
   g_oldshield    = EMPTY;
   startendwave   = EMPTY;

   if ( demo_flag == DEMO_RECORD )
      DEMO_StartRec();

   // == CLEAR ALL BUTTONS ===========================

   start_score = plr.score;
   IMS_StartAck();
   memset ( buttons, 0, sizeof ( buttons ) );

   for (;;)
   {
      SHADOW_Clear();
  
      IPT_MovePlayer();
  
      if ( KBD_IsKey ( SC_F1 ) )
      {
         SWD_SetClearFlag ( FALSE );
         IPT_End();
         HELP_Win ( "OVERVW06_TXT" );
         memset ( displayscreen, 0, 64000 );
         IPT_Start();
         g_oldsuper = EMPTY;
         g_oldshield = EMPTY;
         RAP_ClearSides();
         RAP_DisplayStats();
         SWD_SetClearFlag ( TRUE );
         IMS_StartAck();
         BUT_1 = FALSE;
         BUT_2 = FALSE;
         BUT_3 = FALSE;
         BUT_4 = FALSE;
         b2_flag = FALSE;
         b3_flag = FALSE;
      }

      if ( KBD_IsKey ( SC_P ) )
      {
         while ( IMS_IsAck() );
         SWD_SetClearFlag ( FALSE );
         RAP_ClearSides();
         WIN_Pause();
         g_oldsuper = EMPTY;
         g_oldshield = EMPTY;
         RAP_ClearSides();
         RAP_DisplayStats();
         SWD_SetClearFlag ( TRUE );
         IMS_StartAck();
         BUT_1 = FALSE;
         BUT_2 = FALSE;
         BUT_3 = FALSE;
         BUT_4 = FALSE;
         b2_flag = FALSE;
         b3_flag = FALSE;
      }

      if ( KBD_IsKey ( SC_F8 ) && godmode )
      {
         while ( IMS_IsAck() );
         debugflag ^= TRUE;
      }

      if ( DEMO_Think() )
         break;

      if ( demo_mode  == DEMO_PLAYBACK )
      {
         if ( IMS_IsAck () && !BUT_2 && !BUT_3 && !BUT_4 )
         {
            rval = TRUE;
            break;
         }
      }

      switch ( KBD_LASTSCAN )
      {
         case SC_NONE:
            break;

         default:
            break;

         case SC_1:
            OBJS_MakeSpecial ( S_DUMB_MISSLE );
            break;

         case SC_2:
            OBJS_MakeSpecial ( S_MINI_GUN );
            break;

         case SC_3:
            OBJS_MakeSpecial ( S_TURRET );
            break;

         case SC_4:
            OBJS_MakeSpecial ( S_MISSLE_PODS );
            break;

         case SC_5:
            OBJS_MakeSpecial ( S_AIR_MISSLE  );
            break;

         case SC_6:
            OBJS_MakeSpecial ( S_GRD_MISSLE );
            break;

         case SC_7:
            OBJS_MakeSpecial ( S_BOMB );
            break;

         case SC_8:
            OBJS_MakeSpecial ( S_ENERGY_GRAB );
            break;

         case SC_9:
            OBJS_MakeSpecial ( S_PULSE_CANNON );
            break;

         case SC_0:
            OBJS_MakeSpecial ( S_DEATH_RAY );
            break;

         case SC_MINUS:
            OBJS_MakeSpecial ( S_FORWARD_LASER );
            break;
      }

      if ( BUT_1 )
      {
         OBJS_Use ( S_FORWARD_GUNS );
         OBJS_Use ( S_PLASMA_GUNS );
         OBJS_Use ( S_MICRO_MISSLE );
         BUT_1 = FALSE;
         if ( plr.sweapon != EMPTY )
            OBJS_Use ( plr.sweapon );
      }
  
      if ( BUT_2 )
      {
         BUT_2 = FALSE;
         if ( b2_flag == FALSE )
         {
            SND_Patch ( FX_SWEP, 127 );
            b2_flag = TRUE;
            OBJS_GetNext();
         }
      }
      else if ( b2_flag == TRUE )
      {
         b2_flag = FALSE;
      }
  
      if ( BUT_3 )
      {
         BUT_3 = FALSE;
         if ( b3_flag == FALSE )
         {
            b3_flag = TRUE;
            OBJS_Use ( S_MEGA_BOMB );
         }
      }
      else if ( b3_flag == TRUE )
      {
         b3_flag = FALSE;
      }

      if ( startendwave != EMPTY )
      {
         if ( startendwave == 0 )
         {
            end_wave = TRUE;   
         }

         startendwave--;
      }

      gl_cnt++;

      TILE_Think();
      ENEMY_Think();
      ESHOT_Think();
      BONUS_Think();
      SHOTS_Think();
      ANIMS_Think();
      OBJS_Think();
  
      if ( draw_player )
         SHADOW_Add ( curship [ playerpic + g_flash ], playerx , playery );
  
      TILE_Display();
      SHADOW_DisplayGround();
      ENEMY_DisplayGround();
      SHADOW_DisplaySky();
      ANIMS_DisplayGround();
  
      ENEMY_DisplaySky();
      SHOTS_Display();
      BONUS_Display();
      ANIMS_DisplaySky();

      if ( draw_player )
      {
         FLAME_Down ( player_cx + -o_engine[ playerpic ] - 3, player_cy + 15, 4, gl_cnt % 2 ); 
         FLAME_Down ( player_cx + o_engine[ playerpic ] - 2, player_cy + 15, 4, gl_cnt % 2 ); 
         GFX_PutSprite ( GLB_GetItem ( curship [ playerpic + g_flash ] ), playerx , playery );
         g_flash = 0;
      }

      ANIMS_DisplayHigh();
      ESHOT_Display();
  
      if ( fadeflag )
      {
         if ( fadecnt >= FADE_FRAMES - 1 )
         {
            RAP_ClearSides();
            g_mapleft = MAP_LEFT;
            GFX_SetPalette ( gpal, 0 );
            fadeflag = FALSE;
            g_oldsuper = EMPTY;
            g_oldshield = EMPTY;
         }
         else
         {
            RAP_ClearSides();
            GFX_SetRetraceFlag ( TRUE );
            GFX_FadeFrame ( gpal, fadecnt, FADE_FRAMES );
            GFX_SetRetraceFlag ( FALSE );
            g_mapleft = MAP_LEFT + shakes [ fadecnt ];
            fadecnt++;
         }
      }
      else
      {
         if ( !init_flag )
            RAP_PaletteStuff();
      }

      RAP_DisplayStats();

      while ( FRAME_COUNT - local_cnt < 3 );
      local_cnt = FRAME_COUNT;

      if ( fadeflag )
         TILE_ShakeScreen();
      else
         TILE_DisplayScreen();

      if ( startfadeflag )
      {
         SND_Patch ( FX_GEXPLO, 127 );
         SND_Patch ( FX_AIREXPLO, 127 );
         GFX_SetRetraceFlag ( TRUE );
         GFX_FadeOut ( 63, 60, 60, 1 );
         GFX_FadeStart();
         startfadeflag = FALSE;
         fadeflag = TRUE;
         fadecnt = 0;
         GFX_SetRetraceFlag ( FALSE );
      }

      if ( reg_flag && KBD_Key ( SC_BACKSPACE ) )
      {
         OBJS_Add ( S_DEATH_RAY );
         OBJS_Add ( S_ENERGY );
         OBJS_Add ( S_ENERGY );
         OBJS_Add ( S_ENERGY );
         plr.score = 0;
      }

      if ( init_flag )
      {
         init_flag = FALSE;
         GFX_FadeIn ( gpal, 64 );
      }

      if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
      {
         RAP_ClearSides();
         SWD_SetClearFlag ( FALSE );
         IPT_End();
         WIN_AskExit();
         IPT_Start();
         g_oldsuper = EMPTY;
         g_oldshield = EMPTY;
         RAP_ClearSides();
         RAP_DisplayStats();
         SWD_SetClearFlag ( TRUE );
         IMS_StartAck();
         BUT_1 = FALSE;
         BUT_2 = FALSE;
         BUT_3 = FALSE;
         BUT_4 = FALSE;
         b2_flag = FALSE;
         b3_flag = FALSE;
      }

      if ( KBD_IsKey ( SC_ESC ) )
      {
         if ( godmode )
            end_wave = TRUE;

         if ( demo_mode == DEMO_OFF )
         {
            SWD_SetClearFlag ( FALSE );
            IPT_End();
            RAP_ClearSides();
            if ( WIN_AskBool ( "Abort Mission ?" ) )
            {
               plr.score = start_score;
               rval = TRUE;
               break;
            }
            IPT_Start();
            g_oldsuper = EMPTY;
            g_oldshield = EMPTY;
            RAP_ClearSides();
            RAP_DisplayStats();
            SWD_SetClearFlag ( TRUE );
            IMS_StartAck();
            BUT_1 = FALSE;
            BUT_2 = FALSE;
            BUT_3 = FALSE;
            BUT_4 = FALSE;
            b2_flag = FALSE;
            b3_flag = FALSE;
         }
         else if ( demo_mode == DEMO_PLAYBACK )
         {
            rval = TRUE;
            break;
         }
         else if ( demo_mode == DEMO_RECORD )
            break;
      }

      if ( end_wave )
         break;
   }
  
   GFX_FadeOut ( 0, 0, 0, 32 );

   RAP_FreeMap();
   end_wave = FALSE;

   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate( 0, 0, 320, 200 );

   IPT_PauseControl ( FALSE );
  
   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );
   IPT_End();

   if ( demo_flag == DEMO_RECORD )
      DEMO_SaveFile();

   return ( rval );
}
  
/***************************************************************************
RAP_InitMem() - Allocates memory for VM and GLB to use
 ***************************************************************************/
VOID
RAP_InitMem (
VOID
)
{
   DWORD  segment;
   DWORD  memsize;
   DWORD  lowmem     = 0;
   DWORD  getmem     = 38400;

   // == GET highmem =============================
   memsize = _dpmi_getmemsize();
//   memsize = MIN_MEMREQ;

   printf ("_dpmi_getmemsize() = %u\n",memsize );

   if ( memsize > MAX_HMEM + MEM_KEEP )
      memsize = MAX_HMEM;
   else
      memsize -= MEM_KEEP;

   if ( (INT)memsize < 1 )
      memsize = 0;

   // == GET lowmem =============================
   while (_dpmi_dosalloc ( getmem, &segment ) && getmem > 2 )
      getmem--;

   lowmem = ( ( getmem - 1 ) * 16 );

   if ( lowmem > 4096 && segment != 0 )
   {
      g_lowmem = ( BYTE * )( segment << 4 );
      VM_InitMemory ( g_lowmem, lowmem );
      printf ("Lowmem = %d\n", lowmem );
   }
   else
      printf ("Lowmem = NONE\n" );

   g_highmem = calloc ( memsize, 1 );

   if ( g_highmem == NUL && memsize > MEM_KEEP )
   {
      printf ("MemGet1 = %d\n", memsize );
      while ( g_highmem == NUL && memsize > 0 )
      {
         g_highmem = calloc ( memsize, 1 );
         memsize -= MEM_KEEP;
      }
      printf ("MemGet2 = %d\n", memsize );
   }

   if ( g_highmem )
   {
      if ( memsize < REQ_HMEM && lowmem < REQ_LMEM )
         EXIT_Error ("Not Enough High Memory !! ( need %dk more )", REQ_HMEM - memsize );

      VM_InitMemory ( g_highmem, memsize );
      printf ("Highmem = %d\n", memsize );

      if ( memsize < ( 128 * 1024 ) )
         lowmem_flag = TRUE;
   }
   else
      printf ("Highmem = NUL\n", memsize );

   if ( ( lowmem + memsize ) < ( MIN_MEMREQ-MEM_KEEP ) )
      EXIT_Error ("Not Enough Memory !! ( need %dk more )", MIN_MEMREQ - ( lowmem + memsize ) );

   GLB_UseVM();
}

BOOL
RAP_TestDIZ (
VOID
)
{
   CHAR *   fname = "FILE_ID.DIZ";
   BYTE     temp [768];
   BYTE *   cmp;
   BOOL     rval = FALSE;

   if ( access ( fname, 0 ) != 0 )
      return ( rval );

   GLB_ReadFile ( fname, temp );

   if ( reg_flag == TRUE )
      cmp = GLB_LockItem ( FILE_ID_DIZ );
   else
      cmp = GLB_LockItem ( FILE_ID_DIX );

   if ( memcmp ( cmp, temp, 398 ) == 0 )
      rval = TRUE;

   if ( reg_flag == TRUE )
      GLB_FreeItem ( FILE_ID_DIZ );
   else
      GLB_FreeItem ( FILE_ID_DIX );

   return ( rval );
}

VOID
JoyHack (
VOID
)
{
   extern INT   joy_x, joy_y, joy_buttons;
   union  REGS  regs;

   printf ("Calibrate JoyStick\n");

   for (;;)
   {
      regs.w.ax = 0x200;
      regs.h.bh = 0;
      regs.h.dl = 0;
      regs.h.dh = 1;
      int386( 0x10, (const union REGS *) &regs, &regs);

      _disable();
      PTR_ReadJoyStick();
      _enable();

      printf ("JOY X %03d   JOY Y %03d\n    ", joy_x, joy_y );
      fflush ( stdout );

      if ( ( joy_buttons & 0x0f ) != 0x0 )
         break;
   }
}

VOID
main ( INT argc, CHAR * argv[] )
{
#ifdef   TESTMODE
   CHAR  *  cmp = "C:\\APG_BETA\\RAPCOS.$$$";
   CHAR     tname[16];
#endif
   volatile INT      loop;
   volatile INT      numfiles;
   volatile DWORD    item;
   volatile CHAR   * var1    = getenv("S_HOST");
   volatile BOOL     ptrflag = FALSE;
   volatile BYTE   * tptr;

   if ( argc >= 0 )
      InitScreen();

   godmode = FALSE;

   if ( strcmpi ( argv[1], "joycal" ) == 0 )
   {
      JoyHack();
   }

   printf ("main() = %u\n", main );

   RAP_InitLoadSave();

   if ( access ( RAP_SetupFilename(), 0 ) != 0 )
   {
      printf ("\n\n** You must run SETUP first! **\n");
      exit(0);
   }

   godmode = FALSE;

   if ( strcmp ( ( CHAR *)var1, gdmodestr ) == 0 )
   {
      godmode = TRUE;
   }
   else
      godmode = FALSE;

   if ( strcmpi ( argv[1], "REC" ) == 0 )
   {
      DEMO_SetFileName ( argv[2] );
      demo_flag = DEMO_RECORD;
      printf("DEMO RECORD enabled\n");
   }
   else if ( strcmpi ( argv[1], "PLAY" ) == 0 )
   {
      if ( access ( argv[2], 0 ) == 0 )
      {
         DEMO_SetFileName ( argv[2] );
         demo_flag = DEMO_PLAYBACK;
         printf("DEMO PLAYBACK enabled\n");
      }
   }

   if ( godmode )
      printf("GOD mode enabled\n");

   cur_diff = 0;

   if ( access ( "FILE0001.GLB", 0 ) == 0 )
      gameflag[0] = TRUE;

   if ( access ( "FILE0002.GLB", 0 ) == 0 )
      gameflag[1] = TRUE;
  
   if ( access ( "FILE0003.GLB", 0 ) == 0 &&
      access ( "FILE0004.GLB", 0 ) == 0  )
   {
      gameflag[2] = TRUE;
      gameflag[3] = TRUE;
   }

   if ( gameflag[1] + gameflag[2] )
      reg_flag = TRUE;

   numfiles = 0;
   for ( loop = 0; loop < 4; loop++ )
   {
      if ( gameflag [ loop ] )
         numfiles++;
   }

   if ( access ( "FILE0000.GLB", 0 ) != 0 || numfiles == 0 )
   {
      printf ("All game data files NOT FOUND cannot proceed !!\n");
      exit(0);
   }

   printf ("Init -\n");
   EXIT_Install ( ShutDown );

   // ================================================
   // SET UP B-DAY STUFF O RAMA
   // ================================================

   memset ( bday, 0, sizeof ( bday ) );

   bday [0].month = 5;     // Scott
   bday [0].day   = 16;
   bday [0].year  = 1994;
   bday [0].name  = "Scott Host";

   bday [1].month = 2;     // Dave
   bday [1].day   = 27;
   bday [1].year  = 1996;
   bday [1].name  = "Dave T.";

   bday [2].month = 10;     // JIM
   bday [2].day   = 2;
   bday [2].year  = 1995;
   bday [2].name  = "Jim M.";

   bday [3].month = 3;     // BOBBY P.
   bday [3].day   = 12;
   bday [3].year  = 1995;
   bday [3].name  = "Bobby P.";

   bday [4].month = 8;     // RICH
   bday [4].day   = 28;
   bday [4].year  = 1995;
   bday [4].name  = "Rich F.";

   bday [5].month = 4;     // Paul
   bday [5].day   = 24;
   bday [5].year  = 1996;
   bday [5].name  = "Paul R.";

   RAP_Bday();

   if ( bday_num != EMPTY )
   {
      printf ("Birthday() = %s\n", bday [ bday_num ].name );
   }

   // ================================================

   if ( access ( RAP_SetupFilename(), 0 ) != 0 )
      EXIT_Error ("You Must run SETUP.EXE First !!");

   if ( ! INI_InitPreference( RAP_SetupFilename() ) )
      EXIT_Error ("SETUP Error");

   fflush ( stdout );
   TSM_Install ( 140 );
   KBD_Install();
   GFX_InitSystem();
   SWD_Install ( FALSE );

   IPT_LoadPrefs();

   switch ( control )
   {
      default:
         printf ("PTR_Init()-Auto\n");
         fflush ( stdout );
         ptrflag = PTR_Init ( P_MOUSE );
         usekb_flag = TRUE;
         break;

      case I_JOYSTICK:
         printf ("PTR_Init()-Joystick\n");
         fflush ( stdout );
         ptrflag = PTR_Init ( P_JOYSTICK );
         usekb_flag = FALSE;
         break;

      case I_MOUSE:
         printf ("PTR_Init()-Mouse\n");
         fflush ( stdout );
         ptrflag = PTR_Init ( P_MOUSE );
         usekb_flag = FALSE;
         break;
   }

   #ifdef REG_VERSION
      printf("Registered EXE!\n");
      fflush ( stdout );
      GLB_InitSystem ( argv[0], 6, NUL );
   #else
      reg_flag = FALSE;
      fflush ( stdout );
      GLB_InitSystem ( argv[0], 2, NUL );
      reg_flag = FALSE;
      gameflag[1] = FALSE;
      gameflag[2] = FALSE;
      gameflag[3] = FALSE;
   #endif

   if ( reg_flag )
   {
      tptr = GLB_GetItem ( ATENTION_TXT );
      printf ( "%s\n", tptr );
      GLB_FreeItem ( ATENTION_TXT );
   }

   SND_InitSound();
   IPT_Init();
   GLB_FreeAll();
   RAP_InitMem();

   if ( !RAP_TestDIZ() )
   {
      if ( !reg_flag )
      {
         fflush ( stdout );
      }
      else
      {
         var1 = GLB_LockItem ( DIZ_ERR );
         printf ( "\n%s\n", var1 );
         fflush ( stdout );
         exit(1);
      }
   }

   printf ("Loading Graphics\n");

   tptr     = GLB_LockItem ( PALETTE_DAT );
   memset ( tptr, 0, 3 );
   palette  = (BYTE *)tptr;
   SHADOW_Init();
   FLAME_Init();

   fflush ( stdout );

   if ( ptrflag )
   {
      cursor_pic = GLB_LockItem ( CURSOR_PIC );
      PTR_SetPic ( cursor_pic );
      PTR_SetPos ( 160, 100 );
      PTR_DrawCursor ( FALSE );
   }

   // = SET UP SHIP PICTURES =========================
   for ( loop = 0; loop < 7; loop++ )
   {
      lship [ loop ] = LPLAYER_PIC + (DWORD)loop;

      if ( GAME2 )
      {
         dship [ loop ]    = DPLAYER_PIC + (DWORD)loop;
         fship [ loop ]    = FPLAYER_PIC + (DWORD)loop;
      }
   }
  
   // = LOAD IN FLAT LIBS  =========================
   for ( loop = 0; loop < 4; loop++ )
   {
      if ( gameflag [ loop ] )
      {
         item = GLB_GetItemID ( flatnames [ loop ] );
         flatlib [ loop ] = ( FLATS * )GLB_LockItem ( item );
      }
      else
         flatlib[loop] = NUL;
   }
  
   // = LOAD IN 0-9 $ SPRITE PICS  =========================
   for ( loop = 0; loop < 11; loop++ )
   {
      item = ( N0_PIC + (DWORD)loop );
      numbers[loop] = GLB_LockItem ( item );
   }

   FLAME_InitShades();
   HELP_Init();
   OBJS_Init();
   TILE_Init();
   SHOTS_Init();
   ESHOT_Init();
   BONUS_Init();
   ANIMS_Init();
   SND_Setup();
  
   GFX_SetPalRange ( 0, ROTPAL_START-1 );
   GFX_InitVideo ( palette );
   SHADOW_MakeShades();

   RAP_ClearPlayer();

   #ifdef TAIWAN_VERSION
      tai_flag = TRUE;
      INTRO_Taiwan();
   #endif

   if ( !godmode )
      INTRO_Credits();

   if ( demo_flag != DEMO_PLAYBACK )
   {
      SND_PlaySong ( RINTRO_MUS, TRUE, TRUE );
      INTRO_PlayMain();
      SND_PlaySong ( MAINMENU_MUS, TRUE, TRUE );
   }
   else if ( demo_flag == DEMO_PLAYBACK )
   {
      DEMO_LoadFile();
      DEMO_Play ();
//      EXIT_Error ("Demo Play done");
   }

   cur_game     = 0;
   game_wave[0] = 0;
   game_wave[1] = 0;
   game_wave[2] = 0;
   game_wave[3] = 0;

   if ( cur_game != 0 )
      printf ( copyright );

   for (;;)
   {
      WIN_MainMenu();
      WIN_MainLoop();
   }

}


