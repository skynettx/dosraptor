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
; Two at once
        push    ebp

;        mov     edi,[to]
;        mov     esi,[start]
;        mov     ebp,[position]
;        mov     edx,[Rate]
        mov     ebp, eax

        ; Volume table ptr
;        mov     ecx,[VolumeTable]
        mov     eax,OFFSET apatch1+3            ; convice tasm to modify code...
        mov     [eax],ecx
        mov     eax,OFFSET apatch2+3            ; convice tasm to modify code...
        mov     [eax],ecx

        ; Harsh Clip table ptr
;        mov     ebx,HarshClipTable              ; get harsh clip table address
        add     ebx,128
        mov     eax,OFFSET apatch3+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET apatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
;        mov     edx,[rate]
        mov     eax,OFFSET apatch5+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,OFFSET apatch6+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     ecx, MixBufferSize / 2          ; double pixel count

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

        mov     eax,ebp                         ; begin calculating first sample
        add     ebp,edx                         ; advance frac pointer
        shr     eax,16                          ; finish calculation for first sample

        mov     ebx,ebp                         ; begin calculating second sample
        add     ebp,edx                         ; advance frac pointer
        shr     ebx,16                          ; finish calculation for second sample

        movzx   eax, byte ptr [esi+eax]         ; get first sample
        movzx   ebx, byte ptr [esi+ebx]         ; get second sample

        ALIGN   16
mix8Mloop:
        movzx   edx, byte ptr [edi]             ; get current sample from destination
apatch1:
        movsx   eax, byte ptr [eax+12345678h]   ; volume translate first sample
apatch2:
        movsx   ebx, byte ptr [ebx+12345678h]   ; volume translate second sample
        add     eax, edx                        ; mix first sample
        movzx   edx, byte ptr [edi + 1]         ; get current sample from destination
apatch3:
        mov     eax, [eax + 12345678h]          ; harsh clip new sample
        add     ebx, edx                        ; mix second sample
        mov     [edi], al                       ; write new sample to destination
        mov     edx, ebp                        ; begin calculating third sample
apatch4:
        mov     ebx, [ebx + 12345678h]          ; harsh clip new sample
apatch5:
        add     ebp,12345678h                   ; advance frac pointer
        shr     edx, 16                         ; finish calculation for third sample
        mov     eax, ebp                        ; begin calculating fourth sample
        inc     edi                             ; move destination to second sample
        shr     eax, 16                         ; finish calculation for fourth sample
        mov     [edi], bl                       ; write new sample to destination
apatch6:
        add     ebp,12345678h                   ; advance frac pointer
        movzx   ebx, byte ptr [esi+eax]         ; get fourth sample
        movzx   eax, byte ptr [esi+edx]         ; get third sample
        inc     edi                             ; move destination to third sample
        dec     ecx                             ; decrement count
        jnz     mix8Mloop                       ; loop

        pop     ebp
        ret

        ENDP

        ENDS

        END
