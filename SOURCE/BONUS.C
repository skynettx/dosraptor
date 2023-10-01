#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"

#include "file0000.inc"
#include "file0001.inc"
  
PRIVATE BONUS     bons [ MAX_BONUS ];
  
PRIVATE BONUS     first_bonus;
PRIVATE BONUS     last_bonus;
PRIVATE BONUS *   free_bonus;
PRIVATE DWORD     glow [ 4 ];
PRIVATE INT       glow_lx;
PRIVATE INT       glow_ly;
PRIVATE INT       xpos [ 16 ] = { -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2 };
PRIVATE INT       ypos [ 16 ] = { -3,-3,-3,-2,-1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2 };
PRIVATE INT       energy_count;

/***************************************************************************
BONUS_Clear () - Clears out All bonuses
 ***************************************************************************/
VOID
BONUS_Clear (
VOID
)
{
   INT loop;

   energy_count = 0;

   first_bonus.prev = NUL;
   first_bonus.next = &last_bonus;
  
   last_bonus.prev  = &first_bonus;
   last_bonus.next  = NUL;
  
   free_bonus       = bons;
  
   memset ( bons, 0, sizeof ( bons ) );
  
   for ( loop = 0; loop < MAX_BONUS-1; loop++ )
      bons[ loop ].next = &bons [ loop + 1 ];
}
  
/*-------------------------------------------------------------------------*
BONUS_Get () - Gets A Free BONUS from Link List
 *-------------------------------------------------------------------------*/
BONUS *
BONUS_Get (
VOID
)
{
   BONUS * new;
  
   if ( !free_bonus )
      return ( NUL );
  
   new = free_bonus;
   free_bonus = free_bonus->next;
  
   memset ( new, 0 ,sizeof ( BONUS ) );
  
   new->next         = &last_bonus;
   new->prev         = last_bonus.prev;
   last_bonus.prev   = new;
   new->prev->next   = new;
  
   return ( new );
}
  
/*-------------------------------------------------------------------------*
BONUS_Remove () Removes BONUS from Link List
 *-------------------------------------------------------------------------*/
BONUS *
BONUS_Remove (
BONUS * sh
)
{
   BONUS * next;
  
   if ( sh->type == S_ITEMBUY6 )
      energy_count--;

   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( BONUS ) );
  
   sh->next = free_bonus;
  
   free_bonus = sh;
  
   return ( next );
}
  
/***************************************************************************
BONUS_Init () - Sets up Bonus stuff
 ***************************************************************************/
VOID
BONUS_Init (
VOID
)
{
   INT loop;
   GFX_PIC * h;

   for (loop = 0; loop < 4; loop++ )
      glow [ loop ] = ICNGLW_BLK + (DWORD)loop; 

   h = ( GFX_PIC * ) GLB_CacheItem ( ICNGLW_BLK );

   glow_lx  = h->width;
   glow_ly  = h->height;

   GLB_CacheItem ( ICNGLW_BLK + 1 );
   GLB_CacheItem ( ICNGLW_BLK + 2 );
   GLB_CacheItem ( ICNGLW_BLK + 3 );

   BONUS_Clear();
}

/***************************************************************************
BONUS_Add () - Adds A BONUS to Game so player can Try to pick it up
 ***************************************************************************/
VOID
BONUS_Add (
OBJ_TYPE type,             // INPUT : OBJECT TYPE
INT      x,                // INPUT : X POSITION
INT      y                 // INPUT : Y POSITION
)
{
   BONUS * cur;

   if ( type >= S_LAST_OBJECT )
      return;

   if ( type == S_ITEMBUY6 )
   {
      if ( energy_count > MAX_MONEY )
         return;
   }

   cur  = BONUS_Get();
   if ( !cur ) return;

   if ( type == S_ITEMBUY6 )
      energy_count++;

   cur->type      = type;
   cur->lib       = OBJS_GetLib ( type );
   cur->curframe  = 0;
   cur->x         = x + MAP_LEFT;
   cur->y         = y;
   cur->pos       = random ( 16 );
}

/***************************************************************************
BONUS_Think () - Does all BONUS Thinking
 ***************************************************************************/
VOID
BONUS_Think (
VOID
)
{
   PRIVATE INT gcnt  = 0;
   BONUS *     cur;
   INT         x     = playerx;
   INT         y     = playery;
   INT         x2    = playerx + PLAYERWIDTH;
   INT         y2    = playery + PLAYERHEIGHT;

   for ( cur=first_bonus.next; cur!=&last_bonus; cur=cur->next )
   {
      cur->item = cur->lib->item + cur->curframe;

      cur->bx = ( cur->x - ( BONUS_WIDTH/2 ) + xpos [ cur->pos ] );
      cur->by = ( cur->y - ( BONUS_HEIGHT/2 ) + ypos [ cur->pos ] );

      cur->gx = ( cur->x - ( glow_lx>>1 ) + xpos [ cur->pos ] );
      cur->gy = ( cur->y - ( glow_ly>>1 ) + ypos [ cur->pos ] );

      cur->y++;

      if ( gcnt & 1 )
      {
         cur->pos++;

         if ( cur->pos >= 16 )
            cur->pos = 0;

         cur->curframe++;

         if ( cur->curframe >= cur->lib->numframes )
            cur->curframe = 0;
      }

      cur->curglow++;

      if ( cur->curglow >=4 )
         cur->curglow = 0;

      if ( cur->x > x && cur->x < x2 && cur->y > y && cur->y < y2 )
      {
         if ( !cur->dflag && OBJS_GetAmt ( S_ENERGY ) > 0 )
         {
            SND_Patch ( FX_BONUS, 127 );

            if ( cur->type == S_ENERGY )
               OBJS_AddEnergy ( MAX_SHIELD/4 );
            else
               OBJS_Add ( cur->type );

            if ( cur->lib->moneyflag )
            {
               cur->dflag  = TRUE;
               cur->countdown = 50;
            }
            else
            {
               cur = BONUS_Remove ( cur );
               continue;
            }
         }
      }

      if ( cur->dflag )
      {
         cur->countdown--;
         if ( cur->countdown <= 0 )
         {
            cur = BONUS_Remove ( cur );
            continue;
         }
      }

      if ( cur->gy > 200 )
      {
         cur = BONUS_Remove ( cur );
         continue;
      }
   }

   gcnt++;
}

/***************************************************************************
BONUS_Display () - Displays Active Bonuses in game
 ***************************************************************************/
VOID
BONUS_Display (
VOID
)
{
   BONUS *     cur;

   for ( cur=first_bonus.next; cur!=&last_bonus; cur=cur->next )
   {
      if ( !cur->dflag )
      {
         GFX_PutSprite ( GLB_GetItem ( cur->item ), cur->bx, cur->by );
         GFX_ShadeShape ( LIGHT, GLB_GetItem ( glow [ cur->curglow ] ), cur->gx, cur->gy );
      }
      else
      {
         GFX_PutSprite ( GLB_GetItem ( ( N9_PIC + (DWORD)1 ) ), cur->bx, cur->by );
      }
   }
}

