#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
  
#define MAX_STILES ( MAP_ONSCREEN * MAP_COLS )
  
PUBLIC DWORD   titems [ MAP_SIZE ];
PUBLIC DWORD   eitems [ MAP_SIZE ];
PUBLIC SHORT   hits [ MAP_SIZE ];
PUBLIC SHORT   money [ MAP_SIZE ];
PUBLIC SHORT   tdead [ MAP_SIZE ];
PUBLIC DWORD   startflat[4];
PUBLIC INT     tilepos;
PUBLIC INT     tileyoff;
  
PUBLIC INT     g_mapleft;
PUBLIC BYTE *  tilepic;
PUBLIC BYTE *  tilestart;
PUBLIC INT     tileloopy;
  
PRIVATE CHAR game_start [4][17] = {
   "STARTG1TILES",
   "STARTG2TILES",
   "STARTG3TILES",
   "STARTG4TILES"
};
  
PUBLIC BOOL          scroll_flag;
PUBLIC BOOL          last_tile;

PRIVATE TILESPOT     tspots [ MAX_STILES ];
PRIVATE TILESPOT *   lastspot;

PUBLIC TILEDELAY    first_delay;
PUBLIC TILEDELAY    last_delay;
PUBLIC TILEDELAY *  free_delay;

#define MAX_TILEDELAY ( ( MAP_ONSCREEN + 1 ) * MAP_COLS )
PRIVATE TILEDELAY tdel [ MAX_TILEDELAY ];

PRIVATE INT spark_delay = 0;
PRIVATE INT flare_delay = 0;

/*-------------------------------------------------------------------------*
TClear () - Clears TILE Delay Link List
 *-------------------------------------------------------------------------*/
PRIVATE VOID
TClear (
VOID
)
{
   INT loop;
  
   first_delay.prev = NUL;
   first_delay.next = &last_delay;
  
   last_delay.prev = &first_delay;
   last_delay.next = NUL;
  
   free_delay = tdel;
  
   memset ( tdel, 0, sizeof ( tdel ) );
  
   for ( loop = 0; loop < MAX_TILEDELAY-1; loop++ )
      tdel[ loop ].next = &tdel [ loop + 1 ];
}
  
/*-------------------------------------------------------------------------*
TGet () - Get a TDELAY object
 *-------------------------------------------------------------------------*/
PRIVATE TILEDELAY *
TGet (
VOID
)
{
   TILEDELAY * new;
  
   if ( !free_delay )
      EXIT_Error ("TILEDELAY_Get() - Max ");
  
   new = free_delay;
   free_delay = free_delay->next;
  
   memset ( new, 0 ,sizeof ( TILEDELAY ) );
  
   new->next = &last_delay;
   new->prev = last_delay.prev;
   last_delay.prev = new;
   new->prev->next = new;
  
   return ( new );
}
  
/*-------------------------------------------------------------------------*
TRemove () - Remove a TDELAY Object from link list
 *-------------------------------------------------------------------------*/
PRIVATE TILEDELAY *
TRemove (
TILEDELAY * sh
)
{
   TILEDELAY * next;
  
   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( TILEDELAY ) );
  
   sh->next = free_delay;
  
   free_delay = sh;
  
   return ( next );
}

/*--------------------------------------------------------------------------
TILE_DoDamage
 --------------------------------------------------------------------------*/
PRIVATE VOID
TILE_DoDamage (
INT   mapspot,
INT   damage
)
{
   static INT mlookup [3]  = { -1, -MAP_COLS, 1 };
   static INT xlookup [3]  = { -1, 0, 1 };
   INT ix                  = ( mapspot % MAP_COLS );
   INT loop;
   INT spot;
   INT x;

   for ( loop = 0; loop < 3; loop++ )
   {
      spot = mapspot + mlookup [ loop ];

      if ( spot < 0 ) continue;
      if ( spot >= MAP_SIZE ) continue;

      if ( eitems [ spot ] == titems [ spot ] ) continue;

      x = ix + xlookup [ loop ];
      if ( x < 0 ) continue;
      if ( x >= MAP_COLS ) continue;
      
      hits [ spot ] -= damage;
   }

}

/***************************************************************************
TILE_Draw () - Draws 32 by 32 TILE Clips on y only
 ***************************************************************************/
VOID
TILE_Put (
BYTE * inpic,              // INPUT : pointer to GFX_PIC ( norm )
INT    x,                  // INPUT : x position
INT    y                   // INPUT : y position
)
{
   BOOL flag = FALSE;
  
   if ( y + 32 <= 0 ) return;
   if ( y >= SCREENHEIGHT ) return;
  
   tileloopy   = 32;
   tilepic     = inpic;
  
   if ( y < 0 )
   {
      tilepic += FMUL32 ( -y );
      tileloopy += y;
      y = 0;
      flag = TRUE;
   }
  
   if ( y + tileloopy > SCREENHEIGHT )
   {
      tileloopy = SCREENHEIGHT - y;
      flag = TRUE;
   }
  
   tilestart = displaybuffer + x + ylookup [ y ];
  
   if ( flag )
      TILE_ClipDraw();
   else
      TILE_Draw();
}
  
/***************************************************************************
TILE_Init () - Sets Up A level for Displaying
 ***************************************************************************/
VOID
TILE_Init (
VOID
)
{
  
   TClear();
   g_mapleft = MAP_LEFT;

   scroll_flag = TRUE;
   last_tile = FALSE;

   // FIND START OF GAME TILES ========
   startflat[0] = GLB_GetItemID ( game_start[ 0 ] );
   startflat[0]++;
  
   startflat[1] = GLB_GetItemID ( game_start[ 1 ] );
   startflat[1]++;
  
   startflat[2] = GLB_GetItemID ( game_start[ 2 ] );
   startflat[2]++;
  
   startflat[3] = GLB_GetItemID ( game_start[ 3 ] );
   startflat[3]++;

}

/***************************************************************************
TILE_CacheLevel () - Cache tiles in current level
 ***************************************************************************/
VOID
TILE_CacheLevel (
VOID
)
{
   INT      game;
   FLATS *  lib;
   DWORD    item;
   INT      loop;

   TClear();
   g_mapleft = MAP_LEFT;

   // SET UP START POS ON MAP =========
   tilepos = ( MAP_ROWS - MAP_ONSCREEN ) * MAP_COLS;
   tileyoff = 200 - ( MAP_ONSCREEN * MAP_BLOCKSIZE );
   lastspot = tspots + ( MAX_STILES - 1 );

   scroll_flag = TRUE;
   last_tile = FALSE;

   memset ( titems, 0, sizeof (  titems ) );
   memset ( eitems, 0, sizeof ( eitems ) );
   memset ( hits, 0, sizeof ( hits ) );
   memset ( tdead, 0, sizeof ( tdead ) );

   // == CACHE TILES =========================
   for ( loop = 0; loop < MAP_SIZE; loop++ )
   {
      game = ml->map [ loop ].fgame;
      lib  = flatlib [ game ];

      money [ loop ]  = lib [ ml->map [ loop ].flats ].bounty;

      item = startflat [ game ];
      item += ml->map [ loop ].flats;
      titems [ loop ] = item;
      GLB_CacheItem ( item );

      item = startflat [ game ];
      item += (DWORD)lib [ ml->map [ loop ].flats ].linkflat;
      eitems [ loop ] = item;
      if ( eitems [ loop ] !=  titems [ loop ] )
         GLB_CacheItem ( item );

      if ( eitems [ loop ] !=  titems [ loop ] )
         hits [ loop ]  = lib [ ml->map [ loop ].flats ].bonus;
      else
         hits [ loop ]  = 1;
   }
}

/***************************************************************************
TILE_FreeLevel () - Free tile level
 ***************************************************************************/
VOID
TILE_FreeLevel (
VOID
)
{
   INT      loop;

   for ( loop = 0; loop < MAP_SIZE; loop++ )
   {
      GLB_FreeItem ( titems [ loop ] );

      if ( eitems [ loop ] !=  titems [ loop ] )
         GLB_FreeItem ( eitems [ loop ] );
   }
}

/***************************************************************************
TILE_DamageAll () - Damages All tiles on screen
 ***************************************************************************/
VOID
TILE_DamageAll (
VOID
)
{
   TILESPOT * ts =   tspots;
  
   for (;;)
   {
      if ( eitems [ ts->mapspot ] !=  titems [ ts->mapspot ] )
         hits [ ts->mapspot ] -= 20;
  
      if ( ts == lastspot ) break;

      ts++;
   }
}

/***************************************************************************
TILE_Think () - Does Position Calculations for tiles
 ***************************************************************************/
VOID
TILE_Think (
VOID
)
{
   TILESPOT  *    ts;
   TILEDELAY *    td;
   INT loopx;
   INT loopy;
   INT mapspot;
   INT x;
   INT y;
   INT tx;
   INT ty;

   y        = tileyoff;
   mapspot  = tilepos;

   ts = tspots;

   for ( loopy = 0; loopy < MAP_ONSCREEN; loopy++, y+=32 )
   {
      x = MAP_LEFT;
      for ( loopx = 0; loopx < MAP_COLS; loopx++, x+=32, mapspot++, ts++ )
      {
         ts->mapspot = mapspot;
         ts->x       = x;
         ts->y       = y;
         ts->item    = titems [ mapspot ];

         if ( hits [ mapspot ] < 0 && !tdead [ mapspot ] )
         {
            SND_3DPatch ( FX_GEXPLO, x + 16, y + 16 );

            TILE_DoDamage ( ts->mapspot, 5 );

            plr.score += (INT)money [ mapspot ];

            TILE_Explode ( ts, 10 );
            ANIMS_StartAnim ( A_LARGE_GROUND_EXPLO1, x + 16, y + 16 );

            tdead [ mapspot ] = (SHORT)1;
         }
      }
   }

   for ( td=first_delay.next; td!=&last_delay; td=td->next )
   {
      if ( td->mapspot - tilepos > (MAP_ONSCREEN*MAP_COLS) )
      {
         td = TRemove ( td );
         continue;
      }

      if ( td->frames < 0 )
      {
         tx = td->ts->x + 8 + random ( 8 );
         ty = td->ts->y + 16 + 10;

         TILE_DoDamage ( td->mapspot, 20 );

         spark_delay++;
         flare_delay++;

         if ( spark_delay > 2 )
         {
            ANIMS_StartAnim ( A_GROUND_SPARKLE, tx, ty );
            spark_delay = 0;
         }

         if ( flare_delay > 4 )
         {
            ANIMS_StartAnim ( A_GROUND_FLARE, tx, ty );
            flare_delay = 0;
         }

         ts = tspots + ( td->mapspot - tilepos );
         titems [ ts->mapspot ] = td->item;
         eitems [ ts->mapspot ] = td->item;
         td = TRemove ( td );
         continue;
      }

      td->frames--;
   }
}
  
/***************************************************************************
TILE_Display () - Displays Tiles
 ***************************************************************************/
VOID
TILE_Display (
VOID
)
{
   TILESPOT * ts =   tspots;
   BYTE     * pic;
  
   for (;;)
   {
      pic = GLB_GetItem ( ts->item );
      pic += sizeof ( GFX_PIC );
      TILE_Put ( pic, ts->x, ts->y );
  
      if ( ts == lastspot ) break;

      ts++;
   }

   tileyoff++;
  
   if ( tileyoff > 0 )
   {
      if ( last_tile && tileyoff >=  0 )
      {
         tileyoff = 0;
         scroll_flag = FALSE;
      }
      else
      {
         tileyoff -=MAP_BLOCKSIZE;
         tilepos -= MAP_COLS;
      }

      if ( tilepos <= 0 )
      {
         tilepos = 0;
         last_tile = TRUE;
      }
   }
}
  
/***************************************************************************
TILE_IsHit () - Checks to see if a shot hits an explodable tile
 ***************************************************************************/
BOOL                       // RETURNS : TRUE = Tile Hit
TILE_IsHit (
INT damage,                // INPUT : damage to tile
INT  x,                    // INOUT : x screen pos, out tile x
INT  y                     // INOUT : y screen pos, out tile y
)
{
   TILESPOT * ts  = tspots;
  
   while ( ts != lastspot )
   {
      if ( x >= ts->x && x < ( ts->x + 32 ) &&
         y >= ts->y && y < ( ts->y + 32 ) )
      {
         if ( eitems [ ts->mapspot ] !=  titems [ ts->mapspot ] )
         {
            hits [ ts->mapspot ]-=damage;

            switch ( random ( 2 ) )
            {
               case 0:
                  ANIMS_StartGAnim ( A_BLUE_SPARK, x, y );
                  break;
               case 1:
                  ANIMS_StartGAnim ( A_ORANGE_SPARK, x, y );
                  break;
            }

            return ( TRUE );
         }
      }

      ts++;
   }
  
   return ( FALSE );
}

/***************************************************************************
TILE_Bomb () - Checks to see if a BOMB hits an explodable tile
 ***************************************************************************/
BOOL                       // RETURNS : TRUE = Tile Hit
TILE_Bomb (
INT damage,                // INPUT : damage to tile
INT  x,                    // INOUT : x screen pos, out tile x
INT  y                     // INOUT : y screen pos, out tile y
)
{
   TILESPOT * ts  = tspots;
  
   while ( ts != lastspot )
   {
      if ( x >= ts->x && x < ( ts->x + 32 ) &&
         y >= ts->y && y < ( ts->y + 32 ) )
      {
         if ( eitems [ ts->mapspot ] !=  titems [ ts->mapspot ] )
         {
            hits [ ts->mapspot ]-=damage;

            TILE_DoDamage ( ts->mapspot, damage );

            if ( ts->mapspot > MAP_COLS )
               TILE_DoDamage ( ts->mapspot - MAP_COLS, damage>>1 );

            return ( TRUE );
         }
      }

      ts++;
   }
  
   return ( FALSE );
}
  
/***************************************************************************
TILE_Explode () - Sets the Tile to show explosion tile
 ***************************************************************************/
VOID
TILE_Explode (
TILESPOT * ts,             // INPUT : tilespot of explosion
INT delay                  // INPUT : frames to delay
)
{
   TILEDELAY * td;
  
   if ( ts->mapspot == EMPTY || ts->mapspot < tilepos ) return;

   if ( delay )
   {
      td = TGet();
      if ( td )
      {
         td->ts = ts;
         td->mapspot = ts->mapspot;
         td->frames = delay;
         td->item  = eitems [ ts->mapspot ];
      }
      eitems [ ts->mapspot ]  =  titems [ ts->mapspot ];
   }
   else
   {
      ts->item  = eitems [ ts->mapspot ];
       titems [ ts->mapspot ] = eitems [ ts->mapspot ];
   }
}

