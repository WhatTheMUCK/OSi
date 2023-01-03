#ifndef ALLOCATOR_LIST_H
#define ALLOCATOR_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char* PBYTE_LIST;

typedef struct block_list {
  size_t size;
  struct block_list* previous;
  struct block_list* next;
} block_list; //Двусвязный список свободных блоков

static block_list* begin_list; //Стартовый адрес области 
static block_list* free_list; //Адрес первого свободного блока
static size_t size_list; //Общий размер выделенной памяти
static size_t request_list = 0; //Счётчик количества запрашиваемой информации
static size_t total_list = 0; //Счётчик количества используемой информации

int initialization_list(size_t size);
void destroy_list();
void* alloc_block_list(block_list* block, size_t size);
void* malloc_list(size_t size);
void list_free(void* address);
size_t get_request_list();
size_t get_total_list();

#endif
