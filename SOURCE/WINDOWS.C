#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raptor.h"

#include "prefapi.h"

#include "file0000.inc"
#include "file0001.inc"
#include "file0002.inc"
#include "file0003.inc"
#include "file0004.inc"

// window include files
#include "msg.inc"
#include "shipcomp.inc"
#include "loadcomp.inc"
#include "main.inc"
#include "hangar.inc"
#include "locker.inc"
#include "ask.inc"
#include "regist.inc"
#include "okreg.inc"
#include "wingame.inc"
#include "opts.inc"

extern BOOL   godmode;

#define HANGAR_MISSION   0
#define HANGAR_SUPPLIES  1
#define HANGAR_EXITDOS   2
#define HANGAR_NONE      3

#define MUSIC_VOL 0
#define FX_VOL 1

PRIVATE INT opt_window;
PRIVATE INT opt_vol[2] = { 127, 127 };
PUBLIC  INT opt_detail = 1;

PRIVATE CHAR hangtext [4][18] = {
   "FLY MISSION",
   "SUPPLY ROOM",
   "EXIT HANGAR",
   "SAVE PILOT"
   };

PRIVATE CHAR regtext [3][30] = {
   "ENTER NAME AND CALLSIGN",
   "   CHANGE ID PICTURE",
   "        EXIT"
   };

PRIVATE BOOL      ingameflag = FALSE;
PUBLIC   DWORD    sid_pics [4] = { WMALEID_PIC,
                                  BMALEID_PIC,
                                  WFMALEID_PIC,
                                  BFMALEID_PIC
                          };

PUBLIC   DWORD    id_pics [4] = { WMALE_PIC,
                                  BMALE_PIC,
                                  WFEMALE_PIC,
                                  BFEMALE_PIC
                          };

DWORD songsg1 [9] = {
   RAP8_MUS,     // WAVE 1 
   RAP2_MUS,     // WAVE 2 
   RAP4_MUS,     // WAVE 3 
   RAP7_MUS,     // WAVE 4 
   RAP6_MUS,     // WAVE 5 
   RAP2_MUS,     // WAVE 6 
   RAP3_MUS,     // WAVE 7 
   RAP4_MUS,     // WAVE 8 
   RAP6_MUS      // WAVE 9 
};

DWORD songsg2 [9] = {
   RAP3_MUS,     // WAVE 1 
   RAP2_MUS,     // WAVE 2 
   RAP4_MUS,     // WAVE 3 
   RAP8_MUS,     // WAVE 4 
   RAP6_MUS,     // WAVE 5 
   RAP2_MUS,     // WAVE 6 
   RAP8_MUS,     // WAVE 7 
   RAP1_MUS,     // WAVE 8 
   RAP6_MUS      // WAVE 9 
};

DWORD songsg3 [9] = {
   RAP8_MUS,     // WAVE 1 
   RAP4_MUS,     // WAVE 2 
   RAP1_MUS,     // WAVE 3 
   RAP7_MUS,     // WAVE 4 
   RAP6_MUS,     // WAVE 5 
   RAP2_MUS,     // WAVE 6 
   RAP3_MUS,     // WAVE 7 
   RAP4_MUS,     // WAVE 8 
   RAP6_MUS      // WAVE 9 
};


#define HANGTOSTORE    0
#define HANGTOMISSION  1

#define DEMO_DELAY ( 800 * 5 )


PRIVATE  INT diff_wrap [E_NUM_DIFF] = { 4, 9, 9, 9 };
PRIVATE  INT d_count       = 0;
PRIVATE  INT hangto        = HANGTOSTORE;

/***************************************************************************
WIN_WinGame () - Window text for winners of a game
 ***************************************************************************/
VOID
WIN_WinGame (
INT game                   // INPUT : game number 0,1,2
)
{
   DWORD    dtext[4] = { END1_TXT, END2_TXT, END3_TXT, END0_TXT };
   INT      window;

   if ( game > 3 )
      return;
  
   GFX_FadeOut ( 0, 0, 0, 2 );

   window = SWD_InitWindow ( WINGAME_SWD );
   SWD_SetFieldItem ( window, WIN_TEXT, dtext[game] );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   GFX_FadeIn ( palette, 16 );
  
   IMS_WaitTimed ( 30 );

   SWD_DestroyWindow ( window );
   GFX_DisplayUpdate();
}

/***************************************************************************
WIN_Msg () - Display a Message for ten secs or until user Acks somting
 ***************************************************************************/
VOID
WIN_Msg (
CHAR * msg                 // INPUT : pointer to message to ask
)
{
   INT      window;

   window = SWD_InitWindow ( MSG_SWD );
  
   SWD_SetFieldText ( window, INFO_MSG, msg );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   SND_Patch ( FX_SWEP, 127 );
  
   IMS_WaitTimed ( 10 );

   SWD_DestroyWindow ( window );
   GFX_DisplayUpdate();
   KBD_Clear();
}

/***************************************************************************
WIN_OptDraw() - 
 ***************************************************************************/
PRIVATE VOID
WIN_OptDraw (
SWD_DLG * dlg
)
{
   INT x;
   INT y;
   INT lx;
   INT ly;

   if ( dlg == NUL ) return;

   SWD_GetFieldXYL ( opt_window, OPTS_VMUSIC, &x, &y, &lx, &ly );
   GFX_PutSprite ( GLB_GetItem ( SLIDE_PIC ), x + opt_vol[MUSIC_VOL] - 2, y );

   SWD_GetFieldXYL ( opt_window, OPTS_VFX, &x, &y, &lx, &ly );
   GFX_PutSprite ( GLB_GetItem ( SLIDE_PIC ), x + opt_vol[FX_VOL] - 2, y );
}

/***************************************************************************
WIN_Opts() - Sets Game Options
 ***************************************************************************/
VOID
WIN_Opts (
VOID
)
{
   INT   fpics [3] = { OPTS_PIC1, OPTS_PIC2, OPTS_PIC3 };
   CHAR  detail[2][16] = {
      "LOW DETAIL",
      "HIGH DETAIL"
   };
   SWD_DLG  dlg;
   INT      new_vol;
   INT      cur_field;
   BOOL     curd;
   INT      x;
   INT      y;
   INT      lx;
   INT      ly;
   BOOL     kbactive = FALSE;
   BOOL     patchflag = FALSE;

   curd = opt_detail;
   cur_field = 0;
   opt_vol [ MUSIC_VOL ] = music_volume;
   opt_vol [ FX_VOL ] = fx_volume;

   opt_window = SWD_InitWindow ( OPTS_SWD );

   SWD_SetWindowPtr ( opt_window );
   SWD_SetFieldText ( opt_window, OPTS_DETAIL, detail[ curd ] );
   SWD_SetWinDrawFunc ( opt_window, WIN_OptDraw );

   SWD_SetFieldItem ( opt_window, OPTS_PIC1, EMPTY );
   SWD_SetFieldItem ( opt_window, OPTS_PIC2, EMPTY );
   SWD_SetFieldItem ( opt_window, OPTS_PIC3, EMPTY );

   SWD_ShowAllWindows();

   SND_Patch ( FX_SWEP, 127 );
   GFX_DisplayUpdate();

   mainloop:
   patchflag = FALSE;

   SWD_Dialog ( &dlg );

   switch ( dlg.keypress )
   {
      case SC_ESC:
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = OPTS_EXIT;
         break;

      case SC_LEFT:
         if ( cur_field != 0 )
         {
            if ( cur_field == 2 )
               patchflag = TRUE;
            opt_vol [ cur_field-1 ] -= 8;
            if ( opt_vol [ cur_field-1 ] < 0 )
               opt_vol [ cur_field-1 ] = 0;
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
         }
         break;

      case SC_RIGHT:
         if ( cur_field != 0 )
         {
            if ( cur_field == 2 )
               patchflag = TRUE;
            opt_vol [ cur_field-1 ] += 8;
            if ( opt_vol [ cur_field-1 ] > 127 )
               opt_vol [ cur_field-1 ] = 127;
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
         }
         break;

      case SC_UP:
         if ( kbactive )
         {
            if ( cur_field > 0 )
               cur_field--;
         }
         kbactive = TRUE;
         SND_Patch ( FX_SWEP, 127 );
         SWD_SetFieldItem ( opt_window, OPTS_PIC1, EMPTY );
         SWD_SetFieldItem ( opt_window, OPTS_PIC2, EMPTY );
         SWD_SetFieldItem ( opt_window, OPTS_PIC3, EMPTY );
         SWD_SetFieldItem ( opt_window, fpics [ cur_field ], POINT_PIC );
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         break;

      case SC_DOWN:
         if ( kbactive )
         {
            if ( cur_field < 2 )
               cur_field++;
         }
         kbactive = TRUE;
         SND_Patch ( FX_SWEP, 127 );
         SWD_SetFieldItem ( opt_window, OPTS_PIC1, EMPTY );
         SWD_SetFieldItem ( opt_window, OPTS_PIC2, EMPTY );
         SWD_SetFieldItem ( opt_window, OPTS_PIC3, EMPTY );
         SWD_SetFieldItem ( opt_window, fpics [ cur_field ], POINT_PIC );
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         break;

      case SC_ENTER:
         if ( cur_field != 0 ) break;
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = OPTS_DETAIL;
         break;
   }

   if ( dlg.viewactive )
   {
      switch ( dlg.sfield )
      {
         default:
            break;

         case OPTS_VMUSIC:
            if ( PTR_B1 )
            {
               while ( IMS_IsAck() );
               SWD_GetFieldXYL ( opt_window, OPTS_VMUSIC, &x, &y, &lx, &ly );
               new_vol = PTR_X - x;
               if ( new_vol < 0 )
                  new_vol = 0;
               if ( new_vol > 127 )
                  new_vol = 127;
               opt_vol[ MUSIC_VOL ] = new_vol;
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
            }
            break;

         case OPTS_VFX:
            if ( PTR_B1 )
            {
               while ( IMS_IsAck() );
               SWD_GetFieldXYL ( opt_window, OPTS_VFX, &x, &y, &lx, &ly );
               new_vol = PTR_X - x;
               if ( new_vol < 0 )
                  new_vol = 0;
               if ( new_vol > 127 )
                  new_vol = 127;
               opt_vol[ FX_VOL ] = new_vol;
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
            }
            break;
      }
   }

   opt_detail = (INT)curd;

   if ( opt_vol [ FX_VOL ] != fx_volume )
   {
      fx_volume = opt_vol [ FX_VOL ];
   }

   if ( opt_vol [ MUSIC_VOL ] != music_volume )
   {
      music_volume = opt_vol [ MUSIC_VOL ];
      MUS_SetMasterVolume ( music_volume );
   }

   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            case OPTS_EXIT:
               if ( opt_vol [ MUSIC_VOL ] >=0 && opt_vol [ MUSIC_VOL ] < 128 )
               {
                  music_volume = opt_vol [ MUSIC_VOL ];
                  INI_PutPreferenceLong ( "Music", "Volume", music_volume );
               }
               if ( opt_vol [ FX_VOL ] >=0 && opt_vol [ FX_VOL ] < 128 )
               {
                  fx_volume = opt_vol [ FX_VOL ];
                  INI_PutPreferenceLong ( "SoundFX", "Volume", fx_volume );
               }
               INI_PutPreferenceLong ( "Setup", "Detail", opt_detail );
               SND_Patch ( FX_SWEP, 127 );
               goto exit_opts;

            case OPTS_DETAIL:
               SND_Patch ( FX_SWEP, 127 );
               curd ^= TRUE;
               SWD_SetFieldText ( opt_window, OPTS_DETAIL, detail [ curd ] );
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               break;
         }
      }
   }

   if ( patchflag )
   {
      SND_Patch ( FX_SWEP, 127 );
      patchflag = FALSE;
   }

   goto mainloop;

   exit_opts:

   SWD_SetWinDrawFunc ( opt_window, NUL );

   SWD_DestroyWindow ( opt_window );
   GFX_DisplayUpdate();
   KBD_Clear();
}

/***************************************************************************
WIN_Pause () - Display a Pause Message Wait until user does somthing
 ***************************************************************************/
VOID
WIN_Pause (
VOID
)
{
   INT      window;
  
   window = SWD_InitWindow ( MSG_SWD );
  
   SWD_SetFieldText ( window, INFO_MSG, "GAME PAUSED" );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   SND_Patch ( FX_SWEP, 127 );

   IMS_StartAck();

   while ( !IMS_CheckAck() );

   SWD_DestroyWindow ( window );
   GFX_DisplayUpdate();

   KBD_Clear();
   IMS_StartAck();
}

/***************************************************************************
WIN_Order () - Display a Pause Message Wait until user does somthing
 ***************************************************************************/
VOID
WIN_Order (
VOID
)
{
   INT      window;
   BOOL     dchold = drawcursor;
  
   if ( GAME2 || GAME3 ) return;

   PTR_DrawCursor ( FALSE );
   KBD_Clear();
   GFX_FadeOut ( 0, 0, 0, 2 );

   window = SWD_InitWindow ( ORDER_SWD );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   GFX_FadeIn ( palette, 16 );
  
   IMS_WaitTimed ( 15 );

   GFX_FadeOut ( 0, 0, 0, 16 );
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );
   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );
   KBD_Clear();

   PTR_DrawCursor ( dchold );
}

/***************************************************************************
WIN_Credits () - 
 ***************************************************************************/
BOOL
WIN_Credits (
VOID
)
{
   static   INT   cnt = 1;
   DWORD    csng[3] = { BOSS2_MUS, BOSS3_MUS, BOSS4_MUS };
   INT      window;
   BOOL     rval;

   cnt++;
   if ( cnt >= 3 )
      cnt = 0;

   SND_PlaySong ( csng[cnt], TRUE, TRUE );

   KBD_Clear();
   GFX_FadeOut ( 0, 0, 0, 16 );

   window = SWD_InitWindow ( CREDIT_SWD );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   GFX_FadeIn ( palette, 16 );
  
   rval = IMS_WaitTimed ( 25 );

   GFX_FadeOut ( 0, 0, 0, 16 );
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );
   memset ( displayscreen, 0, 64000 );
   GFX_SetPalette ( palette, 0 );
   KBD_Clear();

   return ( rval );
}
  
/***************************************************************************
   WIN_AskBool () - Askes USER a YES/NO Question????
 ***************************************************************************/
BOOL                       // RETURN: TRUE/FALSE
WIN_AskBool (
CHAR * question            // INPUT : pointer to message to ask
)
{
   BOOL     rval  = FALSE;
   SWD_DLG  dlg;
   INT      ask_window;
   INT      xh;
   INT      yh;
   INT      px;
   INT      py;
   INT      lx;
   INT      ly;
   BOOL     dchold = drawcursor;
  
   xh = PTR_X;
   yh = PTR_Y;
  
   KBD_Clear();
   ask_window = SWD_InitWindow ( ASK_SWD );
  
   SWD_SetFieldText ( ask_window, ASK_DRAGBAR, question );
   SWD_SetActiveField ( ask_window, ASK_YES );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();

   SND_Patch ( FX_SWEP, 127 );
  
   PTR_DrawCursor ( TRUE );
  
   SWD_GetFieldXYL ( ask_window, ASK_YES, &px, &py, &lx, &ly );
   PTR_SetPos ( px + (lx>>1), py + (ly>>1) );

   SWD_SetActiveField ( ask_window, ASK_YES );

   mainloop:
  
   SWD_Dialog ( &dlg );
  
   if ( KBD_IsKey ( SC_ESC ) )
   {
      dlg.cur_act = S_FLD_COMMAND;
      dlg.cur_cmd = F_SELECT;
      dlg.field   = ASK_NO;
   }
  
   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            case ASK_YES:
               rval = TRUE;
            case ASK_NO:
               SWD_DestroyWindow ( ask_window );
               GFX_DisplayUpdate();
               PTR_DrawCursor ( dchold );
               return ( rval );
         }
      }
   }

   goto mainloop;

}
  
/***************************************************************************
WIN_AskExit () - Opens Windows and Askes if USer wants 2 quit
 ***************************************************************************/
VOID
WIN_AskExit (
VOID
)
{
   if ( WIN_AskBool ( "EXIT TO DOS" ) )
   {
      SND_FadeOutSong();

      switch ( bday_num )
      {
         default:
            break;

         case 0:
            SND_Patch ( FX_MON3, 127 );
            while ( SND_IsPatchPlaying ( FX_MON2 ) );
            break;

         case 1:
            SND_Patch ( FX_DAVE, 127 );
            while ( SND_IsPatchPlaying ( FX_DAVE ) );
            break;

         case 2:
            SND_Patch ( FX_MON4, 127 );
            while ( SND_IsPatchPlaying ( FX_MON4 ) );
            SND_Patch ( FX_MON4, 127 );
            while ( SND_IsPatchPlaying ( FX_MON4 ) );
            break;

         case 3:
            SND_Patch ( FX_MON1, 127 );
            while ( SND_IsPatchPlaying ( FX_MON1 ) );
            break;

         case 4:
            SND_Patch ( FX_MON2, 127 );
            while ( SND_IsPatchPlaying ( FX_MON2 ) );
            SND_Patch ( FX_MON2, 127 );
            while ( SND_IsPatchPlaying ( FX_MON2 ) );
            break;

         case 5:
            SND_Patch ( FX_MON6, 127 );
            while ( SND_IsPatchPlaying ( FX_MON6 ) );
            break;
      }

      GFX_SetRetraceFlag ( TRUE );
      GFX_FadeOut ( 60, 15, 2, 32 );
      GFX_FadeOut ( 0, 0, 0, 6 );
      EXIT_Clean();
   }
}

/***************************************************************************
 ***************************************************************************/
INT                        // RETURN -1=ABORT 0=EASY, 1=MED, 2=HARD
WIN_AskDiff (
VOID
)
{
   INT      rval  = EMPTY;
   SWD_DLG  dlg;
   INT      ask_window;
   INT      px;
   INT      py;
   INT      lx;
   INT      ly;
  
   KBD_Clear();
   ask_window = SWD_InitWindow ( ASKDIFF_SWD );
   SWD_SetActiveField ( ask_window, OKREG_MED );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   SWD_GetFieldXYL ( ask_window, OKREG_MED, &px, &py, &lx, &ly );
   PTR_SetPos ( px + (lx>>1), py + (ly>>1) );

   mainloop:
  
   SWD_Dialog ( &dlg );
  
   if ( KBD_IsKey ( SC_ESC ) )
   {
      rval = EMPTY;
      goto askdiff_exit;
   }
  
   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            case OKREG_TRAIN:
               rval = DIFF_0;
               WIN_Msg ("TRAIN MODE plays 4 of 9 levels!");
//               WIN_Msg ("TRAIN MODE only goes to level 4!");
               goto askdiff_exit;

            case OKREG_EASY:
               rval = DIFF_1;
               goto askdiff_exit;

            case OKREG_MED:
               rval = DIFF_2;
               goto askdiff_exit;

            case OKREG_HARD:
               rval = DIFF_3;
               goto askdiff_exit;

            case OKREG_ABORT:
               goto askdiff_exit;
         }
      }
   }

   goto mainloop;

   askdiff_exit:

   SWD_DestroyWindow ( ask_window );
   GFX_DisplayUpdate();

   return ( rval );
}

/***************************************************************************
WIN_Register () - Register Window
 ***************************************************************************/
BOOL
WIN_Register (
VOID
)
{
   extern      BOOL  reg_flag;
   PLAYEROBJ   tp;
   SWD_DLG     dlg;
   INT         window;
   INT         cur_id = 0;
   INT         opt = EMPTY;
   INT         oldopt = -99;
   BOOL        rval = FALSE;
   INT         loop;
   INT         diff;

   PTR_DrawCursor ( FALSE );

   SND_PlaySong ( HANGAR_MUS, TRUE, TRUE );

   KBD_Clear();
   GFX_FadeOut ( 0, 0, 0, 2 );
  
   memset ( &tp, 0, sizeof ( PLAYEROBJ ) );
   tp.sweapon     = EMPTY;
   tp.diff[0]     = DIFF_2;
   tp.diff[1]     = DIFF_2;
   tp.diff[2]     = DIFF_2;
   tp.diff[3]     = DIFF_2;
   tp.id_pic      = 0;

   window = SWD_InitWindow ( REGISTER_SWD );
   SWD_SetFieldItem ( window, REG_IDPIC, sid_pics [ tp.id_pic ] );
   SWD_SetActiveField ( window, REG_NAME );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   GFX_FadeIn ( palette, 16 );
  
   SWD_SetFieldPtr ( window, REG_VIEWID );
   PTR_DrawCursor ( TRUE );
  
   mainloop:
  
   SWD_Dialog ( &dlg );

   if ( KBD_Key ( SC_ESC ) )
   {
      rval = FALSE;
      goto reg_exit;
   }

   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
   {
      WIN_AskExit();
   }

   switch ( dlg.keypress )
   {
      case SC_F1:
         HELP_Win("NEWPLAY1_TXT");
         break;

      case SC_ALT:
      case SC_CTRL:
         cur_id++;
         cur_id = cur_id % 4;
         SWD_SetFieldItem ( window, REG_IDPIC, sid_pics [ cur_id ] );
         tp.id_pic = cur_id;
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         break;
   }
  
   if ( dlg.viewactive )
   {
      switch ( dlg.sfield )
      {
         default:
            break;
  
         case REG_VIEWEXIT:
            opt = dlg.sfield;
            if ( PTR_B1 )
            {
               while ( IMS_IsAck() );
               if ( RAP_IsSaveFile ( &tp ) )
               {
                  WIN_Msg ("Pilot NAME and CALLSIGN Used !");
               }
               else
               {
                  rval = TRUE;
                  goto reg_exit;
               }
            }
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, REG_TEXT, regtext [ 2 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case REG_VIEWID:
            opt = dlg.sfield;
            if ( PTR_B1 )
            {
               while ( IMS_IsAck() );
               cur_id++;
               cur_id = cur_id % 4;
               SWD_SetFieldItem ( window, REG_IDPIC, sid_pics [ cur_id ] );
               tp.id_pic = cur_id;
            }
            SWD_SetFieldText ( window, REG_TEXT, regtext [ 1 ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case REG_VIEWREG:
            opt = dlg.sfield;
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, REG_TEXT, regtext [ 0 ] );
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
         SWD_SetFieldText ( window, REG_TEXT, " " );
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         oldopt = opt;
      }
   }

   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            case REG_NAME:
               SWD_GetFieldText ( window, REG_NAME, tp.name );
               if ( strlen ( tp.name ) && dlg.keypress == SC_ENTER )
                  SWD_SetActiveField ( window, REG_CALLSIGN );
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               break;

            case REG_CALLSIGN:
               SWD_GetFieldText ( window, REG_CALLSIGN, tp.callsign );
               SWD_GetFieldText ( window, REG_CALLSIGN, tp.name );
               if ( !strlen ( tp.name ) )
               {
                  SWD_SetActiveField ( window, REG_NAME );
                  SWD_ShowAllWindows();
                  GFX_DisplayUpdate();
                  break;
               }
               if ( !strlen ( tp.callsign ) )
               {
                  SWD_SetActiveField ( window, REG_CALLSIGN );
                  SWD_ShowAllWindows();
                  GFX_DisplayUpdate();
                  break;
               }
               if ( dlg.keypress == SC_ENTER || KBD_Key ( SC_ENTER ) )
               {
                  if ( RAP_IsSaveFile ( &tp ) )
                  {
                     WIN_Msg("Pilot NAME and CALLSIGN Used !");
                  }
                  else
                  {
                     rval = TRUE;
                     goto reg_exit;
                  }
               }
               break;
         }
      }
   }

   goto mainloop;
  
   reg_exit:

   SWD_GetFieldText ( window, REG_NAME, tp.name );
   SWD_GetFieldText ( window, REG_CALLSIGN, tp.callsign );

   if ( !strlen ( tp.name ) )
   {
      SWD_SetActiveField ( window, REG_NAME );
      rval = FALSE;
   }
   if ( !strlen ( tp.callsign ) )
   {
      SWD_SetActiveField ( window, REG_CALLSIGN );
      rval = FALSE;
   }

   diff = 1;
   if ( rval )
   {
     diff = WIN_AskDiff();
     if ( diff >= 0 )
     {
         ingameflag = FALSE;
         if ( !RAP_FFSaveFile() )
         {
            WIN_Msg("ERROR : YOU MUST DELETE A PILOT");
            rval = FALSE;
         }
      }
      else
      {
         WIN_Msg ("PLAYER ABORTED!");
         rval = FALSE;
      }
   }

   if ( rval )
   {
      ingameflag = FALSE;
      tp.diff[0] = diff;
      tp.diff[1] = diff;
      tp.diff[2] = diff;

      if ( diff == DIFF_0 )
      {
         tp.trainflag = TRUE;
         tp.fintrain = FALSE;
      }
      else
      {
         tp.trainflag = FALSE;
         tp.fintrain = TRUE;
      }

      plr = tp;
      RAP_SetPlayerDiff();
      OBJS_Add ( S_FORWARD_GUNS );
      OBJS_Add ( S_ENERGY );
      OBJS_Add ( S_ENERGY );
      OBJS_Add ( S_ENERGY );
      plr.score = 10000;

      if ( godmode )
      {
         plr.score += 876543;
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_DETECT );
         for (loop = 1;loop < S_ENERGY; loop++ )
            OBJS_Add ( loop );
      }

      OBJS_GetNext();
   }
  
   PTR_DrawCursor ( FALSE );
   GFX_FadeOut ( 0, 0, 0, 16 );
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );

   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );

   hangto = HANGTOSTORE;

   if ( rval )
      ingameflag = FALSE;

   return ( rval );
}

/***************************************************************************
WIN_Hangar() - Does the hanger dude
 ***************************************************************************/
INT
WIN_Hangar (
VOID
)
{
   INT      poslookup[4] = { HANG_MISSION, HANG_SUPPLIES, HANG_MAIN_MENU, HANG_QSAVE };
   CHAR     temp[42];
   SWD_DLG  dlg;
   INT      window;
   INT      opt = HANG_SUPPLIES;
   INT      oldopt = -1;
   INT      pos = 0;
   BOOL     kflag  = FALSE;
   INT      x;
   INT      y;
   INT      ly;
   INT      lx;
   DWORD    item;
   INT      local_cnt = FRAME_COUNT;
   INT      pic_cnt = 0;
  
   PTR_DrawCursor ( FALSE );

   KBD_Clear();

   if ( plr.trainflag )
   {
      plr.trainflag = FALSE;
      opt = HANG_SUPPLIES;
      goto train_exit;
   }

   GFX_FadeOut ( 0, 0, 0, 2 );
  
   window = SWD_InitMasterWindow ( HANGAR_SWD );

   item = SWD_GetFieldItem ( window, HANG_PIC );

   SND_PlaySong ( HANGAR_MUS, TRUE, TRUE );

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   GFX_FadeIn ( palette, 16 );

   if ( control != I_MOUSE )
      kflag = TRUE;

   switch ( hangto )
   {
      case HANGTOMISSION:
         pos = 0;
         SWD_SetActiveField ( window, poslookup[pos] );
         break;

      default:
      case HANGTOSTORE:
         pos = 1;
         SWD_SetActiveField ( window, poslookup[pos] );
         break;
   }

   hangto = HANGTOMISSION;

   SWD_SetWindowPtr ( window );
   PTR_DrawCursor ( TRUE );

   mainloop:
  
   pic_cnt++;

   if ( pic_cnt > 4 )
   {
      pic_cnt = 0;

      if ( random (3) == 0 )
         SWD_SetFieldItem ( window, HANG_PIC, HANGP_PIC );
      else
         SWD_SetFieldItem ( window, HANG_PIC, EMPTY );

      SWD_ShowAllWindows();
      GFX_DisplayUpdate();
   }
   else
   {
      local_cnt = FRAME_COUNT;
      while ( FRAME_COUNT == local_cnt );
   }

   SWD_Dialog ( &dlg );

   if ( KBD_Key ( SC_ESC ) )
   {
      opt = -99;
      goto hangar_exit;
   }
  
   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
   {
      WIN_AskExit();
   }

   switch ( dlg.keypress )
   {
      case SC_F1:
         HELP_Win("HANGHLP1_TXT");
         break;

      case SC_S:
      case SC_F2:
         pos = 3;
         sprintf (temp, "Save %s - %s ?", plr.name, plr.callsign );
         if ( WIN_AskBool ( temp ) )
            RAP_SavePlayer();
         break;

      case SC_TAB:
      case SC_UP:
      case SC_LEFT:
         kflag = TRUE;
         KBD_Wait ( SC_UP );
         KBD_Wait ( SC_LEFT );
         pos++;
         pos = pos % 4;
         break;

      case SC_DOWN:
      case SC_RIGHT:
         kflag = TRUE;
         KBD_Wait ( SC_DOWN );
         KBD_Wait ( SC_RIGHT );
         pos--;
         if ( pos < 0 )
            pos = 3;
         break;

      case SC_SPACE:
      case SC_ENTER:
         KBD_Wait ( SC_ENTER );
         KBD_Wait ( SC_SPACE );
         opt = poslookup [ pos ];
         goto keyboard_part;
   }

   if ( kflag )
   {
      kflag = FALSE;
      switch ( pos )
      {
         case 0:
         case 1:
         case 2:
         case 3:
            opt = poslookup [ pos ];
            if ( opt == oldopt ) break;
            oldopt = opt;
            SWD_GetFieldXYL ( window, opt, &x, &y, &lx, &ly );
            PTR_SetPos ( x + ( lx >> 1 ), y + ( ly >> 1 ) );
            oldopt         = EMPTY;
            dlg.sfield     = opt;
            dlg.viewactive = TRUE;
            break;

         default:
            pos = 0;
            break;
      }
   }

   if ( dlg.viewactive )
   {
      keyboard_part:

      switch ( dlg.sfield )
      {
         default:
            break;
  
         case HANG_MISSION:
            pos = 0;
            opt = dlg.sfield;
            if ( PTR_B1 || dlg.keypress == SC_ENTER )
            {
               SND_Patch ( FX_DOOR, 60 );
               while ( IMS_IsAck() );
               goto hangar_exit;
            }
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, HANG_TEXT, hangtext [ pos ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;
  
         case HANG_SUPPLIES:
            pos = 1;
            opt = dlg.sfield;
            if ( PTR_B1 || dlg.keypress == SC_ENTER )
            {
               SND_Patch ( FX_DOOR, 127 );
               while ( IMS_IsAck() );
               goto hangar_exit;
            }
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, HANG_TEXT, hangtext [ pos ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;
  
         case HANG_MAIN_MENU:
            pos = 2;
            opt = dlg.sfield;
            if ( PTR_B1 || dlg.keypress == SC_ENTER )
            {
               opt = -99;
               SND_Patch ( FX_DOOR, 200 );
               while ( IMS_IsAck() );
               goto hangar_exit;
            }
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, HANG_TEXT, hangtext [ pos ] );
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            oldopt = opt;
            break;

         case HANG_QSAVE:
            pos = 3;
            opt = dlg.sfield;
            if ( PTR_B1 || dlg.keypress == SC_ENTER )
            {
               while ( IMS_IsAck() );
               sprintf (temp, "Save %s - %s ?", plr.name, plr.callsign );
               if ( WIN_AskBool ( temp ) )
                  RAP_SavePlayer();
               while ( IMS_IsAck() );
               break;
            }
            if ( opt == oldopt ) break;
            SWD_SetFieldText ( window, HANG_TEXT, hangtext [ pos ] );
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
         SWD_SetFieldText ( window, HANG_TEXT, " " );
         SWD_ShowAllWindows();
         GFX_DisplayUpdate();
         oldopt = opt;
      }
   }
  
   goto mainloop;
  
   hangar_exit:
  
   PTR_DrawCursor ( FALSE );

   GFX_FadeOut ( 0, 0, 0, 16 );
  
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );
   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );

train_exit:

   return ( opt );
}

PRIVATE INT g_x;
PRIVATE INT g_y;
PRIVATE INT g_lx;
PRIVATE INT g_ly;

/***************************************************************************
WIN_LoadComp() - Shows computer loading screen
 ***************************************************************************/
VOID
WIN_LoadComp (
VOID
)
{
   CHAR     sect [3][15] =
      {
         "BRAVO SECTOR",
         "TANGO SECTOR",
         "OUTER REGIONS"
      };
   INT      window;
   CHAR     temp [ 40 ];

   KBD_Clear();
  
   window = SWD_InitMasterWindow ( LOADCOMP_SWD );
   SWD_GetFieldXYL ( window, LCOMP_LEVEL, &g_x, &g_y, &g_lx, &g_ly );
   
   sprintf ( temp, "WAVE %d", game_wave [ cur_game ] );
   SWD_SetFieldText ( window, LCOMP_WAVE, temp );

   SWD_SetFieldText ( window, LCOMP_SECTOR, sect [ cur_game ] );

   if ( game_wave [ cur_game ] == diff_wrap [ plr.diff [ cur_game ]  ] - 1 )
   {
      sprintf ( temp, "FINAL WAVE %d", game_wave [ cur_game ] + 1 );
      SWD_SetFieldText ( window, LCOMP_WAVE, temp );
   }
   else
   {
      sprintf ( temp, "WAVE %d", game_wave [ cur_game ] + 1 );
      SWD_SetFieldText ( window, LCOMP_WAVE, temp );
   }

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
   GFX_FadeIn ( palette, 8 );

   SWD_DestroyWindow ( window );
}

/***************************************************************************
WIN_ShipComp () - Does Game Selection 1, 2 or 3
 ***************************************************************************/
BOOL
WIN_ShipComp (
VOID
)
{
   SWD_DLG  dlg;
   INT      window;
   BOOL     rval    = TRUE;
   BOOL     secret1 = FALSE;
   BOOL     secret2 = FALSE;
   BOOL     secret3 = FALSE;
   BOOL     secret  = FALSE;
   BYTE     cz1     = ltable[0];
   BYTE     cz2     = dtable[0];
   INT      px;
   INT      py;
   INT      lx;
   INT      ly;

   GLB_FreeAll();

   PTR_DrawCursor ( FALSE );

   ltable[0] = 0;
   dtable[0] = 0;

   SND_PlaySong ( HANGAR_MUS, TRUE, TRUE );

   KBD_Clear();
   GFX_FadeOut ( 0, 0, 0, 2 );
  
   cur_diff = ( cur_diff & ~( EB_SECRET_1 + EB_SECRET_2 + EB_SECRET_3 ) );

   window = SWD_InitMasterWindow ( SHIPCOMP_SWD );

   GLB_LockItem ( LIGHTON_PIC );
   GLB_LockItem ( LIGHTOFF_PIC );

   SWD_GetFieldXYL ( window, COMP_AUTO, &px, &py, &lx, &ly );
   PTR_SetPos ( px + (lx>>1), py + (ly>>1) );
   SWD_SetActiveField ( window, COMP_AUTO );

   SWD_SetFieldItem ( window, COMP_LITE1, LIGHTOFF_PIC );
   SWD_SetFieldItem ( window, COMP_LITE2, LIGHTOFF_PIC );
   SWD_SetFieldItem ( window, COMP_LITE3, LIGHTOFF_PIC );

   if ( bday_flag )
   {
      secret = TRUE;
      secret1 = TRUE;
      secret2 = TRUE;
      secret3 = TRUE;
      SWD_SetFieldItem ( window, COMP_LITE1, LIGHTON_PIC );
      SWD_SetFieldItem ( window, COMP_LITE2, LIGHTOFF_PIC );
      SWD_SetFieldItem ( window, COMP_LITE3, LIGHTON_PIC );
      SND_Patch ( FX_EGRAB, 127 );
   }

   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   GFX_FadeIn ( palette, 16 );
  
   SWD_SetWindowPtr ( window );
   PTR_DrawCursor ( TRUE );
  
   mainloop:
  
   SWD_Dialog ( &dlg );
  
   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
      WIN_AskExit();

   switch ( dlg.keypress )
   {
      default:
         break;
  
      case SC_ESC:
         rval = FALSE;
         goto abort_shipcomp;
  
      case SC_F1:
         HELP_Win("COMPHLP1_TXT");
         break;
  
      case SC_Z:
         if ( godmode || demo_flag )
            cur_game = 0;
         break;

      case SC_X:
         if ( godmode || demo_flag )
            cur_game = 1;
         break;           

      case SC_C:
         if ( godmode || demo_flag )
            cur_game = 2;
         break;

      case SC_Q:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 0;
         goto exit_shipcomp;
  
      case SC_W:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 1;
         goto exit_shipcomp;
  
      case SC_E:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 2;
         goto exit_shipcomp;
  
      case SC_R:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 3;
         goto exit_shipcomp;
  
      case SC_T:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 4;
         goto exit_shipcomp;
  
      case SC_Y:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 5;
         goto exit_shipcomp;
  
      case SC_U:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 6;
         goto exit_shipcomp;
  
      case SC_I:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 7;
         goto exit_shipcomp;
  
      case SC_O:
         if ( godmode || demo_flag )
            game_wave [ cur_game ] = 8;
         goto exit_shipcomp;
   }
  
   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            default:
               break;
  
            case COMP_SECRET:
               secret=TRUE;
               break;

            case COMP_AUTO:
               goto exit_shipcomp;

            case COMP_GAME1:
               cur_game = 0;
               goto exit_shipcomp;

            case COMP_GAME2:
               if ( !gameflag [ 1 ] )
               {
                  WIN_Order();
                  rval = FALSE;
                  goto abort_shipcomp;
               }
               cur_game = 1;
               goto exit_shipcomp;

            case COMP_GAME3:
               if ( !gameflag [ 2 ] )
               {
                  WIN_Order();
                  rval = FALSE;
                  goto abort_shipcomp;
               }
               cur_game = 2;
               goto exit_shipcomp;

            case COMP_B1:
               if ( !secret ) break;
               secret1 ^= TRUE;
               if ( secret1 )
                  SWD_SetFieldItem ( window, COMP_LITE1, LIGHTON_PIC );
               else
                  SWD_SetFieldItem ( window, COMP_LITE1, LIGHTOFF_PIC );
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               break;
  
            case COMP_B2:
               if ( !secret ) break;
               secret2 ^= TRUE;
               if ( secret2 )
                  SWD_SetFieldItem ( window, COMP_LITE2, LIGHTON_PIC );
               else
                  SWD_SetFieldItem ( window, COMP_LITE2, LIGHTOFF_PIC );
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               break;
  
            case COMP_B3:
               if ( !secret ) break;
               secret3 ^= TRUE;
               if ( secret3 )
                  SWD_SetFieldItem ( window, COMP_LITE3, LIGHTON_PIC );
               else
                  SWD_SetFieldItem ( window, COMP_LITE3, LIGHTOFF_PIC );
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               break;

            case COMP_CLOSE:
               rval = FALSE;
               goto abort_shipcomp;
         }
      }
   }

   goto mainloop;
  
   exit_shipcomp:

   ltable[0] = cz1;
   dtable[0] = cz2;
  
   SWD_SetFieldText ( window, COMP_GAME1, NUL );
   SWD_SetFieldText ( window, COMP_GAME2, NUL );
   SWD_SetFieldText ( window, COMP_GAME3, NUL );
   SWD_SetFieldText ( window, COMP_AUTO, NUL );
   
   abort_shipcomp:

   PTR_DrawCursor ( FALSE );

   GFX_FadeOut ( 0, 0, 0, 8 );

   SWD_DestroyWindow ( window );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   if ( secret )
   {
      if ( secret1 == TRUE && secret2 == TRUE && secret3 == TRUE )
      {
         cur_diff |= EB_SECRET_1;
         cur_diff |= EB_SECRET_2;
         cur_diff |= EB_SECRET_3;
         SND_Patch ( FX_EGRAB, 127 );
      }
   }

   if ( bday_flag )
   {
      cur_diff |= EB_SECRET_1;
      cur_diff |= EB_SECRET_2;
      cur_diff |= EB_SECRET_3;
      SND_Patch ( FX_EGRAB, 127 );
   }

   hangto = HANGTOMISSION;

   GLB_FreeItem ( LIGHTON_PIC );
   GLB_FreeItem ( LIGHTOFF_PIC );

   return ( rval );
}

/***************************************************************************
WIN_SetLoadLevel() 
 ***************************************************************************/
VOID
WIN_SetLoadLevel (
INT level
)
{
   DWORD  addx;
   DWORD  curs;

   addx = ( g_lx << 16 ) / 100;
   curs = addx * level;

   GFX_ColorBox ( g_x, g_y, (curs>>16)+1, g_ly, 85 );
   GFX_DisplayUpdate();
}

/***************************************************************************
WIN_EndLoad () - Shows Ship computer while loading level
 ***************************************************************************/
VOID
WIN_EndLoad (
VOID
)
{
   GFX_FadeOut ( 0, 0, 0, 16 );
   memset ( displaybuffer, 0, 64000 );
   GFX_MarkUpdate ( 0, 0, 320, 200 );
   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );
}

/***************************************************************************
WIN_MainLoop() - Handles Locker/Register/Store/Hangar and starting game
 ***************************************************************************/
VOID
WIN_MainLoop (
VOID
)
{
   extern   INT              demo_flag;
   INT      rval           = EMPTY;
   BOOL     abort_flag     = FALSE;
   INT      dwrap;
   INT      loop;

   ingameflag = TRUE;

   SND_PlaySong ( HANGAR_MUS, TRUE, TRUE );

   for (;;)
   {
      if ( demo_flag != DEMO_RECORD )
      {
         if ( rval == EMPTY )
         {
            rval = WIN_Hangar();
         }

         switch ( rval )
         {
            case HANG_MISSION:
               rval = EMPTY;
               break;
  
            default:
               return;
  
            case HANG_SUPPLIES:
               STORE_Enter();
               hangto = HANGTOMISSION;
               rval = EMPTY;
               continue;
         }
      }

      if  ( !WIN_ShipComp() )
         continue;

      if ( demo_flag == DEMO_RECORD )
         DEMO_MakePlayer( cur_game );

      switch ( cur_game )
      {
         default:
         case 0:
            SND_PlaySong ( songsg1 [ game_wave [ cur_game ] ], TRUE, TRUE );
            break;
         case 1:
            SND_PlaySong ( songsg2 [ game_wave [ cur_game ] ], TRUE, TRUE );
            break;
         case 2:
            SND_PlaySong ( songsg3 [ game_wave [ cur_game ] ], TRUE, TRUE );
            break;
      }

      WIN_LoadComp();
      RAP_LoadMap();
      GFX_SetRetraceFlag ( FALSE );

      abort_flag = Do_Game();

      hangto = HANGTOSTORE;

      GFX_SetRetraceFlag ( TRUE );
  
      if ( OBJS_GetAmt ( S_ENERGY ) <= 0 )
      {
         ingameflag = FALSE;
         SND_PlaySong ( RAP5_MUS, TRUE, TRUE );
         INTRO_Death();
         return;
      }

      if ( abort_flag )
      {
         INTRO_Landing();
         continue;
      }

      dwrap = diff_wrap [ plr.diff [ cur_game ] ] - 1;

      if ( game_wave [ cur_game ] == dwrap )
      {
         if ( ( plr.diff [ cur_game ] == DIFF_0 ) && !plr.fintrain )
         {
            OBJS_Init();
            plr.sweapon = EMPTY;
            plr.fintrain = TRUE;
            plr.score = 0;
            OBJS_Add ( S_FORWARD_GUNS );
            OBJS_Add ( S_ENERGY );
            OBJS_Add ( S_ENERGY );
            OBJS_Add ( S_ENERGY );
            plr.score = 10000;

            if ( plr.diff [ cur_game ] < DIFF_3 )
               plr.diff [ cur_game ]++;

            for ( loop = 0; loop < 4; loop++ )
            {
               if ( plr.diff [ loop ] == DIFF_0 )
               {
                  plr.diff [ loop ] = DIFF_1;
               }
            }
         }
         else
         {
            if ( plr.diff [ cur_game ] < DIFF_3 )
               plr.diff [ cur_game ]++;
         }

         RAP_SetPlayerDiff();

         game_wave [ cur_game ] = 0;

         if ( dwrap < 8 )
         {
            WIN_WinGame ( 3 );         // SHOW TRAINING COMPLETE STUFF
            cur_game = 0;
            game_wave [0] = 0;
            game_wave [1] = 0;
            game_wave [2] = 0;
         }
         else
         {
            if ( !abort_flag )
            {
               INTRO_EndGame ( cur_game );
               cur_game++;
            }
         }

         if ( cur_game >= 3 )
            cur_game = 0;

         if ( !gameflag [ cur_game ] )
            cur_game = 0;
      }
      else
      {
         if ( !abort_flag )
            game_wave [ cur_game ]++;

         INTRO_Landing();
      }
   }
}

/***************************************************************************
   WIN_MainAuto()
 ***************************************************************************/
VOID
WIN_MainAuto (
INT cur_opt
)
{
   INT   max_opt  = 5;
   BOOL  end_flag = FALSE;
   BOOL     dchold = drawcursor;

   PTR_DrawCursor ( FALSE );

   if ( GAME2 )
      max_opt += 3;

   if ( GAME3 )
      max_opt += 3;

   for (;;)
   {
      switch ( cur_opt )
      {
         case DEM_INTRO:
            SND_CacheFX();
            SND_PlaySong ( RINTRO_MUS, TRUE, TRUE );
            end_flag = INTRO_PlayMain();
            break;

         case DEM_CREDITS:
            end_flag = WIN_Credits();
            break;

         case DEM_DEMO1G1:
            DEMO_GLBFile ( DEMO1G1_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO2G1:
            DEMO_GLBFile ( DEMO2G1_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO3G1:
            DEMO_GLBFile ( DEMO3G1_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO1G2:
            DEMO_GLBFile ( DEMO1G2_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO2G2:
            DEMO_GLBFile ( DEMO2G2_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO3G2:
            DEMO_GLBFile ( DEMO3G2_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO1G3:
            DEMO_GLBFile ( DEMO1G3_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO2G3:
            DEMO_GLBFile ( DEMO2G3_REC );
            end_flag = DEMO_Play();
            break;

         case DEM_DEMO3G3:
            DEMO_GLBFile ( DEMO3G3_REC );
            end_flag = DEMO_Play();
            break;
      }

      if ( end_flag )
         break;

      cur_opt++;

      cur_opt = cur_opt % max_opt;
   }

   PTR_DrawCursor ( dchold );
   GLB_FreeAll();
   SND_CacheIFX();
}

/***************************************************************************
WIN_DemoDelay (
 ***************************************************************************/
PRIVATE BOOL
WIN_DemoDelay (
BOOL  startflag
)
{
   INT            local_cnt;

   if ( startflag )
   {
      d_count = 0;
      return ( FALSE );
   }

   local_cnt = FRAME_COUNT;
   while ( local_cnt == FRAME_COUNT );

   d_count++;

   if ( d_count < DEMO_DELAY )
      return ( FALSE );

   return ( TRUE );
}
  
/***************************************************************************
WIN_MainMenu () - Main Menu
 ***************************************************************************/
VOID
WIN_MainMenu (
VOID
)
{
   extern   INT demo_flag;
   extern   INT tai_flag;
   SWD_DLG  dlg;
   INT      window;
   BYTE     cz1    = ltable[0];
   INT      cur_opt = 0;

   KBD_Clear();

   PTR_DrawCursor ( FALSE );

   WIN_DemoDelay (TRUE);

   if ( demo_flag == DEMO_RECORD )
      return;

   window = SWD_InitMasterWindow ( MAIN_SWD );

   if ( ingameflag )
   {
      SWD_SetFieldSelect ( window, MAIN_RETURN, TRUE );
      SWD_SetFieldItem ( window, MAIN_RETURN, MENU7_PIC );
      SWD_SetFieldText ( window, MAIN_DEMO, NUL );
   }
   else
   {
      SWD_SetFieldSelect ( window, MAIN_RETURN, FALSE );
      SWD_SetFieldItem ( window, MAIN_RETURN, EMPTY );
   }

   if ( tai_flag )
      SWD_SetFieldItem ( window, MAIN_0000, RAPLOG2_PIC );

   PTR_DrawCursor ( FALSE );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   if ( ingameflag )
      SND_PlaySong ( RINTRO_MUS, TRUE, TRUE );
   else
      SND_PlaySong ( MAINMENU_MUS, TRUE, TRUE );

   GFX_FadeIn ( palette, 10 );
  
   SND_CacheIFX();

   SWD_SetWindowPtr ( window );
   PTR_DrawCursor ( TRUE );
   ltable[0] = 0;
  
   mainloop:
   SWD_Dialog ( &dlg );

   if ( dlg.keypress == SC_D )
   {
      cur_opt = DEM_DEMO1G1;
      d_count = DEMO_DELAY + 2;
   }

   if  ( WIN_DemoDelay ( FALSE ) && !ingameflag )
   {
      GFX_FadeOut ( 0, 0, 0, 3 );
      SWD_DestroyWindow ( window );
      memset ( displaybuffer, 0, 64000 );
      memset ( displayscreen, 0, 64000 );
      WIN_MainAuto ( cur_opt );
      cur_opt = 0;
      window = SWD_InitMasterWindow ( MAIN_SWD );
      if ( ingameflag )
         SWD_SetFieldItem ( window, MAIN_RETURN, MENU7_PIC );
      else
         SWD_SetFieldItem ( window, MAIN_RETURN, EMPTY );
      PTR_DrawCursor ( FALSE );
      SND_PlaySong ( MAINMENU_MUS, TRUE, TRUE );
      GFX_FadeOut ( 0, 0, 0, 2 );
      SWD_ShowAllWindows();
      GFX_DisplayUpdate();
      GFX_FadeIn ( palette, 16 );
      WIN_DemoDelay(TRUE);
      GLB_FreeAll();
      SND_CacheIFX();
      PTR_DrawCursor ( TRUE );
   }

   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
   {
      WIN_AskExit();
   }

   if ( KBD_Key ( SC_ESC ) && ingameflag )
   {
      goto menu_exit;
   }

   if ( dlg.keypress == SC_F1 )
      HELP_Win("HELP1_TXT");

   if ( PTR_B1 || PTR_B2 || dlg.keypress != SC_NONE )
      WIN_DemoDelay ( TRUE );

   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         WIN_DemoDelay(TRUE);
         switch ( dlg.field )
         {
            case MAIN_NEW:
               if ( WIN_Register() )
                  goto menu_exit;
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               GFX_FadeIn ( palette, 16 );
               PTR_DrawCursor ( TRUE );
               break;

            case MAIN_LOAD:
               switch ( RAP_LoadWin() )
               {
                  case -1:
                     WIN_Msg("No Pilots to Load");
                     break;

                  default:
                  case 0:
                     break;

                  case 1:
                     ingameflag = FALSE;
                     goto menu_exit;
               }
               break;

            case MAIN_OPTS:
               WIN_Opts();
               break;

            case MAIN_ORDER:
               if ( reg_flag )
                  HELP_Win("REG_VER1_TXT");
               else
                  HELP_Win("RAP1_TXT");
               break;

            case MAIN_CREDITS:
               PTR_DrawCursor ( FALSE );
               WIN_Credits();
               SWD_ShowAllWindows();
               GFX_DisplayUpdate();
               SND_PlaySong ( MAINMENU_MUS, TRUE, TRUE );
               PTR_DrawCursor ( TRUE );
               break;

            case MAIN_RETURN:
               if ( ingameflag )
                  goto menu_exit;
               break;

            case MAIN_QUIT:
               WIN_AskExit();
               break;

            default:
               break;
         }
      }
   }

   goto mainloop;
  
   menu_exit:
  
   PTR_DrawCursor ( FALSE );
  
   GFX_FadeOut ( 0, 0, 0, 16 );
   SWD_DestroyWindow ( window );
   memset ( displaybuffer, 0, 64000 );

   GFX_DisplayUpdate();
   GFX_SetPalette ( palette, 0 );
  
   ltable[0] = cz1;

   hangto = HANGTOSTORE;

   return;
}

