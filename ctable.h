#ifndef CTABLE_H
#define CTABLE_H

typedef struct cbitnode
{
	unsigned int c;
	char *bits;
	unsigned int length;
}
cbnode_t;

typedef struct ctable
{
	cbnode_t *table;
	unsigned int num_elements;
	unsigned int size;
}
ctable_t;

int ctable_init(ctable_t*, unsigned int);
void ctable_free(ctable_t*);

cbnode_t *ctable_find(ctable_t*, unsigned int);
int ctable_insert(ctable_t*, cbnode_t*);

#endif

