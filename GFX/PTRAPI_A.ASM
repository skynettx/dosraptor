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

SCREENWIDTH  =  320
SCREENHEIGHT =  200

CURSORWIDTH  =  16
CURSORHEIGHT =  16

	.DATA

EXTRN   cursorloopx:DWORD
EXTRN   cursorloopy:DWORD
EXTRN   displaybuffer:DWORD
EXTRN   displayscreen:DWORD
EXTRN   joy_x:DWORD;
EXTRN   joy_y:DWORD;
EXTRN   joy_buttons:DWORD;
EXTRN   cursorstart:DWORD
EXTRN   displaypic:DWORD
EXTRN   cursorsave:DWORD

	.CODE

PUBLIC  PTR_ClipSave
PUBLIC  PTR_Save
PUBLIC  PTR_Erase
PUBLIC  PTR_ClipErase
PUBLIC  PTR_Draw
PUBLIC  PTR_ReadJoyStick

;=========================================================================
;PTR_ReadJoyStick        proc
;=========================================================================
PTR_ReadJoyStick        proc   

                pushad

		mov	edx,201h

		xor	edi,edi
		mov	esi,edi
		mov	ebx,edi
		mov	eax,edi
		mov	ecx,3FFh

		out	dx,al
		in	al,dx

		in	al,dx
		not	al
                shr     eax, 4
		mov	joy_buttons,eax
                xor     eax, eax

	LR_1:
		in	al,dx
		and	al,0fh
		jz	LR_2

		rcr	al,1
		adc	edi,ebx
		rcr	al,1
		adc	esi,ebx
		loop	LR_1

	LR_2:
		mov	joy_y,esi
		mov     joy_x,edi

                popad

                ret

                ENDP        

;=========================================================================
; PTR_Save() - Saves screen before a cursor draw
;=========================================================================
PTR_Save        proc      

                pushad

                mov     esi, [cursorstart]
                mov     edi, [cursorsave]

                mov     ebx, CURSORWIDTH
                shr     ebx, 2
                mov     edx, SCREENWIDTH - CURSORWIDTH

REPT    CURSORHEIGHT - 1
                mov     ecx, ebx
                rep     movsd
                add     esi, edx
ENDM
                mov     ecx, ebx
                rep     movsd

                popad

                ret
	        ENDP

;=========================================================================
; PTR_ClipSave() - Saves screen before a cursor draw
;=========================================================================
PTR_ClipSave    proc       

                pushad

                mov     esi, [cursorstart]
                mov     edi, [cursorsave]

                mov     eax, CURSORWIDTH
                mov     edx, SCREENWIDTH - CURSORWIDTH
                mov     ebx, cursorloopy
                shr     eax, 2
                dec     ebx

ClipSave:
                mov     ecx, eax
                rep     movsd
                add     esi, edx
                dec     ebx
                jns     ClipSave

                popad

                ret
	        ENDP

;=========================================================================
; PTR_Erase() - Erases cursor with stuff from PTR_Save()
;=========================================================================
PTR_Erase       proc    

                pushad

                mov     esi, [cursorsave]
                mov     edi, [cursorstart]

                mov     ebx, CURSORWIDTH
                shr     ebx, 2
                mov     edx, SCREENWIDTH - CURSORWIDTH

REPT    CURSORHEIGHT - 1
                mov     ecx, ebx
                rep     movsd
                add     edi, edx
ENDM
                mov     ecx, ebx
                rep     movsd

                popad

                ret
	        ENDP

;=========================================================================
; PTR_ClipErase() - Erases cursor and clips edges of screen
;=========================================================================
PTR_ClipErase   proc     

                pushad

                mov     esi, [cursorsave]
                mov     edi, [cursorstart]

                mov     eax, CURSORWIDTH
                sub     eax, cursorloopx
                mov     edx, SCREENWIDTH
                sub     edx, cursorloopx
                mov     ebx, cursorloopy
                dec     ebx

cliploop:
                mov     ecx, cursorloopx
                shr     ecx,1
                rep     movsw
                rcl     ecx,1
                rep     movsb
                add     edi, edx 
                add     esi, eax 
                dec     ebx
                jns     cliploop

                popad

                ret
	        ENDP

;=========================================================================
; PTR_Draw() - Draws Cursor
;=========================================================================
PTR_Draw        proc     

                pushad

                mov     esi, [displaypic]
                mov     edi, [cursorstart]

                mov     edx, cursorloopx
                mov     ebx, cursorloopy
                dec     ebx 
                dec     edx 
ALIGN	4
@clipdrawloop:
                mov     ecx, edx 
ALIGN	4
@coneline:
                mov     al, BYTE PTR [esi][ecx]
                or      al, al
                jnz     @cskip_it
                dec     ecx
                jns     @coneline

                add     esi, CURSORWIDTH
                add     edi, SCREENWIDTH
                dec     ebx
                jns     @clipdrawloop
                jmp     @end_it

ALIGN	4
@cskip_it:
                mov     BYTE PTR [edi][ecx], al
                dec     ecx
                jns     @coneline

                add     esi, CURSORWIDTH
                add     edi, SCREENWIDTH
                dec     ebx
                jns     @clipdrawloop
@end_it:

                popad

                ret
	        ENDP

END












                  
