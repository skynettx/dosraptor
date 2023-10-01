#define MAX_SHOTS 70

#ifndef  _OBJAPI
#include "objects.h"
#endif

#ifndef _SHOTSAPI
#define _SHOTSAPI

typedef enum
{
   S_ALL,
   S_AIR,
   S_GROUND,
   S_GRALL,
   S_GTILE,
   S_SUCK
}HIT_TYPE;

typedef enum
{
   S_SHOOT,
   S_LINE,
   S_BEAM
}BEAM_TYPE;

typedef struct 
{
   DWORD             lumpnum;          // LUMPNUM OF PIC(S)
   BYTE *            pic[ 10 ];        // POINTER TO PIC
   BOOL              shadow;           // SHADOW ON/OFF
   OBJ_TYPE          type;             // SHOT TYPE
   INT               hits;             // DAMAGE SHOT WILL DO
   INT               speed;            // START SPEED
   INT               maxspeed;         // MAXIMUM SPEED
   INT               startframe;       // START ANIM ON FRAME
   INT               numframes;        // NUMBER OF FRAMES OF ANIM
   INT               shoot_rate;       // SHOOT EVERY (N) FRAMES
   INT               cur_shoot;        // HOLDS FRAME COUNT DOWN
   BOOL              delayflag;        // DEALAYED ANIM START
   BOOL              smoke;            // SMOKE TRAIL
   BOOL              use_plot;         // TRUE = PLOT X,Y ( SLOWER )
   BOOL              move_flag;        // DOES SHOT NEED MOVEMENT
   HIT_TYPE          ht;               // SHOT HIT TYPE ( AIR/GROUND )
   BOOL              fplrx;            // FOLLOW PLAYER X POS
   BOOL              fplry;            // FOLLOW PLAYER Y POS
   BOOL              meffect;          // TRACK MOVE TO FIND HIT ?
   BEAM_TYPE         beam;             // BEAM WEAPON TYPE
   GFX_PIC *         h;
   INT               hlx;
   INT               hly;
}SHOT_LIB;

typedef struct SHOTS_S
{
   struct SHOTS_S *  prev;             // Link List Prev
   struct SHOTS_S *  next;             // Link List Next
   BYTE  *           pic;              // pointer to cur frame pic
   INT               x;                // cur shot center x
   INT               y;                // cur shot center y
   MOVEOBJ           move;             // MOVE stuff
   INT               speed;            // cur speed
   INT               curframe;         // current anim frame
   BOOL              doneflag;         // shot done = TRUE
   BOOL              delayflag;        // DEALAYED ANIM START
   INT               startx;           // PLAYER X OFFSET;
   INT               starty;           // PLAYER Y OFFSET;
   SHOT_LIB *        lib;
   INT               cnt;
}SHOTS;

/***************************************************************************
SHOTS_Clear () * Clears out SHOTS Linklist
 ***************************************************************************/
VOID
SHOTS_Clear (
VOID
);

/***************************************************************************
SHOTS_Init () - Inits SHOTS system and clears link list
 ***************************************************************************/
VOID
SHOTS_Init (
VOID
);

/***************************************************************************
SHOTS_PlayerShoot() - Shoots the specified weapon
 ***************************************************************************/
BOOL
SHOTS_PlayerShoot (
OBJ_TYPE type              // INPUT : OBJECT TYPE
);

/***************************************************************************
SHOTS_Think () - Does All Thinking for shot system
 ***************************************************************************/
VOID
SHOTS_Think (
VOID
);

/***************************************************************************
SHOTS_Display () - Displays All active Shots
 ***************************************************************************/
VOID
SHOTS_Display (
VOID
);

#endif
