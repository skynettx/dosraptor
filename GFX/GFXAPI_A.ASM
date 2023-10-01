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
UNROLL	        =       4
TEXTUREWIDTH    =       32
TEXTUREHEIGHT   =       32

SPRITE_S_SIZE   =       16

	.DATA

EXTRN displaybuffer:DWORD
EXTRN displayscreen:DWORD
EXTRN stable:DWORD
EXTRN tablelen:DWORD
EXTRN ud_x:DWORD
EXTRN ud_y:DWORD
EXTRN ud_lx:DWORD
EXTRN ud_ly:DWORD
EXTRN update_start:DWORD
EXTRN gfx_inmem:DWORD
EXTRN gfx_xp:DWORD
EXTRN gfx_yp:DWORD
EXTRN gfx_lx:DWORD
EXTRN gfx_ly:DWORD
EXTRN gfx_imga:DWORD

	.CODE

        PUBLIC  GFX_ScaleLine
        PUBLIC  GFX_CScaleLine
        PUBLIC  GFX_ShadeSprite
        PUBLIC  GFX_DrawSprite
        PUBLIC  GFX_Shade
        PUBLIC  GFX_PutPic
        PUBLIC  GFX_PutMaskPic
        PUBLIC  GFX_DisplayScreen
        PUBLIC  GFX_DrawChar

;*************************************************************************
; GFX_ScaleLine() - Does the scale from scale table ( _stable )
;*************************************************************************
GFX_ScaleLine   proc      
        ARG     outmem:DWORD,\
                inmem:DWORD

                mov   ebx, inmem
                mov   edi, outmem
                mov   esi, offset [stable]
                mov   ecx, tablelen
	        add   esi,ecx
	        add   esi,ecx
	        add   esi,ecx
	        add   esi,ecx
                dec   ecx
                js    @Scale_CLine_Exit

@CBlockMove:
	        cmp   ecx,((UNROLL*4)-1)
	        jl    @Cdoloop

REPT	UNROLL
	        sub   esi,16
                mov   edx,[ esi + 4 ]
                mov   al, BYTE PTR [ebx][edx]
                mov   edx,[esi]
                mov   ah, BYTE PTR [ebx][edx]
	        shl   eax,16
                mov   edx,[ esi + 12 ]
                mov   al, BYTE PTR [ebx][edx]
                mov   edx,[ esi + 8 ]
                mov   ah, BYTE PTR [ebx][edx]
	        stosd
ENDM
	        sub   ecx,(UNROLL*4)
                jns   @CBlockMove
	        jmp   @Scale_CLine_Exit

	ALIGN	4
@Cdoloop:
	        sub   esi,4
                mov   edx,[esi]
                mov   al, BYTE PTR [ebx][edx]
	        stosb
                dec   ecx
                jns   @Cdoloop

@Scale_CLine_Exit:

                ret
	        ENDP

;*************************************************************************
; GFX_CScaleLine() - scale from scale table (_stable) color 0 transparent
;*************************************************************************
GFX_CScaleLine  proc     
        ARG     outmem:DWORD,\
                inmem:DWORD

                mov   eax, inmem
                mov   edi, outmem
                mov   esi, offset [stable]
                mov   ecx, tablelen
                dec   ecx
                js    @Scale_Line_Exit

	ALIGN	4
@doloop:
                mov   ebx, [esi][ecx * 4]
                mov   bl, BYTE PTR [eax][ebx]
                or    bl,bl
                jz    @skipit
                mov   BYTE PTR [edi], bl 
@skipit:
                inc   edi
                dec   ecx
                jns   @doloop

@Scale_Line_Exit:

                ret
	        ENDP

;*************************************************************************
;*************************************************************************
GFX_ShadeSprite  proc       
;        ARG     eax:dest:DWORD,\
;                edx:inmem:DWORD, \
;                ebx:dtable:DWORD

                xor     ebx, ebx

        ALIGN 4
        spriteshade_loop:
                mov     bx, WORD PTR +8H[esi]
                cmp     WORD PTR bx, 0ffffH 
                je      spriteshade_exit

                xor     ecx, ecx
                mov     cx, WORD PTR + 0CH[esi]

                add     esi, SPRITE_S_SIZE
                mov     edi, edx
                add     edi, ebx
                add     esi, ecx

                dec     ecx
sblock:
	        cmp     ecx, ((UNROLL*6)-1)
	        jl      endloop

REPT    UNROLL*6
                mov     al, BYTE PTR [edi]
                mov     al, [eax]
                mov     BYTE PTR [edi], al 
                inc     edi
ENDM
	        sub     ecx, ( UNROLL * 6 )
                jns     sblock
                jmp     spriteshade_loop
endloop:
                mov     al, BYTE PTR [edi]
                mov     al, [eax]
                mov     BYTE PTR [edi], al 
                inc     edi
                dec     ecx
                jns     endloop
                jmp     spriteshade_loop
spriteshade_exit:

                ret
	        ENDP

;*************************************************************************
;*************************************************************************
GFX_DrawSprite  proc       
;        ARG     eax:dest:DWORD,\
;                edx:inmem:DWORD
                
                xor     ecx, ecx
                mov     ebx, ecx


        ALIGN 4
        sprite_loop:
                mov     bx, WORD PTR +8H[esi]
                cmp     WORD PTR bx, 0ffffH 
                je      sprite_exit

                mov     cx, WORD PTR + 0CH[esi]

                add     esi, SPRITE_S_SIZE
                mov     edi, edx
                add     edi, ebx

                mov     eax, ecx
                and     ecx, 03H
                rep     movsb
                mov     ecx, eax
                shr     ecx, 02H
                rep     movsd

                jmp     sprite_loop

sprite_exit:

                ret
	        ENDP

;*************************************************************************
;GFX_DrawChar              
;*************************************************************************
GFX_DrawChar  proc      Uses EAX EBX ECX EDX EDI ESI FS GS      
        ARG     dest:DWORD,\
                inmem:DWORD,\
                lx:DWORD,\
                ly:DWORD, \
                addx:DWORD, \
                color:DWORD

        LOCAL   adds:DWORD
                
                mov     eax, SCREENWIDTH
                sub     eax, lx
                mov     adds, eax

                mov     edi, dest  
                mov     esi, inmem 
                mov     ebx, ly
                mov     eax, color
char_loop:
                mov     ecx, lx
char_line:
                mov     ah, BYTE PTR [esi]
                or      ah, ah
                je      skip_char
                add     ah, al
                mov     BYTE PTR [edi], ah
skip_char:
                inc     edi
                inc     esi
                loop    char_line

                add     esi, addx
                add     edi, adds
                dec     ebx
                jnz     char_loop

                ret
	        ENDP

;*************************************************************************
; GFX_Shade() - Remaps Bytes according to shade table 
;*************************************************************************
GFX_Shade       proc       
;        ARG    eax inmem:DWORD,\
;               edx maxlen:DWORD,\
;               ebx dtable:DWORD

                dec   ecx
                js    shade_exit

	ALIGN	4
blockshade:
	        cmp   ecx,((UNROLL*6)-1)
	        jl    @doloopS

REPT    UNROLL*6
                mov   al, BYTE PTR [edi]
                mov   al, [eax]
                mov   BYTE PTR [edi], al 
                inc   edi
ENDM
	        sub   ecx,(UNROLL*6)
                jns   blockshade
                jmp   shade_exit

	ALIGN	4
@doloopS:
                mov   al, BYTE PTR [edi]
                mov   al, [eax]
                mov   BYTE PTR [edi], al 
                inc   edi
                dec   ecx
                jns   @doloopS
shade_exit:

                ret
	        ENDP

;*************************************************************************
; GFX_PutPic() - Puts Picture into buffer 
;*************************************************************************
GFX_PutPic      proc         

	        mov     esi, gfx_inmem

	        mov     edi, [displaybuffer]
	        add     edi, gfx_xp
                mov     eax, SCREENWIDTH
                mov     edx, gfx_yp
                mul     edx
                add     edi, eax

                mov     edx, SCREENWIDTH
                mov     eax, gfx_lx
                sub     edx, eax
                mov     ebx, gfx_ly

                xor     ecx, ecx
                cmp     ecx, gfx_imga
                je      @nocliploop

ALIGN 4
@cliploop:
	        mov     ecx, eax
                shr     ecx, 1
                rep     movsw
                rcl     ecx, 1
                rep     movsb
                add     edi, edx 
                add     esi, gfx_imga 
                dec     ebx
                jnz     @cliploop
                jmp     @PutPicExit

ALIGN 4
@nocliploop:
	        mov     ecx, eax
                shr     ecx, 2
                rep     movsd

                mov     ecx, eax
                and     ecx, 3
                rep     movsb

                add     edi, edx 
                dec     ebx
                jnz     @nocliploop
@PutPicExit:

                ret
	        ENDP

;*************************************************************************
; GFX_PutMaskPic() - Puts Picture into buffer with color 0 see thru
;*************************************************************************
GFX_PutMaskPic  proc          

	        mov     esi, gfx_inmem

	        mov     edi, [displaybuffer]
	        add     edi, gfx_xp
                mov     eax, SCREENWIDTH
                mov     edx, gfx_yp
                mul     edx
                add     edi, eax

                mov     ebx, gfx_ly
                mov     edx, gfx_lx
                dec     edx
                js      @MaskPutPic_Exit

ALIGN 4
@Mask_Loop:
	        mov     ecx, edx
ALIGN 4
@Mask_OneLine:
                mov     al, [esi][ecx]
                or      al, al
                jnz     @Mask_PutPic
                dec     ecx
                jns     @Mask_OneLine

                add     edi, SCREENWIDTH
                add     esi, gfx_imga
                dec     ebx
                jnz     @Mask_Loop
                jmp     @MaskPutPic_Exit
ALIGN 4
@Mask_PutPic:
                mov     BYTE PTR [edi][ecx], al
                dec     ecx
                jns     @Mask_OneLine

                add     edi, SCREENWIDTH
                add     esi, gfx_imga
                dec     ebx
                jnz     @Mask_Loop
@MaskPutPic_Exit:

                ret
	        ENDP

;*************************************************************************
; GFX_DisplayScreen() - Puts Display Buffer into Video memory 
;*************************************************************************
GFX_DisplayScreen proc   

	        mov     esi, [displaybuffer]
	        mov     edi, [displayscreen]

	        mov     ebx, ud_x
                add     esi, ebx
                add     edi, ebx

                mov     eax, SCREENWIDTH
                mov     edx, ud_y
                mul     edx
                add     edi, eax
                add     esi, eax

	        mov     eax, ud_lx
                cmp     eax, SCREENWIDTH
                jne     @partscreen

                mov     edx, ud_ly
                mul     edx
                mov     ecx, eax
                shr     ecx, 2
                rep     movsd
                xor     ebx, ebx
                jmp     @update_exit
ALIGN 4
@partscreen:
                mov     edx, SCREENWIDTH
                sub     edx, ud_lx
                mov     ebx, ud_ly
                shr     eax, 2
ALIGN 4
@dloop:
                mov     ecx,eax
                rep     movsd
                add     edi, edx 
                add     esi, edx 
                dec     ebx
                jnz     @dloop
@update_exit:
                mov     update_start, ebx

                ret
	        ENDP

END











                  
