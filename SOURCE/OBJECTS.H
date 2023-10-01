#define MAX_OBJS 20

#ifndef _OBJAPI
#define _OBJAPI

typedef enum
{
   OBJ_GOTIT,
   OBJ_NOMONEY,
   OBJ_SHIPFULL,
   OBJ_ERROR
}BUYSTUFF;

typedef enum
{
   S_FORWARD_GUNS,   // 00 * DUAL MACHINE GUNS
   S_PLASMA_GUNS,    // 01 * PLASMA GUNS
   S_MICRO_MISSLE,   // 02 * SMALL WING MISSLES
   S_DUMB_MISSLE,    // 03 DUMB FIRE MISSLE         // 1
   S_MINI_GUN,       // 04 AUTO TRACK MINI GUN      // 2
   S_TURRET,         // 05 AUTO TRACK LASER TURRET  // 3
   S_MISSLE_PODS,    // 06 MULTIPLE MISSLE SHOTS    // 4
   S_AIR_MISSLE,     // 07 AIR TO AIR MISSLE        // 5
   S_GRD_MISSLE,     // 08 AIR TO GROUND            // 6
   S_BOMB,           // 09 GROUND BOMB              // 7
   S_ENERGY_GRAB,    // 10 ENERGY GRAB              // 8
   S_MEGA_BOMB,      // 11 MEGA BOMB KILLS ALL AIR  //  
   S_PULSE_CANNON,   // 12 WAVE WEAPON              // 9
   S_FORWARD_LASER,  // 13 ALTERNATING LASER        // MINUS
   S_DEATH_RAY,      // 14 DEATH RAY                // 0
   S_SUPER_SHIELD,   // 15 SUPER SHIELD             //
   S_ENERGY,         // 16 NORMAL SHIELD ENERGY     //
   S_DETECT,         // 18 DAMAGE DETECTOR          //
   S_ITEMBUY1,       // 19 
   S_ITEMBUY2,       // 20
   S_ITEMBUY3,       // 21
   S_ITEMBUY4,       // 22
   S_ITEMBUY5,       // 23
   S_ITEMBUY6,       // 24
   S_LAST_OBJECT     // 25 LAST OBJECT ( NOT USED )
}OBJ_TYPE;

#define LAST_WEAPON     ( S_DEATH_RAY )
#define FIRST_SPECIAL   ( S_DUMB_MISSLE )

typedef struct
{
   DWORD    item;                // LUMPNUM OF PIC
   INT      numframes;           // NUMBER OF FRAMES
   INT      cost;                // COST OF OBJECT
   INT      start_cnt;           // START AMOUNT OF ITEMS
   INT      max_cnt;             // MAX COUNT OF UNITS/ITEMS
   BOOL     (*actf)(OBJ_TYPE);   // FUNCTION PTR FOR ACTION
   DEFX     fxtype;              // SOUND FX TYPE
   BOOL     forever;             // LAST FOREVER
   BOOL     onlyflag;            // UNIQUE FLAG ( ENERGY )
   BOOL     loseit;              //
   BOOL     specialw;            // SPECIAL WEAPON 
   BOOL     moneyflag;           // ADDS MONEY TO PLAYER WHEN PICKED UP
   BOOL     game1flag;           // TRUE=AVALABLE IN ALL GAMES
   BOOL     shieldflag;          // SHOW SHIELD WHEN BONUS
} OBJ_LIB;                   

typedef struct OBJ_S
{
   struct OBJ_S *    prev;       // LINK LIST PREV
   struct OBJ_S *    next;       // LINK LIST NEXT
   INT               num;        // NUMBER OF ITEM(S)
   INT               type;       // OBJECT TYPE
   OBJ_LIB *         lib;        // POINTER TO LIB
   BOOL              inuse;      // CURRENTLY EQUIPED
}OBJ;

/***************************************************************************
OBJS_Clear () - Clears out All Objects
 ***************************************************************************/
VOID
OBJS_Clear (
VOID
);

/***************************************************************************
OBJS_CachePics () - PreLoad bonus/object picstures
 ***************************************************************************/
VOID
OBJS_CachePics (
VOID
);

/***************************************************************************
OBJS_FreePics () - Free bonus/object picstures
 ***************************************************************************/
VOID
OBJS_FreePics (
VOID
);

/***************************************************************************
OBJS_Init () - Sets up object stuff
 ***************************************************************************/
VOID
OBJS_Init (
VOID
);

/***************************************************************************
OBJS_DisplayStats() - Display game screen object stuff
 ***************************************************************************/
VOID
OBJS_DisplayStats (
VOID
);

/***************************************************************************
OBJS_Del () - Removes Object From User Posession
 ***************************************************************************/
VOID
OBJS_Del (
OBJ_TYPE type              //INPUT : OBJ type
);

/***************************************************************************
OBJS_Load () - Adds new OBJ from OBJ
 ***************************************************************************/
BOOL
OBJS_Load (
OBJ * inobj                // INPUT : OBJ type
);

/***************************************************************************
OBJS_Add () - Adds OBJ ( type ) to players possesions
 ***************************************************************************/
BOOL
OBJS_Add (
OBJ_TYPE type              //INPUT : OBJ type
);

/***************************************************************************
OBJS_GetNext () - Sets plr.sweapon to next avalable weapon
 ***************************************************************************/
VOID
OBJS_GetNext (
VOID
);

/***************************************************************************
OBJS_Use () - Player Use An Object
 ***************************************************************************/
VOID
OBJS_Use (
OBJ_TYPE type              //INPUT : OBJ type
);

/***************************************************************************
OBJS_Sell () - Sell Object from player posesion
 ***************************************************************************/
INT                        // RETRUN: amount left
OBJS_Sell (
OBJ_TYPE type              //INPUT : OBJ type
);

/***************************************************************************
OBJS_Buy () - Add Amount from TYPE that is equiped ( BUY )
 ***************************************************************************/
BOOL                       // RETURN: TRUE = worked, FALSE = didnot
OBJS_Buy (
OBJ_TYPE type              //INPUT : OBJ type
);

/***************************************************************************
OBJS_SubAmt () - Subtract Amount From Equiped Item
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_SubAmt (
OBJ_TYPE type,             // INPUT : OBJ type
INT      amt               // INPUT : amount to subtract
);

/***************************************************************************
OBJS_GetAmt() - Returns number of items within TYPE in Equiped Items
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_GetAmt (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetTotal() - Returns number of items within TYPE in all OBJS
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_GetTotal (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_IsOnly () - Is Onlyflag set
 ***************************************************************************/
BOOL                       // RETURN: TRUE/FALSE
OBJS_IsOnly (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetCost () - Returns The game COST of an object
 ***************************************************************************/
INT                        // RETURN: cost of object
OBJS_GetCost (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetResale () - Returns The game Resale Value of an object
 ***************************************************************************/
INT                        // RETURN: cost of object
OBJS_GetResale (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_CanBuy() - Returns TRUE if player can buy object
 ***************************************************************************/
BOOL
OBJS_CanBuy (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_CanSell() - Returns TRUE if player can Sell object
 ***************************************************************************/
BOOL
OBJS_CanSell (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_GetNum () - Returns number of Objects that player has
 ***************************************************************************/
INT                        // RETURN: number of objects
OBJS_GetNum (
VOID
);

/***************************************************************************
OBJS_GetLib () - Returns Pointer to Lib Object
 ***************************************************************************/
OBJ_LIB *
OBJS_GetLib (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_IsEquip() - Returns TRUE if item is Equiped
 ***************************************************************************/
BOOL                       // RETURN: return nums in OBJ
OBJS_IsEquip (
OBJ_TYPE type              // INPUT : OBJ type
);

/***************************************************************************
OBJS_AddEnergy()
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_AddEnergy (
INT      amt               // INPUT : amount to add
);

/***************************************************************************
OBJS_SubEnergy()
 ***************************************************************************/
INT                        // RETURN: return nums in OBJ
OBJS_SubEnergy (
INT      amt               // INPUT : amount to subtract
);

/***************************************************************************
OBJS_LoseObj() - Lose random object
 ***************************************************************************/
BOOL
OBJS_LoseObj (
VOID
);

/***************************************************************************
OBJS_Think () - Does all in game thinking ( recharing )
 ***************************************************************************/
VOID
OBJS_Think (
VOID
);

/***************************************************************************
OBJS_MakeSpecial() - Makes the selected weapon the current special
 ***************************************************************************/
BOOL
OBJS_MakeSpecial (
OBJ_TYPE type              // INPUT : OBJ type
);

#endif
