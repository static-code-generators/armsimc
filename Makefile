IDIR = include
BUILD = build
# we want to place all objects in object directory.
OBJS = $(addprefix $(BUILD)/, shellcmds.o sim.o)
CC = clang
CFLAGS = -O2 -std=c99 -I $(IDIR)
exec = $(BUILD)/armsh
execobj = $(exec).o

all: $(exec)

$(exec): $(OBJS) $(execobj) | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^

# compile the C-file in main directory to object in BUILD
# the | does some magic so this does not care about timestamp of BUILD
$(OBJS): $(BUILD)/%.o : %.c $(IDIR)/%.h | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(execobj): $(BUILD)/%.o : %.c | $(BUILD)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD): 
	mkdir -p $(BUILD)

# because clean and all aren't filenames
.PHONY: clean all

# using -f option to supress file not found errors with rm
# using -r option to recursively delete everything.
clean:
	-rm -rf $(BUILD)
