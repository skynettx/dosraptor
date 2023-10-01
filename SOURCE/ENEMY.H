#ifndef _TYPES_H
#include "types.h"
#endif

#ifndef _ENEMY
#define _ENEMY

typedef enum
{
   E_FORWARD,
   E_BACKWARD
}EDIR;

typedef enum
{
   KAMI_FLY,
   KAMI_CHASE,
   KAMI_END
}KAMI;

typedef enum
{
   GANIM_DONE,
   GANIM_OFF,
   GANIM_ON
}GANIM;

typedef enum
{
   MULTI_OFF,
   MULTI_START,
   MULTI_END
}MULTI;

typedef struct SPRITE_SHIP_S
{
   struct SPRITE_SHIP_S *   prev;
   struct SPRITE_SHIP_S *   next;
   DWORD             item;             // GLB item of current frame
   SPRITE *          lib;              // SPRITE LIB POS
   INT               sx;               // START X
   INT               sy;               // START Y
   INT               x;                // CENTER X POS;
   INT               y;                // Y POS
   INT               x2;               // WIDTH
   INT               y2;               // HEIGHT
   INT               width;            // WIDTH
   INT               height;           // HEIGHT
   INT               hlx;              // WIDTH/2
   INT               hly;              // HEIGHT/2
   INT               movepos;          // CUR POSITION IN FLIGHT (X/Y)
   INT               shootagain;       // * COUNT DOWN TO SHOOT
   INT               shootcount;       // * COUNT TO SHOOT
   INT               shootflag;        // * COUNTER TO SPACE SHOTS
   INT               angleflag;
   INT               dirflag;          // * FLIGHT DIRECTION FLAG
   INT               hits;             // *
   BOOL              groundflag;       // *
   BOOL              doneflag;
   MOVEOBJ           move;
   INT               countdown;
   DWORD             curframe;
   INT               eframe;
   INT               num_frames;
   BOOL              anim_on;
   EDIR              edir;
   INT               kami;
   INT               frame_rate;
   BOOL              shoot_on;
   BOOL              shoot_disable;
   MULTI             multi;
   INT               speed;
   INT               suckagain;
} SPRITE_SHIP;

#define E_NUM_DIFF    4

#define E_SECRET_1    0
#define E_SECRET_2    1
#define E_SECRET_3    2
#define E_EASY_LEVEL  3
#define E_MED_LEVEL   4
#define E_HARD_LEVEL  5
#define E_TRAIN_LEVEL 6

#define EB_SECRET_1    1
#define EB_SECRET_2    2
#define EB_SECRET_3    4
#define EB_EASY_LEVEL  8
#define EB_MED_LEVEL   16
#define EB_HARD_LEVEL  32
#define EB_NOT_USED    64

/***************************************************************************
ENEMY_InitSprites () - Initializes Enmy Sprites
 ***************************************************************************/
VOID
ENEMY_InitSprites (
VOID
);

/***************************************************************************
ENEMY_FreeSprites () - Free Memory Used by Sprites use in last level
 ***************************************************************************/
VOID
ENEMY_FreeSprites (
VOID
);

/***************************************************************************
ENEMY_LoadSprites() - 
 ***************************************************************************/
VOID
ENEMY_LoadSprites (
VOID
);

/***************************************************************************
ENEMY_LoadLib () - Loads and Locks spritelib's
 ***************************************************************************/
VOID
ENEMY_LoadLib (
VOID
);

/***************************************************************************
ENEMY_Clear()
 ***************************************************************************/
VOID
ENEMY_Clear(
VOID
);

/***************************************************************************
ENEMY_GetRandom () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP *
ENEMY_GetRandom (
VOID
);

/***************************************************************************
ENEMY_GetRandomAir () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP *
ENEMY_GetRandomAir (
VOID
);

/***************************************************************************
ENEMY_DamageAll () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL
ENEMY_DamageAll (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT damage                 // INPUT : damage
);

/***************************************************************************
ENEMY_DamageGround () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL
ENEMY_DamageGround (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT damage                 // INPUT : damage
);

/***************************************************************************
ENEMY_DamageAir () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
BOOL
ENEMY_DamageAir (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT damage                 // INPUT : damage
);

/***************************************************************************
ENEMY_DamageEnergy () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
SPRITE_SHIP *
ENEMY_DamageEnergy (
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT damage                 // INPUT : damage
);

/***************************************************************************
ENEMY_Think() - Does all thinking for enemy ships ( ground/air )
 ***************************************************************************/
VOID
ENEMY_Think(
VOID
);

/***************************************************************************
ENEMY_DisplayGround () - Displays Ground ENEMY pics
 ***************************************************************************/
VOID
ENEMY_DisplayGround (
VOID
);

/***************************************************************************
ENEMY_DisplaySky () - Displays AIR ENEMY SHIPS
 ***************************************************************************/
VOID
ENEMY_DisplaySky (
VOID
);

/***************************************************************************
ENEMY_GetBaseDamage() - Gets Base Ship damage
 ***************************************************************************/
INT
ENEMY_GetBaseDamage (
VOID
);

extern CSPRITE   *   csprite;
extern INT           numships;
extern DWORD         cur_diff;

#endif
