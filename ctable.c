#include "ctable.h"

#include <stdlib.h>

int ctable_init(ctable_t *table, unsigned int size, char is_encoding)
{
	table->is_encoding = is_encoding;
	table->num_elements = 0;
	table->size = size < 10 ? 10 : size;
	table->table = calloc(table->size, sizeof(ctnode_t));

	if(table->table)
		return 1;

	return 0;
}

void ctable_free(ctable_t *table)
{
	unsigned int i;

	if(table->is_encoding)
		for(i = 0; i < table->size; i++)
			if(table->table[i].bits) free(table->table[i].bits);

	free(table->table);
}

static int expand(ctable_t *table)
{
	unsigned int i;
	ctnode_t *old = table->table;

	table->table = calloc((table->size * table->size) / 2, sizeof(ctnode_t));

	if(!table->table)
	{
		table->table = old;
		return 0;
	}

	table->num_elements = 0;
	table->size *= 2;

	for(i = 0; i < table->size / 2; i++)
	{
		if(table->is_encoding)
			ctable_insert(table, &old[i]);
		else
			ctable_add(table, old[i].c, old[i].count);
	}

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

int ctable_add(ctable_t *table, unsigned int c, unsigned int amt)
{
	unsigned int i;
	ctnode_t *found;

	if(table->is_encoding) return 0;

	found = ctable_find(table, c);
	if(found != NULL) {
		found->count += amt;
		return 2;
	}

	if(table->num_elements == table->size)
		if(!expand(table)) return 0;

	i = hash(c, table->size);
	while(table->table[i].c != 0)
		i = (i + 1) % table->size;

	/* found is just an alias now */
	found = &table->table[i];
	found->c = c;
	found->count = amt;

	return 1;
}

#ifdef DEBUG

#include <stdio.h>
void ctable_print(const ctable_t *table)
{
	int first = 1;
	unsigned int i;

	fprintf(stderr, "ctable_t { ");
	
	
	for(i = 0; i < table->size; i++)
	{
		if(table->table[i].c != 0)
		{
			if(!first)
				fprintf(stderr, ", ");
			else
				first = 0;

			fprintf(stderr, "%x: ", table->table[i].c);

			if(table->is_encoding)
			{
				unsigned int b;
				for(b = 0; b < table->table[i].length; b++)
					fprintf(stderr, "%c", table->table[i].bits[b] + '0');
			}
			else
				fprintf(stderr, "%d", table->table[i].count);
		}
	}

	fprintf(stderr, " }\n");
}

#endif

