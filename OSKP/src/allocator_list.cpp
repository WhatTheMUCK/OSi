#include "allocator_list.h"

int initialization_list(size_t size) {
  if(size < sizeof(block_list)) {
	size = sizeof(block_list);
  }

  begin_list = (block_list*)malloc(size);
	
  if(begin_list == NULL) {
	return 0;
  }

  begin_list->size = size;
  begin_list->previous = NULL;
  begin_list->next = NULL;
  free_list = begin_list;
  size_list = size;
  //Задаём изначальную карту ресурсов: состоящую из стартового адреса области (begin_list) и общего размера памяти (size)
  return 1;
}

void destroy_list() {
  free(begin_list);
  //Освобождаем стартовый адрес области тем самым удаляя аллокатор списка свободных элементов
}

void* alloc_block_list(block_list* block, size_t size) {
  block_list* next_block = NULL;

  if(block->size >= size + sizeof(block_list)) {
	next_block = (block_list*)((PBYTE_LIST)block + size);
	next_block->size = block->size - size;
	next_block->previous = block->previous;
	next_block->next = block->next;
	block->size = size;

	if(block->previous != NULL) {
	  block->previous->next = next_block;
	}

	if(block->next != NULL) {
	  block->next->previous = next_block;
	}

	if(block == free_list) {
	  free_list = next_block;
	}
  }
  else {
	if(block->previous != NULL) {
	  block->previous->next = block->next;
	}

	if(block->next != NULL) {
	  block->next->previous = block->previous;
	}

	if(block == free_list) {
	  free_list = block->next;
	}
  }

  return (void*)((PBYTE_LIST)block + sizeof(size_t));
}

void* malloc_list(size_t size) {
  size_t first_size = size_list;
  size_t old_size = size;
  block_list* first_block = free_list;
  block_list* current = free_list;

  size += sizeof(size_t);

  if(size < sizeof(block_list)) {
	size = sizeof(block_list);
  }
  
  int flag = 0;

  while(current != NULL && flag == 0) {
	if (current->size < first_size && current->size >= size) {
	//Если размер рассматриваемого блока меньше всего свободного места и блок сможет вместить необходимое количество данных, то это значит что мы нашли первый подходящий свободный блок
	  first_size = current->size;
	  first_block = current;
	  flag = 1;
	}

	current = current->next;
  }

  if(free_list == NULL || first_block->size < size){
  //Если свободное место отсутствует или размер свободного места меньше запрашиваемого размера, то мы не сможем выделить место для запроса
	return NULL;
  }

  request_list += old_size; //Подсчёт запрашиваемого объёма места
  total_list += size; //Подсчёт в итоге используемого объёма места
  return alloc_block_list(first_block, size); //Фрагментируем найденный блок (чтобы потом можно было ещё использовать оставшееся место в нём)
}

void list_free(void* address) { //Освобождение места занимаемого запросом по адресу address
  block_list* block = (block_list*)((PBYTE_LIST)address - sizeof(size_t)); //Находим адрес блока (так как информация находится после заголовка, который занимает sizeof(size_t) бит)
  block_list* current = free_list; //Текущий блок от самого первого свободного блока пройдёт все для того чтобы добавить освобождённый блок в список свободных
  block_list* left_block = NULL;
  block_list* right_block = NULL;

  while(current != NULL) {
	if((block_list*)((PBYTE_LIST)current + current->size) <= block) {
	//Нахождение блока, который располагается левее освобождённого, но является самым близким к освобождённому
	  left_block = current; 
	}

	if((block_list*)((PBYTE_LIST)block + block->size) <= current) {
	//Нахождение блока, который находится правее и сразу выход из цикла, чтобы найти самый близкий правый блок к освобождённому
	  right_block = current;
	  break;
	}

	current = current->next;
  }
	
  //Добавление освобождённого блока в двусвязный список свободных блоков
  if(left_block != NULL) {
	left_block->next = block;
  }
  else {
	//Если блок самый левый, то он является самым первым свободным блоком
	free_list = block;
  }
	
  if(right_block != NULL) {
	right_block->previous = block;
  }

  block->previous = left_block;
  block->next = right_block;
  current = free_list;
  //Объединение свободных блоков стоящих вплотную в единый блок большего размера
  while(current != NULL) {
	if ((block_list*)((PBYTE_LIST)current + current->size) == current->next) {
	  current->size += current->next->size;
	  current->next = current->next->next;

	  if (current->next != NULL) {
		current->next->previous = current;
	  }
	  
	  continue;
	}

	current = current->next;
  }
}

size_t get_request_list() {
  return request_list; //Определение количества запрашиваемого объёма данных за всё время
}

size_t get_total_list() {
  return total_list; //Определение количества используемых данных за всё время
}
