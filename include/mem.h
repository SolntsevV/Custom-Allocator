#ifndef _MEM_H_
#define _MEM_H_

#define _USE_MISC

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <sys/mman.h>

#define HEAP_START ((void *)0x04040000) //адрес начала кучи
#define BLOCK_MIN_SIZE 4 

#pragma pack(push, 1)
typedef struct mem {
  struct mem *next;
  size_t capacity;
  bool is_free;
} mem;
#pragma pack(pop)

void *_malloc(size_t query);
void *_calloc(const size_t number, const size_t query);
void _free(void *const mem);
void *heap_init(const size_t initial_size);

#endif
