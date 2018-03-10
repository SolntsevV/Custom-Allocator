#include <mem.h>

#include <unistd.h>

extern void *_malloc(size_t);
extern void *_calloc(const size_t, const size_t);
extern void _free(void *const);
extern void *heap_init(const size_t);

mem *find_suitable(const size_t); // найти подходящий блок, либо выделить новый
void mem_init(mem *const, const size_t); // инициализировать блок
mem *merge(mem *const, mem *const); //объединить блоки
mem *allocate(mem *last, const size_t query); // выделение памяти под блок
void *page_align(const void *ptr); // выравнить размер страницы
void zero(void *const, const size_t); // заполнение нулями блоков

static mem *heap_start;

void *_malloc(size_t query) {
  if (heap_start != HEAP_START)
	  heap_init(query); //проверка, инициализированна ли куча
  if (BLOCK_MIN_SIZE <= 0)
	  return NULL;
  if (query < BLOCK_MIN_SIZE)
	  query = BLOCK_MIN_SIZE; // размер блока не может быть меньше минимального
  mem *const suitable = find_suitable(query);
  if (suitable == NULL)
	  return NULL;
  mem *extra_block = (mem *)((size_t)suitable + sizeof(mem) + query);
  extra_block->next = suitable->next;
  extra_block->capacity = suitable->capacity - sizeof(mem) - query;
  extra_block->is_free = true;
  suitable->next = extra_block;
  suitable->is_free = false;
  suitable->capacity = query;
  return (void *)((size_t)suitable + sizeof(mem));
}

mem* find_suitable(const size_t query) {
  mem *latest = heap_start;
  for (mem *current = heap_start; current != NULL;
       latest = current, current = current->next) {
    if (current->next != NULL && current->is_free && current->next->is_free &&
        current->next == (mem *)((size_t)current + current->capacity + sizeof(mem))) {
      current = merge(current, current->next);
    }
    if (current->is_free && query <= (current->capacity - sizeof(mem) - BLOCK_MIN_SIZE)) {
      return current;
    }
  }
  //если не найден подходящий пустой блок - выделить память
  return allocate(latest, query);
}

mem *allocate(mem *latest, const size_t query) {
  void *address = page_align((void *)((size_t)latest + sizeof(mem) + latest->capacity));
  size_t size = (size_t)page_align((void *)(query + sizeof(mem)));
  mem *pages = (mem *)mmap((void *)address, size, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  if (pages != MAP_FAILED) {
    mem_init(pages, size - sizeof(mem));
    latest->next = pages;
    return pages;
  }
  pages = (mem *)mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (pages == MAP_FAILED)
	  return NULL;
  mem_init(pages, size - sizeof(mem));
  latest->next = pages;
  return pages;
}

void _free(void *const ptr) {
  mem *block = (mem *)((size_t)ptr - sizeof(mem));
  if (((size_t)(block->next) == (size_t)block + sizeof(mem) + block->capacity) &&
      (block->next->is_free)) {
    block = merge(block, block->next);
  }
  return;
}

void *heap_init(size_t initial_size) {
  size_t size = (size_t)page_align((void *)(initial_size + sizeof(mem)));
  heap_start = (mem *)mmap(HEAP_START, size, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (heap_start == NULL) { 
    return NULL;
  }
  mem_init(heap_start, size - sizeof(mem));
  return heap_start;
}

void mem_init(mem *const block, const size_t capacity) {
  block->next = NULL;
  block->capacity = capacity;
  block->is_free = true;
}

void *_calloc(const size_t number, const size_t size) {
  const size_t query = number * size;
  void *ptr = _malloc(query);
  if (ptr == NULL)
	  return NULL;
  zero(ptr, query);
  return ptr;
}

void zero(void *const req, const size_t query) {
  char *ptr = (char *)req;
  for (size_t i = 0; i < query; i++) {
    *(ptr + i) = 0;
  }
}

mem *merge(mem *const first, mem *const second) {
  mem *merged = (first < second) ? first : second;
  merged->capacity = first->capacity + second->capacity + sizeof(mem);
  merged->next = (first < second) ? second->next : first->next;
  return merged;
}

void *page_align(const void *ptr) {
  const size_t page_size = sysconf(_SC_PAGESIZE); // Размер страницы в байтах
  size_t addr = (size_t)ptr;
  addr = addr - addr % page_size + page_size;
  return (void *)addr;
}


