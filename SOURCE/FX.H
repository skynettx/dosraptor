#ifndef _FXAPI
#define _FXAPI

#define SND_CLOSE    40
#define SND_FAR      500

typedef enum
{
   SND_NONE,
   SND_PC,
   SND_MIDI,
   SND_CANVAS,
   SND_DIGITAL
}SND_TYPE;

typedef enum
{
   FX_THEME,
   FX_MON1,
   FX_MON2,
   FX_MON3,
   FX_MON4,
   FX_MON5,
   FX_MON6,
   FX_DAVE,
   FX_AIREXPLO,
   FX_AIREXPLO2,
   FX_BONUS,
   FX_CRASH,
   FX_DOOR,
   FX_FLYBY,
   FX_EGRAB,
   FX_GEXPLO,
   FX_GUN,
   FX_JETSND,
   FX_LASER,
   FX_MISSLE,
   FX_SWEP,
   FX_TURRET,
   FX_WARNING,
   FX_BOSS1,
   FX_IJETSND,
   FX_EJETSND,
   FX_INTROHIT,
   FX_INTROGUN,
   FX_ENEMYSHOT,
   FX_ENEMYLASER,
   FX_ENEMYMISSLE,
   FX_ENEMYPLASMA,
   FX_SHIT,
   FX_HIT,
   FX_NOSHOOT,
   FX_PULSE,
   FX_LAST_SND
} DEFX;

typedef enum
{
   C_KEY,
   C_MOUSE,
   C_JOYSTICK,
   C_LAST
}CONTROL;

/***************************************************************************
SND_Setup() - Inits SND System  called after SND_InitSound() and GLB_Init
 ***************************************************************************/
VOID
SND_Setup (
VOID
);

/***************************************************************************
SND_CacheFX () _ Caches FX's
 ***************************************************************************/
VOID
SND_CacheFX (
VOID
);

/***************************************************************************
SND_CacheGFX () _ Caches in Game FX's
 ***************************************************************************/
VOID
SND_CacheGFX (
VOID
);

/***************************************************************************
SND_CacheIFX () _ Caches intro and menu FX
 ***************************************************************************/
VOID
SND_CacheIFX (
VOID
);

/***************************************************************************
SND_FreeFX () - Frees up Fx's
 ***************************************************************************/
VOID
SND_FreeFX (
VOID
);

/***************************************************************************
SND_IsPatchPlaying() - Returns TRUE if patch is playing
 ***************************************************************************/
BOOL
SND_IsPatchPlaying (
DEFX   type                  // INPUT : position in fxitems
);

/***************************************************************************
SND_StopPatches () - STops all currently playing patches
 ***************************************************************************/
VOID
SND_StopPatches (
VOID
);

/***************************************************************************
SND_StopPatch () - STops Type patche
 ***************************************************************************/
VOID
SND_StopPatch (
DEFX  type                 // INPUT : DFX type patch to play
);

/***************************************************************************
SND_Patch () - Test patch to see if it will be played by SND_Play
 ***************************************************************************/
VOID
SND_Patch (
DEFX  type,                // INPUT : DFX type patch to play
INT   xpos                 // INPUT : 127=center
);

/***************************************************************************
SND_3DPatch () - playes a patch in 3d for player during game play
 ***************************************************************************/
VOID
SND_3DPatch (
DEFX  type,                // INPUT : DFX type patch to play
INT   x,                   // INPUT : x sprite center
INT   y                    // INPUT : y sprite center
);

/***************************************************************************
SND_PlaySong() - Plays song associated with song id
 ***************************************************************************/
VOID
SND_PlaySong (
DWORD item,                // INPUT : Song GLB item
BOOL  chainflag,           // INPUT : Chain Song to ItSelf
BOOL  fadeflag             // INPUT : Fade Song Out
);

/***************************************************************************
SND_IsSongPlaying () - Is current song playing
 ***************************************************************************/
BOOL
SND_IsSongPlaying (
VOID
);

/***************************************************************************
SND_FadeOutSong () - Fades current song out and stops playing music
 ***************************************************************************/
VOID
SND_FadeOutSong (
VOID
);

/***************************************************************************
SND_InitSound () - Does bout all i can think of for Music/FX initing
 ***************************************************************************/
VOID
SND_InitSound (
VOID
);

extern CONTROL control;
extern      INT   music_volume;
extern      INT   fx_volume;
extern      BOOL  fx_flag;
extern      BOOL  dig_flag;

#endif




























