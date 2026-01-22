
all: lib/libmpm.a lib/libfxcg.a

lib/libmpm.a: libmpm/libmpm.a
	mv libmpm/libmpm.a .

libmpm/libmpm.a:
	make -C libmpm

lib/libfxcg.a: libfxcg/libfxcg.a
	mv libfxcg/libfxcg.a .

libfxcg/libfxcg.a:
	make -C libfxcg

clean:
	make -C libmpm clean
	make -C libfxcg clean
