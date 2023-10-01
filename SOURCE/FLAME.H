#ifndef _TYPES_H
#include "types.h"
#endif

#ifndef _FLAMES
#define _FLAMES

/***************************************************************************
FLAME_Init () - Inits Flame Tables and stuff
 ***************************************************************************/
VOID
FLAME_Init (
VOID
);

/***************************************************************************
FLAME_InitShades () - Inits shading stuff
 ***************************************************************************/
VOID
FLAME_InitShades (
VOID
);

/***************************************************************************
FLAME_Up () - Shows Flame shooting upward
 ***************************************************************************/
VOID
FLAME_Up (
INT ix,                    // INPUT : x position
INT iy,                    // INPUT : y position
INT width,                 // INPUT : width of shade
INT frame                  // INPUT : frame
);

/***************************************************************************
FLAME_Down () - Shows Flame shooting downward
 ***************************************************************************/
VOID
FLAME_Down (
INT ix,                    // INPUT : x position
INT iy,                    // INPUT : y position
INT width,                 // INPUT : width of shade
INT frame                  // INPUT : frame
);

#endif
