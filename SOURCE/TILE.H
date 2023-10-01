/****************************************************************************
* TILE  - 32 by 32 Tile Drawing
*----------------------------------------------------------------------------
* Copyright (C) 1993  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        July, 1993
*
* CONTENTS: tile.c tile_a.asm
*
* EXTERN MODULES - GFXAPI
*
*---------------------------------------------------------------------------*/
  
#ifndef _TYPES_H
#include <types.h>
#endif
  
#ifndef _TILEAPI
#define _TILEAPI
  
typedef enum
{
   TILE_NONE,
   TILE_HIT,
   TILE_DESTROY,
   TILE_BONUS
}T_HIT;

typedef struct
{
   DWORD    item;
   INT      x;
   INT      y;
   INT      mapspot;
}TILESPOT;
 
typedef struct TDELAY_S
{
   struct TDELAY_S * prev;
   struct TDELAY_S * next;
   INT               frames;
   INT               mapspot;
   DWORD             item;
   TILESPOT *        ts;
}TILEDELAY;

/***************************************************************************
TILE_Draw () - Draws 32 by 32 TILE Clips on y only
 ***************************************************************************/
VOID
TILE_Put (
BYTE * inpic,              // INPUT : pointer to GFX_PIC ( norm )
INT   x,                   // INPUT : x position
INT   y                    // INPUT : y position
);
  
/***************************************************************************
TILE_Init () - Sets Up A level for Displaying
 ***************************************************************************/
VOID
TILE_Init (
VOID
);

/***************************************************************************
TILE_FreeLevel () - Free tile level
 ***************************************************************************/
VOID
TILE_FreeLevel (
VOID
);

/***************************************************************************
TILE_CacheLevel () - Cache tiles in current level
 ***************************************************************************/
VOID
TILE_CacheLevel (
VOID
);

/***************************************************************************
TILE_DamageAll () - Damages All tiles on screen
 ***************************************************************************/
VOID
TILE_DamageAll (
VOID
);
  
/***************************************************************************
TILE_Think () - Does Position Calculations for tiles
 ***************************************************************************/
VOID
TILE_Think (
VOID
);
  
/***************************************************************************
TILE_Display () - Displays Tiles
 ***************************************************************************/
VOID
TILE_Display (
VOID
);
  
/***************************************************************************
TILE_IsHit () - Checks to see if a shot hits an explodable tile
 ***************************************************************************/
T_HIT                      // RETURNS : see T_HIT in tile.h
TILE_IsHit (
INT damage,                // INPUT : damage to tile
INT x,                     // INOUT : x screen pos, out tile x
INT y                      // INOUT : y screen pos, out tile y
);

/***************************************************************************
TILE_Bomb () - Checks to see if a BOMB hits an explodable tile
 ***************************************************************************/
BOOL                       // RETURNS : TRUE = Tile Hit
TILE_Bomb (
INT damage,                // INPUT : damage to tile
INT  x,                    // INOUT : x screen pos, out tile x
INT  y                     // INOUT : y screen pos, out tile y
);
  
/***************************************************************************
TILE_Explode () - Sets the Tile to show explosion tile
 ***************************************************************************/
VOID
TILE_Explode (
TILESPOT * ts,             // INPUT : tilespot of explosion
INT delay                  // INPUT : frames to delay
);
  
#pragma aux TILE_Draw "_*" modify [ EAX EBX ECX EDX ESI EDI ];
VOID TILE_Draw ( VOID );
  
#pragma aux TILE_ClipDraw "_*" modify [ EAX EBX ECX EDX ESI EDI ];
VOID TILE_ClipDraw ( VOID );
  
#pragma aux TILE_DisplayScreen "_*" modify [ EAX EBX ECX EDX ESI EDI ];
VOID TILE_DisplayScreen ( VOID );

#pragma aux TILE_ShakeScreen "_*" modify [ EAX EBX ECX EDX ESI EDI ];
VOID TILE_ShakeScreen ( VOID );
  
extern DWORD   startflat[4];
extern INT     tilepos;
extern INT     tileyoff;
extern BOOL    scroll_flag;
extern INT     g_mapleft;

#endif
