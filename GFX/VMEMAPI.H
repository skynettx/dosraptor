/****************************************************************************
* VMEMapi  - Virtual Memory Routines
*----------------------------------------------------------------------------
*
* Created by:  Paul Radek
* Date:        Nov, 1993
*
* CONTENTS:glbapi.c glbapi.h
*
* EXTERN MODULES - Main module of your program
*
* NOTES:
*
*---------------------------------------------------------------------------*/
  
#ifndef _TYPES_H
#include "types.h"
#endif
  
#ifndef _VMEMAPI
#define _VMEMAPI

typedef struct
{
	BYTE		*obj;
	DWORD		age;
//	DWORD		id;
} VM_OWNER;


/*************************************************************************
 VM_InitMemory - Assign memory block to be used as virtual memory.
               - Can be called multiple times to add additional memory.
 *************************************************************************/
PUBLIC void
VM_InitMemory(
	BYTE		*memory,        // INPUT : Memory to be added to the pool
	DWORD		size            // INPUT : Size of memory
	);

/*************************************************************************
 VM_Malloc - Allocates a block of memory - swapping out other blocks if
             needed.
 *************************************************************************/
PUBLIC VOID *
VM_Malloc(
	UINT		size,       // INPUT : Size of object
	VM_OWNER	*owner,     // INPUT : Owner Structure, NULL=Locked
	BOOL		discard		// INPUT : Discard memory to satisfy request.
	);

/*************************************************************************
* VM_Touch - touch a peice of memory to keep track of most recently used.
 *************************************************************************/
PUBLIC VOID
VM_Touch(
	VM_OWNER	*owner		// INPUT : Owner of memory to touch.
	);

/*************************************************************************
 VM_Free - frees a block of memory allocated by VM_Malloc
 *************************************************************************/
PUBLIC VOID
VM_Free(
   VOID     *mem        // INPUT : Memory Object to Free
	);

/*************************************************************************
 VM_Lock - Locks a block of memory from being swapped out.
 *************************************************************************/
PUBLIC VOID
VM_Lock(
   VOID     *mem        // INPUT : Memory Object to Free
	);

/*************************************************************************
 VM_Unlock - Unlocks a block of memory allowing it to be swapped out.
 *************************************************************************/
PUBLIC VOID
VM_Unlock(
   VOID     *mem,       // INPUT : Memory Object to Free
	VM_OWNER *owner      // INPUT : Owner Structure, NULL=Locked
	);

/*************************************************************************
 VM_GetCoreInfo - Get information on core resource
 *************************************************************************/
PUBLIC VOID
VM_GetCoreInfo(
	DWORD			*largest,		// OUTPUT: Largest block free
	DWORD			*totalfree,		// OUTPUT: Total amount free
	DWORD			*totallocked,	// OUTPUT: Total amount locked
	DWORD			*totalused,		// OUTPUT: Total amount used
	DWORD			*discarded		// OUTPUT: Number of items discarded
	);

#endif
