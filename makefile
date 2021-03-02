compile-compress:
	gcc -o compress bitbuffer.c heap.c ctable.c compress.c

run-compress:
	./compress $(file)

compile-decompress:
	gcc -o decompress bitbuffer.c heap.c decompress.c

run-decompress:
	./decompress $(file)

compile-all:
	gcc -o compress bitbuffer.c heap.c ctable.c compress.c
	gcc -o decompress bitbuffer.c heap.c decompress.c

