#include "ctable.h"

#include <stdlib.h>

int ctable_init(ctable_t *table, unsigned short size)
{
	table->num_elements = 0;
	table->size = size;
	table->table = calloc(table->size, sizeof(cbnode_t));

	if(table->table)
		return 1;

	return 0;
}

void ctable_free(ctable_t *table)
{
	unsigned short i;
	for(i = 0; i < table->size; i++)
		if(table->table[i].bits) free(table->table[i].bits);

	free(table->table);
}

static int expand(ctable_t *table)
{
	unsigned int i;
	cbnode_t *old = table->table;

	table->table = calloc(table->size * 2, sizeof(cbnode_t));

	if(!table->table)
	{
		table->table = old;
		return 0;
	}

	table->num_elements = 0;
	table->size *= 2;

	for(i = 0; i < table->size / 2; i++)
		ctable_insert(table, old[i]);

	free(old);

	return 1;
}

static unsigned short hash(char c, unsigned short table_size)
{
	unsigned short key = c;
	return (42227 * key + 22) % table_size;
}

cbnode_t *ctable_find(ctable_t *table, char c)
{
	unsigned short index = hash(c, table->size), i = index;
	cbnode_t *t = table->table;

	while(t[i].c != c && t[i].c != 0 && i != (index - 1 + table->size) % table->size)
		i = (i + 1) % table->size;

	return t[i].c != c ? NULL : &t[i];
}

int ctable_insert(ctable_t *table, cbnode_t node)
{
	unsigned short i;

	if(ctable_find(table, node.c)) return 2;

	if(table->num_elements == table->size)
		if(!expand(table)) return 0;

	i = hash(node.c, table->size);
	while(table->table[i].c != 0)
		i = (i + 1) % table->size;
	
	table->table[i] = node;

	return 1;
}

