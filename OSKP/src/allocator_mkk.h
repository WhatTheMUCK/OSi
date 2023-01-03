#ifndef ALLOCATOR_MKK_H
#define ALLOCATOR_MKK_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char* PBYTE_MKK;

typedef enum memory_structure {
  free_state = 0
} memory_state;

typedef struct block_mkk_structure {
  struct block_mkk_structure* next;
} block_mkk; //Односвязный список блоков

static const size_t PAGE_SIZE_MKK = 4096; //Размер одной страницы (обязательно некоторая степень 2)
static void* heap_mkk = NULL; //Стартовый адрес области
static size_t* memory_size_mkk = NULL; //Массив для управления страницами
//Каждая из страниц может находиться в одном из 3 состояний:
//Быть свободной => соответствующий элемент массива содержит указатель на элемент, описывающий следующую свободную страницу
//Быть разбитой на буферы определённого размера (некоторая степень 2). Элемент массива содержит размер буфера
//Являться частью буфера, объединяющего сразу несколько страниц. Элемент массива указывает на первую страницу буфера, в котором находяться данные о его длине
static block_mkk** list_mkk = NULL; //Массив, содержащий заголовки всех буферов, имеющих размер меньше одной страницы
static size_t pages_mkk = 0; //Общее количество страниц
static size_t pow_mkk = 0; //Степень n двойки: 2^n = PAGE_SIZE_MKK
static size_t pow_index_minimum = 0; //Минимальный размер необходимый для хранения указателя на элемент
static size_t request_mkk = 0; //Счётчик количества запрашиваемой информации
static size_t total_mkk = 0; //Счётчик количества использованной информации

int initialization_mkk(size_t size);
void destroy_mkk();
void* malloc_mkk(size_t size);
void free_mkk(void* address);
block_mkk* alloc_page_mkk(size_t size);
void free_page_mkk(block_mkk* block);
void split_page_mkk(block_mkk* block, size_t powIndex);
size_t pow_of_size_mkk(size_t size);
size_t get_pages_count_mkk(size_t size);
size_t get_page_index_mkk(block_mkk* block);
size_t get_request_mkk();
size_t get_total_mkk();

#endif

