#define MAX_FLIGHT      30

#define MAP_ROWS        150
#define MAP_ONSCREEN    8
#define MAP_COLS        9
#define MAP_BLOCKSIZE   32
#define MAP_BOTTOM      200 - 18
#define MAP_TOP         2
#define MAP_LEFT        16
#define MAP_RIGHT       ( 320 - 16 )
#define MAP_SIZE        ( MAP_ROWS * MAP_COLS )

#define MAX_TILES       2048
#define MAX_GUNS        24

#define ENGINE_COLOR    254
#define SHOT_COLOR      255

typedef struct
{
   INT   x;
   INT   y;
   INT   x2;
   INT   y2;
   INT   delx;
   INT   dely;
   INT   addx;
   INT   addy;
   INT   maxloop;
   INT   err;
   BOOL  done;
}MOVEOBJ;

// ENEMY SHIPS
// SHOOT
// MOVE
// CRASH INTO PLAYER
// ANIMATED
// TAKE DAMAGE
// SKY/GROUND

typedef enum
{
   F_REPEAT,
   F_LINEAR,
   F_KAMI,
   F_GROUND,
   F_GROUNDLEFT,
   F_GROUNDRIGHT,
}FLIGHT_TYPE;

typedef struct
{
   INT          linkflat;
   SHORT        bonus;
   SHORT        bounty;
}FLATS;                 

typedef struct
{
   SHORT        flats;
   SHORT        fgame;
}MAZEDATA;               

typedef struct
{
   DWORD       sizerec;
   DWORD       spriteoff;
   INT         numsprites;
   MAZEDATA    map[ MAP_SIZE ];
}MAZELEVEL;

typedef enum
{
   GANIM_NORM,
   GANIM_SHOOT,
   GANIM_MULTI
}ANIMTYPE;

typedef enum
{
   EXP_AIRSMALL1, // 0  
   EXP_AIRMED,    // 1 NORMAL AIR    ( norm )
   EXP_AIRLARGE,  // 2 bigger that 32 by 32
   EXP_GRDSMALL,  // 3 smaller than 32 by 32          
   EXP_GRDMED,    // 4 NORMAL GROUND ( norm )
   EXP_GRDLARGE,  // 5 bigger than 32 by 32
   EXP_BOSS,      // 6 BIG BOSS SHIP ( AIR ) 
   EXP_PERSON,    // 7 ONE LITTLE PERSON BLWING UP
   EXP_ENERGY,    // 8 ENERGY SHIP BLOWING UP
   EXP_PLATOON,   // 9 ONE LITTLE PERSON BLWING UP
   EXP_AIRSMALL2  // 10  
}EXP_TYPE;

typedef struct
{
   CHAR        iname[16];              // ITEM NAME
   DWORD       item;                   // * GLB ITEM #
   INT         bonus;                  // BONUS # ( -1 == NONE )
   EXP_TYPE    exptype;                // EXPLOSION TYPE           
   INT         shotspace;              // SLOWDOWN SPEED
   BOOL        ground;                 // IS ON GROUND
   BOOL        suck;                   // CAN SUCK WEAPON AFFECT 
   INT         frame_rate;             // FRAME RATE
   INT         num_frames;             // NUM FRAMES
   INT         countdown;              // COUNT DOWN TO START ANIM
   INT         rewind;                 // FRAMES TO REWIND
   ANIMTYPE    animtype;               // FREE SPACE FOR LATER USE
   BOOL        shadow;                 // USE SHADOW ( TRUE/FALSE )
   INT         bossflag;               // SHOULD THIS BE CONSIDERED A BOSS
   INT         hits;                   // HIT POINTS
   INT         money;                  // $$ AMOUNT WHEN KILLED
   INT         shootstart;             // SHOOT START OFFSET
   INT         shootcnt;               // HOW MANY TO SHOOT
   INT         shootframe;             // FRAME RATE TO SHOOT
   INT         movespeed;              // MOVEMENT SPEED
   INT         numflight;              // NUMBER OF FLIGHT POSITIONS
   INT         repos;                  // REPEAT TO POSITION
   FLIGHT_TYPE flighttype;             // FLIGHT TYPE
   INT         numguns;                // NUMBER OF GUNS
   INT         numengs;                // NUMBER OF ENGINES
   INT         sfx;                    // SFX # TO PLAY
   INT         song;                   // SONG # TO PLAY
   SHORT       shoot_type[MAX_GUNS];   // ENEMY SHOOT TYPE
   SHORT       engx[MAX_GUNS];         // X POS ENGINE FLAME
   SHORT       engy[MAX_GUNS];         // Y POS ENGINE FLAME
   SHORT       englx[MAX_GUNS];        // WIDTH OF ENGINE FLAME
   SHORT       shootx[MAX_GUNS];       // X POS SHOOT FROM
   SHORT       shooty[MAX_GUNS];       // Y POS SHOOT FROM
   SHORT       flightx[MAX_FLIGHT];    // FLIGHT X POS
   SHORT       flighty[MAX_FLIGHT];    // FLIGHT Y POS
}SPRITE;

typedef struct
{
   INT    link;
   INT    slib;
   INT    x;
   INT    y;
   INT    game;
   DWORD level;
}CSPRITE;

