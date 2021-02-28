#ifndef BITBUFFER_H
#define BITBUFFER_H

typedef struct bbuffer
{
	unsigned char *buffer;
	unsigned int bitptr;
	unsigned int size;
}
bbuffer_t;

int bbuffer_init(bbuffer_t*);
void bbuffer_free(bbuffer_t*);

int bbuffer_addbit(bbuffer_t*, char);
int bbuffer_addbits(bbuffer_t*, char*, unsigned int);

int bbuffer_addnbits(bbuffer_t*, char, unsigned int);

int bbuffer_addnum(bbuffer_t*, unsigned long, char, char);

int bbuffer_merge(bbuffer_t*, bbuffer_t*);

#endif

