#include "heap.h"

#include <stdio.h>
#include <stdlib.h>


unsigned int getnbits(bbuffer_t *buffer, char n, unsigned int *ptr)
{
	unsigned char byte, bit;
	unsigned int ptr_initial = *ptr, index = *ptr / 8;
	unsigned int nbits = 0;

	for(; *ptr - ptr_initial < n && *ptr < buffer->bitptr; (*ptr)++)
	{
		if(*ptr % 8 == 0 && *ptr > ptr_initial)
			index++;

		byte = buffer->buffer[index];
		bit = !!(byte & (1 << (7 - (*ptr % 8))));
		nbits |= bit << (n - 1 - (*ptr - ptr_initial));
	}

	return nbits;
}

void read_remaining_bytes(FILE *f, bbuffer_t *buffer)
{
	int c;
	while((c = fgetc(f)) != EOF)
	{
		bbuffer_addnum(buffer, c, 7, 8);
	}
}

void fill_heap(bbuffer_t *buffer, heap_t *heap, unsigned char bpn, unsigned int *bptr)
{
	unsigned int i;
	for(i = 0; i < heap->num_elements; i++)
	{
		heap->heap[i] = malloc(sizeof(cnode_t));
		heap->heap[i]->left = heap->heap[i]->right = NULL;

		heap->heap[i]->c = getnbits(buffer, 8, bptr);
		heap->heap[i]->count = getnbits(buffer, bpn, bptr);
	}
}

void write(bbuffer_t *buffer, cnode_t *root, unsigned int *bptr)
{
	char bit;
	cnode_t *cursor = root;
	FILE *f = fopen("decompress.txt", "w");

	while(*bptr < buffer->bitptr)
	{
		bit = getnbits(buffer, 1, bptr);

		if(bit) cursor = cursor->right;
		else cursor = cursor->left;

		if(cursor->c != 0)
		{
			fputc(cursor->c, f);
			cursor = root;
		}
	}

	fclose(f);
}

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


	/* The next 4 bytes holds information about how many of the
	 * following bits are storing character-count data */	
	for(i = 0; i < 4; i++)
	{
		fread(&hold, sizeof(char), 1, f);
		num_bits |= hold << (8 * (3 - i));
	}

	bbuffer_init(&buffer);
	/* The rest of the file holds the huffman-encoded bit
	 * representations of characters */
	read_remaining_bytes(f, &buffer);

	fclose(f);



	heap.size = heap.num_elements = num_bits / (bpn + 8);
	heap.heap = malloc(heap.size * sizeof(cnode_t*));

	fill_heap(&buffer, &heap, bpn, &ptr);

	buffer.bitptr -= getnbits(&buffer, 3, &ptr);

	tree = heap_maketree(&heap);

	write(&buffer, tree, &ptr);

	return 0;
}

