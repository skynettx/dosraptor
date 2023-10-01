;/****************************************************************************
;* GFXapi  - Graphic Library for 320x200 256 color vga/mcga 
;*----------------------------------------------------------------------------
;* Copyright (C) 1992  Scott Hostynski All Rights Reserved
;*----------------------------------------------------------------------------
;*
;* Created by:  Scott Host
;* Date:        Oct, 1992
;* Modifed:     Mar, 1993 ( made work with Watcom C )
;*
;* CONTENTS: gfxapi.c gfxapi_a.asm gfxapi.h
;*
;* EXTERN MODULES - NONE 
;*
;*---------------------------------------------------------------------------*/
	.386
	.MODEL	FLAT, C

SCREENWIDTH     =       320
SCREENHEIGHT    =       200

ANIM_S_SIZE     =       8

	.DATA

EXTRN displaybuffer:DWORD
EXTRN displayscreen:DWORD

	.CODE

        PUBLIC  ANIM_Render

;*************************************************************************
;*************************************************************************
ANIM_Render  proc       
;                    edx:inmem:DWORD
                
                mov     edx, displaybuffer 
                mov     esi, eax  ; arg2
                mov     ecx, 0
                mov     ebx, ecx

        ALIGN 4
        anim_loop:
                mov     bx, WORD PTR [esi]
                cmp     WORD PTR bx, 0H 
                je      anim_exit

                mov     bx, WORD PTR + 04H[esi]
                mov     cx, WORD PTR + 06H[esi]

                add     esi, ANIM_S_SIZE
                mov     edi, edx
                add     edi, ebx

                mov     eax, ecx
                and     ecx, 03H
                rep     movsb
                mov     ecx, eax
                shr     ecx, 02H
                rep     movsd

                jmp     anim_loop

anim_exit:
                ret
	        ENDP


END











                  
