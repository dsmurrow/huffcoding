#include "ctable.h"
#include "heap.h"

#include <stdio.h>
#include <stdlib.h>

static unsigned int f_size = 0;

void traverse_tree(cnode_t*, ctable_t*, char*, unsigned short);
void file_to_bits(const char*, ctable_t*, bbuffer_t*);

void get_utf_char(unsigned int *c, FILE *f)
{
	char bytes_left;
	int temp;

	if(!(*c & 0x80))
		return;

	if((*c & 0xF0) == 0xF0)
		bytes_left = 3;
	else if((*c & 0xE0) == 0xE0)
		bytes_left = 2;
	else
		bytes_left = 1;

	*c = *c << (bytes_left * 8);
	while(bytes_left > 0 && (temp = fgetc(f)) != EOF)
	{
		*c |= temp << (--bytes_left * 8);
	}
}

void readfile(const char *filename, heap_t *heap)
{
	unsigned int c;

	FILE *f = fopen(filename, "r");
	if(f == NULL)
		return;

	while((c = fgetc(f)) != EOF)
	{
		if(c & 0x80) get_utf_char(&c, f);
		heap_addc(heap, c);
		f_size++;
	}

	fclose(f);
}

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

	bbuffer_init(&bb1);
	heap_copyfreqs(&heap, &bb1);

	root = heap_maketree(&heap);

	ctable_init(&table, heap.num_elements * heap.num_elements);
	buffer = calloc(70, sizeof(char));
	traverse_tree(root, &table, buffer, 0);
	free(buffer);

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

/**
 * Recursively traverse the tree to find the Huffman encoding values of the characters
 * in the text.
 *
 * @param root	Root of the Huffman tree
 * @param table	Hash table that will store the Huffman encodings
 * @param bits 	Char buffer to store the working Huffman encoding
 * @param index	Current index in the bit buffer
 */
void traverse_tree(cnode_t *root, ctable_t *table, char *bits, unsigned short index)
{
	if(root->left == NULL && root->right == NULL) /* The character nodes are always the leaves of 
							 the tree. So this is accessed if at a leaf node */
	{
		cbnode_t tnode;
		tnode.c = root->c;
		tnode.length = index;
		tnode.bits = malloc(tnode.length * sizeof(char));

		for(index = 0; index < tnode.length; index++)
			tnode.bits[index] = bits[index];

		ctable_insert(table, tnode);

		return;
	}

	/* Traverse left */
	bits[index] = 0;
	traverse_tree(root->left, table, bits, index + 1);

	/* Traverse right */
	bits[index] = 1;
	traverse_tree(root->right, table, bits, index + 1);
}

void file_to_bits(const char *filename, ctable_t *table, bbuffer_t *buffer)
{
	unsigned int c;
	FILE *f = fopen(filename, "r");
	cbnode_t *table_entry;

	while((c = fgetc(f)) != EOF)
	{
		if(c & 0x80) get_utf_char(&c, f);
		table_entry = ctable_find(table, c);
		bbuffer_addbits(buffer, table_entry->bits, table_entry->length);
	}

	fclose(f);
}

