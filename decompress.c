#include "helpers.h"

int main(int argc, char *argv[])
{
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

	buffer.bitptr -= getnbits(&buffer, 3, &ptr);

	tree = heap_maketree(&heap);

	write(&buffer, tree, &ptr);

	return 0;
}

