### VERSIONS RESTORATION ###
# Makefile modified as desired

.OPTIMIZE

!ifndef libver_libdef
!error libver_libdef not set!
!endif
!ifndef libver_assrev
!error libver_assrev not set!
!endif

#FIXME - Use just LIBVER_ASSREV for now (or get "Argument list too big")
LIBVER_ARGS       = -DLIBVER_ASSREV=$(libver_assrev)
#LIBVER_ARGS       = -DLIBVER_ASSREV=$(libver_assrev)L -DLIBVER_LIBDEF=$(libver_libdef)
LIBVER_ARGS_TASM  = /dLIBVER_ASSREV=$(libver_assrev)
#LIBVER_ARGS_TASM  = /dLIBVER_ASSREV=$(libver_assrev) /dLIBVER_LIBDEF=$(libver_libdef)

version = production
#version = debugging

timbre_file       = gmtimbre
#timbre_file       = leetimbr

prg_dir           = $(libver_libdef)
obj_debugging     = $(prg_dir)\objdb
obj_production    = $(prg_dir)\obj
obj_dir           = $(obj_$(version))
lib_dir           = .\lib
output_dir        = $(prg_dir)
main_dir          = .\source
header_dir        = $(main_dir)
tasm_include_dir  = /i$(main_dir)

lib_filename      = $(libver_libdef).LIB
#model             = f
#compiler          = w
#lib_filename      = audio_$(compiler)$(model).lib
lib_name          = $(obj_dir)\$(lib_filename)
final_lib         = $(output_dir)\$(lib_filename)

.h   :              $(header_dir)
.c   :              $(header_dir)
.asm :              $(header_dir)
.obj :              $(obj_dir)

#offlib_files     = gf1_osf.lib
lib_files         = gf1_osf.lib &
                    pawe32.lib

# FIXME - A *GREAT* DUPLICATION AROUND HERE
#object_files      = &
!ifeq libver_libdef AL_11
object_files      = $(timbre_file).obj &
                    al_midi.obj  &
                    awe32.obj    &
                    blaster.obj  &
                    debugio.obj  &
                    dma.obj      &
                    dpmi.obj     &
                    fx_man.obj   &
                    gus.obj      &
                    gusmidi.obj  &
                    guswave.obj  &
                    irq.obj      &
                    ll_man.obj   &
                    midi.obj     &
                    mpu401.obj   &
                    multivoc.obj &
                    mv_mix.obj  &
                    mv_mix16.obj  &
                    mvreverb.obj &
                    music.obj    &
                    pas16.obj    &
                    pitch.obj    &
                    sndscape.obj &
                    sndsrc.obj   &
                    task_man.obj &
                    user.obj
!else ifeq libver_libdef AL_109
object_files      = $(timbre_file).obj &
                    adlibfx.obj  &
                    al_midi.obj  &
                    awe32.obj    &
                    blastold.obj  &
                    debugio.obj  &
                    dma.obj      &
                    dpmi.obj     &
                    fx_man.obj   &
                    gus.obj      &
                    gusmidi.obj  &
                    guswave.obj  &
                    irq.obj      &
                    ll_man.obj   &
                    midi.obj     &
                    mpu401.obj   &
                    multivoc.obj &
                    mv_mix.obj  &
                    mv_mix16.obj  &
                    mvreverb.obj &
                    music.obj    &
                    pas16.obj    &
                    pcfx.obj     &
                    pitch.obj    &
                    sndscape.obj &
                    sndsrc.obj   &
                    task_man.obj &
                    user.obj
!else
object_files      = $(timbre_file).obj &
                    adlibfx.obj  &
                    al_midi.obj  &
                    awe32.obj    &
                    blastold.obj &
                    debugio.obj  &
                    dma.obj      &
                    dpmi.obj     &
                    fx_man.obj   &
                    gus.obj      &
                    gusmidi.obj  &
                    guswave.obj  &
                    irq.obj      &
                    ll_man.obj   &
                    midi.obj     &
                    mpu401.obj   &
                    multivoc.obj &
                    mv_mix5.obj  &
                    mvreverb.obj &
                    music.obj    &
                    pas16.obj    &
                    pcfx.obj     &
                    pitch.obj    &
                    sndscape.obj &
                    sndsrc.obj   &
                    task_man.obj &
                    user.obj
!endif

#offobject_files      = adlibfx.obj  &
#                    blastold.obj &
#                    debugio.obj  &
#                    dma.obj      &
#                    dpmi.obj     &
#                    fx_man.obj   &
#                    gus.obj      &
#                    guswave.obj  &
#                    irq.obj      &
#                    ll_man.obj   &
#                    multivoc.obj &
#                    mv_mix5.obj  &
#                    pas16.obj    &
#                    pcfx.obj     &
#                    pitch.obj    &
#                    sndscape.obj &
#                    sndsrc.obj   &
#                    task_man.obj &
#                    user.obj


msg_debugging       = Creating debugging library...
tasm_debugging      = /t  /zi /p  /mx /w2 $(tasm_include_dir)
wcc_debugging       = /zq /w4 /d2 /i=$(header_dir)
wlink_debugging     =

gus_debugging       = /zq /w4 /d2 /i=$(header_dir)
gusmidi_debugging   = /zq /w4 /d2 /i=$(header_dir)
multivoc_debugging  = /zq /w4 /d1 /omaxne /zp4 /5r /zu /d2 /i=$(header_dir)
pas16_debugging     = /zq /w4 /d1 /omaxnet /zp4 /5r /i=$(header_dir)
task_man_debugging  = /zq /w4 /d1 /omaxnet /zp4 /5r /i=$(header_dir)
midi_debugging      = /zq /w4 /d2 /zu /i=$(header_dir)
al_midi_debugging   = /zq /w4 /d2 /zu /i=$(header_dir)

msg_production      = Creating production library...
tasm_production     = /t  /zi /p  /mx /w2 $(tasm_include_dir)
wcc_production      = /zq /w4 /d1 /omaxnet /zp4 /5r /i=$(header_dir)
wlink_production    =

gus_production      = /zq /w4 /d2 /i=$(header_dir)
gusmidi_production  = /zq /w4 /d2 /i=$(header_dir)
multivoc_production = /zq /w4 /d1 /omaxne /zp4 /5r /zu /i=$(header_dir)
!ifneq libver_libdef AL950724
music_production = /zq /w4 /d1 /omaxne /zp4 /5r /zu /i=$(header_dir)
!endif
pas16_production    = /zq /w4 /d1 /omaxnet /zp4 /5r /i=$(header_dir)
task_man_production = /zq /w4 /d1 /omaxnet /zp4 /5r /i=$(header_dir)
midi_production     = /zq /w4 /d1 /omaxnet /zp4 /5r /zu /i=$(header_dir)
al_midi_production  = /zq /w4 /d1 /omaxnet /zp4 /5r /zu /i=$(header_dir)

$(final_lib) : $(lib_name)
   copy $(lib_name) $(final_lib)

$(lib_name) : $(object_files) audio.mak
   echo ******************************************************************
   echo
   echo $(msg_$(version))
   echo
   echo ******************************************************************
   %create temp.lnk
   for %i in ($(object_files)) do %append temp.lnk -+$(obj_dir)\%i
   for %i in ($(lib_files)) do %append temp.lnk -+$(lib_dir)\%i
   wlib $^@ /n /b @temp.lnk
   del temp.lnk

.asm.obj :
   tasm $(tasm_$(version)) $(LIBVER_ARGS_TASM)  $[* $(obj_dir)\$^&

.c.obj :
   wcc386 $(wcc_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&


# VERSIONS RESTORATION - Build based on version
adlibfx.obj  : adlibfx.c dpmi.h task_man.h interrupt.h al_midi.h adlibfx.h

al_midi.obj  : al_midi.c dpmi.h interrupt.h sndcards.h blaster.h user.h al_midi.h _al_midi.h ll_man.h
   wcc386 $(al_midi_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

awe32.obj    : awe32.c dpmi.h blaster.h ctaweapi.h awe32.h

# VERSIONS RESTORATION - Build based on version
blaster.obj  : blaster.c dpmi.h dma.h irq.h blaster.h _blaster.h
blastold.obj  : blastold.c dpmi.h dma.h irq.h blaster.h _blaster.h

debugio.obj  : debugio.c debugio.h

dma.obj      : dma.c dma.h

dpmi.obj     : dpmi.c dpmi.h

# VERSIONS RESTORATION - adlibfx.h and pcfx.h are not relevant for all
# versions, but let's check for them either way
fx_man.obj   : fx_man.c ll_man.h sndcards.h multivoc.h blaster.h pas16.h &
   sndscape.h guswave.h sndsrc.h &
   adlibfx.h pcfx.h &
   user.h fx_man.h

$(timbre_file).obj : $(timbre_file).c

gus.obj      : gus.c usrhooks.h interrupt.h gusmidi.h guswave.h _guswave.h newgf1.h
   wcc386 $(gus_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

gusmidi.obj  : gusmidi.c usrhooks.h interrupt.h gusmidi.h newgf1.h
   wcc386 $(gusmidi_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

guswave.obj  : guswave.c debugio.h interrupt.h ll_man.h pitch.h user.h _guswave.h guswave.h newgf1.h multivoc.h

irq.obj      : irq.c irq.h

ll_man.obj   : ll_man.c dpmi.h ll_man.h

# VERSIONS RESTORATION - music.h is not relevant for
# all versions, but let's check for it either way
midi.obj     : midi.c interrupt.h dpmi.h standard.h task_man.h ll_man.h usrhooks.h &
   music.h &
   _midi.h midi.h
   wcc386 $(midi_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

mpu401.obj   : mpu401.c dpmi.h user.h mpu401.h

multivoc.obj : multivoc.c usrhooks.h interrupt.h dpmi.h ll_man.h sndcards.h &
   blaster.h sndscape.h pas16.h sndsrc.h pitch.h multivoc.h _multivc.h dma.h
   wcc386 $(multivoc_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

# VERSIONS RESTORATION - Build based on version
mv_mix.obj   : mv_mix.asm
mv_mix16.obj : mv_mix16.asm
mv_mix5.obj   : mv_mix5.asm
#
mvreverb.obj : mvreverb.asm

music.obj    : music.c ll_man.h task_man.h sndcards.h midi.h al_midi.h pas16.h blaster.h gusmidi.h mpu401.h awe32.h sndscape.h user.h music.h
!ifneq libver_libdef AL950724
   wcc386 $(music_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&
!endif

pas16.obj    : pas16.c dpmi.h dma.h interrupt.h irq.h pas16.h _pas16.h
   wcc386 $(pas16_$(version)) $[* $(LIBVER_ARGS) /fo=$(obj_dir)\$^&

# VERSIONS RESTORATION - Build based on version
pcfx.obj     : pcfx.c dpmi.h task_man.h interrupt.h pcfx.h

pitch.obj    : pitch.c dpmi.h standard.h pitch.h

sndscape.obj : sndscape.c interrupt.h dpmi.h dma.h irq.h sndscape.h _sndscap.h

sndsrc.obj   : sndsrc.c dpmi.h task_man.h sndcards.h user.h sndsrc.h

task_man.obj : task_man.c dpmi.h interrupt.h linklist.h usrhooks.h task_man.h
   wcc386 $(task_man_$(version)) $[* /fo=$(obj_dir)\$^&

user.obj     : user.c user.h

#myprint.obj  : myprint.c myprint.h
