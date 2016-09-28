OBJS = shellcmds.o sim.o
CC = clang
CFLAGS = -O2 -std=c99
exec = armsh
execobj = $(exec).o

all: $(exec)

$(exec): $(OBJS) $(execobj)
	$(CC) $(CFLAGS) -o $(exec) $(OBJS) $(execobj)

$(OBJS): %.o: %.h

# because clean and all aren't filenames
.PHONY: clean all

# using -f option to supress file not found errors with rm
clean:
	-rm -f $(OBJS) $(execobj) $(exec)
