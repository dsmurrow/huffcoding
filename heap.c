#include "heap.h"

#include <stdlib.h>

#define HEAP_INITIAL_SIZE 14

int cnode_init(cnode_t *node, unsigned int c, unsigned int amount)
{
	node->c = c;
	node->count = amount;
	node->left = NULL;
	node->right = NULL;

	return 1;
}

int heap_init(heap_t *heap)
{
	heap->size = HEAP_INITIAL_SIZE;
	heap->num_elements = 0;

	heap->heap = calloc(heap->size, sizeof(cnode_t*));

	if(heap->heap == NULL) return 0;
	return 1;
}

void heap_free(heap_t *heap)
{
	unsigned int i;
	for(i = 0; i < heap->num_elements; i++)
		free(heap->heap[i]);

	free(heap->heap);
}


static int heap_expand(heap_t *heap)
{
	cnode_t **try = realloc(heap->heap, heap->size * 2 * sizeof(cnode_t*));

	if(try)
	{
		heap->heap = try;
		heap->size *= 2;

		return 1;
	}

	return 0;
}

static void heapify(heap_t *heap, unsigned int i)
{
	cnode_t **arr = heap->heap;
	unsigned int smallest = i;
	unsigned int l = 2 * i + 1;
	unsigned int r = 2 * i + 2;

	if(l < heap->num_elements && arr[l]->count < arr[smallest]->count)
		smallest = l;

	if(r < heap->num_elements && arr[r]->count < arr[smallest]->count)
		smallest = r;

	if(smallest != i)
	{
		cnode_t *swp = arr[i];
		arr[i] = arr[smallest];
		arr[smallest] = swp;

		heapify(heap, smallest);
	}
}

static void sift_up(heap_t *heap, unsigned int i)
{
	#define GET_PARENT(i) ((i - 1) / 2)

	unsigned int parent = GET_PARENT(i);
	cnode_t *swp;
	while(i > 0 && heap->heap[i]->count < heap->heap[parent]->count)
	{
		swp = heap->heap[parent];
		heap->heap[parent] = heap->heap[i];
		heap->heap[i] = swp;

		i = parent;
		parent = GET_PARENT(i);
	}

	#undef GET_PARENT
}

int heap_insert(heap_t *heap, cnode_t *node)
{
	if(heap->num_elements == heap->size)
		if(!heap_expand(heap)) return 0;

	heap->heap[heap->num_elements] = node;
	
	sift_up(heap, heap->num_elements++);

	return 1;
}

cnode_t *heap_getmin(heap_t *heap)
{
	if(heap->num_elements == 0) return 0;

	cnode_t *swp = heap->heap[0];
	heap->heap[0] = heap->heap[--heap->num_elements];

	heapify(heap, 0);

	return swp;
}

static char num_bytes(unsigned int n)
{
	char msb = 31;

	while(msb > 0 && !(n & (1 << msb)))
		msb--;

	if(msb > 23)
		return 4;
	if(msb > 15)
		return 3;
	if(msb > 7)
		return 2;
	return 1;
}

int heap_copyfreqs(heap_t *heap, bbuffer_t *buffer)
{
	unsigned char bpn, bytes;
	unsigned int num_bits, most_letters = 0;
	cnode_t *last_node;
	heap_t new_heap;
	
	if(heap->size == 0) return 0;

	new_heap = *heap;
	new_heap.heap = malloc(heap->num_elements * sizeof(cnode_t*));

	/* num_bits is just acting as a generic iterator here */
	for(num_bits = 0; num_bits < heap->num_elements; num_bits++)
		new_heap.heap[num_bits] = heap->heap[num_bits];

	/* Empty out new heap to get most frequent character so the program
	 * can calculate how many bits it needs to represent the quantities */
	while(new_heap.num_elements > 0)
	{
		last_node = heap_getmin(&new_heap);
	}

	most_letters = last_node->count;

	free(new_heap.heap);


	/* Calculate the maximum number of bits required to represent the letter frequencies */
	for(bpn = 1, num_bits = 2; num_bits <= most_letters; bpn++, num_bits *= 2);

	num_bits = 8 * heap->num_elements + bpn * heap->num_elements;


	bbuffer_addnum(buffer, bpn, 7, 8);
	bbuffer_addnum(buffer, heap->num_elements, 31, 32);

	/* num_bits being a generic iterator again */
	for(num_bits = 0; num_bits < heap->num_elements; num_bits++)
	{
		last_node = heap->heap[num_bits];

		bytes = num_bytes(last_node->c);

		bbuffer_addnum(buffer, last_node->c, (bytes * 8) - 1, bytes * 8);
		bbuffer_addnum(buffer, last_node->count, bpn - 1, bpn);
	}

	return 1;
}

static cnode_t *make_treenode(cnode_t *left, cnode_t *right)
{
	cnode_t *root = malloc(sizeof(cnode_t));

	root->c = 0;
	root->count = left->count + right->count;
	root->left = left;
	root->right = right;
	
	return root;
}

cnode_t *heap_maketree(heap_t *heap)
{
	unsigned int i;
	cnode_t *left, *right;
	heap_t copy;

	copy.heap = malloc(heap->num_elements * sizeof(cnode_t*));
	copy.num_elements = copy.size = heap->num_elements;

	for(i = 0; i < copy.num_elements; i++)
		copy.heap[i] = heap->heap[i];

	while(copy.num_elements > 1)
	{
		left = heap_getmin(&copy);
		right = heap_getmin(&copy);

		heap_insert(&copy, make_treenode(left, right));
	}

	left = copy.heap[0]; /* left becomes root of the tree */

	free(copy.heap);

	return left;
}

#undef HEAP_INITIAL_SIZE

