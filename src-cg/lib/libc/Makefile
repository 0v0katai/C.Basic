TOOLCHAIN_PREFIX=sh-elf-
CC=$(TOOLCHAIN_PREFIX)gcc
AS=$(TOOLCHAIN_PREFIX)as
AR=$(TOOLCHAIN_PREFIX)gcc-ar
CFLAGS=-c -ffunction-sections -fdata-sections -Os -flto=auto -ffat-lto-objects \
	   -m4a-nofpu -mhitachi -ffreestanding -Wall -Wsystem-headers -g -I../../include
LDFLAGS=-Wl,-static -lfxcg
ARFLAGS=rsv
FXLIBCSRC=$(wildcard fxlibc/*.c) fxlibc/grisu2b_59_56/grisu2b_59_56.c
OBJECTS=setjmp.o stdio.o stdlib.o ctype.o string.o unistd.o time.o $(FXLIBCSRC:.c=.o)
LIBRARY=libc.a

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS)
