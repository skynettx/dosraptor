#ifndef _MOVIE_H
#define _MOVIE_H

typedef enum
{
   M_ANIM,
   M_PIC,
   M_SEE_THRU
}FRAMETYPE;

typedef enum
{
   M_NORM,
   M_FADEIN,
   M_FADEOUT,
   M_PALETTE,
   M_ERASE
}FRAMEOPT;

typedef enum
{
   S_PLAY,
   S_FADEIN,
   S_FADEOUT,
   S_STOP
}SONGOPTS;

typedef struct
{
   FRAMETYPE   opt;             // TYPE OF DRAWING REQUIRED
   INT         framerate;       // FRAME RATE OF UPDATE
   INT         numframes;       // NUMBER OF FRAMES LEFT
   DWORD       item;            // ITEM # OF PICTURE
   FRAMEOPT    startf;          // START FRAME OPTS
   INT         startsteps;      // # OF STEPS IF FADEIN
   FRAMEOPT    endf;            // END FRAME OPTS
   INT         endsteps;        // # OF STEPS IN FADEOUT
   INT         red;             // RED VAL FOR FADEOUT
   INT         green;           // GREEN VAL FOR FADEOUT
   INT         blue;            // BLUE VAL FOR FADEOUT
   INT         holdframe;       // NUMBER OF TICS TO HOLD FRAME
   INT         songid;          // SONG ID TO PLAY
   SONGOPTS    songopt;         // SONG OPTS
   INT         songstep;        // SONG STEPS FOR FADES
   INT         soundfx;         // SOUND FX START
   INT         fx_vol;          // SOUND FX VOLUME
   INT         fx_xpos;         // SOUND FX XPOS
}FRAME;

/*************************************************************************
MOVIE_BPatch() Plays Sound FX in background for one anim
 *************************************************************************/
VOID
MOVIE_BPatch (
INT soundfx
);

/*************************************************************************
   MOVIE_ShowFrame () - Shows an animation frame
 *************************************************************************/
VOID
MOVIE_ShowFrame (
BYTE * inpic               // INPUT : pointer to animpic
);

/*************************************************************************
MOVIE_Play () - Playes an Animation
 *************************************************************************/
INT
MOVIE_Play (
FRAME * frame,             // INPUT : pointer to array of frame defs
INT     numplay,           // INPUT : number of times to play
BYTE * palette             // INPUT : pointer to palette
);

/*************************************************************************
ANIM_Render () - Renders an ANIM FRAME 
 *************************************************************************/
#pragma aux ANIM_Render "_*" modify [ EAX EBX ECX EDX ESI EDI ]
VOID
ANIM_Render ( 
BYTE * inmem 
);

#endif
