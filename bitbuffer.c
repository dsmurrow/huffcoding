#include "bitbuffer.h"

#include <stdlib.h>

#define BBUFFER_INITIAL_SIZE 10

int bbuffer_init(bbuffer_t *buffer)
{
	buffer->size = BBUFFER_INITIAL_SIZE;
	buffer->buffer = calloc(buffer->size, sizeof(char));

	if(buffer->buffer)
	{
		buffer->bitptr = 0;
		return 1;
	}

	return 0;
}

void bbuffer_free(bbuffer_t *buffer)
{
	free(buffer->buffer);
}

static int expand(bbuffer_t *buffer)
{
	unsigned char *new_buffer = realloc(buffer->buffer, buffer->size * 2 * sizeof(char));

	if(new_buffer)
	{
		unsigned int i;
		for(i = buffer->size; i < buffer->size * 2; i++)
			new_buffer[i] &= 0;

		buffer->buffer = new_buffer;
		buffer->size *= 2;
		return 1;
	}

	return 0;
}

int bbuffer_addbit(bbuffer_t *buffer, char bit)
{
	const char shift = 7 - (buffer->bitptr % 8);
	const unsigned int index = buffer->bitptr / 8;

	buffer->buffer[index] |= (!!bit) << shift;

	if(++buffer->bitptr / 8 == buffer->size)
		if(!expand(buffer)) return 0;

	return 1;
}

int bbuffer_addbits(bbuffer_t *buffer, char *bits, unsigned int size)
{
	char shift;
	unsigned int initial = buffer->bitptr;
	unsigned int index = buffer->bitptr / 8;
	unsigned int i;

	for(i = 0; i < size; i++, buffer->bitptr++)
	{
		if(buffer->bitptr > initial && buffer->bitptr % 8 == 0)
			index++;

		if(index == buffer->size) 
			if(!expand(buffer)) return 0;

		shift = 7 - (buffer->bitptr % 8);

		buffer->buffer[index] |= (!!bits[i]) << shift;
	}

	return 1;
}

int bbuffer_addnum(bbuffer_t *buffer, unsigned long n, char startbit, char numbits)
{
	char *bits;
	int i;

	if(numbits < 1 || startbit < 0) return 0;

	bits = calloc(numbits, sizeof(char));
	for(i = 0; i < numbits && startbit - i >= 0; i++)
	{
		bits[i] = !!(n & (1 << (startbit - i)));
	}

	i = bbuffer_addbits(buffer, bits, i);

	free(bits);
	return i;
}

int bbuffer_addnbits(bbuffer_t *buffer, char value, unsigned int n)
{
	for(; n > 0; n--)
		bbuffer_addbit(buffer, value);

	return 1;
}

int bbuffer_merge(bbuffer_t *b1, bbuffer_t *b2)
{
	char bit;
	unsigned int i;
	for(i = 0; i < b2->bitptr; i++)
	{
		bit = b2->buffer[i / 8] & (1 << (7 - (i % 8)));
		bbuffer_addbit(b1, bit);
	}

	return 1;
}

#undef BBUFFER_INITIAL_SIZE


