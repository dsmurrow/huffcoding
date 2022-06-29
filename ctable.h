#ifndef CTABLE_H
#define CTABLE_H

typedef struct ctablenode
{
	unsigned int c;
	union
	{
		struct
		{
			char *bits;
			unsigned int length;
		};
		unsigned int count;
	};
}
ctnode_t;

typedef struct ctable
{
	char is_encoding;
	ctnode_t *table;
	unsigned int num_elements;
	unsigned int size;
}
ctable_t;

int ctable_init(ctable_t*, unsigned int, char);
void ctable_free(ctable_t*);

ctnode_t *ctable_find(ctable_t*, unsigned int);
int ctable_insert(ctable_t*, ctnode_t*);

int ctable_add(ctable_t*, unsigned int, unsigned int);

#ifdef DEBUG
void ctable_print(const ctable_t*);
#endif

#endif

