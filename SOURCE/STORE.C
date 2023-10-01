#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raptor.h"

#include "file0000.inc"
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"

#include "store.inc"

PRIVATE DWORD items [ S_LAST_OBJECT ] = {
      ITEM00_TXT,
      ITEM01_TXT,
      ITEM02_TXT,
      ITEM03_TXT,
      ITEM04_TXT,
      ITEM05_TXT,
      ITEM06_TXT,
      ITEM07_TXT,
      ITEM08_TXT,
      ITEM09_TXT,
      ITEM10_TXT,
      ITEM11_TXT,
      ITEM12_TXT,
      ITEM13_TXT,
      ITEM14_TXT,
      ITEM15_TXT,
      ITEM16_TXT,
      ITEM17_TXT,
      EMPTY,
      EMPTY,
      EMPTY,
      EMPTY,
      EMPTY 
      };

PRIVATE CHAR storetext [8][20] = {
   "      EXIT",                // 0
   "ACTIVATE SELL MODE",        // 1
   "ACTIVATE BUY MODE",         // 2
   "    NEXT ITEM",             // 3
   "  PREVIOUS ITEM",           // 4
   "  BUY AN ITEM",             // 5
   "  SELL AN ITEM",            // 6
   " HDE MAIN DISPLAY"          // 7
   };

#define BUY_MODE  0
#define SELL_MODE 1

PRIVATE INT mode        =  BUY_MODE;
PRIVATE INT cur_item    =  0;
PRIVATE INT                window;
PRIVATE INT                b_items [ S_LAST_OBJECT ];
PRIVATE INT                s_items [ S_LAST_OBJECT ];
PRIVATE INT                buy_count;
PRIVATE INT                sell_count;

PRIVATE DWORD              buybut[2]      = { BUYLGT_PIC, BUYDRK_PIC };
PRIVATE DWORD              sellbut[2]     = { SELLGT_PIC, SELLDRK_PIC };
PRIVATE DWORD              mainbut[2]     = { BUYITEM_PIC, SELLITEM_PIC };
PRIVATE CHAR               saying[2][9]   = { "COST","RESALE"};
PRIVATE CHAR               yh_hold[16];

extern   OBJ_LIB  obj_lib[];
extern   OBJ      p_objs[];

/*-------------------------------------------------------------------------*
MakeBuyItems () - Makes items you can Buy
 *-------------------------------------------------------------------------*/
PRIVATE INT
MakeBuyItems (
VOID
)
{
   INT   loop;
   INT   num;
   BOOL  flag;
   INT   cost1;
   INT   cost2;

   buy_count = 0;
   memset ( b_items, 0, sizeof ( b_items ) );

   for ( loop = 0; loop < S_ITEMBUY1; loop++ )
   {
      if ( OBJS_CanBuy ( loop ) )
      {
         b_items [ buy_count ] = loop;
         buy_count++;
      }
   }

   // SORT BY $ AMOUNT ============================

   if ( buy_count > 1 )
   {
      for ( ;; )
      {
         flag = FALSE;
         for ( loop = 0; loop < (buy_count-1); loop++ )
         {
            cost1 = OBJS_GetCost ( b_items [ loop ] );
            cost2 = OBJS_GetCost ( b_items [ loop + 1 ] );

            if ( cost1 > cost2 )
            {
               flag = TRUE;
               num = b_items [ loop ];
               b_items [ loop ] = b_items [ loop + 1 ];
               b_items [ loop + 1 ] = num;
            }
         }

         if ( !flag ) break;
      }
   }

   return ( buy_count );
}

/*-------------------------------------------------------------------------*
MakeSellItems() - Makes the items that you can sell
 *-------------------------------------------------------------------------*/
PRIVATE INT
MakeSellItems (
VOID
)
{
   INT   loop;
   INT   num;
   BOOL  flag;
   INT   cost1;
   INT   cost2;

   memset ( s_items, 0, sizeof ( s_items ) );

   sell_count = 0;
   for ( loop = 0; loop < S_LAST_OBJECT; loop++ )
   {
      if ( OBJS_CanSell ( loop ) )
      {
         s_items [ sell_count ] = loop;
         sell_count++;
      }
   }

   // SORT BY $ AMOUNT ============================

   if ( sell_count > 1 )
   {
      for ( ;; )
      {
         flag = FALSE;
         for ( loop = 0; loop < ( sell_count - 1 ); loop++ )
         {
            cost1 = OBJS_GetCost ( s_items [ loop ] );
            cost2 = OBJS_GetCost ( s_items [ loop + 1 ] );

            if ( cost1 > cost2 )
            {
               flag = TRUE;
               num = s_items [ loop ];
               s_items [ loop ] = s_items [ loop + 1 ];
               s_items [ loop + 1 ] = num;
            }
         }

         if ( !flag ) break;
      }
   }

   return ( sell_count );
}

/*-------------------------------------------------------------------------*
Harrold() - Lets Harrold do Some Talking
 *-------------------------------------------------------------------------*/
PRIVATE VOID
Harrold (
DWORD item                 // INPUT : GLB item of harrold text
)
{
   DWORD prev;
   DWORD next;
   DWORD buy;
   DWORD buyit;
   DWORD sell;

   prev  =  SWD_GetFieldItem ( window, STOR_PREV );
   next  =  SWD_GetFieldItem ( window, STOR_NEXT );
   buy   =  SWD_GetFieldItem ( window, STOR_BUY );
   buyit = SWD_GetFieldItem ( window, STOR_BUYIT );
   sell  = SWD_GetFieldItem ( window, STOR_SELL );

   SWD_SetFieldItem ( window, STOR_PREV, EMPTY );
   SWD_SetFieldItem ( window, STOR_NEXT, EMPTY );
   SWD_SetFieldItem ( window, STOR_BUY, EMPTY );
   SWD_SetFieldItem ( window, STOR_BUYIT, EMPTY );
   SWD_SetFieldItem ( window, STOR_SELL, EMPTY );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();

   KBD_Clear();

   SWD_SetFieldText ( window, STOR_STATS, NUL );
   SWD_SetFieldText ( window, STOR_TEXTCOST, NUL );
   SWD_SetFieldText ( window, STOR_NUM, NUL );
   SWD_SetFieldText ( window, STOR_COST, NUL );
   SWD_SetFieldItem ( window, STOR_COMP, item );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();

   IMS_WaitTimed ( 10 );

   SWD_SetFieldItem ( window, STOR_COMP, EMPTY );

   SWD_SetFieldItem ( window, STOR_PREV, prev );
   SWD_SetFieldItem ( window, STOR_NEXT, next );
   SWD_SetFieldItem ( window, STOR_BUY, buy );
   SWD_SetFieldItem ( window, STOR_BUYIT, buyit );
   SWD_SetFieldItem ( window, STOR_SELL, sell );
}

/***************************************************************************
STORE_Enter () - Lets User go in store and buy and sell things
 ***************************************************************************/
VOID
STORE_Enter (
VOID
)
{
   CHAR     youhave[52];
   CHAR     coststr[52];
   BOOL     update      = FALSE;
   INT      opt         = -1;
   INT      oldopt      = -1;
   INT      max_items   = 0;
   SWD_DLG  dlg;
   INT      obj_cnt;
   INT      pos;
   INT      num;
   INT      cost;
   INT      loop;

   PTR_DrawCursor ( FALSE );
   KBD_Clear();
   GFX_FadeOut ( 0, 0, 0, 5 );

   SWD_SetButtonFlag ( FALSE );
   window = SWD_InitMasterWindow ( STORE_SWD );
   SWD_SetFieldItem ( window, STOR_ID, id_pics [ plr.id_pic ] );
   SWD_SetFieldItem ( window, STOR_BUYIT, mainbut [mode] );
   SWD_GetFieldText ( window, STOR_STATS, yh_hold );
   SWD_SetFieldText ( window, STOR_STATS, NUL );
   SWD_SetFieldText ( window, STOR_CALLSIGN, plr.callsign );
   sprintf ( youhave, "%07d", plr.score );
   SWD_SetFieldText ( window, STOR_SCORE, youhave );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   GFX_FadeIn ( palette, 16 );

   SWD_SetFieldPtr ( window, STOR_VEXIT );
   PTR_DrawCursor ( TRUE );

   obj_cnt  = OBJS_GetNum();

   Harrold ( HAR1_TXT );

   cur_item    = 0;
   mode        = BUY_MODE;

   if ( mode == BUY_MODE )
   {
      SWD_SetFieldItem ( window, STOR_BUY, buybut [0] );
      SWD_SetFieldItem ( window, STOR_SELL, sellbut [1] );
   }
   else
   {
      SWD_SetFieldItem ( window, STOR_BUY, buybut [1] );
      SWD_SetFieldItem ( window, STOR_SELL, sellbut [0] );
   }

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   update = TRUE;

   mainloop:

   MakeSellItems();
   MakeBuyItems();
  
   if ( mode == BUY_MODE )
      max_items = buy_count;
   else
      max_items = sell_count;

   if ( max_items < 1 )
   {
      update = TRUE;

      if ( mode == BUY_MODE )
      {
         EXIT_Error("STORE Error ( BUY_MODE )");
      }
      else
      {
         Harrold ( HAR5_TXT );
         mode        = BUY_MODE;
         max_items   = MakeBuyItems();
         if ( max_items < 1 )
            EXIT_Error("STORE THING 2");
      }
   }

   if ( update )
   {
      update = FALSE;

      if ( cur_item < 0 )
         cur_item = max_items - 1;

      if ( cur_item >= max_items )
         cur_item = 0;

      if ( mode == BUY_MODE )
      {
         pos = b_items [ cur_item ];
         SWD_SetFieldItem ( window, STOR_BUY, buybut [0] );
         SWD_SetFieldItem ( window, STOR_SELL, sellbut [1] );
         cost = OBJS_GetCost ( pos );
         if ( OBJS_IsOnly ( pos ) )
            num = OBJS_GetAmt ( pos );
         else
            num = OBJS_GetTotal ( pos );
         sprintf ( youhave, "%02d", num );
         sprintf ( coststr, "%02d", cost );
      }
      else
      {
         pos = s_items [ cur_item ];
         SWD_SetFieldItem ( window, STOR_BUY, buybut [1] );
         SWD_SetFieldItem ( window, STOR_SELL, sellbut [0] );
         cost = OBJS_GetResale ( pos );
         if ( OBJS_IsOnly ( pos ) )
            num = OBJS_GetAmt ( pos );
         else
            num = OBJS_GetTotal ( pos );
         sprintf ( youhave, "%02d", num );
         sprintf ( coststr, "%02d", cost );
      }

      SWD_SetFieldText ( window, STOR_STATS, yh_hold );
      SWD_SetFieldText ( window, STOR_TEXTCOST, saying [ mode ] );
      SWD_SetFieldText ( window, STOR_NUM, youhave );
      SWD_SetFieldText ( window, STOR_COST, coststr );
      sprintf ( youhave, "%07d", plr.score );
      SWD_SetFieldText ( window, STOR_SCORE, youhave );

      SWD_SetFieldItem ( window, STOR_BUYIT, mainbut [mode] );

      if ( pos < S_LAST_OBJECT )
         SWD_SetFieldItem ( window, STOR_COMP, items [ pos ] );

      SWD_ShowAllWindows();
      GFX_DisplayUpdate();
   }

   SWD_Dialog ( &dlg );

   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
   {
      WIN_AskExit();
   }

   if ( dlg.viewactive )
   {
      switch ( dlg.sfield )
      {
         default:
            break;
  
         case STOR_VEXIT:
            opt = dlg.sfield;
            if ( PTR_B1 ) goto store_exit;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 0 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VBUY:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 2 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VSELL:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 1 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VNEXT:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 3 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VPREV:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 4 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VACCEPT:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            if ( mode == BUY_MODE )
               SWD_SetFieldText ( window, STOR_TEXT, storetext [ 5 ] );
            else
               SWD_SetFieldText ( window, STOR_TEXT, storetext [ 6 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case STOR_VSCREEN:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, STOR_TEXT, storetext [ 7 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;
      }
   }
   else
   {
      opt = EMPTY;
      if ( opt != oldopt )
      {
         SWD_SetFieldText ( window, STOR_TEXT, " " );
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         oldopt = opt;
      }
   }

   switch ( dlg.keypress )
   {
      case SC_F1:
         HELP_Win ("STORHLP1_TXT");
         break;

      case SC_ESC:
         goto store_exit;

      case SC_SPACE:
         KBD_Wait ( dlg.keypress );
         mode ^= SELL_MODE;
         if ( mode == BUY_MODE )
         {
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field   = STOR_BUY;
         }
         else
         {
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field   = STOR_SELL;
         }
         break;

      case SC_ENTER:
         KBD_Wait ( dlg.keypress );
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = STOR_BUYIT;
         break;

      case SC_RIGHT:
      case SC_PAGEUP:
      case SC_UP:
         KBD_Wait ( dlg.keypress );
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = STOR_NEXT;
         break;

      case SC_LEFT:
      case SC_PAGEDN:
      case SC_DOWN:
         KBD_Wait ( dlg.keypress );
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = STOR_PREV;
         break;
   }

   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         update = TRUE;
         switch ( dlg.field )
         {
            case STOR_NEXT:
               SND_Patch ( FX_SWEP, 127 );
               if ( max_items )
                  cur_item++;
               break;

            case STOR_PREV:
               SND_Patch ( FX_SWEP, 127 );
               if ( max_items )
                  cur_item--;
               break;

            case STOR_BUY:
               SND_Patch ( FX_SWEP, 127 );
               mode  = BUY_MODE;
               max_items = MakeBuyItems();
               cur_item = 0;
               break;

            case STOR_SELL:
               SND_Patch ( FX_SWEP, 127 );
               mode  = SELL_MODE;
               max_items = MakeSellItems();
               cur_item = 0;
               break;

            case STOR_BUYIT:
               if ( mode == BUY_MODE )
               {
                  pos = b_items [ cur_item ];
                  switch ( OBJS_Buy ( pos ) )
                  {
                     case OBJ_GOTIT:
                        SND_Patch ( FX_SWEP, 127 );
                        break;

                     case OBJ_NOMONEY:
                        SND_Patch ( FX_WARNING, 127 );
                        Harrold ( HAR4_TXT );
                        break;

                     case OBJ_SHIPFULL:
                        SND_Patch ( FX_WARNING, 127 );
                        Harrold ( HAR7_TXT );
                        break;

                     case OBJ_ERROR:
                        SND_Patch ( FX_WARNING, 127 );
                        break;
                  }
                  MakeBuyItems();
                  for ( loop = 0; loop < buy_count; loop++ )
                  {
                     if ( b_items [ loop ] == pos )
                     {
                        cur_item = loop;
                        break;
                     }
                  }
               }
               else
               {
                  pos = s_items [ cur_item ];
                  OBJS_Sell ( pos );    
                  MakeSellItems();
                  SND_Patch ( FX_SWEP, 127 );
                  for ( loop = 0; loop < sell_count; loop++ )
                  {
                     if ( s_items [ loop ] == pos )
                     {
                        cur_item = loop;
                        break;
                     }
                  }
               }
               break;
         }
      }
   }

   goto mainloop;

   store_exit:
   SND_Patch ( FX_DOOR, 127 );
   while ( IMS_IsAck() );
   SWD_SetButtonFlag ( TRUE );

   GFX_FadeOut ( 0, 0, 0, 16 );

   PTR_DrawCursor ( FALSE );
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );

   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );
}

