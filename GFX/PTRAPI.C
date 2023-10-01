/****************************************************************************
* PTRapi  - Cursor routines
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
* Modifed:     Mar, 1993 ( made work with Watcom C )
*
* CONTENTS: ptrapi.c ptrapi_a.asm ptrapi.h
*
* EXTERN MODULES - GFX, TSM(timer) and MOUSE.LIB
*
* NOTES:
*        PTR_UpdateCursor should be called by an intr. routine
*        PTR_FrameHook should not be called in an intr. routine
*
*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <conio.h>
  
#include "gfxapi.h"
#include "ptrapi.h"
#include "exitapi.h"
#include "tsmapi.h"
#include "dpmiapi.h"
#include "kbdapi.h"

#define JOYADD        8  
#define CURSORHEIGHT  16
#define CURSORWIDTH   16
#define CURSORSIZE    (CURSORHEIGHT*CURSORWIDTH)
#define HOTSPOTCOLOR  255
  
extern INT     ud_x;               // update x pos
extern INT     ud_y;               // update y pos
extern INT     ud_lx;              // update x length
extern INT     ud_ly;              // update y length
  
#define MOUSE_MV  0x01  // Mouse movement occurred
#define LB_PRESS  0x02  // Left button pressed
#define LB_OFF    0x04  // Left button released
#define RB_PRESS  0x08  // Right button pressed
#define RB_OFF    0x10  // Right button released
#define CB_OFF    0x20  // Center button released
#define CB_PRESS  0x40  // Center button pressed

PUBLIC BOOL    mouse_b1_ack   = FALSE;
PUBLIC BOOL    mouse_b2_ack   = FALSE;
PUBLIC BOOL    mouse_b3_ack   = FALSE;
PUBLIC BYTE *  cursorstart;
PUBLIC BYTE *  displaypic;
PUBLIC BYTE *  cursorpic;
PUBLIC BYTE *  cursorsave;
PUBLIC BOOL    mousepresent   = FALSE;
PUBLIC BOOL    joypresent     = FALSE;
PUBLIC BOOL    joyactive      = FALSE;
PUBLIC INT     joy2b1         = 0;
PUBLIC INT     joy2b2         = 0;
PUBLIC INT     mouseb1        = 0;
PUBLIC INT     mouseb2        = 0;
PUBLIC INT     mouseb3        = 0;
PUBLIC INT     cursorx        = 0;
PUBLIC INT     cursory        = 0;
PUBLIC INT     cursorloopx    = 0;
PUBLIC INT     cursorloopy    = 0;
PUBLIC INT     cur_mx         = 0;
PUBLIC INT     cur_my         = 0;
PRIVATE INT    dm_x           = 0;
PRIVATE INT    dm_y           = 0;
PRIVATE INT    hot_mx         = 0;
PRIVATE INT    hot_my         = 0;
PRIVATE BOOL   mouseonhold    = FALSE;
PRIVATE BOOL   mouseaction    = TRUE;
PRIVATE BOOL   mouse_erase    = FALSE;
PRIVATE BOOL   not_in_update  = TRUE;
PRIVATE VOID   (*cursorhook)(VOID)           = (VOID (*))0;
PRIVATE VOID   (*checkbounds)(VOID)          = (VOID (*))0;

PUBLIC  BOOL   drawcursor     = FALSE;
PRIVATE BOOL   g_paused       = FALSE;  
PRIVATE BOOL   lastclip       = FALSE;
PRIVATE DWORD  tsm_id         = EMPTY;
PUBLIC  INT    joy_limit_xh   = 10;
PUBLIC  INT    joy_limit_xl   = -10;
PUBLIC  INT    joy_limit_yh   = 10;
PUBLIC  INT    joy_limit_yl   = -10;
PUBLIC  INT    joy_sx         = 0;
PUBLIC  INT    joy_sy         = 0;
PRIVATE BOOL   joy_present    = FALSE;
PUBLIC  INT    joy_x          = 0;
PUBLIC  INT    joy_y          = 0;
PUBLIC  INT    joy_buttons    = 0;
PUBLIC  BOOL   ptr_init_flag  = FALSE;
PRIVATE INT    g_addx = 0;
PRIVATE INT    g_addy = 0;


/*------------------------------------------------------------------------
PTR_IsJoyPresent() - Checks to see if joystick is present
  ------------------------------------------------------------------------*/
BOOL
PTR_IsJoyPresent(
VOID
)
{
   INT loop;
   INT rval = TRUE;

   _disable();
   outp ( 0x201, 1 );
   inp  ( 0x201 );
   for ( loop = 0; loop <= 10000; loop++ )
   {
      if ( ( inp (0x201) & 0x0f ) == 0 )
         break;
   }
   _enable();

   if ( loop >= 10000 )
      rval = FALSE;

   return ( rval );
}

/*------------------------------------------------------------------------
   PTR_MouseHandler() - Mouse Handler Function
  ------------------------------------------------------------------------*/
VOID _loadds far  
PTR_MouseHandler(
INT m_bx,
INT m_cx,
INT m_dx
)
{
#pragma aux PTR_MouseHandler parm [EBX] [ECX] [EDX]
  
   if ( not_in_update )
   {
      cur_mx = m_cx&0xffff;
      cur_mx = cur_mx>>1;
      cur_my = m_dx;
      mouseb1 = m_bx & 1;
      mouseb2 = m_bx & 2;
      mouseb3 = m_bx & 4;
      mouseaction = TRUE;
      if ( mouseb1 )
         mouse_b1_ack = TRUE;
      if ( mouseb2 )
         mouse_b2_ack = TRUE;
      if ( mouseb3 )
         mouse_b3_ack = TRUE;
   }
}

VOID
PTR_JoyReset(
    VOID
)
{
   if (joyactive)
   {
      g_addx = 0;
      g_addy = 0;
   }
}

/*------------------------------------------------------------------------
  PTR_JoyHandler () - Handles Joystick Input
  ------------------------------------------------------------------------*/
TSMCALL VOID
PTR_JoyHandler (
VOID
)
{
   INT xm;
   INT ym;

   if ( !joyactive )
      return;

   _disable();
   PTR_ReadJoyStick();
   _enable();

   mouseb1 = joy_buttons&1;
   mouseb2 = joy_buttons&2;
   joy2b1  = joy_buttons&4;
   joy2b2  = joy_buttons&8;

   if ( mouseb1 )
      mouse_b1_ack = TRUE;

   if ( mouseb2 )
      mouse_b2_ack = TRUE;

   xm = joy_x - joy_sx;
   ym = joy_y - joy_sy;

   if ( xm < joy_limit_xl || xm > joy_limit_xh )
   {
      if ( xm < 0 )
      {
         if ( g_addx > 0 )
            g_addx = 0;
         else
            g_addx--;

         if ( -g_addx > JOYADD )
            g_addx = -JOYADD;
      }
      else
      {
         if ( g_addx < 0 )
            g_addx = 0;
         else
            g_addx++;
         if ( g_addx > JOYADD )
            g_addx = JOYADD;
      }
   }
   else
   {
      g_addx = 0;
   }

   if ( ym < joy_limit_yl || ym > joy_limit_yh )
   {
      if ( ym < 0 )
      {
         if ( g_addy > 0 )
            g_addy = 0;
         else
            g_addy--;
         if ( -g_addy > JOYADD )
            g_addy = -JOYADD;
      }
      else
      {
         if ( g_addy < 0 )
            g_addy = 0;
         else
            g_addy++;
         if ( g_addy > JOYADD )
            g_addy = JOYADD;
      }
   }
   else
   {
      g_addy = 0;
   }

   cur_mx += g_addx;
   cur_my += g_addy;

   if ( cur_mx < 0 )
      cur_mx = 0;
   else if ( cur_mx > 319 )
      cur_mx = 319;

   if ( cur_my < 0 )
      cur_my = 0;
   else if ( cur_my > 199 )
      cur_my = 199;

   mouseaction = TRUE;
}

/*------------------------------------------------------------------------
   PTR_ClipCursor () Clips cursor from screen
  ------------------------------------------------------------------------*/
PRIVATE VOID
PTR_ClipCursor (
VOID
)
{
   lastclip = FALSE;
  
   displaypic = cursorpic;

   if ( ( dm_x + CURSORWIDTH ) > SCREENWIDTH )
   {
      cursorloopx = SCREENWIDTH - dm_x;
      lastclip = TRUE;
   }
   else
   {
      if ( dm_x < 0 )
      {
         displaypic -= dm_x;
         cursorloopx = CURSORWIDTH + dm_x;
         dm_x = 0;
         lastclip = TRUE;
      }
      else cursorloopx = CURSORWIDTH;
   }
  
   if ( ( dm_y + CURSORHEIGHT ) > SCREENHEIGHT )
   {
      cursorloopy = SCREENHEIGHT - dm_y;
      lastclip = TRUE;
   }
   else
   {
      if ( dm_y < 0 )
      {
         displaypic += -dm_y * CURSORWIDTH;
         cursorloopy = CURSORHEIGHT + dm_y;
         dm_y = 0;
         lastclip = TRUE;
      }
      else cursorloopy = CURSORHEIGHT;
   }
}
  
/*========================================================================
  PTR_UpdateCursor() - Updates Mouse Cursor - should be called by intterupt
  ========================================================================*/
TSMCALL INT
PTR_UpdateCursor (
VOID
)
{
   if ( mouseonhold )
      return ( 0 );

   if ( joyactive )
   {
      PTR_JoyHandler();
   }

   if ( mouseaction )
   {
      not_in_update = FALSE;
      mouseaction = FALSE;
  
      if ( mouse_erase )
      {
         if ( lastclip ) PTR_ClipErase();
         else PTR_Erase();
  
         mouse_erase = FALSE;
      }
  
      if ( checkbounds )
         checkbounds();

      dm_x = cur_mx;
      dm_y = cur_my;
  
      dm_x -= hot_mx;
      dm_y -= hot_my;
  
      if ( drawcursor )
      {
         PTR_ClipCursor ();
  
         cursorstart = displayscreen + dm_x + ylookup[dm_y] ;
  
         PTR_Save();
         PTR_Draw();
  
         mouse_erase = TRUE;
      }
  
      if ( cursorhook )
         cursorhook();
  
      cursorx = dm_x;
      cursory = dm_y;
      not_in_update = TRUE;
   }

   return(0);
}
  
/*==========================================================================
  PTR_FrameHook() - Mouse framehook Function
 ==========================================================================*/
SPECIAL VOID
PTR_FrameHook (
VOID (*update)(VOID)        // INPUT : pointer to function
)
{
   INT ck_x1;
   INT ck_y1;
   INT ck_x2;
   INT ck_y2;
  
   if ( !drawcursor )
   {
      update();
      return;
   }
  
   while ( !(volatile)not_in_update );
   not_in_update = FALSE;
   mouseonhold = TRUE;
  
   if ( joyactive )
   {
      PTR_JoyHandler();
   }

   if ( checkbounds )
      checkbounds();
  
   dm_x = cur_mx - hot_mx;
   dm_y = cur_my - hot_my;
   not_in_update = TRUE;
  
   ck_x1 = ud_x - CURSORWIDTH;
   ck_y1 = ud_y - CURSORHEIGHT;
   ck_x2 = ud_x + ud_lx;
   ck_y2 = ud_y + ud_ly;
  
   if ( dm_x >= ck_x1 && dm_x <= ck_x2 &&
      dm_y >= ck_y1 && dm_y <= ck_y2 )
   {
      if ( mouse_erase )
      {
         if ( cursorx >= ck_x1 && cursorx <= ck_x2 &&
            cursory >= ck_y1 && cursory <= ck_y2  )
         {
            GFX_MarkUpdate ( cursorx, cursory, cursorloopx, cursorloopy );
         }
         else
         {
            if ( lastclip ) PTR_ClipErase();
            else PTR_Erase();
  
            mouse_erase = FALSE;
         }
      }
  
      PTR_ClipCursor ();
  
      GFX_MarkUpdate ( dm_x, dm_y, cursorloopx, cursorloopy );
  
      cursorstart = displaybuffer + dm_x + ylookup[ dm_y ];
  
      if ( cursorloopy < CURSORHEIGHT )
         PTR_ClipSave();
      else
         PTR_Save();

      PTR_Draw();
  
      update();
  
      if ( lastclip ) PTR_ClipErase();
      else PTR_Erase();
  
      cursorstart = displayscreen + dm_x + ylookup[ dm_y ];
      mouse_erase = TRUE;
   }
   else
   {
      if ( mouseaction )
      {
         if ( mouse_erase )
         {
            if ( lastclip ) PTR_ClipErase();
            else PTR_Erase();
  
            mouse_erase = FALSE;
         }
  
         PTR_ClipCursor ();
  
         cursorstart = displayscreen + dm_x + ylookup[ dm_y ];
  
         PTR_Save();
         PTR_Draw();
  
         mouse_erase = TRUE;
      }
      update();
   }
  
   if ( cursorhook )
      cursorhook();
  
   cursorx = dm_x;
   cursory = dm_y;
   mouseonhold = FALSE;
}

/***************************************************************************
PTR_CalJoy() - Calibrate Joystick
 ***************************************************************************/
VOID
PTR_CalJoy (
VOID
)
{
   _disable();
   PTR_ReadJoyStick();
   _enable();

   joy_sx = joy_x;
   joy_sy = joy_y;
}
  
/***************************************************************************
   PTR_DrawCursor () - Turns Cursor Drawing to ON/OFF ( TRUE/FALSE )
 ***************************************************************************/
VOID
PTR_DrawCursor (
BOOL  flag                 // INPUT: TRUE/FALSE
)
{
   if ( ptr_init_flag )
   {
      if ( flag == FALSE && mouse_erase == TRUE )
      {
         if ( lastclip ) PTR_ClipErase();
         else PTR_Erase();
         mouse_erase = FALSE;
      }

      if ( flag == TRUE )
         mouseaction = TRUE;
  
      drawcursor = flag;
   }
   else
      drawcursor = FALSE;

}
  
/***************************************************************************
   PTR_SetPic () - Sets up a new cursor picture with hotspot
 ***************************************************************************/
VOID
PTR_SetPic (
BYTE * newp                // INPUT : pointer to new Cursor picture
)
{
   BYTE * pic;
   INT loop;
  
   hot_mx = 0;
   hot_my = 0;

   if ( ptr_init_flag == FALSE ) return;

   newp += sizeof ( GFX_PIC );
  
   pic = ( BYTE * )cursorpic;
  
   for ( loop = 0; loop < CURSORSIZE; loop++, newp++, pic++ )
   {
      *pic = *newp;
      if ( *newp == (BYTE)HOTSPOTCOLOR )
      {
         hot_mx = loop%CURSORWIDTH;
         hot_my = loop/CURSORWIDTH;
         *pic = *( newp + 1 );
      }
   }
  
   if ( hot_mx > 16 )
      hot_mx = 0;

   if ( hot_my > 16 )
      hot_my = 0;

   mouseaction = TRUE;
}
  
/***************************************************************************
 PTR_SetBoundsHook() - Sets User function to OK or change mouse x,y values
 ***************************************************************************/
VOID                       // RETURN: none
PTR_SetBoundsHook (
VOID (*func)(VOID)         // INPUT : pointer to function
)
{
   checkbounds = func;
   mouseaction = TRUE;
}
  
/***************************************************************************
 PTR_SetCursorHook() - Sets User function to call from mouse handler
 ***************************************************************************/
VOID                       // RETURN: none
PTR_SetCursorHook (
VOID (*hook)(VOID)         // INPUT : pointer to function
)
{
   cursorhook = hook;
   mouseaction = TRUE;
}
  
/***************************************************************************
   PTR_SetUpdateFlag () - Sets cursor to be update next cycle
 ***************************************************************************/
VOID
PTR_SetUpdateFlag (
VOID
)
{
   mouseaction = TRUE;
}
  
/***************************************************************************
 PTR_SetPos() - Sets Cursor Position
 ***************************************************************************/
VOID                          // RETURN: none
PTR_SetPos(
INT x,                     // INPUT : x position
INT y                      // INPUT : y position
)
{
   union REGS   regs;
  
   if ( mousepresent )
   {
      // Set Mouse Position ================
      regs.x.eax = 4;
      regs.x.ecx = x<<1;
      regs.x.edx = y;
      int386(0x33,(const union REGS *) &regs, &regs);
   }
  
   cur_mx = x;
   cur_my = y;

   if ( ptr_init_flag )
      mouseaction = TRUE;
}

/***************************************************************************
PTR_Pause() - Pauses/ Starts PTR routines after already initing
 ***************************************************************************/
VOID
PTR_Pause (
BOOL flag                  // INPUT : TRUE / FALSE
)
{
   if ( ptr_init_flag == FALSE ) return;

   if ( flag == g_paused ) return;

   if ( flag )
   {
      PTR_DrawCursor ( FALSE );
      TSM_PauseService ( tsm_id );   
   }
   else
   {
      drawcursor = FALSE;
      TSM_ResumeService ( tsm_id );   
      PTR_SetPos ( 160, 100 );
      PTR_DrawCursor ( FALSE );
   }

   g_paused = flag;
}

/***************************************************************************
 PTR_Init() - Inits Mouse Driver and sets mouse handler function
 ***************************************************************************/
BOOL                          // RETURN true = Installed, false  = No mouse
PTR_Init (
PTRTYPE type                  // INPUT : Pointer Type to Use
)
{
   CHAR * err = "PTR_Init() - DosMemAlloc";
   struct SREGS sregs;
   union REGS   regs;
   int (far *function_ptr)();
   INT rval = 0;
   DWORD segment;

   drawcursor = FALSE;

   if ( _dpmi_dosalloc ( 16, &segment ) ) EXIT_Error(err);
   cursorsave = ( BYTE *)( segment<<4 );

   if ( _dpmi_dosalloc ( 16, &segment ) ) EXIT_Error(err);
   cursorpic  = ( BYTE *)( segment<<4 );
  
   joyactive = FALSE;
   mousepresent = FALSE;

   joy_present = FALSE;

   if ( type == P_JOYSTICK )
   {
      joy_present = TRUE;
   }

   segread( &sregs );
  
   if ( type == P_AUTO || type == P_MOUSE )
   {
      // Reset Mouse Driver ===================
      regs.w.ax = 0;
      int386(0x33, &regs, &regs);
  
      if ( regs.w.ax == -1 )
      {
         mousepresent = TRUE;

         // Hide Mouse ========================
         regs.w.ax = 2;
         int386(0x33, &regs, &regs);
 
         // Install Mouse Handler==============
         function_ptr = PTR_MouseHandler;
         regs.w.ax = 0x0C;
         regs.w.cx = LB_OFF | LB_PRESS | RB_OFF | RB_PRESS | MOUSE_MV;
         regs.x.edx = FP_OFF( function_ptr );
         sregs.es   = FP_SEG( function_ptr );
         int386x ( 0x33, &regs, &regs, &sregs );
      }
   }
  
   if ( type == P_JOYSTICK )
   {
      if ( joy_present )
      {
         joyactive = TRUE;
      }
   }

   if ( mousepresent || joyactive )
   {
      ptr_init_flag = TRUE;
	   tsm_id = TSM_NewService ( PTR_UpdateCursor, 15, 254, 0 );
      GFX_SetFrameHook ( PTR_FrameHook );
   }
   else
      tsm_id = -1;

   if ( joy_present )
   {
      PTR_CalJoy();
   }

   PTR_SetPos ( 160, 100 );

   if ( mousepresent || joyactive )
      return ( TRUE );
   else
      return ( FALSE );
}
  
/***************************************************************************
 PTR_End() - End Cursor system
 ***************************************************************************/
VOID
PTR_End (
VOID
)
{
   union REGS   regs;
  
   if ( tsm_id != EMPTY )
	   TSM_DelService( tsm_id );

   // reset to remove mouse handler ========
   regs.x.eax = 0x0;
   int386( 0x33, ( const union REGS * ) &regs, &regs );
  
}

