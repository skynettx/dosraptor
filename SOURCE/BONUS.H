#define MAX_BONUS 12

#ifndef _OBJAPI
#include "objects.h"
#endif

#ifndef _BONUSAPI
#define _BONUSAPI

#define BONUS_WIDTH   16
#define BONUS_HEIGHT  16

#define MAX_MONEY     ( MAX_BONUS - 3 )

typedef struct BONUS_S
{
   struct BONUS_S *  prev;       // LINK LIST PREV
   struct BONUS_S *  next;       // LINK LIST NEXT
   DWORD             item;       // GLB item
   INT               curframe;   // CURRENT ANIM FRAME
   INT               curglow;    // CURRENT GLOW FRAME
   INT               x;          // X POSITION
   INT               y;          // Y POSITION
   INT               bx;         // BONUS X POSITION
   INT               by;         // BONUS Y POSITION
   INT               gx;         // GLOW X POSITION
   INT               gy;         // GLOW Y POSITION
   INT               pos;        // POS IN CIRCLE
   BOOL              dflag;      // DONEFLAG
   INT               countdown;  // COUNTDOWN TO REMOVE
   OBJ_TYPE          type;       // OBJECT TYPE 
   OBJ_LIB      *    lib;        // POINTER TO OBJECT LIBRARY  
}BONUS;

/***************************************************************************
BONUS_Clear () - Clears out All bonuses
 ***************************************************************************/
VOID
BONUS_Clear (
VOID
);

/***************************************************************************
BONUS_Init () - Sets up Bonus stuff
 ***************************************************************************/
VOID
BONUS_Init (
VOID
);

/***************************************************************************
BONUS_Add () - Adds A BONUS to Game so player can Try to pick it up
 ***************************************************************************/
VOID
BONUS_Add (
OBJ_TYPE type,             // INPUT : OBJECT TYPE
INT      x,                // INPUT : X POSITION
INT      y                 // INPUT : Y POSITION
);

/***************************************************************************
BONUS_Think () - Does all BONUS Thinking
 ***************************************************************************/
VOID
BONUS_Think (
VOID
);

/***************************************************************************
BONUS_Display () - Displays Active Bonuses in game
 ***************************************************************************/
VOID
BONUS_Display (
VOID
);

#endif
