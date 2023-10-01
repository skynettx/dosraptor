#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#include "raptor.h"
  
#include "file0001.inc"

PRIVATE OBJ     objs [ MAX_OBJS ];
  
OBJ             first_objs;
OBJ             last_objs;
OBJ *           free_objs;
PRIVATE INT     obj_cnt;


PUBLIC OBJ_LIB  obj_lib [ S_LAST_OBJECT ];
PUBLIC OBJ  *   p_objs  [ S_LAST_OBJECT ];

PRIVATE BOOL   objuse_flag;
PRIVATE INT    think_cnt;

#define  CHARGE_SHIELD  ( 24 * 4 )  

/***************************************************************************
OBJS_Clear () - Clears out All Objects
 ***************************************************************************/
VOID
OBJS_Clear (
VOID
)
{
   INT loop;
  
   obj_cnt = 0;
  
   first_objs.prev = NUL;
   first_objs.next = &last_objs;
  
   last_objs.prev  = &first_objs;
   last_objs.next  = NUL;
  
   free_objs       = objs;
  
   memset ( objs, 0, sizeof ( objs ) );
   memset ( p_objs, 0, sizeof ( p_objs ) );
  
   for ( loop = 0; loop < MAX_OBJS-1; loop++ )
      objs[ loop ].next = &objs [ loop + 1 ];
}
  
/*-------------------------------------------------------------------------*
OBJS_Get () - Gets A Free OBJ from Link List
 *-------------------------------------------------------------------------*/
PRIVATE OBJ *
OBJS_Get (
VOID
)
{
   OBJ * new;
  
   if ( !free_objs )
      return ( NUL );
  
   new       = free_objs;
   free_objs = free_objs->next;
  
   memset ( new, 0 ,sizeof ( OBJ ) );
  
   new->next = &last_objs;
   new->prev = last_objs.prev;
   last_objs.prev  = new;
   new->prev->next = new;
  
   obj_cnt++;
   return ( new );
}
  
/*-------------------------------------------------------------------------*
OBJS_Remove () Removes OBJ from Link List
 *-------------------------------------------------------------------------*/
PRIVATE OBJ *
OBJS_Remove (
OBJ * sh
)
{
   OBJ * next;
  
   next = sh->prev;
  
   sh->next->prev = sh->prev;
   sh->prev->next = sh->next;
  
   memset ( sh, 0, sizeof ( OBJ ) );
  
   sh->next = free_objs;
  
   free_objs = sh;
  
   obj_cnt--;
  
   return ( next );
}

/***************************************************************************
OBJS_CachePics () - PreLoad bonus/object pictures
 ***************************************************************************/
VOID
OBJS_CachePics (
VOID
)
{
   OBJ_LIB * lib;
   INT       loop,i;

   for ( loop = 0; loop < S_LAST_OBJECT; loop++ )
   {
      lib = &obj_lib [ loop ];

      if ( lib && lib->item != EMPTY )
      {
         for ( i = 0; i < lib->numframes ; i++ )
         {
            GLB_CacheItem ( lib->item + (DWORD)i );            
         }
      }
   }

   GLB_CacheItem ( SMSHIELD_PIC );
   GLB_CacheItem ( SMBOMB_PIC );

}

/***************************************************************************
OBJS_FreePics () - Free bonus/object picstures
 ***************************************************************************/
VOID
OBJS_FreePics (
VOID
)
{
   OBJ_LIB * lib;
   INT       loop,i;

   for ( loop = 0; loop < S_LAST_OBJECT; loop++ )
   {
      lib = &obj_lib [ loop ];

      if ( lib && lib->item != EMPTY )
      {
         for ( i = 0; i < lib->numframes ; i++ )
         {
            GLB_FreeItem ( lib->item + (DWORD)i );            
         }
      }
   }

   GLB_FreeItem ( SMSHIELD_PIC );
   GLB_FreeItem ( SMBOMB_PIC );
}
  
/***************************************************************************
OBJS_Init () - Sets up object stuff
 ***************************************************************************/
VOID
OBJS_Init (
VOID
)
{
   OBJ_LIB * lib;
  
   OBJS_Clear();
  
   memset ( obj_lib, 0, sizeof ( obj_lib ) );
   memset ( p_objs, 0, sizeof ( p_objs ) );
  
   // == FORWARD GUNS ===  *
   lib = &obj_lib [ S_FORWARD_GUNS ];
   lib->cost      = 12000;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS00_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = FALSE;
   lib->specialw  = FALSE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == PLASMA GUNS ===  *
   lib = &obj_lib [ S_PLASMA_GUNS ];
   lib->cost      = 78800;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS01_PIC;
   lib->numframes = 2;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == MICRO MISSLES ===  *
   lib = &obj_lib [ S_MICRO_MISSLE ];
   lib->cost      = 175600;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS02_PIC;
   lib->numframes = 2;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == DUMB MISSLES === *
   lib = &obj_lib [ S_DUMB_MISSLE ];
   lib->cost      = 145200;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS03_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_MISSLE;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == MINI GUN ===
   lib = &obj_lib [ S_MINI_GUN ];
   lib->cost      = 250650;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS04_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == LASER TURRET ===
   lib = &obj_lib [ S_TURRET ];
   lib->cost      = 512850;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS05_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_LASER;
   lib->moneyflag = FALSE;
   lib->game1flag = FALSE;
   lib->shieldflag = TRUE;
  
   // == MISSLE PODS ===
   lib = &obj_lib [ S_MISSLE_PODS ];
   lib->cost      = 204950;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS06_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == AIR TO AIR MISSLES === *
   lib = &obj_lib [ S_AIR_MISSLE ];
   lib->cost      = 63500;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS07_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_MISSLE;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == AIR TO GROUND MISSLES === *
   lib = &obj_lib [ S_GRD_MISSLE ];
   lib->cost      = 110000;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS08_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_MISSLE;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == GROUND BOMB ===
   lib = &obj_lib [ S_BOMB ];
   lib->cost      = 98200;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS21_PIC;
   lib->numframes = 1;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_MISSLE;
   lib->moneyflag = FALSE;
   lib->game1flag = FALSE;
   lib->shieldflag = TRUE;
  
   // == ENERGY GRAB ===
   lib = &obj_lib [ S_ENERGY_GRAB ];
   lib->cost      = 300750;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS09_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = FALSE;
   lib->shieldflag = TRUE;
  
   // == MEGA BOMB === *
   lib = &obj_lib [ S_MEGA_BOMB ];
   lib->cost      = 32250;
   lib->start_cnt = 1;
   lib->max_cnt   = 5;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS10_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = TRUE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == PULSE CANNON ===
   lib = &obj_lib [ S_PULSE_CANNON ];
   lib->cost      = 725000;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS11_PIC;
   lib->numframes = 2;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_GUN;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == FORWARD LASER ===
   lib = &obj_lib [ S_FORWARD_LASER ];
   lib->cost      = 1750000;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS12_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_LASER;
   lib->moneyflag = FALSE;
   lib->game1flag = FALSE;
   lib->shieldflag = TRUE;
  
   // == DEATH RAY ===
   lib = &obj_lib [ S_DEATH_RAY ];
   lib->cost      = 950000;
   lib->start_cnt = 1;
   lib->max_cnt   = 1;
   lib->actf      = SHOTS_PlayerShoot;
   lib->item      = BONUS13_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = TRUE;
   lib->fxtype    = FX_LASER;
   lib->moneyflag = FALSE;
   lib->game1flag = FALSE;
   lib->shieldflag = TRUE;
  
   // == SUPER SHIELD ===
   lib = &obj_lib [ S_SUPER_SHIELD ];
   lib->cost      = 78500;
   lib->start_cnt = MAX_SHIELD;
   lib->max_cnt   = MAX_SHIELD;
   lib->item      = BONUS14_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = FALSE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;
  
   // == SHIP NORM ENERGY ===
   lib = &obj_lib [ S_ENERGY ];
   lib->cost      = 400;
   lib->start_cnt = MAX_SHIELD/4;
   lib->max_cnt   = MAX_SHIELD;
   lib->item      = BONUS15_PIC;
   lib->numframes = 4;
   lib->forever   = TRUE;
   lib->onlyflag  = TRUE;
   lib->loseit    = FALSE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = FALSE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == ENEMY DAMAGE DETECTOR ===
   lib = &obj_lib [ S_DETECT ];
   lib->cost       = 10000;
   lib->start_cnt  = 1;
   lib->max_cnt    = 1;
   lib->item       = BONUS16_PIC;
   lib->numframes  = 1;
   lib->forever    = FALSE;
   lib->onlyflag   = TRUE;
   lib->loseit     = FALSE;
   lib->specialw   = FALSE;
   lib->fxtype     = EMPTY;
   lib->moneyflag  = FALSE;
   lib->game1flag  = TRUE;
   lib->shieldflag = FALSE;

   // == BUY ITEM 1 ===
   lib = &obj_lib [ S_ITEMBUY1 ];
   lib->cost      = 93800;
   lib->start_cnt = lib->cost;
   lib->max_cnt   = lib->cost;
   lib->item      = BONUS16_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = TRUE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == BUY ITEM 2 ===
   lib = &obj_lib [ S_ITEMBUY2 ];
   lib->cost      = 76000;
   lib->start_cnt = lib->cost;
   lib->max_cnt   = lib->cost;
   lib->item      = BONUS17_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = TRUE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == BUY ITEM 3 ===
   lib = &obj_lib [ S_ITEMBUY3 ];
   lib->cost      = 55700;
   lib->start_cnt = lib->cost;
   lib->max_cnt   = lib->cost;
   lib->item      = BONUS18_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = TRUE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == BUY ITEM 4 ===
   lib = &obj_lib [ S_ITEMBUY4 ];
   lib->cost      = 35200;
   lib->start_cnt = lib->cost;
   lib->max_cnt   = lib->cost;
   lib->item      = BONUS19_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = TRUE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == BUY ITEM 5 ===
   lib = &obj_lib [ S_ITEMBUY5 ];
   lib->cost      = 122500;
   lib->start_cnt = lib->cost;
   lib->max_cnt   = lib->cost;
   lib->item      = BONUS20_PIC;
   lib->numframes = 1;
   lib->forever   = FALSE;
   lib->onlyflag  = FALSE;
   lib->loseit    = TRUE;
   lib->specialw  = FALSE;
   lib->fxtype    = EMPTY;
   lib->moneyflag = TRUE;
   lib->game1flag = TRUE;
   lib->shieldflag = TRUE;

   // == BUY ITEM 6 ===
   lib = &obj_lib [ S_ITEMBUY6 ];
   lib->cost       = 50;
   lib->start_cnt  = lib->cost;
   lib->max_cnt    = lib->cost;
   lib->item       = BONUS22_PIC;
   lib->numframes  = 4;
   lib->forever    = FALSE;
   lib->onlyflag   = FALSE;
   lib->loseit     = TRUE;
   lib->specialw   = FALSE;
   lib->fxtype     = EMPTY;
   lib->moneyflag  = TRUE;
   lib->game1flag  = TRUE;
   lib->shieldflag = FALSE;
}

/***************************************************************************
OBJS_DisplayStats() - Display game screen object stuff
 ***************************************************************************/
VOID
OBJS_DisplayStats (
VOID
)
{
   static int dpos = 0;
   DWORD item;
   INT   loop;
   INT   x;
   INT   maxloop;

   if ( p_objs [ S_DETECT ] )
   {
      loop = ENEMY_GetBaseDamage();
      if ( loop > 0 )
      {
         GFX_ColorBox ( 109, MAP_BOTTOM + 9, 102, 8, 74 );
         GFX_ColorBox ( 110, MAP_BOTTOM + 10, loop, 6, 68 );
      }
      else
      {
         GFX_VLine ( 110 + dpos, MAP_BOTTOM + 8, 3 , 68 );
         GFX_VLine ( 110 + 99 - dpos, MAP_BOTTOM + 8, 3 , 68 );
         dpos++;
         dpos = dpos % 50;
      }
   }

   if ( plr.sweapon != EMPTY )
   {
      item = obj_lib [ plr.sweapon ].item;
      GFX_PutSprite ( GLB_GetItem ( item ), MAP_RIGHT - 18, MAP_TOP );
   }

   if ( p_objs [ S_SUPER_SHIELD ] )
   {
      x = MAP_LEFT + 2;
      maxloop = OBJS_GetTotal ( S_SUPER_SHIELD );
      for ( loop = 0; loop < maxloop; loop++ )
      {
         GFX_PutSprite ( GLB_GetItem ( SMSHIELD_PIC ), x, 1 );
         x += 13;
      }
   }

   if ( p_objs [ S_MEGA_BOMB ] )
   {
      x = MAP_LEFT + 2;
      for ( loop = 0; loop < p_objs [ S_MEGA_BOMB ]->num; loop++ )
      {
         GFX_PutSprite ( GLB_GetItem ( SMBOMB_PIC ), x, 199 - 13 );
         x += 13;
      }
   }
}

/*-------------------------------------------------------------------------*
OBJS_Equip () - Equips an OBJ to be used by Player
 *-------------------------------------------------------------------------*/
BOOL                       // RETURN: TRUE/FALSE
OBJS_Equip (
OBJ_TYPE type              // INPUT: OBJ type
)
{
   OBJ * cur;
  
   for ( cur=first_objs.next; cur!=&last_objs; cur=cur->next )
   {
      if ( cur->type == type && p_objs [ type ] == NUL )
      {
         cur->inuse = TRUE;
         p_objs [ type ] = cur;
         return ( TRUE );
      }
   }
  
   return ( FALSE );
}

/***************************************************************************
OBJS_Load () - Adds new OBJ from OBJ
 ***************************************************************************/
BOOL
OBJS_Load (
OBJ * inobj                // INPUT : pointer to OBJ 
)
{
   OBJ *       cur;
  
   cur = OBJS_Get();
   if ( !cur ) return ( FALSE );
  
   cur->num    = inobj->num;
   cur->type   = inobj->type;
   cur->lib    = &obj_lib [ inobj->type ];
   cur->inuse  = inobj->inuse;

   if ( cur->inuse )
   {
      p_objs [ inobj->type ] = cur;
   }
  
   return ( TRUE );
}
  
/***************************************************************************
OBJS_Add () - Adds OBJ ( type ) to players possesions
 ***************************************************************************/
BUYSTUFF
OBJS_Add (
OBJ_TYPE type               // INPUT : OBJ type
)
{
   extern INT  g_oldsuper;
   extern INT  g_oldshield;
   OBJ *       cur;
   OBJ_LIB *   lib;
  
   if ( type >= S_LAST_OBJECT )
      return( OBJ_ERROR );

   g_oldsuper = EMPTY;
   g_oldshield = EMPTY;

   lib = &obj_lib [ type ];

   if ( lib->moneyflag )
   {
      plr.score += lib->cost;
      return ( OBJ_GOTIT );
   }

   if ( !reg_flag )
   {
      if ( !lib->game1flag )
         return ( OBJ_GOTIT );
   }

   if ( lib->onlyflag )
   {
      for ( cur=first_objs.next; cur!=&last_objs; cur=cur->next )
      {
         if ( cur->type == type )
         {
            if ( cur->num >= lib->max_cnt )
               return ( OBJ_SHIPFULL );

            cur->num += lib->start_cnt;
            if ( cur->num > lib->max_cnt )
               cur->num = lib->max_cnt;

            return ( OBJ_GOTIT );
         }
      }
   }

   cur = OBJS_Get();
   if ( !cur ) return ( OBJ_SHIPFULL );
  
   cur->num    = lib->start_cnt;
   cur->type   = type;
   cur->lib    = lib;

   // == equip item if needed =====
   if ( p_objs [ type ] == NUL )
   {
      cur->inuse = TRUE;
      p_objs [ type ] = cur;
      if ( plr.sweapon == EMPTY && lib->specialw )
         plr.sweapon = type;
   }

   return ( OBJ_GOTIT );
}
  
/***************************************************************************
OBJS_Del () - Removes Object From User Posession
 ***************************************************************************/
VOID
OBJS_Del (
OBJ_TYPE type              //INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
  
   if ( cur == NUL ) return;

   OBJS_Remove ( cur );
   p_objs [ type ] = NUL;
   OBJS_Equip ( type );

   if ( type == plr.sweapon )
      OBJS_GetNext();
}

/***************************************************************************
OBJS_GetNext () - Sets plr.sweapon to next avalable weapon
 ***************************************************************************/
VOID
OBJS_GetNext (
VOID
)
{
   INT   loop;
   INT   pos;
   INT   setval = EMPTY;
   OBJ * cur;

   if ( plr.sweapon < S_DUMB_MISSLE )
      pos = S_DUMB_MISSLE;
   else
      pos = plr.sweapon + 1;

   for ( loop = FIRST_SPECIAL; loop <= LAST_WEAPON; loop++ )
   {
      if ( pos > LAST_WEAPON )
         pos = FIRST_SPECIAL;

      cur = p_objs [ pos ];

      if ( cur && cur->num && cur->lib->specialw )
      {
         setval = pos;
         break;
      }

      pos++;
   }

   plr.sweapon = setval;

}
  
/***************************************************************************
OBJS_Use () - Player Use An Object
 ***************************************************************************/
VOID
OBJS_Use (
OBJ_TYPE type              //INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
   OBJ_LIB *   lib = &obj_lib [ type ];
  
   if ( !cur )
      return;
  
   objuse_flag = TRUE;
   think_cnt   = 0;

   if ( lib->actf ( type ) )
   {
      if ( !lib->forever )
         cur->num--;

   }

   if ( cur->num <= 0 && !lib->forever )
   {
      OBJS_Remove ( cur );
      p_objs [ type ] = NUL;
      OBJS_Equip ( type );
      if ( plr.sweapon == type )
         OBJS_GetNext();
      return;
   }
}

/***************************************************************************
OBJS_Sell () - Sell Object from player posesion
 ***************************************************************************/
INT                        // RETRUN: amount left
OBJS_Sell (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur   = p_objs [ type ];
   OBJ_LIB *   lib   = &obj_lib [ type ];
   INT         rval  = 0;
  
   if ( !cur )
      return ( 0 );

   plr.score += OBJS_GetResale ( type );

   if ( type == S_DETECT )
   {
      p_objs [ type ] = NUL;
      return ( 0 );
   }

   if ( lib->onlyflag )
   {
      cur->num -= lib->start_cnt;

      if ( cur->num <= 0 )
      {
         rval = 0;
         cur->num = 0;
         if ( !lib->forever )
         {
            OBJS_Remove ( cur );
            p_objs [ type ] = NUL;
            OBJS_Equip ( type );
            if ( plr.sweapon == type )
               OBJS_GetNext();
         }
      }
      else
         rval = cur->num;
   }
   else
   {
      OBJS_Del ( type );

      rval = OBJS_GetTotal ( type );
   }

   return ( rval );
}

/***************************************************************************
OBJS_Buy () - Add Amount from TYPE that is equiped ( BUY )
 ***************************************************************************/
BUYSTUFF                   // RETURN: see BUYSTUFF
OBJS_Buy (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   BUYSTUFF rval = OBJ_NOMONEY;
   INT num;

   if ( type == S_SUPER_SHIELD )
   {
      num = OBJS_GetTotal ( S_SUPER_SHIELD );
      if ( num >= 5 )
         return ( OBJ_SHIPFULL );
   }

   if ( plr.score >= OBJS_GetCost ( type ) )
   {
      rval = OBJS_Add ( type );

      if ( rval == OBJ_GOTIT )
         plr.score -= OBJS_GetCost ( type );
   }
  
   return ( rval );
}
 
/***************************************************************************
OBJS_SubAmt () - Subtract Amount From Equiped Item
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_SubAmt (
OBJ_TYPE type,             // INPUT : OBJ type
INT      amt               // INPUT : amount to subtract
)
{
   OBJ *       cur = p_objs [ type ];
  
   if ( !cur )
      return(0);

   cur->num -= amt;

   if ( cur->num < 0 )
      cur->num = 0;

   return ( cur->num );
}

/***************************************************************************
OBJS_GetAmt() - Returns number of items within TYPE in Equiped Items
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_GetAmt (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
  
   if ( !cur )
      return(0);

   return ( cur->num );
}
 
/***************************************************************************
OBJS_GetTotal() - Returns number of items within TYPE in all OBJS
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_GetTotal (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur;
   INT         total;
  
   total = 0;

   for ( cur=first_objs.next; cur!=&last_objs; cur=cur->next )
   {
      if ( type == cur->type )
         total++;
   }

   return ( total );
}

/***************************************************************************
OBJS_IsOnly () - Is Onlyflag set
 ***************************************************************************/
BOOL                       // RETURN: TRUE/FALSE
OBJS_IsOnly (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ_LIB *   lib = &obj_lib [ type ];

   return ( lib->onlyflag );
}

/***************************************************************************
OBJS_GetCost () - Returns The game COST of an object
 ***************************************************************************/
INT                        // RETURN: cost of object
OBJS_GetCost (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ_LIB *   lib = &obj_lib [ type ];
   INT         cost;

   if ( !lib )
      return ( 99999999 );

   if ( lib->onlyflag )
      cost = lib->cost * lib->start_cnt;
   else
      cost = lib->cost;

   return ( cost );
}

/***************************************************************************
OBJS_GetResale () - Returns The game Resale Value of an object
 ***************************************************************************/
INT                        // RETURN: cost of object
OBJS_GetResale (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
   OBJ_LIB *   lib = &obj_lib [ type ];
   INT         cost;

   if ( !cur )
      return(0);

   if ( lib->onlyflag )
      cost = lib->cost * lib->start_cnt;
   else
      cost = lib->cost;

   cost = cost >> 1;

   return ( cost );
}

/***************************************************************************
OBJS_CanBuy() - Returns TRUE if player can buy object
 ***************************************************************************/
BOOL
OBJS_CanBuy (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ_LIB *   lib = &obj_lib [ type ];
   INT         cost;

   if ( type >= S_LAST_OBJECT )
      return ( FALSE );

#if 0
   if ( type == S_SUPER_SHIELD )
   {
      cost = OBJS_GetTotal ( S_SUPER_SHIELD );
      if ( cost >= 5 )
         return ( FALSE );
   }
#endif

   if ( type == S_FORWARD_GUNS )
   {
      if ( OBJS_IsEquip ( type ) )
         return ( FALSE );
   }

   if ( !reg_flag )
   {
      if ( !lib->game1flag )
         return ( FALSE );
   }

   cost = OBJS_GetCost ( type );

   if ( cost == 0 )
      return ( FALSE );

   return ( TRUE );
}


/***************************************************************************
OBJS_CanSell() - Returns TRUE if player can Sell object
 ***************************************************************************/
BOOL
OBJS_CanSell (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
   OBJ_LIB *   lib = &obj_lib [ type ];

   if ( type >= S_LAST_OBJECT )
      return ( FALSE );

   if ( cur == NULL )
      return ( FALSE );

   if ( lib->onlyflag && type == S_ENERGY )
   {
      if ( cur->num <= lib->start_cnt )
         return ( FALSE );
   }

   if ( cur->num < lib->start_cnt )
      return ( FALSE );

   return ( TRUE );
}

/***************************************************************************
OBJS_GetNum () - Returns number of Objects that player has
 ***************************************************************************/
INT                        // RETURN: number of objects
OBJS_GetNum (
VOID
)
{
   return ( obj_cnt );
}

/***************************************************************************
OBJS_GetLib () - Returns Pointer to Lib Object
 ***************************************************************************/
OBJ_LIB *
OBJS_GetLib (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ_LIB *   lib = &obj_lib [ type ];
  
   return ( lib );
}

/***************************************************************************
OBJS_IsEquip() - Returns TRUE if item is Equiped
 ***************************************************************************/
BOOL                       // RETURN: return nums in OBJ
OBJS_IsEquip (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   if ( p_objs [ type ] )
      return ( TRUE );

   return ( FALSE );
}

/***************************************************************************
OBJS_SubEnergy()
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_SubEnergy (
INT      amt               // INPUT : amount to subtract
)
{
   extern BOOL godmode;
   OBJ *  cur;
 
   if ( godmode ) return ( 0 );

   if ( startendwave != EMPTY )  return ( 0 );
 
   cur = p_objs [ S_SUPER_SHIELD ];

   if ( curplr_diff == DIFF_0 && amt > 1 )
      amt = amt>>1;

   if ( cur )
   {
      ANIMS_StartAnim ( A_SUPER_SHIELD, 0, 0 );

      SND_Patch ( FX_SHIT, 127 );

      cur->num -= amt;

      if ( cur->num < 0 )
         OBJS_Del ( S_SUPER_SHIELD );
   }
   else
   {
      cur = p_objs [ S_ENERGY ];
      if ( !cur ) return(0);

      SND_Patch ( FX_HIT, 127 );

      cur->num -= amt;

      if ( cur->num < 0 )
         cur->num = 0;
   }

   return ( cur->num );
}

/***************************************************************************
OBJS_AddEnergy()
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_AddEnergy (
INT      amt               // INPUT : amount to add
)
{
   OBJ *  cur;
  
   cur = p_objs [ S_ENERGY ];
   if ( !cur ) return(0);

   if ( cur->num < cur->lib->max_cnt )
   {
      cur = p_objs [ S_ENERGY ];

      if ( cur->num == 0 )
         return ( 0 );

      cur->num += amt;

      if ( cur->num > cur->lib->max_cnt )
         cur->num = cur->lib->max_cnt;
   }
   else
   {
      cur = p_objs [ S_SUPER_SHIELD ];
      if ( !cur ) return(0);

      if ( cur->num == 0 )
         return ( 0 );

      cur->num += amt>>2;

      if ( cur->num > cur->lib->max_cnt )
         cur->num = cur->lib->max_cnt;
   }

   return ( cur->num );
}

/***************************************************************************
OBJS_LoseObj() - Lose random object
 ***************************************************************************/
BOOL
OBJS_LoseObj (
VOID
)
{
   OBJ_LIB *   lib;
   INT         type;
   BOOL        rval = TRUE;

   if ( plr.sweapon == EMPTY )
   {
      for ( type = S_LAST_OBJECT-1; type >= 0; type-- )
      {
         lib = &obj_lib [ type ];
         if ( p_objs [ type ] && lib->loseit )
         {
            OBJS_Del ( type );
            rval = TRUE;
            break;
         }
      }
   }
   else
   {
      OBJS_Del ( plr.sweapon );
      rval = TRUE;
   }

   return ( rval );
}

/***************************************************************************
OBJS_Think () - Does all in game thinking ( recharing )
 ***************************************************************************/
VOID
OBJS_Think (
VOID
)
{
   if ( curplr_diff >= DIFF_3 )
      return;

   if ( objuse_flag )
   {
      objuse_flag = FALSE;
      return;
   }

   think_cnt++;

   if ( think_cnt > CHARGE_SHIELD )
   {
      if ( startendwave == EMPTY )
         OBJS_AddEnergy ( 1 );
      think_cnt = 0;
   }
}

/***************************************************************************
OBJS_MakeSpecial() - Makes the selected weapon the current special
 ***************************************************************************/
BOOL
OBJS_MakeSpecial (
OBJ_TYPE type              // INPUT : OBJ type
)
{
   OBJ *       cur = p_objs [ type ];
   OBJ_LIB *   lib = &obj_lib [ type ];

   if ( type >= S_LAST_OBJECT )
      return ( FALSE );

   if ( cur == NULL )
      return ( FALSE );

   if ( lib->specialw == FALSE )
      return ( FALSE );

   plr.sweapon = ( OBJ_TYPE )type;

   return ( TRUE );
}
