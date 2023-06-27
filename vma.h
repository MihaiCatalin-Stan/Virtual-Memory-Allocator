#pragma once
#include <inttypes.h>
#include <stddef.h>

typedef struct node {
	void *data;
	struct node *next, *prev;
} node_t;

typedef struct {
	unsigned int size, data_size;
	node_t *head, *tail;
} list_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	list_t *miniblock_list;
} block_t;

typedef struct {
	uint64_t start_address;
	size_t size;
	uint8_t perm;
	void *rw_buffer;
} miniblock_t;

typedef struct {
	uint64_t arena_size;
	list_t *alloc_list;
} arena_t;

list_t *list_create(unsigned int data_size);
void add_node(list_t *list, unsigned int n, const void *new_data);
node_t *remove_node(list_t *list, unsigned int n);
void list_free(list_t *pp_list);

arena_t *alloc_arena(const uint64_t size);
void dealloc_arena(arena_t *arena);

int alloc_errors(arena_t *arena, const uint64_t address, const uint64_t size);
miniblock_t *alloc_miniblock(arena_t *arena, const uint64_t address,
							 const uint64_t size);
block_t *alloc_in_func_block(arena_t *arena, const uint64_t address,
							 const uint64_t size);
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size);
void free_block(arena_t *arena, const uint64_t address);

void read(arena_t *arena, uint64_t address, uint64_t size);
void write(arena_t *arena, const uint64_t address,  const uint64_t size,
		   int8_t *data);
void turn_to_char_perms(int8_t perm, char *permissions);
void pmap(const arena_t *arena);
void mprotect(arena_t *arena, uint64_t address, int8_t *permission);
