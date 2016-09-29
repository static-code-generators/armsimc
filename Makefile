IDIR = include
OBJ_DIR = build
# we want to place all objects in object directory.
OBJS = $(addprefix $(OBJ_DIR)/, shellcmds.o sim.o)
CC = clang
CFLAGS = -O2 -std=c99 -I $(IDIR)
exec = armsh
execobj = $(OBJ_DIR)/$(exec).o

all: $(exec)

$(exec): $(OBJS) $(execobj) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(exec) $(OBJS) $(execobj)

# compile the C-file in main directory to object in OBJ_DIR
# the | does some magic so this does not care about timestamp of OBJ_DIR
$(OBJS): $(OBJ_DIR)/%.o: %.c $(IDIR)/%.h | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(execobj): $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)

# because clean and all aren't filenames
.PHONY: clean all

# using -f option to supress file not found errors with rm
clean:
	-rm -f $(OBJS) $(execobj) $(exec)
