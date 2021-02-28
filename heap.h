#ifndef HEAP_H
#define HEAP_H

#include "bitbuffer.h"

typedef struct cnode
{
	char c;
	unsigned int count;
	struct cnode *left;
	struct cnode *right;
}
cnode_t;


typedef struct heap
{
	unsigned int num_elements;
	unsigned int size;
	cnode_t **heap;
}
heap_t;

int cnode_init(cnode_t*, char);

int heap_init(heap_t*);
void heap_free(heap_t*);

cnode_t *heap_find(heap_t*, char);

int heap_insert(heap_t*, cnode_t*);
int heap_addc(heap_t*, char);

cnode_t *heap_getmin(heap_t*);

cnode_t *heap_maketree(heap_t*);

int heap_copyfreqs(heap_t*, bbuffer_t*);

#endif

