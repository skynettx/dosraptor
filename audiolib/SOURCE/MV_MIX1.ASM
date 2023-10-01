        .386
        .MODEL  flat

        .data
        .code
SEGMENT text  USE32
        ALIGN 16

;================
;
; MV_Mix8BitMonoFast
;
;================

; eax - position
; edx - rate
; ecx - Volume table
; ebx - Harsh clip table
; edi - to
; esi - start

MixBufferSize equ 256

PROC    MV_Mix8BitMonoFast_
PUBLIC  MV_Mix8BitMonoFast_

        push    ebp

;        mov     edi,[to]
;        mov     esi,[start]
;        mov     ebp,[position]
;        mov     edx,[Rate]
        mov     ebp, eax

        ; Rate scale ptr
;        mov     edx,[rate]
        mov     eax,OFFSET apatch1+2            ; convice tasm to modify code...
        mov     [eax],edx

        ; Volume table ptr
;        mov     ecx,[VolumeTable]
        mov     eax,OFFSET apatch2+3            ; convice tasm to modify code...
        mov     [eax],ecx

        ; Harsh Clip table ptr
;        mov     ebx,HarshClipTable              ; get harsh clip table address
        add     ebx,128
        mov     eax,OFFSET apatch3+3            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     ecx, MixBufferSize

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; apatch1 - volume table
; apatch2 - volume table
; apatch3 - harsh clip table
; apatch4 - harsh clip table
; apatch5 - sample rate
; apatch6 - sample rate

        ALIGN   16
mix8Mloop:
        mov     eax,ebp                         ; begin calculating first sample
apatch1:
        add     ebp,12345678h                   ; advance frac pointer
        shr     eax,16                          ; finish calculation for first sample
        movzx   eax, byte ptr [esi+eax]         ; get first sample
        movzx   edx, byte ptr [edi]             ; get current sample from destination
apatch2:
        movsx   eax, byte ptr [eax+12345678h]   ; volume translate first sample
apatch3:
        mov     eax, [eax + edx + 12345678h]    ; harsh clip new sample
        mov     [edi], al                       ; write new sample to destination
        inc     edi                             ; move destination to second sample
        dec     ecx                             ; decrement count
        jnz     mix8Mloop                       ; loop

        pop     ebp
        ret

        ENDP

        ENDS

        END
