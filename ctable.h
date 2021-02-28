#ifndef CTABLE_H
#define CTABLE_H

typedef struct cbitnode
{
	char c;
	char *bits;
	unsigned short length;
}
cbnode_t;

typedef struct ctable
{
	cbnode_t *table;
	unsigned short num_elements;
	unsigned short size;
}
ctable_t;

int ctable_init(ctable_t*, unsigned short);
void ctable_free(ctable_t*);

cbnode_t *ctable_find(ctable_t*, char);
int ctable_insert(ctable_t*, cbnode_t);

#endif

