CC=gcc
DBG=gdb

ALWAYS = bitbuffer.c ctable.c heap.c helpers.c
COMP_EX = compress
DECOMP_EX = decompress

DBG_FLAGS = -ex run --batch --args
DBG_FLAG_CC = -g
DBG_COMP = dbg_comp
DBG_DECOMP = dbg_decomp

compile-all: compile-compress compile-decompress

compile-compress:
	$(CC) $(flags) -o $(COMP_EX) $(ALWAYS) compress.c

run-compress:
	./compress $(file)

debug-compress:
	$(CC) $(DBG_FLAG_CC) -o $(DBG_COMP) $(COMP_FILES)
	$(DBG) $(DBG_FLAGS) $(DBG_COMP) $(file)
	rm $(DBG_COMP)


compile-decompress:
	$(CC) $(flags) -o $(DECOMP_EX) $(ALWAYS) decompress.c

run-decompress:
	./decompress $(file)



