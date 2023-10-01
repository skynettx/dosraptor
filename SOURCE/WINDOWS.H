#ifndef _WINDOW_H
#define _WINDOW_H

/***************************************************************************
WIN_WinGame () - WIndow text for winners of a game
 ***************************************************************************/
VOID
WIN_WinGame (
INT game                   // INPUT : game number 0,1,2
);

/***************************************************************************
WIN_Msg () - Display a Message for ten secs or until user Acks somting
 ***************************************************************************/
VOID
WIN_Msg (
CHAR * msg                 // INPUT : pointer to message to ask
);

/***************************************************************************
WIN_Pause () - Display a Pause Message Wait until user does somthing
 ***************************************************************************/
VOID
WIN_Pause (
VOID
);

/***************************************************************************
WIN_Order () - Display a Pause Message Wait until user does somthing
 ***************************************************************************/
VOID
WIN_Order (
VOID
);

/***************************************************************************
   WIN_AskBool () - Askes USER a YES/NO Question????
 ***************************************************************************/
BOOL                       // RETURN: TRUE/FALSE
WIN_AskBool (
CHAR * question            // INPUT : pointer to message to ask
);

/***************************************************************************
WIN_AskExit () - Opens Windows and Askes if USer wants 2 quit
 ***************************************************************************/
VOID
WIN_AskExit (
VOID
);

/***************************************************************************
WIN_MainMenu () - Main Menu
 ***************************************************************************/
VOID
WIN_MainMenu (
VOID
);

/***************************************************************************
WIN_Register () - Register Window
 ***************************************************************************/
BOOL
RAP_Register (
VOID
);

/***************************************************************************
WIN_Locker () - Locker Room Window
 ***************************************************************************/
INT
RAP_Locker (
VOID
);

/***************************************************************************
WIN_Hangar() - Does the hanger dude
 ***************************************************************************/
INT
WIN_Hangar (
VOID
);

/***************************************************************************
WIN_ShipComp () - Does Game Selection 1, 2 or 3
 ***************************************************************************/
BOOL
WIN_ShipComp (
VOID
);

/***************************************************************************
WIN_LoadComp() - Shows computer loading screen
 ***************************************************************************/
VOID
WIN_LoadComp (
VOID
);

/***************************************************************************
WIN_SetLoadLevel() 
 ***************************************************************************/
VOID
WIN_SetLoadLevel (
INT level
);

/***************************************************************************
WIN_EndLoad () - Shows Ship computer while loading level
 ***************************************************************************/
VOID
WIN_EndLoad (
VOID
);

/***************************************************************************
WIN_MainLoop() - Handles Locker/Register/Store/Hangar and starting game
 ***************************************************************************/
VOID
WIN_MainLoop (
VOID
);

extern DWORD id_pics[4];

typedef enum
{
   DEM_INTRO,   
   DEM_CREDITS,   
   DEM_DEMO1G1,   
   DEM_DEMO2G1,   
   DEM_DEMO3G1,   
   DEM_DEMO1G2,   
   DEM_DEMO2G2,   
   DEM_DEMO3G2,   
   DEM_DEMO1G3,   
   DEM_DEMO2G3,   
   DEM_DEMO3G3   
} DEMOTYPE;

extern INT opt_detail;

#endif




