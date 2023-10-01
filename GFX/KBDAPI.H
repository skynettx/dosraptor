/****************************************************************************
* KBDapi  - keyboard routines
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
*
* CONTENTS: kbdapi.c kbdapi.h keys.h
*
* EXTERN MODULES - NONE
*
*---------------------------------------------------------------------------*/
  
#ifndef _TYPES_H
#include <types.h>
#endif
  
#ifndef SC_NONE
#include "keys.h"
#endif
  
#ifndef _KBDAPI
#define _KBDAPI
  
/***************************************************************************
   KBD_Clear() - Resets all flags
 ***************************************************************************/
VOID
KBD_Clear (
VOID
);

/***************************************************************************
 KBD_SetKeyboardHook() - Sets User function to call from keyboard handler
 ***************************************************************************/
VOID                       // RETURN: none
KBD_SetKeyboardHook (
VOID (*hook)(VOID)         // INPUT : pointer to function
);
  
/***************************************************************************
   KBD_Ascii2Scan () - converts most ASCII chars to keyboard scan code
 ***************************************************************************/
INT                           // RETURN: scan code
KBD_Ascii2Scan (
INT ascii                  // INPUT : ASCII character
);
  
/***************************************************************************
KBD_Wait() - Waits for Key to be released
 ***************************************************************************/
VOID
KBD_Wait (
INT scancode               // SCANCODE see keys.h
);

/***************************************************************************
KBD_IsKey() - Tests to see if key is down if so waits for release
 ***************************************************************************/
BOOL
KBD_IsKey (
INT scancode               // SCANCODE see keys.h
);

/***************************************************************************
   KBD_Install() - Sets up keyboard system
 ***************************************************************************/
void
KBD_Install (
VOID
);
  
/***************************************************************************
   KBD_End() - Shuts down KBD system
 ***************************************************************************/
VOID
KBD_End (
VOID
);
  
/***************************************************************************
   KBD_Key() - returns true/false for scancode
 ***************************************************************************/
MACRO VOID
KBD_Key (
INT scancode               // INPUT : scan code to read
);
  
extern BOOL paused, capslock, kbd_ack;
extern BOOL keyboard[256];
extern INT  lastscan;
extern INT  lastascii;
  
#define KBD_Key(c)    keyboard[c]
#define KBD_ISPAUSED  paused
#define KBD_ISCAPS    (capslock+keyboard[SC_LEFT_SHIFT]+keyboard[SC_RIGHT_SHIFT])
#define KBD_LASTSCAN  lastscan
#define KBD_LASTASCII lastascii
  
#endif
