#ifndef HEAP_H
#define HEAP_H

#include "bitbuffer.h"

typedef struct cnode
{
	unsigned int c;
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

int cnode_init(cnode_t*, unsigned int, unsigned int);

int heap_init(heap_t*);
void heap_free(heap_t*);

void heap_sort(heap_t*);

int heap_insert(heap_t*, cnode_t*);

cnode_t *heap_getmin(heap_t*);

cnode_t *heap_maketree(heap_t*);

int heap_copyfreqs(heap_t*, bbuffer_t*);

#ifdef DEBUG
void heap_print(const heap_t*);
#endif

#endif

