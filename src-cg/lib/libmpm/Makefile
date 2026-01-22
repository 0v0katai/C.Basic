TOOLCHAIN_PREFIX=sh-elf-
CC=$(TOOLCHAIN_PREFIX)gcc
AS=$(TOOLCHAIN_PREFIX)as
AR=$(TOOLCHAIN_PREFIX)ar
CFLAGS=-c -ffunction-sections -fdata-sections \
       -m4a-nofpu -mb -Os -mhitachi -nostdlib \
       -Wall -Wno-int-conversion
ARFLAGS=rs
SOURCES=crt0.S syscalls.c
OBJECTS=crt0.o syscalls.o
LIBRARY=libmpm.a

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS) 

.S.o:
	$(CC) $(CFLAGS) $< -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(LIBRARY)

