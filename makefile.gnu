# GNU make makefile

CFLAGS += -Wall -Wno-unused-variable

OBJS = tbl2pig.o hash.o cfile.o iff.o fix.o tables.o \
	error.o mglobal.o path.o piggy.o bmread.o mono.o \
	file.o stub.o

all: tbl2pig

tbl2pig: $(OBJS)

clean:
	rm -f $(OBJS)
