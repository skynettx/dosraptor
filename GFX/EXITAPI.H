/****************************************************************************
* EXITapi  - Program Exiting routines
*----------------------------------------------------------------------------
* Copyright (C) 1992  Scott Hostynski All Rights Reserved
*----------------------------------------------------------------------------
*
* Created by:  Scott Host
* Date:        Oct, 1992
*
* CONTENTS: exitapi.c exitapi.h
*
* EXTERN MODULES - Main module of your program
*
* NOTES: Use EXIT_Install ( function pointer ) and set it to a routine
*        that Shuts down all your systems
*---------------------------------------------------------------------------*/
  
#ifndef _TYPES_H
#include "types.h"
#endif
  
#ifndef _EXITAPI
#define _EXITAPI

#undef	ASSERT

#ifdef __386__
#ifdef DEBUG
void EXIT_Assert( char *, char *, unsigned );
#define ASSERT(exp) \
	( (exp) ? (void) 0 : EXIT_Assert(#exp, __FILE__, __LINE__) )
#else
#define ASSERT(__ignore) ((void)0)
#endif

#else  
#ifdef DEBUG
#define ASSERT(exp) \
	( (exp) ? (void) 0 : EXIT_Assert(#exp, __FILE__, __LINE__) )
#else
#define ASSERT(exp)	((void)0)
#endif
#endif  

/***************************************************************************
   EXIT_Install() Sets User routine to shut down systems
 ***************************************************************************/
VOID
EXIT_Install(
VOID (*SD)(INT)           // INPUT : pointer to function
);
  
/***************************************************************************
   EXIT_Error() - Terminate Program in Error with message
 ***************************************************************************/
VOID
EXIT_Error(
char *instr,               // INPUT : message string ot format
...                        // INPUT : args for instr
);
  
/***************************************************************************
   EXIT_Clean() - Terminate Program Clean
 ***************************************************************************/
VOID
EXIT_Clean(
VOID
);
  
#endif

