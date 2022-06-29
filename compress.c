#include "helpers.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned int i;
	double compression;
	char *buffer, bits_left;
	bbuffer_t bb1, bb2;
	cnode_t *root;
	ctable_t table;
	heap_t heap;

	heap_init(&heap);

	if(argc > 1)
		readfile(argv[1], &heap);
	else return 1;

	bbuffer_init(&bb1);
	heap_copyfreqs(&heap, &bb1);

	root = heap_maketree(&heap);

	ctable_init(&table, heap.num_elements * heap.num_elements, 1);
	buffer = calloc(70, sizeof(char));
	traverse_tree(root, &table, buffer, 0);
	free(buffer);

	#ifdef DEBUG
	ctable_print(&table);
	#endif

	/* Get the text from the file and put the huffman encodings of the
	 * characters into the seconds bit buffer */
	bbuffer_init(&bb2);
	if(argc > 1)
		file_to_bits(argv[1], &table, &bb2);

	/* Calculate how many untouched bits will be at the end of the file */
	bits_left = (bb2.bitptr + bb1.bitptr + 3) % 8 ? 8 - ((bb2.bitptr + bb1.bitptr + 3) % 8) : 0;
	bbuffer_addnum(&bb1, bits_left, 2, 3);


	/* Merge both buffers to prepare for writing to file */
	bbuffer_merge(&bb1, &bb2);
	bbuffer_free(&bb2);

	FILE *f = fopen("compress.out", "wb");

	fwrite(bb1.buffer, sizeof(char), (bb1.bitptr / 8) + !!bits_left, f);

	fclose(f);

	compression = (double)(bb1.bitptr / 8 + !!bits_left) / (double)(f_size);
	printf("Compressed file is %.1lf%% the size of the original file\n", compression * 100.0);

	return 0;
}






