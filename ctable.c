#include "ctable.h"

#include <stdlib.h>

int ctable_init(ctable_t *table, unsigned int size)
{
	table->num_elements = 0;
	table->size = size;
	table->table = calloc(table->size, sizeof(ctnode_t));

	if(table->table)
		return 1;

	return 0;
}

void ctable_free(ctable_t *table)
{
	unsigned int i;
	for(i = 0; i < table->size; i++)
		if(table->table[i].bits) free(table->table[i].bits);

	free(table->table);
}

static int expand(ctable_t *table)
{
	unsigned int i;
	ctnode_t *old = table->table;

	table->table = calloc(table->size * 2, sizeof(ctnode_t));

	if(!table->table)
	{
		table->table = old;
		return 0;
	}

	table->num_elements = 0;
	table->size *= 2;

	for(i = 0; i < table->size / 2; i++)
		ctable_insert(table, &old[i]);

	free(old);

	return 1;
}

static unsigned int hash(unsigned int c, unsigned int table_size)
{
	unsigned int key = c;
	return (42227 * key + 22) % table_size;
}


ctnode_t *ctable_find(ctable_t *table, unsigned int c)
{
	unsigned int index = hash(c, table->size), i = index;
	ctnode_t *t = table->table;


	while(t[i].c != c && t[i].c != 0 && (i + 1) % table->size != index)
		i = (i + 1) % table->size;


	return t[i].c != c ? NULL : &t[i];
}

int ctable_insert(ctable_t *table, ctnode_t *node)
{
	unsigned int i;

	if(ctable_find(table, node->c)) return 2;

	if(table->num_elements == table->size)
		if(!expand(table)) return 0;

	i = hash(node->c, table->size);
	while(table->table[i].c != 0)
		i = (i + 1) % table->size;
	
	table->table[i] = *node;

	return 1;
}

