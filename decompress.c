#include "helpers.h"

int main(int argc, char *argv[])
{
	#ifdef DEBUG
	char debug_buffer[70];
	unsigned char bits_left;
	ctable_t debug_table;
	#endif

	unsigned char bpn, hold;
	unsigned int i, num_bits = 0, ptr = 0;
	FILE *f;
	cnode_t *tree;
	bbuffer_t buffer;
	heap_t heap;

	if(argc > 1)
		f = fopen(argv[1], "rb");
	else
		return -1;

	if(f == NULL)
		return -2;

	/* Read the first byte of the file which describes how many bits
	 * are used to represent the counts of each character */
	fread(&bpn, sizeof(char), 1, f);


	/* The next 4 bytes holds information about how many
	 * elements will be in the heap */
	heap.num_elements = 0;
	for(i = 0; i < 4; i++)
	{
		fread(&hold, sizeof(char), 1, f);
		heap.num_elements |= hold << (8 * (3 - i));
	}

	bbuffer_init(&buffer);
	/* The rest of the file holds the huffman-encoded bit
	 * representations of characters */
	read_remaining_bytes(f, &buffer);

	fclose(f);



	heap.size = heap.num_elements;
	heap.heap = malloc(heap.size * sizeof(cnode_t*));

	fill_heap(&buffer, &heap, bpn, &ptr);

	#ifdef DEBUG
	bits_left = getnbits(&buffer, 3, &ptr);
	fprintf(stderr, "%d bits left\n", bits_left);
	buffer.bitptr -= bits_left;
	#else
	buffer.bitptr -= getnbits(&buffer, 3, &ptr);
	#endif

	tree = heap_maketree(&heap);

	#ifdef DEBUG
	/* print heap */
	heap_print(&heap);

	/* print table */
	ctable_init(&debug_table, heap.size * heap.size, 1);

	traverse_tree(tree, &debug_table, debug_buffer, 0);
	ctable_print(&debug_table);

	ctable_free(&debug_table);
	#endif


	write(&buffer, tree, &ptr);

	return 0;
}

