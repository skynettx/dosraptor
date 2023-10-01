#ifndef _ESHOTAPI
#define _ESHOTAPI

typedef enum
{
   ES_ATPLAYER,      // 0
   ES_ATDOWN,        // 1
   ES_ANGLELEFT,     // 2
   ES_ANGLERIGHT,    // 3
   ES_MISSLE,        // 4
   ES_LASER,         // 5
   ES_MINES,         // 6
   ES_PLASMA,        // 7
   ES_COCONUTS       // 8
}ESHOT_TYPE;

/***************************************************************************
ESHOT_Clear () - Clears out ESHOT Linklist
 ***************************************************************************/
VOID
ESHOT_Clear (
VOID
);

/***************************************************************************
ESHOT_Init () - Inits ESHOT system and clears link list
 ***************************************************************************/
VOID
ESHOT_Init (
VOID
);

/***************************************************************************
ESHOT_Shoot() - Shoots ENEMY GUNS
 ***************************************************************************/
VOID
ESHOT_Shoot (
SPRITE_SHIP * enemy,       // INPUT : pointer to Enemy stuff
INT           gun_num      // INPUT : gun number to shoot
);

/***************************************************************************
ESHOT_Think () - Does All Thinking for shot system
 ***************************************************************************/
VOID
ESHOT_Think (
VOID
);

/***************************************************************************
ESHOT_Display () - Displays All active Shots
 ***************************************************************************/
VOID
ESHOT_Display (
VOID
);

#endif
