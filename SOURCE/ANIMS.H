#ifndef _ANIMAPI
#define _ANIMAPI

typedef enum 
{
   A_NORM,   
   A_MOVEUP,   
   A_MOVEDOWN   
}ANIM_DIR;

#define A_LARGE_GROUND_EXPLO1  0
#define A_SMALL_GROUND_EXPLO   1
#define A_PERSON               2
#define A_PLATOON              3

#define A_LARGE_AIR_EXPLO      4
#define A_MED_AIR_EXPLO        5
#define A_SMALL_AIR_EXPLO      6
#define A_MED_AIR_EXPLO2       7
#define A_ENERGY_AIR_EXPLO     8
#define A_LASER_BLAST          9

#define A_SMALL_SMOKE          10
#define A_SMALL_SMOKE_DOWN     11
#define A_SMALL_SMOKE_UP       12
#define A_ENERGY_GRAB_HIT      13
#define A_BLUE_SPARK           14
#define A_ORANGE_SPARK         15
#define A_PLAYER_SHOOT         16
#define A_GROUND_FLARE         17
#define A_GROUND_SPARKLE       18
#define A_ENERGY_GRAB          19
#define A_SUPER_SHIELD         20

/***************************************************************************
ANIMS_Clear () - Clears out All ANIM Objects
 ***************************************************************************/
VOID
ANIMS_Clear (
VOID
);

/***************************************************************************
ANIMS_Init () Initializes ANIM STuff
 ***************************************************************************/
VOID
ANIMS_Init (
VOID
);

/***************************************************************************
ANIMS_Register () - Register a ANIM for USE with this stuff
 ***************************************************************************/
INT
ANIMS_Register (
   DWORD item,             // INPUT : lumpnum of first frame
   INT   numframes,        // INPUT : number of frames
   BOOL  groundflag,       // INPUT : on the ground = TRUE
   BOOL  playerflag,       // INPUT : follow player movements
   BOOL  transparent,      // INPUT : Transparent ( LIGHT )
   ANIM_DIR adir           // INPUT : Anim Direction
);

/***************************************************************************
ANIMS_CachePics() - Cache registered anim pics
 ***************************************************************************/
VOID
ANIMS_CachePics (
VOID
);

/***************************************************************************
ANIMS_FreePics() - Free Up Anims Used 
 ***************************************************************************/
VOID
ANIMS_FreePics (
VOID
);

/***************************************************************************
ANIMS_StartAnim () - Start An ANIM Playing
 ***************************************************************************/
VOID
ANIMS_StartAnim (
INT handle,                // INPUT : ANIM handle
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
);

/***************************************************************************
ANIMS_StartGAnim () - Start An ANIM Playing with groundflag == TRUE
 ***************************************************************************/
VOID
ANIMS_StartGAnim (
INT handle,                // INPUT : ANIM handle
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
);

/***************************************************************************
ANIMS_Think () - Does all thinking for ANIMS
 ***************************************************************************/
VOID
ANIMS_Think (
VOID
);

/***************************************************************************
ANIMS_DisplayGround () - Displays All Active ANIMS on the Ground
 ***************************************************************************/
VOID
ANIMS_DisplayGround (
VOID
);

/***************************************************************************
ANIMS_DisplaySky () - Displays All Active ANIMS in SKY
 ***************************************************************************/
VOID
ANIMS_DisplaySky (
VOID
);

/***************************************************************************
ANIMS_DisplayHigh () - Displays All Active ANIMS in ABOVE PLAYER
 ***************************************************************************/
VOID
ANIMS_DisplayHigh (
VOID
);

/***************************************************************************
ANIMS_StartAAnim () - Start An ANIM Playing with groundflag == GROUND
 ***************************************************************************/
VOID
ANIMS_StartAAnim (
INT handle,                // INPUT : ANIM handle
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
);

/***************************************************************************
ANIMS_StartEAnim () - Start An ANIM Playing locked onto ENEMY 
 ***************************************************************************/
VOID
ANIMS_StartEAnim (
SPRITE_SHIP * en,          // INPUT : pointer to ENEMY
INT handle,                // INPUT : ANIM handle
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
);

#endif
