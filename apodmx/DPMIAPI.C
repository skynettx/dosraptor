//
// Copyright (C) 1993-1996 Id Software, Inc.
// Copyright (C) 1993-2008 Raven Software
// Copyright (C) 2015-2021 Nuke.YKT
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
#include <dos.h>
#include "dpmiapi.h"

int DPMI_LockMemory(void *address, unsigned length);
int DPMI_UnlockMemory(void *address, unsigned length);

int _dpmi_lockregion(void *address, unsigned length)
{
	return DPMI_LockMemory(address, length);
}

int _dpmi_unlockregion(void *address, unsigned length)
{
	return DPMI_UnlockMemory(address, length);
}

int _dpmi_dosalloc(unsigned short size, unsigned int* segment)
{
    char *ptr;
	int i;
	int ret = DPMI_GetDOSMemory(&ptr, &i, size << 4);
    if (!ret)
        *segment = (unsigned int)ptr >> 4;
    return ret;
}

static struct SREGS segregs;
static union REGS regs;

int _dpmi_getmemsize(void)
{
    int             meminfo[32];
    int             heap;

    memset (meminfo,0,sizeof(meminfo));
    segread(&segregs);
    segregs.es = segregs.ds;
    regs.w.ax = 0x500;      // get memory info
    regs.x.edi = (int)&meminfo;
    int386x( 0x31, &regs, &regs, &segregs );

    heap = meminfo[0];

    return heap;
}
