        .386
        .MODEL  flat

        .data
        .code
SEGMENT text  USE32
        ALIGN 16

EXTRN   _MV_HarshClipTable:DWORD
EXTRN   _MV_MixDestination:DWORD
EXTRN   _MV_MixPosition:DWORD
EXTRN   _MV_LeftVolume:DWORD
EXTRN   _MV_RightVolume:DWORD


;================
;
; MV_Mix8BitMonoFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

MixBufferSize equ 256

PROC    MV_Mix8BitMonoFast_
PUBLIC  MV_Mix8BitMonoFast_
; Two at once
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume             ; Since we're mono, use left volume
        mov     eax,OFFSET apatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET apatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Harsh Clip table ptr
        mov     ebx, _MV_HarshClipTable
        add     ebx, 128
        mov     eax,OFFSET apatch3+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET apatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx
        ; Rate scale ptr
        mov     eax,OFFSET apatch5+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,OFFSET apatch6+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        shr     ecx, 1                          ; double sample count
        cmp     ecx, 0
        je      short exit8m

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
        movsx   eax, byte ptr [2*eax+12345678h] ; volume translate first sample
apatch2:
        movsx   ebx, byte ptr [2*ebx+12345678h] ; volume translate second sample
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

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position
exit8m:
        popad
        ret
ENDP    MV_Mix8BitMonoFast_

;================
;
; MV_Mix8BitStereoFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

PROC    MV_Mix8BitStereoFast_
PUBLIC  MV_Mix8BitStereoFast_
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume
        mov     eax,OFFSET bpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     ebx, _MV_RightVolume
        mov     eax,OFFSET bpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     eax,OFFSET bpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx

        ; Harsh Clip table ptr
        mov     ebx, _MV_HarshClipTable
        add     ebx,128
        mov     eax,OFFSET bpatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET bpatch5+2            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        cmp     ecx, 0
        je      short exit8S

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; bpatch1 - left volume table
; bpatch2 - right volume table
; bpatch3 - sample rate
; bpatch4 - harsh clip table
; bpatch5 - harsh clip table

        mov     eax,ebp                         ; begin calculating first sample
        shr     eax,16                          ; finish calculation for first sample

        movzx   ebx, byte ptr [esi+eax]         ; get first sample

        ALIGN   16
mix8Sloop:
bpatch1:
        movsx   eax, byte ptr [2*ebx+12345678h] ; volume translate left sample
        movzx   edx, byte ptr [edi]             ; get current sample from destination
bpatch2:
        movsx   ebx, byte ptr [2*ebx+12345678h] ; volume translate right sample
        add     eax, edx                        ; mix left sample
bpatch3:
        add     ebp,12345678h                   ; advance frac pointer
        movzx   edx, byte ptr [edi+1]           ; get current sample from destination
bpatch4:
        mov     eax, [eax + 12345678h]          ; harsh clip left sample
        add     ebx, edx                        ; mix right sample
        mov     [edi], al                       ; write left sample to destination
bpatch5:
        mov     ebx, [ebx + 12345678h]          ; harsh clip right sample
        inc     edi                             ; move destination to second sample
        mov     edx, ebp                        ; begin calculating second sample
        mov     [edi], bl                       ; write right sample to destination
        shr     edx, 16                         ; finish calculation for second sample
        inc     edi                             ; move destination to second sample
        movzx   ebx, byte ptr [esi+edx]         ; get second sample
        dec     ecx                             ; decrement count
        jnz     mix8Sloop                       ; loop

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position

EXIT8S:
        popad
        ret
ENDP    MV_Mix8BitStereoFast_

;================
;
; MV_Mix8Bit1ChannelFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

PROC    MV_Mix8Bit1ChannelFast_
PUBLIC  MV_Mix8Bit1ChannelFast_
; Two at once
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume
        mov     eax,OFFSET epatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET epatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Harsh Clip table ptr
        mov     ebx, _MV_HarshClipTable
        add     ebx,128
        mov     eax,OFFSET epatch3+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET epatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     eax,OFFSET epatch5+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,OFFSET epatch6+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        shr     ecx, 1                          ; double sample count
        cmp     ecx, 0
        je      exit81C

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
mix81Cloop:
        movzx   edx, byte ptr [edi]             ; get current sample from destination
epatch1:
        movsx   eax, byte ptr [2*eax+12345678h] ; volume translate first sample
epatch2:
        movsx   ebx, byte ptr [2*ebx+12345678h] ; volume translate second sample
        add     eax, edx                        ; mix first sample
        movzx   edx, byte ptr [edi + 2]         ; get current sample from destination
epatch3:
        mov     eax, [eax + 12345678h]          ; harsh clip new sample
        add     ebx, edx                        ; mix second sample
        mov     [edi], al                       ; write new sample to destination
        mov     edx, ebp                        ; begin calculating third sample
epatch4:
        mov     ebx, [ebx + 12345678h]          ; harsh clip new sample
epatch5:
        add     ebp,12345678h                   ; advance frac pointer
        shr     edx, 16                         ; finish calculation for third sample
        mov     eax, ebp                        ; begin calculating fourth sample
        add     edi, 2                          ; move destination to second sample
        shr     eax, 16                         ; finish calculation for fourth sample
        mov     [edi], bl                       ; write new sample to destination
epatch6:
        add     ebp,12345678h                   ; advance frac pointer
        movzx   ebx, byte ptr [esi+eax]         ; get fourth sample
        movzx   eax, byte ptr [esi+edx]         ; get third sample
        add     edi, 2                          ; move destination to third sample
        dec     ecx                             ; decrement count
        jnz     mix81Cloop                      ; loop

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position
EXIT81C:
        popad
        ret
ENDP    MV_Mix8Bit1ChannelFast_

;================
;
; MV_Mix16BitMonoFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

MixBufferSize equ 256

PROC    MV_Mix16BitMonoFast_
PUBLIC  MV_Mix16BitMonoFast_
; Two at once
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume
        mov     eax,OFFSET cpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET cpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     eax,OFFSET cpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,OFFSET cpatch4+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        shr     ecx, 1                          ; double sample count
        cmp     ecx, 0
        je      exit16M

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; cpatch1 - volume table
; cpatch2 - volume table
; cpatch3 - sample rate
; cpatch4 - sample rate

        mov     eax,ebp                         ; begin calculating first sample
        add     ebp,edx                         ; advance frac pointer
        shr     eax,16                          ; finish calculation for first sample

        mov     ebx,ebp                         ; begin calculating second sample
        add     ebp,edx                         ; advance frac pointer
        shr     ebx,16                          ; finish calculation for second sample

        movzx   eax, byte ptr [esi+eax]         ; get first sample
        movzx   ebx, byte ptr [esi+ebx]         ; get second sample

        ALIGN   16
mix16Mloop:
        movsx   edx, word ptr [edi]             ; get current sample from destination
cpatch1:
        movsx   eax, word ptr [2*eax+12345678h] ; volume translate first sample
cpatch2:
        movsx   ebx, word ptr [2*ebx+12345678h] ; volume translate second sample
        add     eax, edx                        ; mix first sample
        movsx   edx, word ptr [edi + 2]         ; get current sample from destination

        cmp     eax, -32768                     ; Harsh clip sample
        jge     short m16skip1
        mov     eax, -32768
        jmp     short m16skip2
m16skip1:
        cmp     eax, 32767
        jle     short m16skip2
        mov     eax, 32767
m16skip2:
        add     ebx, edx                        ; mix second sample
        mov     [edi], ax                       ; write new sample to destination
        mov     edx, ebp                        ; begin calculating third sample

        cmp     ebx, -32768                     ; Harsh clip sample
        jge     short m16skip3
        mov     ebx, -32768
        jmp     short m16skip4
m16skip3:
        cmp     ebx, 32767
        jle     short m16skip4
        mov     ebx, 32767
m16skip4:
cpatch3:
        add     ebp,12345678h                   ; advance frac pointer
        shr     edx, 16                         ; finish calculation for third sample
        mov     eax, ebp                        ; begin calculating fourth sample
        mov     [edi + 2], bx                   ; write new sample to destination
        shr     eax, 16                         ; finish calculation for fourth sample

cpatch4:
        add     ebp,12345678h                   ; advance frac pointer
        movzx   ebx, byte ptr [esi+eax]         ; get fourth sample
        add     edi, 4                          ; move destination to third sample
        movzx   eax, byte ptr [esi+edx]         ; get third sample
        dec     ecx                             ; decrement count
        jnz     mix16Mloop                      ; loop

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position
EXIT16M:
        popad
        ret
ENDP    MV_Mix16BitMonoFast_

;================
;
; MV_Mix16BitStereoFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

PROC    MV_Mix16BitStereoFast_
PUBLIC  MV_Mix16BitStereoFast_
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume
        mov     eax,OFFSET dpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     ebx, _MV_RightVolume
        mov     eax,OFFSET dpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     eax,OFFSET dpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        cmp     ecx, 0
        je      exit16S

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; dpatch1 - left volume table
; dpatch2 - right volume table
; dpatch3 - sample rate

        mov     eax,ebp                         ; begin calculating first sample
        shr     eax,16                          ; finish calculation for first sample

        movzx   ebx, byte ptr [esi+eax]         ; get first sample

        ALIGN   16
mix16Sloop:
dpatch1:
        movsx   eax, word ptr [2*ebx+12345678h] ; volume translate left sample
        movsx   edx, word ptr [edi]             ; get current sample from destination
dpatch2:
        movsx   ebx, word ptr [2*ebx+12345678h] ; volume translate right sample
        add     eax, edx                        ; mix left sample
dpatch3:
        add     ebp,12345678h                   ; advance frac pointer
        movsx   edx, word ptr [edi+2]           ; get current sample from destination

        cmp     eax, -32768                     ; Harsh clip sample
        jge     short s16skip1
        mov     eax, -32768
        jmp     short s16skip2
s16skip1:
        cmp     eax, 32767
        jle     short s16skip2
        mov     eax, 32767
s16skip2:
        add     ebx, edx                        ; mix right sample
        mov     [edi], ax                       ; write left sample to destination

        cmp     ebx, -32768                     ; Harsh clip sample
        jge     short s16skip3
        mov     ebx, -32768
        jmp     short s16skip4
s16skip3:
        cmp     ebx, 32767
        jle     short s16skip4
        mov     ebx, 32767
s16skip4:

        mov     edx, ebp                        ; begin calculating second sample
        mov     [edi+2], bx                     ; write right sample to destination
        shr     edx, 16                         ; finish calculation for second sample
        add     edi, 4                          ; move destination to second sample
        movzx   ebx, byte ptr [esi+edx]         ; get second sample
        dec     ecx                             ; decrement count
        jnz     mix16Sloop                      ; loop

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position
exit16S:
        popad
        ret
ENDP    MV_Mix16BitStereoFast_

;================
;
; MV_Mix16Bit1ChannelFast
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

MixBufferSize equ 256

PROC    MV_Mix16Bit1ChannelFast_
PUBLIC  MV_Mix16Bit1ChannelFast_
; Two at once
        pushad

        mov     ebp, eax

        mov     esi, ebx                        ; Source pointer

        ; Volume table ptr
        mov     ebx, _MV_LeftVolume
        mov     eax,OFFSET fpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,OFFSET fpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     eax,OFFSET fpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,OFFSET fpatch4+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, _MV_MixDestination         ; Get the position to write to

        ; Number of samples to mix
        shr     ecx, 1                          ; double sample count
        cmp     ecx, 0
        je      exit161C

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; cpatch1 - volume table
; cpatch2 - volume table
; cpatch3 - sample rate
; cpatch4 - sample rate

        mov     eax,ebp                         ; begin calculating first sample
        add     ebp,edx                         ; advance frac pointer
        shr     eax,16                          ; finish calculation for first sample

        mov     ebx,ebp                         ; begin calculating second sample
        add     ebp,edx                         ; advance frac pointer
        shr     ebx,16                          ; finish calculation for second sample

        movzx   eax, byte ptr [esi+eax]         ; get first sample
        movzx   ebx, byte ptr [esi+ebx]         ; get second sample

        ALIGN   16
mix161Cloop:
        movsx   edx, word ptr [edi]             ; get current sample from destination
fpatch1:
        movsx   eax, word ptr [2*eax+12345678h] ; volume translate first sample
fpatch2:
        movsx   ebx, word ptr [2*ebx+12345678h] ; volume translate second sample
        add     eax, edx                        ; mix first sample
        movsx   edx, word ptr [edi + 4]         ; get current sample from destination

        cmp     eax, -32768                     ; Harsh clip sample
        jge     short m16c1skip1
        mov     eax, -32768
        jmp     short m16c1skip2
m16c1skip1:
        cmp     eax, 32767
        jle     short m16c1skip2
        mov     eax, 32767
m16c1skip2:
        add     ebx, edx                        ; mix second sample
        mov     [edi], ax                       ; write new sample to destination
        mov     edx, ebp                        ; begin calculating third sample

        cmp     ebx, -32768                     ; Harsh clip sample
        jge     short m16c1skip3
        mov     ebx, -32768
        jmp     short m16c1skip4
m16c1skip3:
        cmp     ebx, 32767
        jle     short m16c1skip4
        mov     ebx, 32767
m16c1skip4:
fpatch3:
        add     ebp,12345678h                   ; advance frac pointer
        shr     edx, 16                         ; finish calculation for third sample
        mov     eax, ebp                        ; begin calculating fourth sample
        mov     [edi + 4], bx                   ; write new sample to destination
        shr     eax, 16                         ; finish calculation for fourth sample

fpatch4:
        add     ebp,12345678h                   ; advance frac pointer
        movzx   ebx, byte ptr [esi+eax]         ; get fourth sample
        add     edi, 8                          ; move destination to third sample
        movzx   eax, byte ptr [esi+edx]         ; get third sample
        dec     ecx                             ; decrement count
        jnz     mix161Cloop                     ; loop

        mov     _MV_MixDestination, edi         ; Store the current write position
        mov     _MV_MixPosition, ebp            ; return position
exit161C:
        popad
        ret
ENDP    MV_Mix16Bit1ChannelFast_

        ENDS

        END
