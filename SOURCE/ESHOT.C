#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
#include "file0001.inc"

#define MAX_ESHOT 80

typedef struct 
{
   DWORD             item;             // BASE ITEM NUMBER
   BYTE  *           pic[10];          // POINTER TO PICS
   INT               num_frames;       // NUMBER OF FRAMES
   BOOL              smokeflag;        // TRUE = SMOKETRAIL
   INT               speed;            // SPEED OF SHOT
   INT               xoff;             // X CENTER OFFSET
   INT               yoff;             // Y CENTER OFFSET
   INT               hits;             // HIT POINT DAMAGE TO PLAYER
}ESHOT_LIB;

typedef struct ESHOT_S
{
   struct ESHOT_S *  prev;             // LINK LIST PREV
   struct ESHOT_S *  next;             // LINK LIST NEXT
   BYTE  *           pic;              // POINTER TO CUR FRAME PIC
   INT               curframe;         // CURRENT ANIM FRAME
   INT               x;                // CUR SHOT CENTER X
   INT               y;                // CUR SHOT CENTER Y
   MOVEOBJ           move;             // MOVE STUFF
   BOOL              doneflag;         // SHOT DONE = TRUE
   ESHOT_LIB   *     lib;              // POINTER TO LIB
   INT               cnt;
   INT               speed;
   INT               pos;
   INT               type;
   SPRITE_SHIP *     en;
   INT               gun_num;
}ESHOT;

typedef enum
{
  LIB_NORMAL,
  LIB_ATPLAY,
  LIB_MISSLE,
  LIB_LASER,
  LIB_MINES,
  LIB_PLASMA,
  LIB_COCO,
  LIB_LASTPIC
} LIB_PIC;

PRIVATE ESHOT_LIB plib [ LIB_LASTPIC ];

PRIVATE ESHOT     eshots [ MAX_ESHOT ];
  
PRIVATE ESHOT     first_eshot;
PRIVATE ESHOT     last_eshot;
PRIVATE ESHOT *   free_eshot;
PRIVATE INT       xpos [ 16 ] = { -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2 };
PRIVATE INT       ypos [ 16 ] = { -3,-3,-3,-2,-1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2 };
PRIVATE BYTE *    elaspow[4];

extern BYTE *     lashit[];

PUBLIC INT        eshotnum  = 0;
PUBLIC INT        eshothigh = 0;

PRIVATE DEFX      monkeys [ 6 ] = { FX_MON1, FX_MON2, FX_MON3, FX_MON4, FX_MON5, FX_MON6 };
                            
/***************************************************************************
ESHOT_Clear () - Clears out ESHOT Linklist
 ***************************************************************************/
VOID
ESHOT_Clear (
VOID
)
{
   INT loop;
  
   eshotnum = 0;

   first_eshot.prev = NUL;
   first_eshot.next = &last_eshot;
  
   last_eshot.prev  = &first_eshot;
   last_eshot.next  = NUL;
  
   free_eshot       = eshots;
  
   memset ( eshots, 0, sizeof ( eshots ) );
  
   for ( loop = 0; loop < MAX_ESHOT-1; loop++ )
      eshots[ loop ].next = &eshots [ loop + 1 ];
}

/*-------------------------------------------------------------------------*
ESHOT_Get () - gets a Free ESHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT *
ESHOT_Get (
VOID
)
{
   ESHOT * new;
  
   if ( !free_eshot )
      return ( NUL );
  
   eshotnum++;
   if ( eshothigh < eshotnum )
      eshothigh = eshotnum;

   new = free_eshot;
   free_eshot = free_eshot->next;
  
   memset ( new, 0 ,sizeof ( ESHOT ) );
  
   new->next = &last_eshot;
   new->prev = last_eshot.prev;
   last_eshot.prev = new;
   new->prev->next = new;
  
   return ( new );
}
  
/*-------------------------------------------------------------------------*
ESHOT_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE ESHOT *
ESHOT_Remove (
ESHOT * sh
)
{
   ESHOT * next;
  
   eshotnum--;

   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( ESHOT ) );
  
   sh->next = free_eshot;
  
   free_eshot = sh;
  
   return ( next );
}
  
/***************************************************************************
ESHOT_Init () - Inits ESHOT system and clears link list
 ***************************************************************************/
VOID
ESHOT_Init (
VOID
)
{
   ESHOT_LIB  *   cur;
   INT            loop;
   GFX_PIC    *   h;
   DWORD          item;

   for ( loop = 0; loop < 4; loop++ )
   {
      item = ELASEPOW_BLK + (DWORD)loop ;
      elaspow [ loop ] = GLB_LockItem ( item );
   }

   ESHOT_Clear();

   memset ( plib, 0, sizeof ( plib ) );

   cur = &plib [ LIB_NORMAL ];
   cur->hits            = 2;
   cur->item            = ESHOT_BLK;
   cur->num_frames      = 2;
   cur->smokeflag       = FALSE;
   cur->speed           = 6;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;

   cur = &plib [ LIB_ATPLAY ];
   cur->hits            = 1;
   cur->item            = ESHOT_BLK;
   cur->num_frames      = 2;
   cur->smokeflag       = FALSE;
   cur->speed           = 6;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;

   cur = &plib [ LIB_MISSLE ];
   cur->hits            = 4;
   cur->item            = EMISLE_BLK;
   cur->num_frames      = 2;
   cur->smokeflag       = TRUE;
   cur->speed           = 10;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;

   cur = &plib [ LIB_MINES ];
   cur->hits            = 16;
   cur->item            = MINE_BLK;
   cur->num_frames      = 2;
   cur->smokeflag       = FALSE;
   cur->speed           = 0;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;

   cur = &plib [ LIB_LASER ];
   cur->hits            = 12;
   cur->item            = ELASER_BLK;
   cur->num_frames      = 4;
   cur->smokeflag       = FALSE;
   cur->speed           = 6;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;

   cur = &plib [ LIB_PLASMA ];
   cur->hits            = 15;
   cur->item            = EPLASMA_PIC;
   cur->num_frames      = 1;
   cur->smokeflag       = FALSE;
   cur->speed           = 10;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;


   cur = &plib [ LIB_COCO ];
   cur->hits            = 1;
   cur->item            = COCONUT_PIC;
   cur->num_frames      = 4;
   cur->smokeflag       = FALSE;
   cur->speed           = 6;
   for ( loop = 0; loop < cur->num_frames; loop++ )
   {
      item = cur->item + (DWORD)loop;
      cur->pic [ loop ] = GLB_LockItem ( item );
   }
   h = ( GFX_PIC * )cur->pic [ 0 ];
   cur->xoff            = h->width>>1;
   cur->yoff            = h->height>>1;
}

/***************************************************************************
ESHOT_Shoot() - Shoots ENEMY GUNS
 ***************************************************************************/
VOID
ESHOT_Shoot (
SPRITE_SHIP * enemy,       // INPUT : pointer to Enemy stuff
INT           gun_num      // INPUT : gun number to shoot
)
{
   ESHOT *     cur;
   INT         x;
   INT         y;

   x = enemy->x + enemy->lib->shootx [ gun_num ];
   y = enemy->y + enemy->lib->shooty [ gun_num ];

   if ( x < 0 && x >= 320 )
      return;

   if ( y < 0 && y >= 200 )
      return;

   cur = ESHOT_Get();
   if ( !cur ) return;
  
   cur->move.x  = x;
   cur->move.y  = y;
   cur->en      = enemy;
   cur->gun_num = gun_num;

   switch ( ( ESHOT_TYPE )enemy->lib->shoot_type [ gun_num ] )
   {
      default:
         EXIT_Error ("ESHOT_Shoot() - Invalid EShot type");
         break;

      case ES_ATPLAYER:
         SND_3DPatch ( FX_ENEMYSHOT, x, y );
         cur->lib       = &plib [ LIB_ATPLAY ];
         cur->move.x -= cur->lib->xoff;
         cur->move.y -= cur->lib->yoff;
         cur->move.x2   = player_cx;
         cur->move.y2   = player_cy;
         cur->speed     = 1;
         cur->type      = ES_ATPLAYER;
         break;

      case ES_ATDOWN:
         SND_3DPatch ( FX_ENEMYSHOT, x, y );
         cur->lib       = &plib [ LIB_NORMAL ];
         cur->move.x -= cur->lib->xoff;
         cur->move.y -= cur->lib->yoff;
         cur->move.x2   = cur->move.x;
         cur->move.y2   = 200;
         cur->speed     = cur->lib->speed>>1;
         cur->type      = ES_ATDOWN;
         break;

      case ES_ANGLELEFT:
         SND_3DPatch ( FX_ENEMYSHOT, x, y );
         cur->lib       = &plib [ LIB_NORMAL ];
         cur->move.x -= cur->lib->xoff;
         cur->move.y -= cur->lib->yoff;
         cur->move.x2   = cur->move.x - 32;
         cur->move.y2   = cur->move.y + 32;
         cur->speed     = cur->lib->speed>>1;
         cur->type      = ES_ANGLELEFT;
         break;

      case ES_ANGLERIGHT:
         SND_3DPatch ( FX_ENEMYSHOT, x, y );
         cur->lib       = &plib [ LIB_NORMAL ];
         cur->move.x -= cur->lib->xoff;
         cur->move.y -= cur->lib->yoff;
         cur->move.x2   = cur->move.x + 32;
         cur->move.y2   = cur->move.y + 32;
         cur->speed     = cur->lib->speed>>1;
         cur->type      = ES_ANGLERIGHT;
         break;

      case ES_MISSLE:
         SND_3DPatch ( FX_ENEMYMISSLE, x, y );
         cur->lib       = &plib [ LIB_MISSLE ];
         cur->move.x   -= cur->lib->xoff;
         cur->move.x2   = cur->move.x;
         cur->move.y2   = 200;
         cur->speed     = enemy->speed+1;
         cur->type      = ES_MISSLE;
         break;

      case ES_MINES:
         SND_3DPatch ( FX_ENEMYSHOT, x, y );
         cur->lib       = &plib [ LIB_MINES ];
         cur->x         = cur->move.x;
         cur->y         = cur->move.y;
         cur->move.x2   = 320;
         cur->move.y2   = 200;
         cur->speed     = 150;
         cur->pos       = random ( 16 );
         cur->type      = ES_MINES;
         break;

      case ES_LASER:
         SND_3DPatch ( FX_ENEMYLASER, x, y );
         cur->lib       = &plib [ LIB_LASER ];
         cur->move.x   -= cur->lib->xoff;
         cur->move.x2   = cur->move.x;
         cur->move.y2   = 200;
         cur->speed     = enemy->speed;
         cur->type      = ES_LASER;
         break;

      case ES_PLASMA:
         SND_3DPatch ( FX_ENEMYPLASMA, x, y );
         cur->lib       = &plib [ LIB_PLASMA ];
         cur->move.x   -= cur->lib->xoff;
         cur->move.x2   = cur->move.x;
         cur->move.y2   = 200;
         cur->speed     = 8;
         cur->type      = ES_PLASMA;
         break;

      case ES_COCONUTS:
         SND_3DPatch ( monkeys [ random ( 6 ) ], x, y );
         cur->lib       = &plib [ LIB_COCO ];
         cur->move.x -= cur->lib->xoff;
         cur->move.y -= cur->lib->yoff;
         cur->move.x2   = player_cx;
         cur->move.y2   = player_cy;
         cur->speed     = 1;
         cur->type      = ES_COCONUTS;
         break;
   }

   InitMobj ( &cur->move );
   MoveSobj ( &cur->move, 1 );

   if ( cur->move.x < 0 || cur->move.x >= 320 )
      cur->move.done = TRUE;

   if ( cur->move.y < 0 || cur->move.y >= 200 )
      cur->move.done = TRUE;

   if ( cur->move.done )
      ESHOT_Remove ( cur );
}
  
/***************************************************************************
ESHOT_Think () - Does All Thinking for shot system
 ***************************************************************************/
VOID
ESHOT_Think (
VOID
)
{
   ESHOT       *  shot;
   ESHOT_LIB   *  lib;
   INT            dx;
   INT            dy;
  
   for ( shot=first_eshot.next; shot!=&last_eshot; shot=shot->next )
   {
      lib = shot->lib;

      shot->pic = lib->pic [ shot->curframe ];

      shot->curframe++;

      switch ( shot->type )
      {
         case ES_LASER:
            if ( shot->curframe < lib->num_frames)
            {
               shot->x        = shot->en->x + shot->en->lib->shootx [ shot->gun_num ] - 4;
               shot->y        = shot->en->y + shot->en->lib->shooty [ shot->gun_num ];
               shot->move.y2  = 200;

               dx = abs ( shot->x - player_cx );

               if ( dx < ( PLAYERWIDTH/2 ) && shot->y < player_cy )
               {
                  shot->move.y2 = player_cy + random(4) - 2;
                  OBJS_SubEnergy ( lib->hits );
               }
            }
            else
               shot->doneflag = TRUE;
            break;

         default:
            if ( shot->curframe >= lib->num_frames )
               shot->curframe = 0;

            if ( lib->speed )
            {
               shot->x = shot->move.x;
               shot->y = shot->move.y;

               MoveSobj ( &shot->move, shot->speed );

               if ( shot->speed < lib->speed )
                  shot->speed++;
            }
            else
            {
               shot->speed--;

               if ( shot->speed )
               {
//                  SHADOW_Add ( shot->pic, shot->x, shot->y );

                  shot->x = shot->move.x + xpos [ shot->pos ];
                  shot->y = shot->move.y + ypos [ shot->pos ];
                  shot->move.y++;

                  shot->pos++;

                  if ( shot->pos >= 16 )
                     shot->pos = 0;
               }
               else
               {
                  shot->doneflag = TRUE;
                  ANIMS_StartAnim ( A_SMALL_AIR_EXPLO, shot->x + 4, shot->y + 4 );
               }
            }

            if ( shot->y >= 200 || shot->y < 0 )
               shot->doneflag = TRUE;

            if ( shot->x >= 320 || shot->x < 0 )
               shot->doneflag = TRUE;

            dx = abs ( shot->x - player_cx );
            dy = abs ( shot->y - player_cy );

            if ( dx < ( PLAYERWIDTH/2 ) && dy < ( PLAYERHEIGHT/2 ) )
            {
               ANIMS_StartAnim ( A_SMALL_AIR_EXPLO, shot->x, shot->y );
               shot->doneflag = TRUE;
               OBJS_SubEnergy ( lib->hits );
            }
            break;
      }

      if ( shot->doneflag )
      {
         shot = ESHOT_Remove ( shot );
         continue;
      }

      shot->cnt++;

      if ( lib->smokeflag && shot->cnt & 1 )
      {
         ANIMS_StartAAnim ( A_SMALL_SMOKE_UP, shot->x + lib->xoff, shot->y );
      }
   }
}
  
/***************************************************************************
ESHOT_Display () - Displays All active Shots
 ***************************************************************************/
VOID
ESHOT_Display (
VOID
)
{
   ESHOT *        shot;
   INT            loop;
   GFX_PIC *      h;
   INT            y;
  
   for ( shot=first_eshot.next; shot!=&last_eshot; shot=shot->next )
   {
      if ( shot->type == ES_LASER )
      {
         for ( loop = shot->y; loop < shot->move.y2; loop+=3 )
         {
            GFX_PutSprite ( shot->pic, shot->x, loop );
         }

         GFX_PutSprite ( elaspow [ shot->curframe - 1 ], shot->x, shot->y );

         h = ( GFX_PIC * )lashit [ shot->curframe - 1 ];

         y = shot->move.y2 - 8;

         if ( y > 0 && y < 200 )
            GFX_PutSprite ( ( BYTE * )h, shot->x - ( h->width>>2 ), y );
      }
      else
         GFX_PutSprite ( shot->pic, shot->x, shot->y );
   }
}
  
  
