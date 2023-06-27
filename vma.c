#include "vma.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// de adaugat antete functii in vma.h

list_t *list_create(unsigned int data_size)
{
	list_t *new_list = malloc(sizeof(list_t));
	new_list->data_size = data_size;
	new_list->size = 0;
	new_list->head = NULL;
	new_list->tail = NULL;
	return new_list;
}

void add_node(list_t *list, unsigned int n, const void *new_data)
{
	node_t *new_node = malloc(sizeof(node_t));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);
	new_node->next = NULL;
	new_node->prev = NULL;

	if (n >= list->size && list->size) { // Addin node on the last position
		list->tail->next = new_node;
		new_node->prev = list->tail;
		list->tail = new_node;
	} else if (n <= 0) { // Adding node on the first slot in a list
		new_node->next = list->head;
		if (list->size) { // When the list already has nodes
			list->head->prev = new_node;
			list->head = new_node;
		} else { // When the list is fresh
			list->head = new_node;
			list->tail = new_node;
		}
	} else if (n < list->size) { // Adding node on any other position
		node_t *cursor = list->head;
		for (unsigned int pos = 0; pos < n - 1; pos++)
			cursor = cursor->next;
		new_node->next = cursor->next;
		new_node->prev = cursor;
		cursor->next->prev = new_node;
		cursor->next = new_node;
	}
	list->size++;
}

node_t *remove_node(list_t *list, unsigned int n)
{
	if (n >= list->size - 1 && list->size - 1) { // remove last if list->size>1
		list->tail->prev->next = NULL;
		node_t *deleted_node = list->tail;
		list->tail = list->tail->prev;
		list->size--;
		return deleted_node;
	} else if (n <= 0) { // remove from first pos
		node_t *deleted_node = list->head;
		if (list->size > 1) {
			list->head = list->head->next;
			list->head->prev = NULL;
		} else {
			list->head = NULL;
			list->tail = NULL;
		}
		list->size--;
		return deleted_node;
	}
	// remove any other node
	node_t *deleted_node = list->head, *cursor = NULL;
	for (unsigned int pos = 0; pos < n; pos++) {
		cursor = deleted_node;
		deleted_node = deleted_node->next;
	}
	cursor->next = deleted_node->next;
	deleted_node->next->prev = cursor;
	list->size--;
	return deleted_node;
}

void list_free(list_t *pp_list)
{
	while (pp_list->size) { // remove first node until list is null
		node_t *deleted_node = pp_list->head;
		pp_list->head = pp_list->head->next;
		free(deleted_node->data);
		free(deleted_node);
		pp_list->size--;
	}
	free(pp_list);
}

arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = malloc(sizeof(arena_t));
	if (!arena) {
		printf("Error allocating the arena!\n");
		exit(0);
	}
	arena->arena_size = size;
	arena->alloc_list = list_create(sizeof(block_t));
	return arena;
}

void dealloc_arena(arena_t *arena)
{
	node_t *cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		node_t *cursor_mb = ((block_t *)cursor->data)->miniblock_list->head;
		for (unsigned int j = 0;
		j < ((block_t *)cursor->data)->miniblock_list->size; j++) {
			free(((miniblock_t *)cursor_mb->data)->rw_buffer);
			cursor_mb = cursor_mb->next;
		}
		/* free la lista de miniblocuri, nodurile din el
		si data urile din noduri (miniblocurile insesi) */
		list_free(((block_t *)cursor->data)->miniblock_list);
		cursor = cursor->next;
	}
	// free la lista de blocuri, nodurile adiacente si blocurile in sine
	list_free(arena->alloc_list);
	free(arena);
}

// Abordam cazurile de eroare pentru alloc_block
int alloc_errors(arena_t *arena, const uint64_t address, const uint64_t size)
{
	node_t *cursor = arena->alloc_list->head;
	if (address >= arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return 1;
	}
	if (address + size > arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return 1;
	}
	while (cursor) {
		block_t *cursor_block = (block_t *)cursor->data;
		if (cursor_block->size + cursor_block->start_address >
		address + size && cursor_block->start_address < address + size) {
			printf("This zone was already allocated.\n");
			return 1;
		}
		if (cursor_block->size + cursor_block->start_address >
		address && cursor_block->start_address < address) {
			printf("This zone was already allocated.\n");
			return 1;
		}
		if (cursor_block->size + cursor_block->start_address <
		address + size && cursor_block->start_address > address) {
			printf("This zone was already allocated.\n");
			return 1;
		}
		if (cursor_block->size + cursor_block->start_address ==
		address + size && cursor_block->start_address == address) {
			printf("This zone was already allocated.\n");
			return 1;
		}
		cursor = cursor->next;
	}
	return 0;
}

miniblock_t *alloc_miniblock(arena_t *arena, const uint64_t address,
							 const uint64_t size)
{
	miniblock_t *new_mb = malloc(sizeof(miniblock_t));
	if (!new_mb) {
		printf("Error allocating miniblocking!\n");
		dealloc_arena(arena);
		exit(1);
	}
	new_mb->start_address = address;
	new_mb->size = size;
	new_mb->perm = 6;
	new_mb->rw_buffer = calloc(size, 1);
	if (!new_mb->rw_buffer) {
		printf("Error allocating buffer\n");
		free(new_mb);
		dealloc_arena(arena);
		exit(1);
	}
	return new_mb;
}

block_t *alloc_in_func_block(arena_t *arena, const uint64_t address,
							 const uint64_t size)
{
	block_t *new_blk = malloc(sizeof(block_t));
	if (!new_blk) {
		printf("Error allocating block!\n");
		dealloc_arena(arena);
		exit(1);
	}
	new_blk->start_address = address;
	new_blk->size = size;
	new_blk->miniblock_list = list_create(sizeof(miniblock_t));
	return new_blk;
}

/* Din cauza limitarii de linii per functie impuse de checker voi comenta
cazurile din functie aici:
Prima data verificam daca lista de blocuri e goala, caz in care adaugam
miniblocul intr un bloc nou
Ulterior parcurgem blocurile si verificam daca miniblocul nou trebuie
contopit cu blocul curent. In caz afirmativ verificam daca blocul curent
trebuie contopit cu blocul anterior.
(Amalgamul de instructiuni din acest caz uneste cele doua blocuri)
In urmatorul caz adaugam un bloc nou inaintea blocului curent
Pe urmatorul if avem doua cazuri:
1) adaugam pe ultima pozitie din lista
2) contopim cu ultimul bloc din lista
Iar in final contopim cu finalul unui bloc curent, in cazul in care nu se
va intra pe cazul de contopire cu blocul urmator.
*/
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	node_t *cursor = arena->alloc_list->head;
	if (alloc_errors(arena, address, size))
		return;

	miniblock_t *new_mb = alloc_miniblock(arena, address, size);
	if (!arena->alloc_list->head) {
		block_t *new_blk = alloc_in_func_block(arena, address, size);
		add_node(new_blk->miniblock_list, 0, new_mb);
		free(new_mb);
		add_node(arena->alloc_list, 0, new_blk);
		free(new_blk);
		return;
	}
	cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		if (curr_blk->start_address == size + address) {
			add_node(curr_blk->miniblock_list, 0, new_mb);
			curr_blk->size += size;
			curr_blk->start_address = address;
			if (i) { // verif daca exista un bloc precedent
				block_t *prev_blk = cursor->prev->data;
				if (prev_blk->start_address + prev_blk->size == address) {
					prev_blk->miniblock_list->tail->next =
					curr_blk->miniblock_list->head;
					curr_blk->miniblock_list->head->prev =
					prev_blk->miniblock_list->tail;
					prev_blk->miniblock_list->tail =
					curr_blk->miniblock_list->tail;
					prev_blk->size += curr_blk->size;
					prev_blk->miniblock_list->size +=
					curr_blk->miniblock_list->size;

					node_t *removed = remove_node(arena->alloc_list, i);
					free(((block_t *)removed->data)->miniblock_list);
					free(removed->data);
					free(removed);
				}
			}
			free(new_mb);
			return;
		} else if (curr_blk->start_address > size + address) {
			block_t *new_blk = alloc_in_func_block(arena, address, size);
			add_node(new_blk->miniblock_list, 0, new_mb);
			add_node(arena->alloc_list, i, new_blk);
			free(new_mb);
			free(new_blk);
			return;
		}
		if (!cursor->next) {
			if (address > curr_blk->start_address + curr_blk->size) {
				block_t *new_blk = alloc_in_func_block(arena, address, size);
				add_node(new_blk->miniblock_list, 0, new_mb);
				add_node(arena->alloc_list, arena->alloc_list->size,
						 new_blk);
				free(new_mb);
				free(new_blk);
				return;
			} else if (address == curr_blk->start_address + curr_blk->size) {
				add_node(curr_blk->miniblock_list,
						 curr_blk->miniblock_list->size, new_mb);
				curr_blk->size += size;
				free(new_mb);
				return;
			}
		} else if (curr_blk->start_address + curr_blk->size == address) {
			block_t *next_blk = cursor->next->data;
			if (next_blk->start_address != address + size) {
				add_node(curr_blk->miniblock_list,
						 curr_blk->miniblock_list->size, new_mb);
				curr_blk->size += size;
				free(new_mb);
				return;
			}
		}
		cursor = cursor->next;
	}
}

/* Din acelasi motiv ca mai devreme, voi fi nevoit sa comentez functia aici
Parcurgem blocurile cu un for pana la intalnirea blocului ce contine
adresa data
Primul caz vizeaza blocurile care contin un singur minibloc.
Ulterior se parcurg miniblocurile si se testeaza pe rand
1) eliminarea primului minibloc
2) eliminarea ultimului minibloc
3) eliminarea unui minibloc intermediar
In cazul eliminarii unui bloc intermediar se vor updata head, tail si size
pentru blocul modificat si noul bloc.
Apoi adaugam legaturi in lista nou creata catre restul listei vechi, si rupem
legaturile inainte de free
*/
void free_block(arena_t *arena, const uint64_t address)
{
	node_t *cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = (block_t *)cursor->data;
		if (address >= curr_blk->start_address &&
			address < curr_blk->start_address + curr_blk->size) {
			if (address == curr_blk->start_address &&
				curr_blk->miniblock_list->size == 1) {
				free(((miniblock_t *)curr_blk->miniblock_list->head->data)
					 ->rw_buffer);
				free(curr_blk->miniblock_list->head->data);
				free(curr_blk->miniblock_list->head);
				free(curr_blk->miniblock_list);

				node_t *removed = remove_node(arena->alloc_list, i);
				free(removed->data);
				free(removed);
				return;
			}
			node_t *cursor_mb = curr_blk->miniblock_list->head;
			unsigned int address_offset = 0;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				miniblock_t *curr_mb = (miniblock_t *)cursor_mb->data;
				address_offset += curr_mb->size;
				if (address == curr_mb->start_address) {
					if (!j) { // daca eliminam primul minibloc
						curr_blk->start_address += curr_mb->size;
						curr_blk->size -= curr_mb->size;

						node_t *removed =
						remove_node(curr_blk->miniblock_list, j);
						free(((miniblock_t *)removed->data)->rw_buffer);
						free(removed->data);
						free(removed);
						return;
					} else if (j == curr_blk->miniblock_list->size - 1) {
						curr_blk->size -= curr_mb->size;
						node_t *removed = remove_node
						(curr_blk->miniblock_list, j);
						free(((miniblock_t *)removed->data)->rw_buffer);
						free(removed->data);
						free(removed);
						return;
					}
					block_t *new_blk = alloc_in_func_block(arena,
					((miniblock_t *)cursor_mb->next->data)->start_address,
					curr_blk->size - address_offset);

					new_blk->miniblock_list->head = cursor_mb->next;
					new_blk->miniblock_list->tail =
					curr_blk->miniblock_list->tail;
					new_blk->miniblock_list->size =
					curr_blk->miniblock_list->size - j - 1;

					add_node(arena->alloc_list, i + 1, new_blk);
					curr_blk->size = curr_blk->size -
					new_blk->size - curr_mb->size;
					curr_blk->miniblock_list->size = j + 1;

					curr_blk->miniblock_list->tail = cursor_mb;
					node_t *removed =
					remove_node(curr_blk->miniblock_list, j);
					new_blk->miniblock_list->head->prev = NULL;

					free(((miniblock_t *)removed->data)->rw_buffer);
					free(removed->data);
					free(removed);
					free(new_blk);
					return;
				}
				cursor_mb = cursor_mb->next;
			}
		}
		cursor = cursor->next;
	}
	printf("Invalid address for free.\n");
}

void read(arena_t *arena, uint64_t address, uint64_t size)
{
	node_t *cursor = arena->alloc_list->head;
	uint64_t message_offset = 0;
	char *message = calloc(size + 1, 1);
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		if (address >= curr_blk->start_address &&
			address < curr_blk->start_address + curr_blk->size) {
			node_t *cursor_mb = curr_blk->miniblock_list->head;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				miniblock_t *curr_mb = cursor_mb->data;
				if (curr_mb->perm < 4) {
					printf("Invalid permissions for read.\n");
					free(message);
					return;
				}
				cursor_mb = cursor_mb->next;
			}
			cursor_mb = curr_blk->miniblock_list->head;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				miniblock_t *curr_mb = cursor_mb->data;
				if (address >= curr_mb->start_address &&
					address < curr_mb->start_address + curr_mb->size) {
					size_t address_offset = address - curr_mb->start_address;
					if (size < curr_mb->size - address_offset) {
						strncat(message + message_offset,
								curr_mb->rw_buffer + address_offset, size);
						message_offset += size;
					} else {
						strncat(message + message_offset, curr_mb->rw_buffer +
								address_offset, curr_mb->size - address_offset);
						message_offset += (curr_mb->size - address_offset);
					}
					address += (curr_mb->size - address_offset);
				}
				cursor_mb = cursor_mb->next;
			}
		}
		cursor = cursor->next;
	}
	if (message_offset == size) {
		printf("%s\n", message);
	} else if (message_offset < size && message_offset) {
		printf("Warning: size was bigger than the block size. ");
		printf("Reading %ld characters.\n%s\n", message_offset, message);
	} else {
		printf("Invalid address for read.\n");
	}
	free(message);
}

void write(arena_t *arena, const uint64_t address, const uint64_t size,
		   int8_t *data)
{
	node_t *cursor = arena->alloc_list->head;
	uint64_t curr_address = address, data_offset = 0;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		if (curr_address >= curr_blk->start_address &&
			curr_address < curr_blk->start_address + curr_blk->size) {
			node_t *cursor_mb = curr_blk->miniblock_list->head;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				miniblock_t *curr_mb = cursor_mb->data;
				if ((curr_mb->perm >= 4 && curr_mb->perm - 4 < 2) ||
					curr_mb->perm < 2) {
					printf("Invalid permissions for write.\n");
					return;
				}
				cursor_mb = cursor_mb->next;
			}
			cursor_mb = curr_blk->miniblock_list->head;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				miniblock_t *curr_mb = cursor_mb->data;
				if (curr_address >= curr_mb->start_address &&
					curr_address < curr_mb->start_address + curr_mb->size) {
					size_t address_offset = curr_address -
					curr_mb->start_address;
					if (size < curr_mb->size - address_offset) {
						memcpy(curr_mb->rw_buffer + address_offset,
							   data + data_offset, size);
						data_offset += size;
					} else {
						memcpy(curr_mb->rw_buffer + address_offset, data +
							   data_offset, curr_mb->size - address_offset);
						data_offset += curr_mb->size - address_offset;
					}
					curr_address += (curr_mb->size - address_offset);
				}
				cursor_mb = cursor_mb->next;
			}
		}
		cursor = cursor->next;
	}
	if (data_offset == size)
		return;
	if (data_offset < size && data_offset) {
		printf("Warning: size was bigger than the block size. ");
		printf("Writing %ld characters.\n", data_offset);
	} else {
		printf("Invalid address for write.\n");
	}
}

void turn_to_char_perms(int8_t perm, char *permissions)
{
	if (perm >= 4) {
		permissions[0] = 'R';
		perm -= 4;
	}
	if (perm >= 2) {
		permissions[1] = 'W';
		perm -= 2;
	}
	if (perm >= 1)
		permissions[2] = 'X';
}

void pmap(const arena_t *arena)
{
	size_t free_size = arena->arena_size;
	unsigned int mb_cnt = 0;
	node_t *cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		free_size -= curr_blk->size;
		mb_cnt += curr_blk->miniblock_list->size;
		cursor = cursor->next;
	}
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", free_size);
	printf("Number of allocated blocks: %d\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %d\n", mb_cnt);

	cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		printf("\nBlock %d begin\n", i + 1);
		printf("Zone: 0x%lX", curr_blk->start_address);
		printf(" - 0x%lX\n", curr_blk->start_address + curr_blk->size);
		node_t *cursor_mb = curr_blk->miniblock_list->head;
		for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
			miniblock_t *curr_mb = cursor_mb->data;
			char permissions[4] = "---";
			turn_to_char_perms(curr_mb->perm, permissions);
			printf("Miniblock %d:", j + 1);
			printf("\t\t0x%lX\t\t-", curr_mb->start_address);
			printf("\t\t0x%lX\t\t| ", curr_mb->start_address + curr_mb->size);
			printf("%s\n", permissions);
			cursor_mb = cursor_mb->next;
		}
		printf("Block %d end\n", i + 1);
		cursor = cursor->next;
	}
}

void mprotect(arena_t *arena, uint64_t address, int8_t *permission)
{
	int8_t perm = 0;
	if (permission[0] == 'R')
		perm += 4;
	if (permission[1] == 'W')
		perm += 2;
	if (permission[2] == 'X')
		perm += 1;
	node_t *cursor = arena->alloc_list->head;
	for (unsigned int i = 0; i < arena->alloc_list->size; i++) {
		block_t *curr_blk = cursor->data;
		if (address >= curr_blk->start_address &&
			address <= curr_blk->start_address + curr_blk->size) {
			node_t *cursor_mb = curr_blk->miniblock_list->head;
			for (unsigned int j = 0; j < curr_blk->miniblock_list->size; j++) {
				if (address ==
					((miniblock_t *)cursor_mb->data)->start_address) {
					((miniblock_t *)cursor_mb->data)->perm = perm;
					return;
				}
				cursor_mb = cursor_mb->next;
			}
		}
		cursor = cursor->next;
	}
	printf("Invalid address for mprotect.\n");
}
