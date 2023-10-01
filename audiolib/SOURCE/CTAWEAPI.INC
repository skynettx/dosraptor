/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

;/****************************************************************************\
;*                                                                            *
;* CTAWEAPI.INC SB AWE32 DOS Assembly Header                                  *
;*                                                                            *
;* (C) Copyright Creative Technology Ltd. 1992-94. All rights reserved        *
;* worldwide.                                                                 *
;*                                                                            *
;* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY      *
;* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        *
;* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR      *
;* PURPOSE.                                                                   *
;*                                                                            *
;* You have a royalty-free right to use, modify, reproduce and                *
;* distribute the Sample Files (and/or any modified version) in               *
;* any way you find useful, provided that you agree to                        *
;* the Creative's Software Licensing Aggreement and you also agree that       *
;* Creative has no warranty obligations or liability for any Sample Files.    *
;*                                                                            *
;\****************************************************************************/
;
;/****************************************************************************\
;*      File name       : CTAWEAPI.INC                                        *
;*                                                                            *
;*      Programmer      : Creative SB AWE32 Team                              *
;*                        Creative Technology Ltd, 1994. All rights reserved. *
;*                                                                            *
;*      Version         : 2.0b                                                *
;*                                                                            *
;\****************************************************************************/

SCRATCH     STRUC
            DB          702 DUP(0)
SCRATCH     ENDS

GCHANNEL    STRUC
            DB          20 DUP(0)
GCHANNEL    ENDS

MIDICHANNEL STRUC
            DB          32 DUP(0)
MIDICHANNEL ENDS

SOUNDFONT   STRUC
            DB          124 DUP(0)
SOUNDFONT   ENDS

NRPNCHANNEL STRUC
            DB          96 DUP(0)
NRPNCHANNEL ENDS

SOUNDPAD    STRUC
    SPad1   DD          0
    SPad2   DD          0
    SPad3   DD          0
    SPad4   DD          0
    SPad5   DD          0
    SPad6   DD          0
    SPad7   DD          0
SOUNDPAD    ENDS

AWE32_ALLOCMEM          EQU     0001h
AWE32_FREEMEM           EQU     0002h

AWE32ERR_ALLOCMEM       EQU     1
AWE32ERR_SBKFORMAT      EQU     2
AWE32ERR_DRAMSIZE       EQU     3
AWE32ERR_SBKREAD        EQU     4

