/****************************************************************************
* SWDapi  - windowing routines
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
#include "types.h"
#endif
  
  
typedef enum {
   C_IDLE,
   F_DOWN,
   F_UP,
   F_NEXT,
   F_PREV,
   F_RIGHT,
   F_LEFT,
   F_TOP,
   F_BOTTOM,
   F_FIRST,
   F_SELECT,
   F_OBJ_AREA,
   F_VIEW_AREA,
   W_NEW,
   W_NEXT,
   W_MOVE,
   W_CLOSE,
   W_CLOSE_ALL
} CMD;
  
typedef enum {
   S_IDLE,
   S_FLD_COMMAND,
   S_WIN_COMMAND,
   S_FIRSTCALL,
   S_REDRAW,
   S_UPDATE,
   S_END
} ACT;
  
typedef enum {
   R_IDLE,
   R_CLOSED,
   R_CLOSE_ALL,
   R_END_DIALOG
} FCMD;
  
typedef enum {
   FILL,
   TEXTURE,
   PICTURE,
   SEE_THRU,
   INVISABLE
} DSTYLE;
  
typedef enum { NORMAL, UP, DOWN }               BUTTON;
typedef enum { I_NORM, I_TOUPPER, I_NUMERIC }   INPUTOPT;
  
#define FLD_OFF                   0   // 0
#define FLD_TEXT                  1   // 1
#define FLD_BUTTON                2   // 2 *
#define FLD_INPUT                 3   // 3 *
#define FLD_MARK                  4   // 5 *
#define FLD_CLOSE                 5   // 6
#define FLD_DRAGBAR               6   // 7 *
#define FLD_BUMPIN                7   // 8
#define FLD_BUMPOUT               8   // 9
#define FLD_ICON                  9   // 4
#define FLD_OBJAREA               10  // 10
#define FLD_VIEWAREA              11  // 11
  
#define SWD_FILENUM 1
  
// basic format of a window data object
//
//  SWIN structure
//  SFIELDS..
//  TEXT AREA for window
//
  
#define MAX_WINDOWS 12
#define MAX_FONTS 2
  
extern INT  active_field;
extern BOOL g_button_flag;
extern BOOL usekb_flag;

void SWD_SetButtonFlag ( BOOL );
#define SWD_SetButtonFlag(c)    ( g_button_flag = c )

typedef struct
{
   INT      opt            ;  // FIELD TYPE
   INT      id             ;  // FIELD ID
   DWORD    hotkey         ;  // SCAN CODE OF HOT KEY
   BOOL     kbflag         ;  // TRUE if field should be KBACTIVE
   DWORD    opt3           ;  // not used
   DWORD    opt4           ;  // not used
   INPUTOPT input_opt      ;  // OPTIONS used in INPUT FIELDS
   BUTTON   bstatus        ;  // BUTTON STATUS NORMAL/UP/DOWN
   CHAR     name[16]       ;  // TEXT NAME OF FIELD ( NOT DISPLAYED )
   CHAR     item_name[16]  ;  // TEXT NAME OF ITEM #
   DWORD    item           ;  // ITEM ID NUMBER
   CHAR     font_name[16]  ;  // FONT .GLB NAME
   DWORD    fontid         ;  // FONT NUMBER
   INT      fontbasecolor  ;  // FONT BASE COLOR
   INT      maxchars       ;  // MAXCHARS IN FIELD TEXT
   DSTYLE   picflag        ;  // PICTURE TRUE/FALSE
   INT      color          ;  // COLOR OF FIELD
   INT      lite           ;  // HIGHLIGHT COLOR
   BOOL     mark           ;  // FIELD MARK ( TRUE/FLASE )
   BOOL     saveflag       ;  // MEM TO SAVE PIC UNDER FIELD ( Y/N )
   BOOL     shadow         ;  // SHADOW ON/OFF
   BOOL     selectable     ;  // SELECTABLE ON/OFF
   INT      x              ;  // X POSITION ON SCREEN
   INT      y              ;  // Y POSITION ON SCREEN
   INT      lx             ;  // WIDTH IN PIXELS
   INT      ly             ;  // HEIGHT IN PIXELS
   DWORD    txtoff         ;  // OFFSET TO TEXT DATA ( BYTES )
   BYTE  *  sptr           ;  // SEG POINTER TO SAVE INFO
} SFIELD;
  
typedef struct
{
   INT      version        ;  // VERSION NUMBER
   INT      swdsize        ;  // SIZE OF WIN/FIELD AND TEXT
   BOOL     arrowflag      ;  // Use Arrow Keys ( TRUE/FALSE )
   BOOL     display        ;  // DISPLAY FLAG
   DWORD    opt3           ;  // OPTION 3 ( not used )
   DWORD    opt4           ;  // OPTION 4 ( not used )
   INT      id             ;  // WINDOW ID NUMBER
   INT      type           ;  // WINDOW TYPE NUMBER
   CHAR     name[16]       ;  // TEXT NAME OF WINDOW ( NOT DISPLAYED )
   CHAR     item_name[16]  ;  // TEXT NAME OF ITEM
   DWORD    item           ;  // ITEM ID NUMBER
   DSTYLE   picflag        ;  // FILL/TEXTURE/PICTURE
   BOOL     lock           ;  // TRUE = cannot goto other windows
   INT      fldofs         ;  // OFFSET IN BYTES TO FIRST FIELD
   INT      txtofs         ;  // OFFSET IN BYTES TO TEXT AREA
   INT      firstfld       ;  // FIELD TO GOTO FIRST
   INT      opt            ;  // WINDOW TYPE
   INT      color          ;  // COLOR OF WINDOW
   INT      numflds        ;  // NUMBER OF FIELDS
   INT      x              ;  // X POSITON ON SCREEN
   INT      y              ;  // Y POSITION ON SCREEN
   INT      lx             ;  // WIDTH IN PIXELS
   INT      ly             ;  // HEIGHT IN PIXELS
   BOOL     shadow         ;  // SHADOW TRUE/FALSE
} SWIN;
  
typedef struct
{
   DWORD          gitem    ;  // GLB ITEM ID
   BOOL           flag     ;  // TRUE = in use ,FALSE = not in use
   BOOL           viewflag ;  // TRUE = has viewarea(s) FALSE = none
   SWIN  *        win      ;  // POINTER TO WINDOW
} SWD_WIN;
  
typedef struct
{
   INT window              ;
   INT field               ;
   ACT cur_act             ;
   CMD cur_cmd             ;
   INT keypress            ;
   INT id                  ;
   INT type                ;
   INT x                   ;
   INT y                   ;
   INT height              ;
   INT width               ;
   BOOL objactive          ;
   BOOL viewactive         ;
   INT sfield              ;
   INT sx                  ;
   INT sy                  ;
   INT sheight             ;
   INT swidth              ;
} SWD_DLG;
  
/***************************************************************************
 SWD_Install() - Initializes Window system
 ***************************************************************************/
VOID
SWD_Install (
BOOL  moveflag             // INPUT : Use Move Window feature ( 64k )
);
  
/***************************************************************************
   SWD_End () Frees up resources used by SWD System
 ***************************************************************************/
VOID
SWD_End (
VOID
);
  
/***************************************************************************
SWD_FillText () - Fills Text from GLB intro an AREA
 ***************************************************************************/
SWD_FillText (
FONT * font,               // INPUT : pointer to FONT
DWORD item,                // INPUT : GLB text Item
INT color,                 // INPUT : field color
INT x,                     // INPUT : x position
INT y,                     // INPUT : y position
INT lx,                    // INPUT : width of field
INT ly                     // INPUT : height of field
);
  
/***************************************************************************
 SWD_InitWindow() - Adds window to list and initailizes
 ***************************************************************************/
INT                          // RETURN: handle to window
SWD_InitWindow (
DWORD   handle             // INPUT : GLB Item Number
);
  
/***************************************************************************
   SWD_InitMasterWindow () - Inits the Master Window ( must be full screen )
 ***************************************************************************/
INT
SWD_InitMasterWindow (
DWORD   handle             // INPUT : GLB Item Number
);
  
/***************************************************************************
   SWD_SetWinDrawFunc () - Function called after window is drawn
 ***************************************************************************/
VOID
SWD_SetWinDrawFunc (
INT handle,                // INPUT :handle of window
VOID (*infunc)(SWD_DLG *)  // INPUT :pointer to function
);
 
/***************************************************************************
SWD_SetClearFlag() - Turns ON/OFF memsetting of display buffer in showallwins
 ***************************************************************************/
VOID
SWD_SetClearFlag (
BOOL inflag
);
 
/***************************************************************************
 SWD_ShowAllWindows()- Diplays all windows.. puts active window on top
 ***************************************************************************/
BOOL                          // RETURN : TRUE = OK, FALSE = Error
SWD_ShowAllWindows (
VOID
);
  
/***************************************************************************
   SWD_SetViewDrawHook () Sets Function to draw after the master window
 ***************************************************************************/
VOID
SWD_SetViewDrawHook (
VOID (*func)( VOID )        // INPUT : pointer to function
);
  
/***************************************************************************
SWD_SetWindowPtr() - Sets Pointer to center of active field
 ***************************************************************************/
VOID
SWD_SetWindowPtr (
INT handle                 // INPUT : number/handle of window
);

/***************************************************************************
SWD_SetFieldPtr () - Sets Pointer on a field
 ***************************************************************************/
VOID
SWD_SetFieldPtr (
INT handle,                // INPUT : number/handle of window
INT field                  // INPUT : field
);

/***************************************************************************
 SWD_SetActiveWindow() - Sets the current working window
 ***************************************************************************/
VOID
SWD_SetActiveWindow (
INT handle                 // INPUT : number/handle of window
);
  
/***************************************************************************
 SWD_SetActiveField() - Sets the current working field
 ***************************************************************************/
VOID
SWD_SetActiveField (
INT handle,                // INPUT : handle of window
INT field_id               // INPUT : number/handle of field
);
  
/***************************************************************************
 SWD_DestroyWindow() - removes a window from SWD system
 ***************************************************************************/
VOID
SWD_DestroyWindow (
INT handle                 // INPUT : handle of window
);
  
/***************************************************************************
   SWD_Dialog () - performs all window in/out/diaplay/move stuff
 ***************************************************************************/
VOID
SWD_Dialog (
SWD_DLG * swd_dlg          // OUTPUT: pointer to info structure
);
  
/***************************************************************************
   SWD_SetWindowLock() - Locks Window so no others can be selected
 ***************************************************************************/
VOID
SWD_SetWindowLock(
INT handle,                // INPUT : handle to window
BOOL lock                  // INPUT : TRUE/FALSE
);
  
/***************************************************************************
 SWD_SetWindowXY() - Sets the window x,y position
 ***************************************************************************/
INT                           // RETURN: window opt flag
SWD_SetWindowXY (
INT handle,                // INPUT : handle to window
INT xpos,                  // INPUT : x position
INT ypos                   // INPUT : y position
);
  
/***************************************************************************
 SWD_GetWindowXYL () - gets the window x,y ,x length, y length
 ***************************************************************************/
INT                           // RETURN: window opt flag
SWD_GetWindowXYL (
INT handle,                // INPUT : handle to window
INT *xpos,                 // OUTPUT: x position
INT *ypos,                 // OUTPUT: y position
INT *lx,                   // OUTPUT: x length
INT *ly                    // OUTPUT: y length
);
  
/***************************************************************************
 SWD_GetFieldText() - Gets the field text
 ***************************************************************************/
INT                           // RETURN: text max length
SWD_GetFieldText (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
CHAR * out_text            // OUTPUT: text
);
  
/***************************************************************************
 SWD_SetFieldText() - Sets The default field text
 ***************************************************************************/
INT                           // RETURN: text max length
SWD_SetFieldText (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
CHAR * in_text             // OUTPUT: pointer to string
);
  
/***************************************************************************
   SWD_GetFieldValue () Returns INT value of field text string
 ***************************************************************************/
INT
SWD_GetFieldValue (
INT handle,                // INPUT : window handle
INT field_id               // INPUT : field handle
);
  
/***************************************************************************
   SWD_SetFieldValue () Sets Numeric (INT) Value into Field Text
 ***************************************************************************/
INT
SWD_SetFieldValue (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
INT num                    // INPUT : number to set in fld text
);
  
/***************************************************************************
SWD_SetFieldSelect() - Sets Field Selectable status
 ***************************************************************************/
VOID
SWD_SetFieldSelect (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
BOOL opt                   // INPUT : TRUE, FALSE
);

/***************************************************************************
 SWD_GetFieldMark() - Gets the field mark status ( TRUE or FALSE )
 ***************************************************************************/
BOOL                          // RETURN: mark status ( TRUE, FALSE )
SWD_GetFieldMark (
INT handle,                // INPUT : window handle
INT field_id               // INPUT : field handle
);
  
/***************************************************************************
 SWD_GetFieldInputOpt() - Gets the field InputOpt status
 ***************************************************************************/
INPUTOPT                      // RETURN: InputOpt status
SWD_GetFieldInputOpt (
INT handle,                // INPUT : window handle
INT field_id               // INPUT : field handle
);
  
/***************************************************************************
 SWD_SetFieldInputOpt() - Sets the Field InputOpt ( button )
 ***************************************************************************/
INPUTOPT                      // RETURN: InputOpt status
SWD_SetFieldInputOpt (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
INPUTOPT opt               // INPUT : input option
);
  
/***************************************************************************
   SWD_SetFieldItem () - Sets field Item ID ( picture )
 ***************************************************************************/
VOID
SWD_SetFieldItem (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
DWORD item                 // INPUT : GLB item id
);
  
/***************************************************************************
   SWD_GetFieldItem () - Returns Field Item number
 ***************************************************************************/
DWORD                         // RETURN: Item GLB ID
SWD_GetFieldItem (
INT handle,                // INPUT : window handle
INT field_id               // INPUT : field handle
);
  
/***************************************************************************
   SWD_SetFieldItemName () - Sets Field Item Name and Loads it in
 ***************************************************************************/
VOID
SWD_SetFieldItemName (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
CHAR * item_name           // INPUT : pointer to Item Name
);
  
/***************************************************************************
   SWD_GetFieldItemName () - Gets Field Item Name
 ***************************************************************************/
VOID
SWD_GetFieldItemName (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
CHAR * item_name           // OUTPUT: pointer to Item Name
);
  
/***************************************************************************
 SWD_SetFieldMark() - Sets the Field Mark ( button )
 ***************************************************************************/
VOID
SWD_SetFieldMark (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
BOOL opt                   // INPUT : TRUE, FALSE
);
  
/***************************************************************************
   SWD_SetWindowID () - Sets Window ID number
 ***************************************************************************/
INT                           // RETURN: old Window ID
SWD_SetWindowID (
INT handle,                // INPUT : window handle
INT id                     // INPUT : NEW window ID
);
  
/***************************************************************************
   SWD_GetWindowID () - Returns Window ID number
 ***************************************************************************/
INT
SWD_GetWindowID (
INT handle                 // INPUT : window handle
);
  
/***************************************************************************
SWD_SetWindowFlag () - Sets A window to be turned on/off
 ***************************************************************************/
INT
SWD_SetWindowFlag (
INT  handle,                // INPUT : window handle
BOOL flag                   // INPUT : TRUE/FALSE
);
  
/***************************************************************************
   SWD_SetWindowType () Sets Window TYPE number
 ***************************************************************************/
INT                           // RETURN: old Window TYPE
SWD_SetWindowType (
INT handle,                // INPUT : window handle
INT type                   // INPUT : NEW window TYPE
);
  
/***************************************************************************
   SWD_GetWindowType () - Returns Window TYPE number
 ***************************************************************************/
INT                           // RETURN: window TYPE
SWD_GetWindowType (
INT handle                 // INPUT : window handle
);
  
/***************************************************************************
   SWD_GetFieldXYL () Gets Field X,Y, WIDTH, HEIGHT
 ***************************************************************************/
INT
SWD_GetFieldXYL (
INT handle,                // INPUT : window handle
INT field_id,              // INPUT : field handle
INT * x,                   // OUTPUT: x
INT * y,                   // OUTPUT: y
INT * lx,                  // OUTPUT: width
INT * ly                   // OUTPUT: height
);
