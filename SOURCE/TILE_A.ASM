;/****************************************************************************
;* PTRapi  - Cursor routines 
;*----------------------------------------------------------------------------
;* Copyright (C) 1992  Scott Hostynski All Rights Reserved
;*----------------------------------------------------------------------------
;*
;* Created by:  Scott Host
;* Date:        Oct, 1992
;* Modifed:     Mar, 1993 ( made work with Watcom C )
;*
;*
;* CONTENTS: ptrapi.c ptrapi_a.asm ptrapi.h
;*
;* EXTERN MODULES - GFX, TSM and MOUSE.LIB 
;*
;*---------------------------------------------------------------------------*/
	.386
	.MODEL	FLAT, C

SCREENWIDTH     = 320
SCREENHEIGHT    = 200

TILEWIDTH       = 32
TILEHEIGHT      = 32
MAP_LEFT        = 16;

	.DATA

EXTRN   tileloopy:DWORD
EXTRN   displaybuffer:DWORD
EXTRN   displayscreen:DWORD
EXTRN   g_mapleft:DWORD

	.CODE

EXTRN tilepic:DWORD
EXTRN tilestart:DWORD

PUBLIC  TILE_Draw
PUBLIC  TILE_ClipDraw
PUBLIC  TILE_DisplayScreen
PUBLIC  TILE_ShakeScreen

;=========================================================================
;TILE_Draw      Fast 32 by 32 Tile draw     
;=========================================================================
TILE_Draw       proc     

                mov     esi, [tilepic]
                mov     edi, [tilestart]

                mov     ebx, (TILEWIDTH/4)
                mov     edx, SCREENWIDTH - TILEWIDTH

REPT    TILEHEIGHT - 1
                mov     ecx, ebx
                rep     movsd
                add     edi, edx
ENDM
                mov     ecx, ebx
                rep     movsd

                ret
	        ENDP

;=========================================================================
;
;=========================================================================
TILE_ClipDraw       proc     

                mov     esi, [tilepic]
                mov     edi, [tilestart]

                mov     eax, (TILEWIDTH/4)
                mov     edx, SCREENWIDTH - TILEWIDTH
                mov     ebx, tileloopy
                dec     ebx

ALIGN 4
ClipDraw:
                mov     ecx, eax
                rep     movsd
                add     edi, edx
                dec     ebx
                jns     ClipDraw

                ret
	        ENDP


;*************************************************************************
;*************************************************************************
TILE_ShakeScreen proc   

	        mov     esi, [displaybuffer]
	        mov     edi, [displayscreen]

                add     esi, MAP_LEFT - 4
                add     edi, g_mapleft
                sub     edi, 4

                mov     eax, (296/4)
                mov     edx, 24
REPT    200
                mov     ecx, eax
                rep     movsd
                add     edi, edx
                add     esi, edx
ENDM

                ret
	        ENDP

;*************************************************************************
;*************************************************************************
TILE_DisplayScreen proc   

	        mov     esi, [displaybuffer]
	        mov     edi, [displayscreen]

                add     esi, MAP_LEFT
                add     edi, MAP_LEFT

                mov     eax, (288/4)
                mov     edx, 32
REPT    200
                mov     ecx, eax
                rep     movsd
                add     edi, edx
                add     esi, edx
ENDM

                ret
	        ENDP

END





                  
