
all: lib/libfxcg.a lib/libc.a

lib/libfxcg.a: libfxcg/libfxcg.a
	mv libfxcg/libfxcg.a ../

libfxcg/libfxcg.a:
	make -C libfxcg

lib/libc.a: libc/libc.a
	mv libc/libc.a ../

libc/libc.a:
	make -C libc

clean:
	make -C libfxcg clean
	make -C libc clean
