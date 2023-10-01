#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
#include "file0000.inc"
#include "file0001.inc"
  
PRIVATE SHOTS     shots [ MAX_SHOTS ];
PRIVATE SHOT_LIB  shot_lib [ LAST_WEAPON + 1 ];
  
PRIVATE SHOTS     first_shots;
PRIVATE SHOTS     last_shots;
PRIVATE SHOTS *   free_shots;
PRIVATE BYTE *    laspow[4];
PRIVATE BYTE *    detpow[4];

PUBLIC BYTE *     lashit [4];

PUBLIC INT        shotnum = 0;
PUBLIC INT        shothigh = 0;

/***************************************************************************
SHOTS_Clear () * Clears out SHOTS Linklist
 ***************************************************************************/
VOID
SHOTS_Clear (
VOID
)
{
   INT loop;
  
   shotnum = 0;

   first_shots.prev = NUL;
   first_shots.next = &last_shots;
  
   last_shots.prev  = &first_shots;
   last_shots.next  = NUL;
  
   free_shots       = shots;
  
   memset ( shots, 0, sizeof ( shots ) );
  
   for ( loop = 0; loop < MAX_SHOTS*1; loop++ )
      shots[ loop ].next = &shots [ loop + 1 ];
}
  
/*-------------------------------------------------------------------------*
SHOTS_Get () - gets a Free SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE SHOTS *
SHOTS_Get (
VOID
)
{
   SHOTS * new;
  
   if ( !free_shots )
      return ( NUL );
  
   shotnum++;
   if ( shotnum > shothigh )
      shothigh = shotnum;

   new = free_shots;
   free_shots = free_shots->next;
  
   memset ( new, 0 ,sizeof ( SHOTS ) );
  
   new->next = &last_shots;
   new->prev = last_shots.prev;
   last_shots.prev = new;
   new->prev->next = new;
  
   return ( new );
}
  
/*-------------------------------------------------------------------------*
SHOTS_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
PRIVATE SHOTS *
SHOTS_Remove (
SHOTS * sh
)
{
   SHOTS * next;
  
   shotnum--;

   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( SHOTS ) );
  
   sh->next = free_shots;
  
   free_shots = sh;
  
   return ( next );
}

/***************************************************************************
SHOTS_Init () - Inits SHOTS system and clears link list
 ***************************************************************************/
VOID
SHOTS_Init (
VOID
)
{
   SHOT_LIB *  slib;
   INT         i;
   DWORD       item;
  
   SHOTS_Clear();

   for ( i = 0; i < 4; i++ )
   {
      detpow [ i ] = GLB_LockItem ( DETHPOW_BLK + (DWORD)i );
   }

   for ( i = 0; i < 4; i++ )
   {
      laspow [ i ] = GLB_LockItem ( LASERPOW_BLK + (DWORD)i );
   }

   for ( i = 0; i < 4; i++ )
   {
      lashit [i] = GLB_LockItem ( DRAYHIT_BLK + (DWORD)i );
   }
  
   memset ( shot_lib, 0, sizeof ( shot_lib ) );
  
   // == FORWARD_GUNS =====================================
   slib              = &shot_lib [ S_FORWARD_GUNS ];
   slib->lumpnum     = NMSHOT_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_FORWARD_GUNS;
   slib->hits        = 1;
   slib->speed       = 8;
   slib->maxspeed    = 16;
   slib->startframe  = 0;
   slib->numframes   = 4;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 2;
   slib->cur_shoot   = 0;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_ALL;

   // == PLASMA_GUNS =====================================
   slib              = &shot_lib [ S_PLASMA_GUNS ];
   slib->lumpnum     = PLASMA_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_PLASMA_GUNS;
   slib->hits        = 2;
   slib->speed       = 4;
   slib->maxspeed    = 8;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 10;
   slib->cur_shoot   = 0;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_AIR;
  
   // == MICRO_MISSLE =====================================
   slib              = &shot_lib [ S_MICRO_MISSLE ];
   slib->lumpnum     = MICROM_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_MICRO_MISSLE;
   slib->hits        = 2;
   slib->speed       = 2;
   slib->maxspeed    = 8;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 4;
   slib->cur_shoot   = 0;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_GRALL;
  
   // == DUMB_MISSLE =====================================
   slib              = &shot_lib [ S_DUMB_MISSLE ];
   slib->lumpnum     = MISDUM_BLK;
   slib->shadow      = TRUE;
   slib->type        = S_DUMB_MISSLE;
   slib->hits        = 4;
   slib->speed       = 2;
   slib->maxspeed    = 12;
   slib->startframe  = 1;
   slib->numframes   = 3;
   slib->delayflag   = TRUE;
   slib->shoot_rate  = 10;
   slib->cur_shoot   = 0;
   slib->use_plot    = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_ALL;
  
   // == MINI_GUN =====================================
   slib              = &shot_lib [ S_MINI_GUN ];
   slib->lumpnum     = NMSHOT_BLK;
   slib->shadow      = TRUE;
   slib->type        = S_MINI_GUN;
   slib->hits        = 1;
   slib->speed       = 8;
   slib->maxspeed    = 10;
   slib->startframe  = 1;
   slib->numframes   = 4;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 1;
   slib->cur_shoot   = 0;
   slib->use_plot    = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_GRALL;
  
   // == LASER TURRET =====================================
   slib              = &shot_lib [ S_TURRET ];
   slib->lumpnum     = EMPTY;
   slib->shadow      = FALSE;
   slib->type        = S_TURRET;
   slib->hits        = 5;
   slib->speed       = 0;
   slib->maxspeed    = 0;
   slib->startframe  = 0;
   slib->numframes   = 0;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 6;
   slib->cur_shoot   = 0;
   slib->use_plot    = FALSE;
   slib->move_flag   = FALSE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_LINE;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_ALL;
  
   // == MISSLE_PODS =====================================
   slib              = &shot_lib [ S_MISSLE_PODS ];
   slib->lumpnum     = MISRAT_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_MISSLE_PODS;
   slib->hits        = 4;
   slib->speed       = 1;
   slib->maxspeed    = 16;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 5;
   slib->cur_shoot   = 0;
   slib->smoke       = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_AIR;
  
   // == AIR TO AIR =====================================
   slib              = &shot_lib [ S_AIR_MISSLE ];
   slib->lumpnum     = MISRAT_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_MISSLE_PODS;
   slib->hits        = 4;
   slib->speed       = 1;
   slib->maxspeed    = 12;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 10;
   slib->cur_shoot   = 0;
   slib->smoke       = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_AIR;
  
   // == AIR TO GROUND =====================================
   slib              = &shot_lib [ S_GRD_MISSLE ];
   slib->lumpnum     = MISGRD_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_GRD_MISSLE;
   slib->hits        = 20;
   slib->speed       = 1;
   slib->maxspeed    = 6;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 20;
   slib->cur_shoot   = 0;
   slib->smoke       = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_GROUND;

   // == GROUND BOMB =====================================
   slib              = &shot_lib [ S_BOMB ];
   slib->lumpnum     = BLDGBOMB_PIC;
   slib->shadow      = FALSE;
   slib->type        = S_BOMB;
   slib->hits        = 50;
   slib->speed       = 1;
   slib->maxspeed    = 4;
   slib->startframe  = 0;
   slib->numframes   = 1;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 30;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_GTILE;

   // == ENERGY GRAB =====================================
   slib              = &shot_lib [ S_ENERGY_GRAB ];
   slib->lumpnum     = POWDIS_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_ENERGY_GRAB;
   slib->hits        = 3;
   slib->speed       = 4;
   slib->maxspeed    = 8;
   slib->startframe  = 0;
   slib->numframes   = 6;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 2;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_SUCK;
  
   // == MEGA BOMB =====================================
   slib              = &shot_lib [ S_MEGA_BOMB ];
   slib->lumpnum     = MEGABM_BLK;
   slib->shadow      = TRUE;
   slib->type        = S_MEGA_BOMB;
   slib->hits        = 50;
   slib->speed       = 2;
   slib->maxspeed    = 2;
   slib->startframe  = 0;
   slib->numframes   = 4;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 60;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->use_plot    = TRUE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = TRUE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_ALL;
  
   // == PULSE CANNON =====================================
   slib              = &shot_lib [ S_PULSE_CANNON ];
   slib->lumpnum     = SHOKWV_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_PULSE_CANNON;
   slib->hits        = 5;
   slib->speed       = 8;
   slib->maxspeed    = 8;
   slib->startframe  = 0;
   slib->numframes   = 2;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 3;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->use_plot    = FALSE;
   slib->move_flag   = TRUE;
   slib->fplrx       = FALSE;
   slib->fplry       = FALSE;
   slib->meffect     = FALSE;
   slib->beam        = S_SHOOT;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_ALL;
  
   // == FORWARD LASER =====================================
   slib              = &shot_lib [ S_FORWARD_LASER ];
   slib->lumpnum     = FRNTLAS_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_FORWARD_LASER;
   slib->hits        = 10;
   slib->speed       = 0;
   slib->maxspeed    = 0;
   slib->startframe  = 0;
   slib->numframes   = 4;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 7;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->use_plot    = FALSE;
   slib->move_flag   = FALSE;
   slib->fplrx       = TRUE;
   slib->fplry       = TRUE;
   slib->meffect     = TRUE;
   slib->beam        = S_BEAM;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_AIR;
  
   // == DEATH RAY =====================================
   slib              = &shot_lib [ S_DEATH_RAY ];
   slib->lumpnum     = DETHRY_BLK;
   slib->shadow      = FALSE;
   slib->type        = S_DEATH_RAY;
   slib->hits        = 6;
   slib->speed       = 0;
   slib->maxspeed    = 0;
   slib->startframe  = 0;
   slib->numframes   = 4;
   slib->delayflag   = FALSE;
   slib->shoot_rate  = 7;
   slib->cur_shoot   = 0;
   slib->smoke       = FALSE;
   slib->use_plot    = FALSE;
   slib->move_flag   = FALSE;
   slib->fplrx       = TRUE;
   slib->fplry       = TRUE;
   slib->meffect     = TRUE;
   slib->beam        = S_BEAM;
   for ( i = 0; i < slib->numframes; i++ )
   {
      item = ( slib->lumpnum + (DWORD)i );
      slib->pic[i] = GLB_LockItem ( item );
   }
   slib->h           = ( GFX_PIC * )slib->pic [ 0 ];
   slib->hlx         = slib->h->width>>1;
   slib->hly         = slib->h->height>>1;
   slib->ht          = S_GRALL;
}
  
/***************************************************************************
SHOTS_PlayerShoot() - Shoots the specified weapon
 ***************************************************************************/
BOOL
SHOTS_PlayerShoot (
OBJ_TYPE type              // INPUT : OBJECT TYPE
)
{
   extern BOOL           gus_flag;
   extern INT            g_flash;
   extern SPRITE_SHIP    first_enemy;
   extern SPRITE_SHIP    last_enemy;
   SHOTS *               cur;
   SHOT_LIB *            lib  = &shot_lib [ type ];
   SPRITE_SHIP *         enemy;
  
   if ( type == EMPTY )
      EXIT_Error ("SHOTS_PlayerShoot() type = EMPTY  ");

   if ( lib->cur_shoot ) return ( FALSE );
  
   lib->cur_shoot = lib->shoot_rate;
  
   cur = SHOTS_Get();
   if ( cur == NUL ) return ( FALSE );
  
   switch ( type )
   {
      default:
         EXIT_Error ("SHOTS_PlayerShoot() - Invalid Shot type");
         break;
  
      case S_FORWARD_GUNS:
         if ( !gus_flag )
            SND_Patch ( FX_GUN, 127 );
         g_flash = 7;
         cur->curframe  = random ( lib->numframes );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun1 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         ANIMS_StartAnim ( A_PLAYER_SHOOT, o_gun1[playerpic], 0 );
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->curframe  = random ( lib->numframes );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun1 [ playerpic ] - 1;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         ANIMS_StartAnim ( A_PLAYER_SHOOT, -o_gun1[playerpic] - 1, 0 );
         break;
  
      case S_PLASMA_GUNS:
         if ( !gus_flag )
            SND_Patch ( FX_GUN, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->curframe  = random ( lib->numframes );
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_MICRO_MISSLE:
         if ( !gus_flag )
            SND_Patch ( FX_GUN, 127 );
         g_flash        = 7;
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun3 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun3 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_DUMB_MISSLE:
         if ( !gus_flag )
            SND_Patch ( FX_MISSLE, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x + random(16) + 10;
         cur->move.y2   = cur->y + 5;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         InitMobj ( &cur->move );
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x - random(16) - 10;
         cur->move.y2   = cur->y + 5;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         InitMobj ( &cur->move );
         break;
  
      case S_MINI_GUN:
         enemy = ENEMY_GetRandom();
         if ( enemy == NUL )
         {
            SHOTS_Remove ( cur );
            break;
         }
         if ( !gus_flag )
            SND_Patch ( FX_GUN, 127 );
         cur->curframe  = random ( lib->numframes );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = enemy->x + random ( enemy->width ) - 1;
         cur->move.y2   = enemy->y + enemy->hly + random ( enemy->height ) - 1;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         InitMobj ( &cur->move );
         break;
  
      case S_TURRET:
         enemy = ENEMY_GetRandomAir();
         if ( enemy == NUL )
         {
            SHOTS_Remove ( cur );
            SND_Patch ( FX_NOSHOOT, 127 );
            break;
         }
         SND_Patch ( FX_TURRET, 127 );
         cur->lib       = &shot_lib [ type  ];
         enemy->hits   -= lib->hits;
         cur->curframe  = 0;
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = enemy->move.x + random ( enemy->width ) - 1;
         cur->move.y    = enemy->move.y + random ( enemy->height ) - 1;
         cur->move.x2   = player_cx;
         cur->move.y2   = player_cy;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         InitMobj ( &cur->move );
         ANIMS_StartAnim ( A_LASER_BLAST, cur->move.x, cur->move.y );
         break;
  
      case S_MISSLE_PODS:
         SND_Patch ( FX_GUN, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         ANIMS_StartAnim ( A_PLAYER_SHOOT, o_gun2[playerpic], 1 );
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         ANIMS_StartAnim ( A_PLAYER_SHOOT, -o_gun2[playerpic] - 1, 1 );
         break;
  
      case S_AIR_MISSLE:
         SND_Patch ( FX_MISSLE, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_GRD_MISSLE:
         SND_Patch ( FX_MISSLE, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun2 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_BOMB:
         SND_Patch ( FX_MISSLE, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;

      case S_ENERGY_GRAB:
         SND_Patch ( FX_GUN, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - 4;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_MEGA_BOMB:
         SND_Patch ( FX_GUN, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = 160;
         cur->move.y2   = 75;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         InitMobj ( &cur->move );
         break;
  
      case S_PULSE_CANNON:
         SND_Patch ( FX_PULSE, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = 0;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_FORWARD_LASER:
         SND_Patch ( FX_LASER, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx + o_gun3 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = -24;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
  
         cur = SHOTS_Get();
         if ( cur == NUL ) return ( FALSE );
  
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx - o_gun3 [ playerpic ];
         cur->y         = player_cy;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->move.x2   = cur->x;
         cur->move.y2   = -24;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         break;
  
      case S_DEATH_RAY:
         SND_Patch ( FX_LASER, 127 );
         cur->lib       = &shot_lib [ type  ];
         cur->delayflag = lib->delayflag;
         cur->speed     = lib->speed;
         cur->x         = player_cx;
         cur->y         = player_cy - 24;
         cur->move.x    = cur->x;
         cur->move.y    = cur->y;
         cur->startx    = player_cx;
         cur->starty    = player_cy;
         cur->move.x2   = cur->x;
         cur->move.y2   = -24;
         break;
   }
  
   return ( TRUE );
}
  
/***************************************************************************
SHOTS_Think () - Does All Thinking for shot system
 ***************************************************************************/
VOID
SHOTS_Think (
VOID
)
{
   extern SPRITE_SHIP    first_enemy;
   extern SPRITE_SHIP    last_enemy;
   SHOT_LIB    *         lib;
   SHOTS       *         shot;
   SPRITE_SHIP *         enemy;
   INT                   i;
  
   lib = shot_lib;
   for ( i = 0; i <= LAST_WEAPON; i++, lib++ )
   {
      if ( lib->cur_shoot > 0 )
         lib->cur_shoot--;
   }
  
   for ( shot=first_shots.next; shot!=&last_shots; shot=shot->next )
   {
      lib = shot->lib;
  
      switch ( lib->beam )
      {
         default:
            EXIT_Error("SHOTS_Think()");
            break;

         case S_SHOOT:
            shot->pic = lib->pic [ shot->curframe ];
            shot->x = shot->move.x - lib->hlx;
            if ( lib->move_flag )
               shot->y = shot->move.y - lib->hly;
            else
               shot->y = shot->move.y;
            if ( lib->smoke )
               ANIMS_StartAnim ( A_SMALL_SMOKE_DOWN, shot->x + lib->hlx, shot->y+(lib->hly<<1) );
            break;

         case S_LINE:
            shot->x = shot->move.x;
            shot->y = shot->move.y;
            break;

         case S_BEAM:
            shot->pic = lib->pic [ shot->curframe ];
            shot->x = shot->move.x - lib->hlx;
            shot->y = shot->move.y;

            for ( enemy=first_enemy.next; enemy!=&last_enemy;
               enemy=enemy->next )
            {
               if ( shot->x > enemy->x && shot->x < enemy->x2 &&
                  enemy->y < player_cy && enemy->y > -30 )
               {
                  enemy->hits -= lib->hits;

                  if ( enemy->hits != -1 )
                  {
                     shot->move.y2 = enemy->y + enemy->hly;
                     break;
                  }
               }
            }
            break;
      }

      if ( lib->fplrx )
      {
         shot->x += ( player_cx - shot->startx );
      }
      if ( lib->fplry )
      {
         shot->y += ( player_cy - shot->starty );
      }
  
      if ( (shot->y+16) < 0 || shot->x < 0 || shot->x > 320 || shot->y > 200 )
      {
         if ( lib->move_flag )
         {
            shot->move.done = TRUE;
            goto shot_done;
         }
      }
  
      if ( !shot->delayflag )
      {
         if ( shot->speed < lib->maxspeed )
            shot->speed++;
  
         shot->curframe++;
  
         if ( shot->curframe >= lib->numframes )
         {
            if ( lib->move_flag )
            {
               shot->curframe = lib->startframe;
            }
            else
            {
               shot->move.done = TRUE;
               goto shot_done;
            }
         }
      }
  
      if ( shot->doneflag )
      {
         shot->move.done = TRUE;
         goto shot_done;
      }
  
      if ( !lib->meffect )
      {
         switch ( lib->ht )
         {
            default:
            case S_SUCK:
               enemy = ENEMY_DamageEnergy ( shot->x, shot->y, lib->hits );
               if ( enemy )
               {
                  shot->doneflag = TRUE;

                  ANIMS_StartAnim ( A_BLUE_SPARK, shot->x, shot->y );
                  ANIMS_StartEAnim ( enemy, A_ENERGY_GRAB, enemy->hlx, enemy->hly );
               }
               break;

            case S_GRALL:
               if ( ENEMY_DamageAll ( shot->x, shot->y, lib->hits ) )
               {
                  shot->doneflag = TRUE;

                  if ( random ( 2 ) )
                     ANIMS_StartAnim ( A_BLUE_SPARK, shot->x, shot->y );
                  else
                     ANIMS_StartAnim ( A_ORANGE_SPARK, shot->x, shot->y );
               }
               break;

            case S_ALL:
               if ( ENEMY_DamageAll ( shot->x, shot->y, lib->hits ) )
               {
                  shot->doneflag = TRUE;

                  if ( random ( 2 ) )
                     ANIMS_StartAnim ( A_BLUE_SPARK, shot->x, shot->y );
                  else
                     ANIMS_StartAnim ( A_ORANGE_SPARK, shot->x, shot->y );
               }
               else if  ( TILE_IsHit ( lib->hits, shot->x, shot->y ) )
               {
                  shot->move.done = TRUE;
               }
               break;

            case S_AIR:
               if ( ENEMY_DamageAir ( shot->x, shot->y, lib->hits ) )
               {
                  shot->doneflag = TRUE;

                  if ( random ( 2 ) )
                     ANIMS_StartAnim ( A_BLUE_SPARK, shot->x, shot->y );
                  else
                     ANIMS_StartAnim ( A_ORANGE_SPARK, shot->x, shot->y );
               }
               break;

            case S_GROUND:
               if ( ENEMY_DamageGround ( shot->x, shot->y, lib->hits ) )
               {
                  shot->doneflag = TRUE;

                  ANIMS_StartAnim ( A_ORANGE_SPARK, shot->x, shot->y );
               }
               else if  ( TILE_IsHit ( lib->hits, shot->x, shot->y ) )
                  shot->move.done = TRUE;
               break;

            case S_GTILE:
               if  ( TILE_Bomb ( lib->hits, shot->x, shot->y ) )
                  shot->move.done = TRUE;
               if ( ENEMY_DamageGround ( shot->x, shot->y, 5 ) )
               {
                  shot->doneflag = TRUE;

                  ANIMS_StartAnim ( A_SMALL_GROUND_EXPLO, shot->x, shot->y );
               }
               break;
         }
      }

      shot_done:
  
      if ( shot->move.done )
      {
         if ( shot->delayflag )
         {
            shot->delayflag = FALSE;
            shot->move.x2 = shot->move.x + ( random ( 32 ) - 16 );
            shot->move.y2 = 0;
            ANIMS_StartAnim ( A_SMALL_SMOKE_DOWN, shot->move.x, shot->move.y );
            InitMobj ( &shot->move );
         }
         else
         {
            switch ( lib->type )
            {
               case S_MEGA_BOMB:
                  ESHOT_Clear();
                  TILE_DamageAll();
                  for ( enemy=first_enemy.next; enemy!=&last_enemy;
                     enemy=enemy->next )
                     enemy->hits -= lib->hits;
                  startfadeflag = TRUE;
                  ANIMS_StartAnim ( A_SUPER_SHIELD, 0, 0 );
                  shot = SHOTS_Remove ( shot );
                  continue;

               case S_TURRET:
                  break;

                default:
                  shot = SHOTS_Remove ( shot );
                  continue;
            }
         }
      }
  
      if ( !lib->move_flag ) continue;
  
      if ( lib->use_plot )
      {
         MoveSobj ( &shot->move, shot->speed );
      }
      else
      {
         shot->move.y -= shot->speed;
         if (shot->move.y < 0 )
         {
            shot->move.done = TRUE;
            shot->doneflag = TRUE;
         }
      }
   }
}
  
/***************************************************************************
SHOTS_Display () - Displays All active Shots
 ***************************************************************************/
VOID
SHOTS_Display (
VOID
)
{
   SHOTS *        shot;
   INT            loop;
   INT            x, y;
   GFX_PIC *      h;
  
   for ( shot=first_shots.next; shot!=&last_shots; shot=shot->next )
   {
      switch ( shot->lib->beam )
      {
         default:
            EXIT_Error("SHOTS_Display()");
            break;

         case S_SHOOT:
            GFX_PutSprite ( shot->pic, shot->x, shot->y );
            break;

         case S_LINE:
            GFX_Line ( player_cx + 1, player_cy, shot->move.x, shot->move.y, 69 );
            GFX_Line ( player_cx - 1, player_cy, shot->move.x, shot->move.y, 69 );
            GFX_Line ( player_cx, player_cy, shot->move.x, shot->move.y, 64 );
            shot = SHOTS_Remove ( shot );
            continue;

         case S_BEAM:
            for ( loop = shot->move.y2; loop < shot->y; loop+=3 )
            {
               GFX_PutSprite ( shot->pic, shot->x, loop );
            }

            if ( shot->lib->type == S_DEATH_RAY )
               GFX_PutSprite ( detpow [ shot->cnt ], shot->x-4, shot->y );
            else
               GFX_PutSprite ( laspow [ shot->cnt ], shot->x, shot->y );
            h = ( GFX_PIC * )lashit [ shot->cnt ];

            x = shot->x - ( h->width>>2 );
            y = shot->move.y2 - 8;

            if ( y > 0 )
               GFX_PutSprite ( ( BYTE * )h, x, y );

            shot->cnt++;
            shot->cnt = shot->cnt % 4;
            break;
      }
   }
}
  
  
