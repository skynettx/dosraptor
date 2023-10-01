# Apogee Sound System backed DMX wrapper makefile

# --------------------------------------------------------------------------
#
#      4r  use 80486 timings and register argument passing
#       c  compile only
#      d1  include line number debugging information
#      d2  include full sybolic debugging information
#      ei  force enums to be of type int
#       j  change char default from unsigned to signed
#      oa  relax aliasing checking
#      od  do not optimize
#  oe[=#]  expand functions inline, # = quads (default 20)
#      oi  use the inline library functions
#      om  generate inline 80x87 code for math functions
#      ot  optimize for time
#      ox  maximum optimization
#       s  remove stack overflow checks
#     zp1  align structures on bytes
#      zq  use quiet mode
#  /i=dir  add include directories
#
# --------------------------------------------------------------------------

CCOPTS = /d2 /omaxet /zp1 /4r /ei /j /zq

GLOBOBJS = &
 dmx.obj &
 dpmiapi.obj &
 usrhooks.obj &
 mus2mid.obj &
 tsmapi.obj

apodmx.lib : $(GLOBOBJS)
 wlib $^@ /n /b $<

tsmapi.obj : tsmapi.c
 wcc386 $(CCOPTS) /zu tsmapi
 
.c.obj :
 wcc386 $(CCOPTS) $[*

clean : .SYMBOLIC
 del *.obj
 del *.lib
