#ifndef _PUBLIC
#define _PUBLIC

#define DMX_FLAG       1

typedef enum
{
   K_OK,
   K_NEXTFRAME,
   K_SKIPALL,
   K_EXITDOS
}KEYOPTS;

typedef enum
{
   R_YESNO,
   R_QUANTITY
}ASKBUY;

typedef struct
{
   BYTE  b1;
   BYTE  b2;
   BYTE  b3;
   BYTE  b4;
   SHORT px;
   SHORT py;
   SHORT playerpic;
   SHORT fil;
}RECORD;

typedef struct
{
   CHAR        name [ 20 ];
   CHAR        callsign [ 12 ];
   INT         id_pic;
   DWORD       score;                  // MONEY/POINTS
   INT         sweapon;                // CURENT SPECIAL WEAPON
   INT         cur_game;               // CURRENT GAME
   INT         game_wave[3];           // CURRENT WAVE
   INT         numobjs;                // NUMBER OF OBJECTS TO FOLLOW
   INT         diff[4];                // DIFFICULTY LEVEL
   BOOL        trainflag;
   BOOL        fintrain;
}PLAYEROBJ;

#define             END_DURATION ( 20 * 3 )
#define             END_EXPLODE  ( 24 )
#define             END_FLYOFF   ( 20 * 2 )

#define  DIFF_0   0  // TRAINING MODE
#define  DIFF_1   1  // EASY MODE
#define  DIFF_2   2  // NORMAL MODE
#define  DIFF_3   3  // HARD MODE

#define GAME1 ( gameflag[0] )
#define GAME2 ( gameflag[1] )
#define GAME3 ( gameflag[2] + gameflag[3] )

#define SHIELD_COLOR_RUN 9
#define SHIELD_LOW       10

#define PLAYERWIDTH  32
#define PLAYERHEIGHT 32
#define PLAYERMINX   5
#define PLAYERMAXX   314
#define PLAYERINITX  160-(PLAYERWIDTH/2)
#define PLAYERINITY  160

#define MINPLAYERY   0
#define MAXPLAYERY   160
  
#define GREEN        100
#define RED          34
#define YELLOW       50
  
#define MAP_ABORT    -1
#define MAP_NEXT     -2

//#define MAX_BADGUYS  300
#define MAX_ONSCREEN 30

/***************************************************************************
   InitMobj() - Inits an object to be moved
 ***************************************************************************/
VOID
InitMobj(
MOVEOBJ * cur              // INPUT : pointer to MOVEOBJ
);

/***************************************************************************
   MoveMobj() - gets next postion for an Object
 ***************************************************************************/
VOID
MoveMobj(
MOVEOBJ * cur              // INPUT : pointer to MOVEOBJ
);

/***************************************************************************
   MoveSobj() - gets next postion for an Object at speed
 ***************************************************************************/
INT
MoveSobj(
MOVEOBJ * cur,             // INPUT : pointer to MOVEOBJ
INT       speed            // INPUT : speed to plot at
);

/***************************************************************************
AskExit () - Opens Windows and Askes if USer wants 2 quit
 ***************************************************************************/
VOID
AskExit (
VOID
);

/***************************************************************************
Do_Game () - The main game thing this is it dude
 ***************************************************************************/
BOOL           // TRUE=Aborted, FALSE = timeout
Do_Game (
VOID
);

extern PLAYEROBJ     plr               ;
extern INT           choose_special [] ;
extern INT           cur_special   ;
extern INT           player_cx     ;
extern INT           player_cy     ;
extern INT           playerx       ;
extern INT           playery       ;
extern INT           playerbasepic ;
extern INT           playerpic     ;
extern INT           numbadguys    ;
extern INT           cur_game      ;
extern INT           game_wave[4]  ;
extern MAZELEVEL *   ml            ;
  
extern BYTE * palette;
extern INT    o_engine [];
extern INT    o_gun1 [];
extern INT    o_gun2 [];
extern INT    o_gun3 [];
extern FLATS * flatlib [];
extern BOOL   startfadeflag;
extern BYTE * patch1;  
extern BYTE * patch2;  
extern BOOL   end_wave;
extern INT    gl_cnt;
extern BOOL   testflag;
extern BOOL   gameflag[];
extern INT    startendwave;
extern BOOL   reg_flag;
extern BOOL   lowmem_flag;
extern INT    curplr_diff;
extern BOOL   bday_flag;
extern INT    bday_num;
extern INT    demo_flag;

#endif

