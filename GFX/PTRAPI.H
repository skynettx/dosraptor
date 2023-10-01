/****************************************************************************
* PTRapi  - Cursor routines
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
*
* CONTENTS: ptrapi.c ptrapi_a.asm ptrapi.h
*
* EXTERN MODULES - GFX, TSM and MOUSE.LIB
*
*---------------------------------------------------------------------------*/
  
#ifndef _TYPES_H
#include <types.h>
#endif
  
#ifndef _PTRAPI
#define _PTRAPI

#include "dpmiapi.h"

typedef enum
{
   P_AUTO,
   P_MOUSE,
   P_JOYSTICK
}PTRTYPE;
  
#define JOY_X        ((volatile)cur_mx)
#define JOY_Y        ((volatile)cur_my)
#define PTR_X        ((volatile)cur_mx)
#define PTR_Y        ((volatile)cur_my)
#define PTR_B1       ((volatile)mouseb1)
#define PTR_B2       ((volatile)mouseb2)
#define PTR_B3       ((volatile)mouseb3)
#define PTR_TM1      ((volatile)joy2b1)
#define PTR_TM2      ((volatile)joy2b2)
#define PTR_PRESENT  (mousepresent + joypresent)
  
extern   BOOL     mouse_b1_ack    ;
extern   BOOL     mouse_b2_ack    ;
extern   BOOL     mouse_b3_ack    ;
extern   BOOL     drawcursor      ;
extern   BOOL     joypresent      ;
extern   BOOL     mousepresent    ;
extern   INT      joy2b1          ;
extern   INT      joy2b2          ;
extern   INT      mouseb1         ;
extern   INT      mouseb2         ;
extern   INT      mouseb3         ;
extern   INT      cursorx         ;
extern   INT      cursory         ;
extern   INT      cur_mx          ;
extern   INT      cur_my          ;
extern   BOOL     ptr_init_flag   ;
  
/*========================================================================
  PTR_UpdateCursor() - Updates Mouse Cursor - should be called by intterupt
  ========================================================================*/
SPECIAL INT
PTR_UpdateCursor (
VOID
);
  
/*==========================================================================
  PTR_FrameHook() - Mouse framehook Function
 ==========================================================================*/
SPECIAL VOID
PTR_FrameHook (
VOID (*update)(VOID)        // INPUT : pointer to function
);

/*************************************************************************
PTR_JoyReset() - 
 *************************************************************************/
VOID
PTR_JoyReset (
VOID
);

/*************************************************************************
  PTR_JoyHandler () - Handles Joystick Input
 *************************************************************************/
VOID
PTR_JoyHandler (
VOID
);
  
/***************************************************************************
   PTR_SetPic () - Sets up a new cursor picture with hotspot
 ***************************************************************************/
VOID
PTR_SetPic (
BYTE * newp                // INPUT : pointer to new Cursor picture
);
  
/***************************************************************************
 PTR_SetBoundsHook() - Sets User function to OK or change mouse x,y values
 ***************************************************************************/
VOID                       // RETURN: none
PTR_SetBoundsHook (
VOID (*func)(VOID)         // INPUT : pointer to function
);

/***************************************************************************
 PTR_SetCursorHook() - Sets User function to call from mouse handler
 ***************************************************************************/
VOID                       // RETURN: none
PTR_SetCursorHook (
VOID (*hook)(VOID)         // INPUT : pointer to function
);
  
/***************************************************************************
   PTR_SetUpdateFlag () - Sets cursor to be update next cycle
 ***************************************************************************/
VOID
PTR_SetUpdateFlag (
VOID
);
  
/***************************************************************************
   PTR_DrawCursor () - Turns Cursor Drawing to ON/OFF ( TRUE/FALSE )
 ***************************************************************************/
VOID
PTR_DrawCursor (
BOOL  flag                 // INPUT: TRUE/FALSE
);
  
/***************************************************************************
 PTR_SetPos() - Sets Cursor Position
 ***************************************************************************/
VOID                       // RETURN: none
PTR_SetPos(
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
);

/***************************************************************************
PTR_Pause() - Pauses/ Starts PTR routines after already initing
 ***************************************************************************/
VOID
PTR_Pause (
BOOL flag                  // INPUT : TRUE / FALSE
);

/***************************************************************************
 PTR_Init() - Inits Mouse Driver and sets mouse handler function
 ***************************************************************************/
BOOL                          // RETURN true = Installed, false  = No mouse
PTR_Init (
PTRTYPE type                  // INPUT : Pointer Type to Use
);
  
/***************************************************************************
 PTR_End() - End Cursor system
 ***************************************************************************/
VOID
PTR_End (
VOID
);
  
/*==========================================================================
; PTR_Save() - Saves screen before a cursor draw
 ==========================================================================*/
#pragma aux PTR_Save "_*";
VOID
PTR_Save (
VOID
);

/*==========================================================================
; PTR_ClipSave() - Saves screen before a cursor draw
 ==========================================================================*/
#pragma aux PTR_ClipSave "_*";
VOID
PTR_ClipSave (
VOID
);
  
/*==========================================================================
; PTR_Erase() - Erases cursor and clips edges of screen
 ==========================================================================*/
#pragma aux PTR_Erase "_*";
VOID
PTR_Erase (
VOID
);
  
/*==========================================================================
; PTR_ClipErase() - Erases cursor and clips edges of screen
 ==========================================================================*/
#pragma aux PTR_ClipErase "_*";
VOID
PTR_ClipErase (
VOID
);
  
/*==========================================================================
; PTR_Draw() - Draws Cursor
 ==========================================================================*/
#pragma aux PTR_Draw "_*";
VOID
PTR_Draw (
VOID
);
  

/*==========================================================================
;PTR_ReadJoyStick() - Reads JoyStick Values        
 ==========================================================================*/
#pragma aux PTR_ReadJoyStick "_*";
VOID  
PTR_ReadJoyStick (
VOID
);

#endif
