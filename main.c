#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vma.h"
#define MAX_STRING_SIZE 512

int main(void)
{
	char command[MAX_STRING_SIZE];
	arena_t *arena;
	uint64_t address;
	size_t size;
	do {
		scanf("%s", command);
		if (!strcmp(command, "ALLOC_ARENA")) {
			scanf("%lu", &size);
			arena = alloc_arena(size);
		} else if (!strcmp(command, "ALLOC_BLOCK")) {
			scanf("%lu %lu", &address, &size);
			alloc_block(arena, address, size);
		} else if (!strcmp(command, "PMAP")) {
			pmap(arena);
		} else if (!strcmp(command, "FREE_BLOCK")) {
			scanf("%lu", &address);
			free_block(arena, address);
		} else if (!strcmp(command, "DEALLOC_ARENA")) {
			dealloc_arena(arena);
			exit(0);
		} else if (!strcmp(command, "MPROTECT")) {
			scanf("%lu ", &address);
			char *protects = malloc(MAX_STRING_SIZE), *p;
			int8_t permissions[4] = "---";
			fgets(protects, 500, stdin);
			protects[strlen(protects) - 1] = '\0';
			p = strtok(protects, " |");
			while (p) {
				if (!strcmp(p, "PROT_NONE")) {
					permissions[0] = '-';
					permissions[1] = '-';
					permissions[2] = '-';
				} else if (!strcmp(p, "PROT_READ") && permissions[0] != 'R') {
					permissions[0] = 'R';
				} else if (!strcmp(p, "PROT_WRITE") && permissions[1] != 'W') {
					permissions[1] = 'W';
				} else if (!strcmp(p, "PROT_EXEC") && permissions[2] != 'X') {
					permissions[2] = 'X';
				}
			p = strtok(NULL, " |");
			}
			free(protects);
			mprotect(arena, address, permissions);
		} else if (!strcmp(command, "WRITE")) {
			scanf("%lu%lu", &address, &size);
			char *data = calloc(size + 2, 1), *aux = calloc(size + 1, 1);
			do {
				fgets(aux, size + 1, stdin);
				if (aux[0] == ' ')
					strncat(data, aux + 1, strlen(aux) - 1);
				else
					strncat(data, aux, strlen(aux));
			} while (strlen(data) < size);
			data[strlen(data) - 1] = '\0';
			if (strlen(data) > size) {
				char *p = strtok(data, " ");
				while (p) {
					printf("Invalid command. Please try again.\n");
					p = strtok(NULL, " ");
				}
			}
			write(arena, address, size, (int8_t *)data);
			free(data);
			free(aux);
		} else if (!strcmp(command, "READ")) {
			scanf("%lu %lu", &address, &size);
			read(arena, address, size);
		} else {
			printf("Invalid command. Please try again.\n");
		}
	} while (1);
	return 0;
}
