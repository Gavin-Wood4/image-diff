# Use	make PI=1	to cross compile binaries for Raspberry Pi 5.
# Use	make HOST=1	to build only portable diff for the host processor. 

PI	?= 0
HOST	?= 0


HOST_CC		?= gcc
HOST_CFLAGS	?= -Wall -g -std=c11

PI_CC		?= aarch64-linux-gnu-gcc
PI_CFLAGS	?= -Wall -g -mcpu=cortex-a76

# Picks compiler and flags depending on command line switch and processor architecture detection.
ifneq ($(HOST),1)
	ifeq ($(PI),1)
		CC	:= $(PI_CC)
		CFLAGS	:= $(PI_CFLAGS)
		TARGETS	:= diff neon-diff
	else
		ifeq ($(shell uname -m),aarch64)	# Processor architecture is detected.
			CC	:= $(HOST_CC)
			CFLAGS	:= $(HOST_CFLAGS) -mcpu=native
			TARGETS	:= diff neon-diff
		else					# If processor architecture is not aarch64.
			CC	:= $(HOST_CC)
			CFLAGS	:= $(HOST_CFLAGS)
			TARGETS	:= diff
		endif
	endif
else
	CC	:= $(HOST_CC)
	CFLAGS	:= $(HOST_CFLAGS)
	TARGETS := diff
endif


COMMON = rgba_io.o
DIFF_OBJS = diff.o	$(COMMON)
NEON_OBJS = neon-diff.o	$(COMMON)


all: $(TARGETS)

diff: $(DIFF_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

neon-diff: $(NEON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@


%.o: %.c rgba_io.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f diff.o neon-diff.o rgba_io.o diff neon-diff $(TARGETS)


.PHONY: all clean
