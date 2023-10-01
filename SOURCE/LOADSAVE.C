#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "prefapi.h"

#include "file0001.inc"
  
#include "raptor.h"
#include "load.inc"

#define MAX_SAVE  10

PRIVATE  CHAR * fmt        = "CHAR%04u.FIL";
PRIVATE  CHAR * cdfmt      = "%s\\CHAR%04u.FIL";
PRIVATE  CHAR * pogpath    = "APOGEECD";
PRIVATE  INT    filepos    = EMPTY;
PRIVATE  DWORD  map_item   = EMPTY;
PRIVATE  BYTE * mapmem;
PRIVATE  BYTE   cdflag     = FALSE;
PRIVATE  CHAR   cdpath[33];
PRIVATE  CHAR   g_setup_ini[66];

/***************************************************************************
RAP_SetPlayerDiff () - Set Player Difficulty
 ***************************************************************************/
VOID
RAP_SetPlayerDiff (
VOID
)
{
   cur_diff = 0;

   curplr_diff = plr.diff [ cur_game ];

   switch ( curplr_diff )
   {
      case DIFF_0:
         cur_diff |= EB_EASY_LEVEL;
         break;

      case DIFF_1:
         cur_diff |= EB_EASY_LEVEL;
         break;

      default:
      case DIFF_2:
         cur_diff |= EB_EASY_LEVEL;
         cur_diff |= EB_MED_LEVEL;
         break;

      case DIFF_3:
         cur_diff |= EB_EASY_LEVEL;
         cur_diff |= EB_MED_LEVEL;
         cur_diff |= EB_HARD_LEVEL;
         break;
   }
}

/***************************************************************************
RAP_ClearPlayer () - Clear Player stuff
 ***************************************************************************/
VOID
RAP_ClearPlayer (
VOID
)
{
   OBJS_Clear();
   filepos = EMPTY;
   memset ( &plr, 0, sizeof ( PLAYEROBJ ) );
   plr.sweapon = EMPTY;
   plr.diff[0] = DIFF_2;
   plr.diff[1] = DIFF_2;
   plr.diff[2] = DIFF_2;
   plr.diff[3] = DIFF_2;
   plr.fintrain = FALSE;
   cur_game = 0;
   memset ( game_wave, 0, sizeof ( game_wave ) );
}

/***************************************************************************
RAP_IsPlayer () - Returns TRUE if a player is defined
 ***************************************************************************/
BOOL
RAP_IsPlayer (
VOID
)
{
   if ( filepos != EMPTY )
      return ( TRUE );
   else
      return ( FALSE );
}

/***************************************************************************
RAP_AreSavedFiles() - Returns TRUE if thier are previously saved game files
 ***************************************************************************/
BOOL
RAP_AreSavedFiles (
VOID
)
{
   CHAR temp[41];
   INT  loop;

   for ( loop = 0; loop < MAX_SAVE; loop++ )
   {
      if ( cdflag )
         sprintf ( temp, cdfmt, cdpath, loop );
      else
         sprintf ( temp, fmt, loop );

      if ( access ( temp, 0 ) == 0 )
      {
         return ( TRUE );
      }
   }

   return ( FALSE );
}

/***************************************************************************
RAP_ReadFile() - Reads file into buffer for sizerec and DECRYTES
 ***************************************************************************/
PRIVATE INT                         // RETURN: size of record
RAP_ReadFile (
CHAR *name,                 // INPUT : filename
VOID *buffer,               // OUTPUT: pointer to buffer
INT   sizerec               // INPUT : number of bytes to read
)
{
   extern  CHAR   gdmodestr[];
   INT     handle;

   if ( ( handle = open ( name,O_RDONLY | O_BINARY ) ) == -1 )
   {
      WIN_Msg ( "File open Error");
      return ( 0 );
   }
  
   read ( handle, buffer, sizerec );

   GLB_DeCrypt ( gdmodestr, buffer ,sizerec );

   close (handle);
  
   return ( sizerec );
}

/***************************************************************************
RAP_FFSaveFile() - Finds a filename to use
 ***************************************************************************/
BOOL
RAP_FFSaveFile (
VOID
)
{
   CHAR temp[41];
   INT  loop;
   BOOL rval = FALSE;

   filepos = EMPTY;

   for ( loop = 0; loop < MAX_SAVE; loop++ )
   {
      if ( cdflag )
         sprintf ( temp, cdfmt, cdpath, loop );
      else
         sprintf ( temp, fmt, loop );

      if ( access ( temp, 0 ) != 0 )
      {
         RAP_ClearPlayer();
         filepos = loop;
         rval = TRUE;
         break;
      }
   }

   return ( rval );
}

/***************************************************************************
RAP_IsSaveFile() - Returns True if thier is a sopt to save a character
 ***************************************************************************/
BOOL
RAP_IsSaveFile (
PLAYEROBJ * in_plr
)
{
   PLAYEROBJ   tp;
   CHAR        temp[41];
   INT         loop;
   BOOL        rval = FALSE;
   INT         handle;

   for ( loop = 0; loop < MAX_SAVE; loop++ )
   {
      if ( cdflag )
         sprintf ( temp, cdfmt, cdpath, loop );
      else
         sprintf ( temp, fmt, loop );

      if ( ( handle = open ( temp, O_RDONLY | O_BINARY ) ) != -1 )
      {
         read ( handle, &tp, sizeof ( PLAYEROBJ ) );
         if ( strcmpi ( tp.name, in_plr->name ) == 0 &&
              strcmpi ( tp.callsign, in_plr->callsign ) == 0 )
         {
            rval = TRUE;
            break;
         }
      }
   }

   close ( handle );

   return ( rval );
}

/***************************************************************************
RAP_LoadPlayer () - Loads player from disk
 ***************************************************************************/
BOOL
RAP_LoadPlayer (
VOID
)
{
   extern  CHAR   gdmodestr[];
   CHAR    filename[41];
   INT     handle;
   INT     loop;
   INT     rval = FALSE;
   OBJ     inobj;

   if ( filepos == EMPTY )
      return ( FALSE );

   // == Clear Player =======================
   OBJS_Clear();
   memset ( &plr, 0, sizeof ( PLAYEROBJ ) );

   if ( cdflag )
      sprintf ( filename, cdfmt, cdpath, filepos );
   else
      sprintf ( filename, fmt, filepos );

   if ( ( handle = open ( filename, O_RDONLY | O_BINARY ) ) == -1 )
   {
      WIN_Msg ( "Load Player Error");
      return ( FALSE );
   }
   
   read ( handle, &plr, sizeof ( PLAYEROBJ ) );
   GLB_DeCrypt ( gdmodestr,(VOID*)&plr ,sizeof ( PLAYEROBJ ) );

   for ( loop = 0; loop < plr.numobjs; loop++ )
   {
      read ( handle, &inobj, sizeof ( OBJ ) );
      GLB_DeCrypt ( gdmodestr, (VOID*)&inobj, sizeof ( OBJ ) );

      if ( ! OBJS_Load ( &inobj ) )
         break;
   }

   close ( handle );

   cur_game = plr.cur_game;
   game_wave [ 0 ] = plr.game_wave [ 0 ];
   game_wave [ 1 ] = plr.game_wave [ 1 ];
   game_wave [ 2 ] = plr.game_wave [ 2 ];

   if ( !OBJS_IsEquip ( plr.sweapon ) )
   {
      OBJS_GetNext();
   }

   if ( OBJS_GetAmt ( S_ENERGY ) <= 0 )
   {
      EXIT_Error("RAP_LoadPLayer() - Loaded DEAD player");
   }

   rval = TRUE;
   RAP_SetPlayerDiff();

   return ( rval );
}

/***************************************************************************
RAP_SavePlayer() - Saves player data to filename
 ***************************************************************************/
BOOL
RAP_SavePlayer (
VOID
)
{
   extern  CHAR   gdmodestr[];
   extern OBJ     first_objs;
   extern OBJ     last_objs;
   CHAR           filename[41];
   INT            handle;
   INT            rval = FALSE;
   OBJ *          cur;

   if ( filepos == EMPTY )
      EXIT_Error("RAP_Save() ERR: Try to Save Invalid Player");

   if ( OBJS_GetAmt ( S_ENERGY ) <= 0 )
      EXIT_Error("RAP_Save() ERR: Try to save Dead player");

   if ( cdflag )
      sprintf ( filename, cdfmt, cdpath, filepos );
   else
      sprintf ( filename, fmt, filepos );

   if ( ( handle = open ( filename, O_BINARY | O_WRONLY | O_CREAT |
      O_TRUNC,S_IREAD | S_IWRITE ) ) == -1 )
   {
      WIN_Msg ( "Save Player Error !!!");
      return ( FALSE );
   }

   plr.cur_game        = cur_game;
   plr.game_wave [ 0 ] = game_wave [ 0 ];
   plr.game_wave [ 1 ] = game_wave [ 1 ];
   plr.game_wave [ 2 ] = game_wave [ 2 ];
   plr.numobjs         = 0;

   for ( cur=first_objs.next; cur!=&last_objs; cur=cur->next )
      plr.numobjs++;

   GLB_EnCrypt ( gdmodestr, (VOID*)&plr ,sizeof ( PLAYEROBJ ) );
   write ( handle, &plr, sizeof ( PLAYEROBJ ) );
   GLB_DeCrypt ( gdmodestr, (VOID*)&plr ,sizeof ( PLAYEROBJ ) );

   for ( cur=first_objs.next; cur!=&last_objs; cur=cur->next )
   {
      GLB_EnCrypt ( gdmodestr, (VOID*)cur, sizeof ( OBJ ) );
      write ( handle, cur, sizeof ( OBJ ) );
      GLB_DeCrypt ( gdmodestr, (VOID*)cur, sizeof ( OBJ ) );
   }

   rval = TRUE;

   close ( handle );

   return ( rval );
}

/***************************************************************************
 RAP_LoadMap () - Loads A level Map
 ***************************************************************************/
VOID
RAP_LoadMap (
VOID
)
{
   CHAR           temp[42];

   if ( !gameflag [ cur_game ] )
      EXIT_Error ("Loading Invalid map game %d", cur_game );

   GLB_FreeAll();
  
   sprintf ( temp, "MAP%uG%u_MAP", game_wave [ cur_game ] + 1, cur_game + 1 );
   map_item = GLB_GetItemID ( temp );

   if ( map_item == EMPTY )
      EXIT_Error ("RAP_LoadMap() - Invalid MAP.(%s)", temp );

   mapmem = GLB_LockItem ( map_item );

   ml       = ( MAZELEVEL * ) mapmem;
   csprite  = ( CSPRITE * ) ( mapmem + sizeof ( MAZELEVEL ) );
  
   ENEMY_LoadLib();
   SND_CacheGFX();
   BONUS_Init();
   WIN_SetLoadLevel ( 20 );
   OBJS_CachePics();
   WIN_SetLoadLevel ( 40 );
   ANIMS_CachePics();
   WIN_SetLoadLevel ( 60 );
   ENEMY_LoadSprites();
   WIN_SetLoadLevel ( 80 );
   TILE_CacheLevel();
   WIN_SetLoadLevel ( 100 );
   WIN_EndLoad();
}

/***************************************************************************
RAP_FreeMap() - Frees up cached map stuff
 ***************************************************************************/
VOID
RAP_FreeMap (
VOID
)
{
   if ( map_item != EMPTY )
   {
      TILE_FreeLevel();
      ENEMY_FreeSprites();
      ANIMS_FreePics();
      OBJS_FreePics();
      SND_FreeFX();

      // FREE MAP ========================

      GLB_FreeItem ( map_item );

      map_item = EMPTY;
   }

   GLB_FreeAll();
   SND_CacheIFX();
}
  
/***************************************************************************
RAP_LoadWin() - 
 ***************************************************************************/
INT                        // RETURN : -1 = no FIles, 0=cancel, 1=loaded
RAP_LoadWin(
VOID
)
{
   CHAR           temp[41];
   CHAR           filenames[MAX_SAVE][33];
   PLAYEROBJ      tplr;
   SWD_DLG        dlg;
   INT            window;
   BOOL           update = TRUE;
   INT            loop;
   INT            pos = EMPTY;
   INT            oldpos = -2;
   INT            addnum;
   BOOL           fndflag = FALSE;
   BOOL           rval = 0;

   memset ( filenames, 0, sizeof ( filenames ) );
   for ( loop = 0; loop < MAX_SAVE; loop++ )
   {
      if ( cdflag )
         sprintf ( temp, cdfmt, cdpath, loop );
      else
         sprintf ( temp, fmt, loop );

      if ( access ( temp, 0 ) == 0 )
      {
         if ( pos == EMPTY )
         {
            pos      = loop;
         }
         strncpy ( filenames [ loop ], temp, 66 );
      }
   }

   if ( pos == EMPTY )
      return ( -1 );

   RAP_ReadFile ( filenames [ pos ], &tplr, sizeof ( PLAYEROBJ ) );
   KBD_Clear();
   window = SWD_InitWindow ( LOAD_SWD );
   SWD_SetActiveField ( window, LOAD_LOAD );
   SND_Patch ( FX_SWEP, 127 );
  
   mainloop:
  
   SWD_Dialog ( &dlg );

   if ( KBD_IsKey ( SC_ESC ) )
   {
      rval = 0;
      goto load_exit;
   }

   if ( KBD_Key ( SC_X ) && KBD_Key ( SC_ALT ) )
   {
      WIN_AskExit();
   }

   if ( update )
   {
      update = FALSE;
      if ( pos != oldpos )
      {
         if ( pos < oldpos )
            addnum = -1;
         else
            addnum = 1;

         if ( pos >= 0 )
            pos = pos % MAX_SAVE;
         else
            pos = MAX_SAVE + pos;

         if ( pos < 0 )
            EXIT_Error ("Help");

         fndflag = FALSE;
         for ( loop = 0; loop < MAX_SAVE; loop++ )
         {
            if  ( filenames [ pos ][ 0 ] == NULL )
            {
               pos += addnum;

               if ( pos >= 0 )
                  pos = pos % MAX_SAVE;
               else
                  pos = MAX_SAVE + pos;
            }
            else
            {
               fndflag = TRUE;
               break;
            }
         }

         if ( fndflag == FALSE )
         {
            rval = -1;
            goto load_exit;
         }

         RAP_ReadFile ( filenames [ pos ], &tplr, sizeof ( PLAYEROBJ ) );
         oldpos = pos;
      }

      SWD_SetFieldItem ( window, LOAD_IDPIC, id_pics [ tplr.id_pic ] );
      SWD_SetFieldText ( window, LOAD_NAME, tplr.name );
      SWD_SetFieldText ( window, LOAD_CALL, tplr.callsign );
      sprintf ( temp, "%07u", tplr.score );
      SWD_SetFieldText ( window, LOAD_CREDITS, temp );
      SWD_ShowAllWindows();
      GFX_DisplayUpdate();
      SND_Patch ( FX_SWEP, 127 );
   }

   switch ( dlg.keypress )
   {
      case SC_LEFT:
      case SC_PAGEDN:
      case SC_DOWN:
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = LOAD_NEXT;
         break;

      case SC_RIGHT:
      case SC_PAGEUP:
      case SC_UP:
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = LOAD_PREV;
         break;

      case SC_DELETE:
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = LOAD_DEL;
         break;

      case SC_ENTER:
         dlg.cur_act = S_FLD_COMMAND;
         dlg.cur_cmd = F_SELECT;
         dlg.field   = LOAD_LOAD;
         break;
   }

   switch ( dlg.cur_act )
   {
      case S_FLD_COMMAND:
      switch ( dlg.cur_cmd )
      {
         case F_SELECT:
         switch ( dlg.field )
         {
            case LOAD_NEXT:
               pos++;
               update = TRUE;
               break;

            case LOAD_PREV:
               pos--;
               update = TRUE;
               break;

            case LOAD_DEL:
               update = TRUE;
               sprintf ( temp, "Delete Pilot %s ?", tplr.callsign );
               if ( WIN_AskBool ( temp ) )
               {
                  remove ( filenames[pos] );
                  WIN_Msg ("Pilot Removed !");
                  filenames [ pos ][ 0 ] = NUL; 
                  pos++;
               }
               break;

            case LOAD_CANCEL:
               goto load_exit;

            case LOAD_LOAD:
               filepos = pos;
               RAP_LoadPlayer();
               rval = 1;
               goto load_exit;
         }
      }
   }

   goto mainloop;

   load_exit:
  
   SWD_DestroyWindow ( window );
   SWD_ShowAllWindows();
   GFX_DisplayUpdate();
  
   return ( rval );
}

/***************************************************************************
RAP_InitLoadSave() - Inits the load and save path stuff
 ***************************************************************************/
CHAR *
RAP_InitLoadSave (
VOID
)
{
   BYTE * var1;
   CHAR * n1 = "setup.ini";

   var1 = getenv ( pogpath );

   strncpy ( cdpath, var1, 32 );

   if ( access ( cdpath, F_OK ) == 0 )
      cdflag = TRUE;
   else
      cdflag = FALSE;

   if ( cdflag )
      printf ("Data Path = %s\n", cdpath );

   if ( cdflag )
      sprintf ( g_setup_ini, "%s\\%s", cdpath, n1 );
   else
      strcpy ( g_setup_ini , n1 );

   return ( cdpath );
}

/***************************************************************************
RAP_SetupFilename() - Gets current setup.ini path and name
 ***************************************************************************/
CHAR *
RAP_SetupFilename (
VOID
)
{
   return ( g_setup_ini );
}
