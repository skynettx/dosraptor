#ifndef _SHADOWS
#define _SHADOWS

extern INT num_shadows;
extern INT num_gshadows;

/***************************************************************************
SHADOW_Draw () - Draws AIR shadows in 3D perspective
 ***************************************************************************/
VOID
SHADOW_Draw (
BYTE * pic,                // INPUT : pointer to sprite data
INT    x,                  // INPUT : x position of sprite
INT    y                   // INPUT : y position of sprite
);

/***************************************************************************
SHADOW_Init() - Allocate memory and set 3D view
 ***************************************************************************/
VOID
SHADOW_Init (
VOID
);

/***************************************************************************
SHADOW_MakeShades() - Make shade tables
 ***************************************************************************/
VOID
SHADOW_MakeShades (
VOID
);

/***************************************************************************
SHADOW_Add() - Add a Air ship shadow
 ***************************************************************************/
VOID  
SHADOW_Add (
DWORD    item,             // INPUT : GLB item
INT      x,                // INPUT : x position
INT      y                 // INPUT : y position
);

/***************************************************************************
SHADOW_GAdd() - Adds Ground shadow
 ***************************************************************************/
VOID  
SHADOW_GAdd (
DWORD    item,             // INPUT : GLB item
INT      x,                // INPUT : x position
INT      y                 // INPUT : y position
);

/***************************************************************************
SHADOW_DisplaySky () - Display Sky Shadows
 ***************************************************************************/
VOID
SHADOW_DisplaySky (
VOID
);

/***************************************************************************
SHADOW_DisplayGround() - Display Ground Shadows
 ***************************************************************************/
VOID
SHADOW_DisplayGround (
VOID
);

VOID SHADOW_Clear ( VOID );

#define SHADOW_Clear()  ( num_shadows = num_gshadows = 0 )

#endif
