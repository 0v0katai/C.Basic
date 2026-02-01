
all: rebuild lib/libcg.a lib/libcw.a

rebuild:
	rm -rf ../build-cg
	rm -rf ../build-cg-push

lib/libcg.a: libcg/libcg.a
	mv libcg/libcg.a .

libcg/libcg.a:
	make -C libcg

lib/libcw.a: libcw/libcw.a
	mv libcw/libcw.a .

libcw/libcw.a:
	make -C libcw

clean:
	make -C libcg clean
	make -C libcw clean