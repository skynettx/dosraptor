The Apogee Sound System backed DMX wrapper
==========================================

This is a DMX sound library wrapper, which is powered by the
Apogee Sound System, the latter being written by Jim Dose for
3D Realms. When used together, they form a replacement for DMX.

The DMX wrapper was written by Nuke.YKT for PCDoom, a DOS port
of id Software's Doom title from the 90s.
It also includes the mus2mid converter, contributed by Ben Ryves for
Simon Howard's Chocolate Doom port, as well as the PC Speaker frequency table,
dumped by Gez from a DOOM2.EXE file and later also added to Chocolate Doom.
A few years later, this wrapper was modified by NY00123; Mostly to be built
as a standalone library, while removing dependencies on game code.

Terms of use
------------

This wrapper is licensed under the terms of the GNU GPLv2+.
See GPL-2.0.txt for more details.

Prerequisites for building the wrapper
--------------------------------------

A compatible version of Watcom C32 that supports targeting DOS.

How to build
------------

1. Prepare a working setup of Watcom.
2. Enter "wmake". This should create the wrapper under the file APODMX.LIB.
When using it to link with a program which depends on DMX, you'll also
need to separately bring the Apogee Sound System library.
Version 1.09 is known to be compatible.
