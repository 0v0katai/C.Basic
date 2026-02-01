TOOLCHAIN_PREFIX=sh-elf-
CC=$(TOOLCHAIN_PREFIX)gcc
AS=$(TOOLCHAIN_PREFIX)as
AR=$(TOOLCHAIN_PREFIX)ar
CFLAGS=-c -ffunction-sections -fdata-sections -I ../../src-cg/include \
	   -Os -mhitachi -m4a-nofpu -mb -Wall -Wextra
ARFLAGS=rs
SOURCES=$(wildcard syscalls/*.S) crt0.S
OBJECTS=$(SOURCES:.S=.o)
LIBRARY=libcg.a

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
	rm $(OBJECTS)

