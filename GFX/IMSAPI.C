/****************************************************************************
* IMSapi  - Input Managenment System
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
*
* CONTENTS: imsapi.c imsapi.h keys.h
*
* EXTERN MODULES - ptrapi, kbdapi, tsmapi
*
*---------------------------------------------------------------------------*/
#include <dos.h>
#include <string.h>
#include <conio.h>
  
#include "ptrapi.h"
#include "kbdapi.h"
#include "tsmapi.h"
#include "gfxapi.h"

/***************************************************************************
IMS_StartAck () - Starts up checking for a happening
 ***************************************************************************/
VOID
IMS_StartAck (
VOID
)
{
   KBD_Clear();

   mouse_b1_ack = FALSE;
   mouse_b2_ack = FALSE;
   mouse_b3_ack = FALSE;
   kbd_ack = FALSE;
}

/***************************************************************************
IMS_CheckAck () - Tells if somthing has happend since last IMS_StartAck
 ***************************************************************************/
BOOL
IMS_CheckAck (
VOID
)
{
   INT rval = FALSE;

   if ( mouse_b1_ack )
      rval = TRUE;

   if ( mouse_b2_ack )
      rval = TRUE;

   if ( kbd_ack )
      rval = TRUE;

   return ( rval );
}
  
/***************************************************************************
IMS_IsAck() - Returns TRUE if ptr button or key pressed
 ***************************************************************************/
BOOL
IMS_IsAck(
VOID
)
{
   BOOL ret_val = FALSE;
  
   if ( KBD_LASTSCAN )
   {
      KBD_LASTSCAN = FALSE;
      ret_val = TRUE;
   }
   else if ( PTR_B1 )
      ret_val = TRUE;
   else if ( PTR_B2 )
      ret_val = TRUE;
   else if ( PTR_B3 )
      ret_val = TRUE;
  
   return ( ret_val );
}
  
/***************************************************************************
IMS_WaitAck() - Waits for a pointer button or key press
 ***************************************************************************/
VOID
IMS_WaitAck (
VOID
)
{
   IMS_StartAck();

   for (;;)
   {
      if ( IMS_CheckAck() )
         break;
   }

   IMS_StartAck();
}
  
/***************************************************************************
IMS_WaitTimed() - Wait for aprox secs
 ***************************************************************************/
INT                        // RETURN: keypress (lastscan)
IMS_WaitTimed (
INT   secs                 // INPUT : seconds to wait
)
{
   volatile INT hold;
   volatile INT loop;
   INT rval;
  
   KBD_LASTSCAN = SC_NONE;
   rval = KBD_LASTSCAN;

   IMS_StartAck();

   while ( secs > 0 )
   {
      for ( loop = 0; loop < 55; loop++ )
      {
         hold = FRAME_COUNT;
         while ( FRAME_COUNT == hold );

         if ( IMS_CheckAck() )
         {
            rval = 1;
            goto end_func;
         }

      }
      secs--;
   }

end_func:
   loop = 100;
   while (IMS_IsAck())
      loop--;

   IMS_StartAck();

   return ( rval );
}


