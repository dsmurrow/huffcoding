#ifndef HELPERS_H
#define HELPERS_H

#include "ctable.h"
#include "heap.h"

#include <stdio.h>
#include <stdlib.h>

extern unsigned int f_size;



void fill_heap(bbuffer_t*, heap_t*, unsigned char, unsigned int*);

void traverse_tree(cnode_t*, ctable_t*, char*, unsigned short);

void get_utf_char_from_file(unsigned int*, FILE*);
void get_utf_char_from_bbuffer(unsigned int*, bbuffer_t*, unsigned int*);

void file_to_bits(const char*, ctable_t*, bbuffer_t*);

unsigned int getnbits(bbuffer_t*, char, unsigned int*);

int load_heap(const ctable_t*, heap_t*);

void readfile(const char*, heap_t*);

void read_remaining_bytes(FILE*, bbuffer_t*);

void write(bbuffer_t*, cnode_t*, unsigned int*);

#endif
