#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
  
#include "raptor.h"
#include "prefapi.h"
#include "file0000.inc"
#include "file0001.inc"

PUBLIC INT buttons[4] = { FALSE, FALSE, FALSE, FALSE };

PUBLIC ITYPE control = I_MOUSE;

#define MAX_ADDX 10
#define MAX_ADDY 8

PRIVATE INT k_Up;
PRIVATE INT k_Down;
PRIVATE INT k_Left;
PRIVATE INT k_Right;

PRIVATE INT k_Fire;
PRIVATE INT k_FireSp;
PRIVATE INT k_ChangeSp;
PRIVATE INT k_Mega;

PRIVATE INT m_lookup[3] = { 0, 1, 2 };
PRIVATE INT j_lookup[4] = { 0, 1, 2, 3 };

PUBLIC INT  f_addx  = 0;
PUBLIC INT  f_addy  = 0;

PRIVATE INT g_addx  = 0;
PRIVATE INT g_addy  = 0;
PRIVATE INT xm;
PRIVATE INT ym;
PRIVATE INT tsm_id;

extern  INT joy_x;
extern  INT joy_y;

PUBLIC   INT      demo_mode   = DEMO_OFF;
PRIVATE  RECORD   playback [ MAX_DEMO + 1 ];
PRIVATE  INT      cur_play    = 0;
PRIVATE  CHAR     demo_name [32];
PRIVATE  INT      demo_wave;
PRIVATE  INT      demo_game;
PRIVATE  INT      max_play;
PRIVATE  BOOL     control_pause = FALSE;

/***************************************************************************
DEMO_MakePlayer() - 
 ***************************************************************************/
VOID
DEMO_MakePlayer (
INT game
)
{
   memset ( &plr, 0, sizeof ( PLAYEROBJ ) );

   plr.sweapon     = EMPTY;
   plr.diff[0]     = DIFF_3;
   plr.diff[1]     = DIFF_3;
   plr.diff[2]     = DIFF_3;
   plr.id_pic      = 0;

   RAP_SetPlayerDiff();

   OBJS_Add ( S_FORWARD_GUNS );
   OBJS_Add ( S_ENERGY );
   OBJS_Add ( S_ENERGY );
   OBJS_Add ( S_ENERGY );
   OBJS_Add ( S_ENERGY );
   OBJS_Add ( S_DETECT );
   plr.score = 10000;

   switch ( game )
   {
      default:
         OBJS_Add ( S_MICRO_MISSLE );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MINI_GUN );
         OBJS_Add ( S_AIR_MISSLE );
         OBJS_Add ( S_TURRET );
         OBJS_Add ( S_DEATH_RAY );
         break;

      case  1:
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_SUPER_SHIELD );
         OBJS_Add ( S_PLASMA_GUNS );
         OBJS_Add ( S_MICRO_MISSLE );
         OBJS_Add ( S_TURRET );
         OBJS_Add ( S_GRD_MISSLE );
         plr.score += 327683;
         break;

      case  2:
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_MEGA_BOMB );
         OBJS_Add ( S_SUPER_SHIELD );
         OBJS_Add ( S_PLASMA_GUNS );
         OBJS_Add ( S_MICRO_MISSLE );
         OBJS_Add ( S_FORWARD_LASER );
         OBJS_Add ( S_GRD_MISSLE );
         plr.score += 876543;
         break;
   }

   OBJS_GetNext();
}

/***************************************************************************
DEMO_DisplayStats (
 ***************************************************************************/
VOID
DEMO_DisplayStats (
VOID
)
{
   CHAR temp[81];

   if ( demo_mode != DEMO_RECORD )  return;

   sprintf ( temp, "REC %d", MAX_DEMO-cur_play );
   GFX_Print ( MAP_LEFT + 5, 20, temp, GLB_GetItem ( FONT2_FNT ), 84 );
}

/***************************************************************************
DEMO_StartRec (
 ***************************************************************************/
VOID
DEMO_StartRec (
VOID
)
{
   SND_Patch ( FX_BONUS, 127 );
   memset ( playback, 0, sizeof ( playback ) );
   demo_mode = DEMO_RECORD;
   cur_play = 1;
}

/***************************************************************************
DEMO_StartPlayback (
 ***************************************************************************/
VOID
DEMO_StartPlayback (
VOID
)
{
   demo_mode = DEMO_PLAYBACK;
   cur_play    = 1;
}

/***************************************************************************
DEMO_SetFileName ()
 ***************************************************************************/
VOID
DEMO_SetFileName (
CHAR * in_name
)
{
   strcpy ( demo_name, in_name );
}

/***************************************************************************
DEMO_GLBFile (
 ***************************************************************************/
VOID
DEMO_GLBFile (
DWORD item
)
{
   BYTE * mem;

   mem = GLB_GetItem ( item );
   if ( !mem ) return;

   memcpy ( playback, mem, sizeof ( playback ) );

   cur_play = 1;

   max_play  = playback[0].playerpic;
   demo_game = playback [0].px;
   demo_wave = playback [0].py;

   GLB_FreeItem ( item );
}

/***************************************************************************
DEMO_LoadFile()
 ***************************************************************************/
VOID
DEMO_LoadFile (
VOID
)
{
   INT   filesize;

   filesize = GLB_ReadFile ( demo_name, NUL );
   GLB_ReadFile ( demo_name, playback );

   cur_play = 1;

   max_play  = playback[0].playerpic;
   demo_game = playback [0].px;
   demo_wave = playback [0].py;
}

/***************************************************************************
DEMO_SaveFile (
 ***************************************************************************/
VOID
DEMO_SaveFile (
VOID
)
{
   if ( cur_play < 2 )  return;

   playback [0].px         = cur_game;
   playback [0].py         = game_wave [ cur_game ];
   playback[0].playerpic   = cur_play;

   GLB_SaveFile ( demo_name, playback, cur_play * sizeof ( RECORD ) );
}

/***************************************************************************
DEMO_Play() - Demo playback routine
 ***************************************************************************/
BOOL           // TRUE=Aborted, FALSE = timeout
DEMO_Play (
VOID
)
{
   extern DWORD   songsg1[];
   extern DWORD   songsg2[];
   extern DWORD   songsg3[];
   BOOL   rval =  FALSE;

   DEMO_StartPlayback();

   cur_game = demo_game;
   game_wave [ cur_game ] = demo_wave;

   DEMO_MakePlayer ( demo_game );

   switch ( cur_game )
   {
      default:
      case 0:
         SND_PlaySong ( songsg1 [ demo_wave ], TRUE, TRUE );
         break;
      case 1:
         SND_PlaySong ( songsg2 [ demo_wave ], TRUE, TRUE );
         break;
      case 2:
         SND_PlaySong ( songsg3 [ demo_wave ], TRUE, TRUE );
         break;
   }

   WIN_LoadComp();
   RAP_LoadMap();
   GFX_SetRetraceFlag ( FALSE );

   rval = Do_Game();

   GFX_SetRetraceFlag ( TRUE );

   if ( ! rval )
   {
      if ( OBJS_GetAmt ( S_ENERGY ) <= 0 )
      {
         SND_PlaySong ( RAP5_MUS, TRUE, TRUE );
         INTRO_Death();
      }
      else
      {
         INTRO_Landing();
      }
   }	
   
   RAP_ClearPlayer();

   demo_mode = DEMO_OFF;

   return ( rval );
}

/***************************************************************************
DEMO_Think (
 ***************************************************************************/
BOOL
DEMO_Think (
VOID
)
{
   INT rval = FALSE;

   switch ( demo_mode )
   {
      default:
         rval = FALSE;
         break;

      case DEMO_PLAYBACK:
         BUT_1 = playback [ cur_play ].b1;
         BUT_2 = playback [ cur_play ].b2;
         BUT_3 = playback [ cur_play ].b3;
         BUT_4 = playback [ cur_play ].b4;
         playerx = playback [ cur_play ].px;
         playery = playback [ cur_play ].py;
         player_cx = playerx + ( PLAYERWIDTH/2 );
         player_cy = playery + ( PLAYERHEIGHT/2 );
         playerpic = playback [ cur_play ].playerpic;
         cur_play++;
         if ( cur_play > max_play )
         {
            demo_mode = DEMO_OFF;
            rval = TRUE;
         }
         break;

      case DEMO_RECORD:
         playback [ cur_play ].b1 = (BYTE)BUT_1;
         playback [ cur_play ].b2 = (BYTE)BUT_2;
         playback [ cur_play ].b3 = (BYTE)BUT_3;
         playback [ cur_play ].b4 = (BYTE)BUT_4;
         playback [ cur_play ].px = (SHORT)playerx;
         playback [ cur_play ].py = (SHORT)playery;
         playback [ cur_play ].playerpic = (SHORT)playerpic;
         cur_play++;
         if ( cur_play == MAX_DEMO )
         {
            SND_Patch ( FX_BONUS, 127 );
            demo_mode = DEMO_OFF;
            rval = TRUE;
         }
         break;
   }

   return ( rval );

}

/*------------------------------------------------------------------------
   IPT_GetButtons () - Reads in Joystick and Keyboard game buttons
  ------------------------------------------------------------------------*/
TSMCALL VOID
IPT_GetButtons (
VOID
)
{
   INT num;

   if ( control == I_JOYSTICK )
   {
      num = inp ( 0x201 );

      num = num >> 4;

      if ( ! ( num & 1 ) )
         buttons [ j_lookup [0] ] = TRUE;
      if ( ! ( num & 2 ) )
         buttons [ j_lookup [1] ] = TRUE;
      if ( ! ( num & 4 ) )
         buttons [ j_lookup [2] ] = TRUE;
      if ( ! ( num & 8 ) )
         buttons [ j_lookup [3] ] = TRUE;
   }

   if ( KBD_Key ( k_Fire ) )
      buttons[0] = TRUE;

   if ( KBD_Key ( k_FireSp ) )
      buttons[1] = TRUE;

   if ( KBD_Key ( k_ChangeSp ) )
      buttons[2] = TRUE;

   if ( KBD_Key ( k_Mega ) )
      buttons[3] = TRUE;
}

/*------------------------------------------------------------------------
IPT_GetJoyStick()
  ------------------------------------------------------------------------*/
PRIVATE VOID
IPT_GetJoyStick (
VOID
)
{
   extern INT joy_sx, joy_sy;
   extern INT joy_limit_xh, joy_limit_xl;
   extern INT joy_limit_yh, joy_limit_yl;

   _disable();
   PTR_ReadJoyStick();
   _enable();

   xm = joy_x - joy_sx;
   ym = joy_y - joy_sy;

   if ( xm < joy_limit_xl || xm > joy_limit_xh )
   {
      if ( xm < 0 )
      {
         if ( g_addx >= 0 )
            g_addx = -1;
         g_addx--;
         if ( -g_addx > MAX_ADDX )
            g_addx = -MAX_ADDX;
      }
      else
      {
         if ( g_addx <= 0 )
            g_addx = 1;
         g_addx++;
         if ( g_addx > MAX_ADDX )
            g_addx = MAX_ADDX;
      }
   }
   else
   {
      if ( g_addx != 0 )
         g_addx /= 2;
   }

   if ( ym < joy_limit_yl || ym > joy_limit_yh )
   {
      if ( ym < 0 )
      {
         if ( g_addy >= 0 )
            g_addy = -1;

         g_addy--;
         if ( -g_addy > MAX_ADDY )
            g_addy = -MAX_ADDY;
      }
      else
      {
         if ( g_addy <= 0 )
            g_addy = 1;

         g_addy++;
         if ( g_addy > MAX_ADDY )
            g_addy = MAX_ADDY;
      }
   }
   else
   {
      if ( g_addy != 0 )
         g_addy /= 2;
   }
}

/*------------------------------------------------------------------------
IPT_GetKeyBoard (
  ------------------------------------------------------------------------*/
PRIVATE VOID
IPT_GetKeyBoard (
VOID
)
{
   if ( KBD_Key ( k_Left ) || KBD_Key ( k_Right ) )
   {
      if ( KBD_Key ( k_Left ) )
      {
         if ( g_addx >= 0 )
            g_addx = -1;
         g_addx--;
         if ( -g_addx > MAX_ADDX )
            g_addx = -MAX_ADDX;
      }
      else if ( KBD_Key ( k_Right ) )
      {
         if ( g_addx <= 0 )
            g_addx = 1;
         g_addx++;
         if ( g_addx > MAX_ADDX )
            g_addx = MAX_ADDX;
      }
   }
   else
   {
      if ( g_addx != 0 )
         g_addx /= 2;
   }

   if ( KBD_Key ( k_Up ) || KBD_Key ( k_Down ) )
   {
      if ( KBD_Key ( k_Up ) )
      {
         if ( g_addy >= 0 )
            g_addy = -1;
         g_addy--;
         if ( -g_addy > MAX_ADDY )
            g_addy = -MAX_ADDY;
      }
      else if ( KBD_Key ( k_Down ) )
      {
         if ( g_addy <= 0 )
            g_addy = 1;
         g_addy++;
         if ( g_addy > MAX_ADDY )
            g_addy = MAX_ADDY;
      }
   }
   else
   {
      if ( g_addy != 0 )
         g_addy /= 2;
   }
}

/*------------------------------------------------------------------------
IPT_GetMouse (
  ------------------------------------------------------------------------*/
PRIVATE VOID
IPT_GetMouse (
VOID
)
{
   INT plx;
   INT ply;
   INT ptrx;
   INT ptry;

   plx = playerx + ( PLAYERWIDTH/2 );
   ply = playery + ( PLAYERHEIGHT/2 );
   
   ptrx = PTR_X;
   ptry = PTR_Y;

   xm = ptrx - plx;
   ym = ptry - ply;

   if ( xm != 0 )
   {
      xm = xm>>3;
  
      if ( xm == 0 )
         xm = 1;
      else if ( xm > 10 )
         xm = 10;
      else if ( xm < -10 )
         xm = -10;
   }

   if ( ym != 0 )
   {
      ym = ym>>3;
  
      if ( ym == 0 )
         ym = 1;
      else if ( ym > 10 )
         ym = 10;
      else if ( ym < -10 )
         ym = -10;
   }

   g_addx = xm;
   g_addy = ym;

   if ( PTR_B1 )
      buttons [ m_lookup [ 0 ] ] = TRUE;

   if ( PTR_B2 )
      buttons [ m_lookup [ 1 ] ] = TRUE;

   if ( PTR_B3 )
      buttons [ m_lookup [ 2 ] ] = TRUE;
}

PRIVATE VOID
WaitJoyButton (
VOID
)
{
   INT num = 0;

   for (;;)
   {
      num = inp ( 0x201 );
      num = num >> 4;

      if ( ( num & 0x0f ) != 0x0f )
         break;
   }

   for(;;)
   {
      num = inp ( 0x201 );
      num = num >> 4;
      
      if ( ( num & 0x0f ) == 0xf )
         break;
   }

   num = inp ( 0x201 );
   num = inp ( 0x201 );

   for(;;)
   {
      num = inp ( 0x201 );
      num = num >> 4;
      
      if ( ( num & 0x0f ) == 0xf )
         break;
   } 
}

/***************************************************************************
IPT_CalJoy() - Calibrates Joystick ( joystick must be centered )
 ***************************************************************************/
VOID
IPT_CalJoy (
VOID
)
{
   extern BOOL godmode;
   extern INT joy_sx, joy_sy;
   extern INT joy_limit_xh, joy_limit_xl;
   extern INT joy_limit_yh, joy_limit_yl;
   INT   xh, yh;
   INT   xl, yl;

   if ( control != I_JOYSTICK )
      return;

   printf ("\n");

   _disable();
   PTR_ReadJoyStick();
   _enable();

   printf ("CENTER Joystick and press a button\n");
   fflush ( stdout );
   WaitJoyButton();
   PTR_ReadJoyStick();
   _disable();
   PTR_ReadJoyStick();
   _enable();
   joy_sx = joy_x;
   joy_sy = joy_y;

   printf ("PUSH the Joystick in the UPPER LEFT and press a button\n");
   fflush ( stdout );
   WaitJoyButton();
   PTR_ReadJoyStick();
   _disable();
   PTR_ReadJoyStick();
   _enable();
   xl = joy_x;
   yl = joy_y;

   printf ("PUSH the Joystick in the LOWER RIGHT and press a button\n");
   fflush ( stdout );
   WaitJoyButton();
   PTR_ReadJoyStick();
   _disable();
   PTR_ReadJoyStick();
   _enable();
   xh = joy_x;
   yh = joy_y;

   if ( godmode )
   {
      printf ("INPUT xh = %d    xl = %d\n", xh, xl );
      printf ("INPUT yh = %d    yl = %d\n", yh, yl );
   }

   joy_limit_xh = abs ( xh - joy_sx ) / 3;
   joy_limit_xl = -( abs ( xl - joy_sx ) / 3 );
   joy_limit_yh = abs ( yh - joy_sy ) / 3;
   joy_limit_yl = -( abs ( yl - joy_sy ) / 3 );

   if ( godmode )
   {
      printf ("LIMIT xh = %d    xl = %d\n", joy_limit_xh, joy_limit_xl );
      printf ("LIMIT yh = %d    yl = %d\n", joy_limit_yh, joy_limit_yl );
      printf ("CENTER sx = %d    sy = %d\n", joy_sx, joy_sy );
   }

   printf ("\n");
   fflush ( stdout );
}

/***************************************************************************
IPT_Init () - Initializes INPUT system
 ***************************************************************************/
VOID
IPT_Init (
VOID
)
{
   tsm_id = TSM_NewService( IPT_GetButtons, 26, 254, 1 );
   IPT_CalJoy();
}

/***************************************************************************
IPT_DeInit() - Freeze up resources used by INPUT system
 ***************************************************************************/
VOID
IPT_DeInit (
VOID
)
{
   TSM_DelService( tsm_id );
}

/***************************************************************************
IPT_Start() - Tranfers control from PTRAPI stuff to IPT stuff
 ***************************************************************************/
VOID
IPT_Start (
VOID
)
{
   PTR_DrawCursor ( FALSE );
   PTR_Pause ( TRUE );
   TSM_ResumeService( tsm_id );
}

/***************************************************************************
IPT_End() - Tranfers control from IPT stuff to PTR stuff
 ***************************************************************************/
VOID
IPT_End (
VOID
)
{
   TSM_PauseService( tsm_id );
   PTR_Pause ( FALSE );
   PTR_DrawCursor ( FALSE );
}

/***************************************************************************
IPT_MovePlayer() - Perform player movement for current input device
 ***************************************************************************/
VOID
IPT_MovePlayer (
VOID
)
{
   PRIVATE INT oldx = PLAYERINITX;
   INT delta;
  
   if ( demo_mode == DEMO_PLAYBACK )
      return;

   if ( !control_pause )
   {
      switch ( control )
      {
         default:
         case I_KEYBOARD:
            IPT_GetKeyBoard();
            break;

         case I_JOYSTICK:
            IPT_GetJoyStick();
            break;

         case I_MOUSE:
            IPT_GetMouse();
            break;
      }
   }

   playerx += g_addx;
   playery += g_addy;

   if ( startendwave == EMPTY )
   {
      if ( playery < MINPLAYERY )
      {
         playery = MINPLAYERY;
         g_addy = 0;
      }
      else if ( playery > MAXPLAYERY )
      {
         playery = MAXPLAYERY;
         g_addy = 0;
      }
  
      if ( playerx < PLAYERMINX )
      {
         playerx = PLAYERMINX;
         g_addx = 0;
      }
      else if ( playerx + PLAYERWIDTH > PLAYERMAXX )
      {
         playerx = PLAYERMAXX-PLAYERWIDTH;
         g_addx = 0;
      }
   }

   delta = abs ( playerx - oldx );
   delta=delta>>2;
  
   if ( delta > 3 )
      delta = 3;
  
   if ( playerx < oldx )
   {
      if ( playerpic < playerbasepic + delta )
         playerpic++;
   }
   else if ( playerx > oldx )
   {
      if ( playerpic > playerbasepic - delta )
         playerpic--;
   }
   else
   {
      if ( playerpic > playerbasepic )
         playerpic--;
      else if ( playerpic < playerbasepic )
         playerpic++;
   }
  
   oldx = playerx;
  
   player_cx = playerx + ( PLAYERWIDTH/2 );
   player_cy = playery + ( PLAYERHEIGHT/2 );
}

/***************************************************************************
IPT_PauseControl() - Lets routines run without letting user control anyting
 ***************************************************************************/
VOID
IPT_PauseControl (
BOOL flag
)
{
   control_pause = flag;
}

/***************************************************************************
IPT_FMovePlayer() - Forces player to move addx/addy
 ***************************************************************************/
VOID
IPT_FMovePlayer (
INT addx,                  // INPUT : add to x pos
INT addy                   // INPUT : add to y pos
)
{
   g_addx   = addx;
   g_addy   = addy;

   IPT_MovePlayer();
}

/***************************************************************************
IPT_LoadPrefs() - Load Input Prefs from setup.ini
 ***************************************************************************/
VOID
IPT_LoadPrefs (
VOID
)
{
   opt_detail     = INI_GetPreferenceLong ( "Setup", "Detail", 1 );
   control        = INI_GetPreferenceLong ( "Setup","Control", 0L );

   k_Up           = INI_GetPreferenceLong ( "Keyboard", "MoveUp", (long)SC_UP );
   k_Down         = INI_GetPreferenceLong ( "Keyboard", "MoveDn", (long)SC_DOWN );
   k_Left         = INI_GetPreferenceLong ( "Keyboard", "MoveLeft", (long)SC_LEFT );
   k_Right        = INI_GetPreferenceLong ( "Keyboard", "MoveRight",(long)SC_RIGHT ); 
   k_Fire         = INI_GetPreferenceLong ( "Keyboard", "Fire", (long)SC_CTRL ); 
   k_FireSp       = INI_GetPreferenceLong ( "Keyboard", "FireSp", (long)SC_ALT ); 
   k_ChangeSp     = INI_GetPreferenceLong ( "Keyboard", "ChangeSp", (long)SC_SPACE ); 
   k_Mega         = INI_GetPreferenceLong ( "Keyboard", "MegaFire", (long)SC_RIGHT_SHIFT ); 

   m_lookup[0]    = INI_GetPreferenceLong ( "Mouse", "Fire", 0L ); 
   m_lookup[1]    = INI_GetPreferenceLong ( "Mouse", "FireSp", 1L ); 
   m_lookup[2]    = INI_GetPreferenceLong ( "Mouse", "ChangeSp", 2L );

   j_lookup[0]    = INI_GetPreferenceLong ( "JoyStick", "Fire", 0L ); 
   j_lookup[1]    = INI_GetPreferenceLong ( "JoyStick", "FireSp", 1L ); 
   j_lookup[2]    = INI_GetPreferenceLong ( "JoyStick", "ChangeSp", 2L ); 
   j_lookup[3]    = INI_GetPreferenceLong ( "JoyStick", "MegaFire", 3L ); 
}

