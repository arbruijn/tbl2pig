# Watcom wmake makefile

.EXTENSIONS:
.EXTENSIONS: .exe .obj .c .asm

OBJS = tbl2pig.obj hash.obj cfile.obj iff.obj fix.obj tables.obj &
	error.obj mglobal.obj path.obj piggy.obj bmread.obj mono.obj &
	file.obj stub.obj

all: tbl2pig.exe .SYMBOLIC

tbl2pig.exe: $(OBJS)
	wcl386 $<

.c.obj:
	wcc386 $<

clean:
	del *.obj
