#include "helpers.h"

void fill_heap(bbuffer_t *buffer, heap_t *heap, unsigned char bpn, unsigned int *bptr)
{
	unsigned int i;
	for(i = 0; i < heap->num_elements; i++)
	{
		heap->heap[i] = malloc(sizeof(cnode_t));
		heap->heap[i]->left = heap->heap[i]->right = NULL;

		heap->heap[i]->c = getnbits(buffer, 8, bptr);
		if(heap->heap[i]->c & 0x80) get_utf_char_from_bbuffer(&heap->heap[i]->c, buffer, bptr);
		heap->heap[i]->count = getnbits(buffer, bpn, bptr);
	}

	heap_sort(heap);
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
		ctnode_t tnode;
		tnode.c = root->c;
		tnode.length = index;
		tnode.bits = malloc(tnode.length * sizeof(char));

		for(index = 0; index < tnode.length; index++)
			tnode.bits[index] = bits[index];

		ctable_insert(table, &tnode);

		return;
	}

	/* Traverse left */
	bits[index] = 0;
	traverse_tree(root->left, table, bits, index + 1);

	/* Traverse right */
	bits[index] = 1;
	traverse_tree(root->right, table, bits, index + 1);
}

void get_utf_char_from_file(unsigned int *c, FILE *f)
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

void get_utf_char_from_bbuffer(unsigned int *c, bbuffer_t *buffer, unsigned int *ptr)
{
	char bytes_left;

	if(!(*c & 0x80)) return;

	if((*c & 0xF0) == 0xF0)
		bytes_left = 3;
	else if((*c & 0xE0) == 0xE0)
		bytes_left = 2;
	else
		bytes_left = 1;

	*c = *c << (bytes_left * 8);
	while(bytes_left-- > 0)
		*c |= (getnbits(buffer, 8, ptr) << (bytes_left * 8));
}

void file_to_bits(const char *filename, ctable_t *table, bbuffer_t *buffer)
{
	unsigned int c;
	FILE *f = fopen(filename, "r");
	ctnode_t *table_entry;

	while((c = fgetc(f)) != EOF)
	{
		if(c & 0x80) get_utf_char_from_file(&c, f);
		table_entry = ctable_find(table, c);
		if(table_entry == NULL) {
			printf("Invalid character %X at ~%X\n", c, ftell(f));
			exit(1);
		}
		bbuffer_addbits(buffer, table_entry->bits, table_entry->length);
	}

	fclose(f);
}

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

int load_heap(const ctable_t* table, heap_t *heap) {
	unsigned int i;
	cnode_t *cnode;

	if(table->is_encoding) return 0;

	for(i = 0; i < table->size; i++) {
		if(table->table[i].c != 0) {
			cnode = malloc(sizeof(cnode_t));
			cnode_init(cnode, table->table[i].c, table->table[i].count);
			heap_insert(heap, cnode);
		}
	}

	return 1;
}

long int readfile(const char *filename, heap_t *heap)
{
	unsigned int c;
	long int file_size;
	ctable_t table;

	FILE *f = fopen(filename, "r");
	if(f == NULL)
		return 0l;

	c = ctable_init(&table, 9216, 0);
	if(!c) return 0l;

	while((c = fgetc(f)) != EOF)
	{
		if(c & 0x80) get_utf_char_from_file(&c, f);
		ctable_add(&table, c, 1);
	}

	load_heap(&table, heap);

	ctable_free(&table);

	fseek(f, 0l, SEEK_END);
	file_size = ftell(f);
	fclose(f);

	return file_size;
}

void read_remaining_bytes(FILE *f, bbuffer_t *buffer)
{
	int c;
	while((c = fgetc(f)) != EOF)
	{
		bbuffer_addnum(buffer, c, 7, 8);
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
			/* printf("character = %x\n", cursor->c); */

			if((cursor->c & 0xF0000000) == 0xF0000000)
			{
				fputc(cursor->c >> 24, f); /* printf("4writing %x-", cursor->c >> 24); */
				fputc((cursor->c & ~0xFF000000) >> 16, f); /* printf("%x-", (cursor->c & ~0xFF000000) >> 16); */
				fputc((cursor->c & ~0xFFFF0000) >> 8, f); /* printf("%x-", (cursor->c & ~0xFFFF0000) >> 8); */
			}
			else if((cursor->c & 0xFFE00000) == 0xE00000)
			{
				fputc(cursor->c >> 16, f); /* printf("3writing %x-", cursor->c >> 16); */
				fputc((cursor->c & ~0xFF0000) >> 8, f); /* printf("%x-", (cursor->c & ~0xFF0000) >> 8); */
			}
			else if((cursor->c & 0xFFFFC000) == 0xC000)
			{
				fputc(cursor->c >> 8, f); /* printf("2writing %x-", cursor->c >> 8); */
			}

			fputc(cursor->c & 0xFF, f); /* printf("%x\n", cursor->c & 0xFF); */
			cursor = root;
		}
	}

	fclose(f);
}
