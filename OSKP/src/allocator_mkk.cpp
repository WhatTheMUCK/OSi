#include "allocator_mkk.h"

int initialization_mkk(size_t size) {
  size_t index;
  block_mkk* block = NULL;
	
  pages_mkk = get_pages_count_mkk(size); //Определяем общее количество страниц (плгоритм подразумевает, что память разбита на набор последовательных страниц)
  pow_mkk = pow_of_size_mkk(PAGE_SIZE_MKK); //Определяем степень двйоки, описывающей размер 1 страницы
  pow_index_minimum = pow_of_size_mkk(sizeof(block_mkk)); //Определяем степень двойки, описывающей размер структуры block_mkk
  heap_mkk = malloc(pages_mkk * PAGE_SIZE_MKK); //Стартовый адрес области страниц
  memory_size_mkk = (size_t*)malloc(sizeof(size_t) * pages_mkk); //Массив для управления страницами
  list_mkk = (block_mkk**)malloc(sizeof(block_mkk*) * pow_mkk); //Массив, содержащий заголовки всех буферов, имеющих размер меньше одной страницы

  if(heap_mkk == NULL || memory_size_mkk == NULL || list_mkk == NULL) {
	  return 0;
  }
	
  memory_size_mkk[free_state] = free_state; //0 страница пока свободна
  list_mkk[free_state] = (block_mkk*)heap_mkk;
  block = list_mkk[free_state]; 

  for(index = 1; index < pages_mkk; ++index) {
	  memory_size_mkk[index] = free_state; //все страницы от 1 до количества страниц считаются свободными
	  block->next = (block_mkk*)((PBYTE_MKK)block + PAGE_SIZE_MKK);
	  block = block->next;
  }

  block->next = NULL;

  for(index = 1; index < pow_mkk; ++index) {
	  list_mkk[index] = NULL; //Пока нет буферов => нет буферов, имеющих размер меньше одной страницы
  }

  return 1;
}

void destroy_mkk() {
  free(heap_mkk);
  //Освобождаем стартовый адрес области
  free(memory_size_mkk);
  //Освобождаем массив для управления страницами
  free(list_mkk);
  //Освобождаем массив, содержащий заголовки буферов,
  //Тем самым удаляя алгоритм аллокации Мак-Кьюзи-Кэрэлса
}

void* malloc_mkk(size_t size) {
  size_t pow_index = pow_of_size_mkk(size); //Округляем вверх size до степени двойки
  size_t old_size = size;
  block_mkk* block = NULL;

  if(pow_index < pow_index_minimum) {
  //Если размер меньше минимального, то просто делаем его минимальным
	  pow_index = pow_index_minimum;
  }

  size = 1 << pow_index;
  //Определяем размер равный степени двойки, который вмести в себя old_size
	
  if(size < PAGE_SIZE_MKK) {
  //Если размер меньше страницы, то мы сможем работать только с одной страницей
	  if(list_mkk[pow_index] == NULL) {
	  //Если нет ни единого буфера размером в 2^pow_index, то выделим страницу для этого
	    block = alloc_page_mkk(size);

	    if(block == NULL) {
		    return NULL;
	    }

	  split_page_mkk(block, pow_index); //Разделим страницу которую мы выделили под буфер размером 2^pow_index, на максимальное количество буферов размером 2^pow_index
	  }
		
	  block = list_mkk[pow_index]; //block берёт первый свободный буффер размером 2^pow_index 
	  list_mkk[pow_index] = block->next; //list_mkk[pow_index] начинает хранить следующий свободный буффер, так как предыдущий стал занят

          request_mkk += old_size; //Подсчёт запрашиваемого количества данных
	  total_mkk += size; //Подсчёт используемого количества данных

	  return (void*)block;
  }
  else {
	  request_mkk += old_size; //Подсчёт запрашиваемого количества данных
	  total_mkk += size; //Подсчёт используемого количества данных

	  return alloc_page_mkk(size);
  }
}

void free_mkk(void* address) {
  size_t page_index = get_page_index_mkk((block_mkk*)address); //Определяем номер страницы, на которой располагается буффер, который мы хотим освободить
  size_t pow_index = pow_of_size_mkk(memory_size_mkk[page_index]); //Определяем степень 2 этого буффера
  block_mkk* block = (block_mkk*)address;
	
  if(memory_size_mkk[page_index] < PAGE_SIZE_MKK) { //Если буффер размером меньше страницы
	  block->next = list_mkk[pow_index]; //Первым свободным буффером размером 2^pow_index становится block 
	  list_mkk[pow_index] = block;
  }
  else { //Если буффер размером больше страницы
	  free_page_mkk(block);
  }
}

block_mkk* alloc_page_mkk(size_t size) {
  size_t count = 0;
  size_t page_index = 0;
  size_t previous_index = get_page_index_mkk(list_mkk[free_state]); //Определяем номер страницы с которой есть свободное место
  size_t pages = get_pages_count_mkk(size); //Определяем количество страниц которое понадобится для того чтобы вместить size бит
  block_mkk* current = list_mkk[free_state]; //Определяем текущий блок для дальнейшей работы с ним
  block_mkk* previous = NULL; 
  block_mkk* page = NULL;

  while(current != NULL) {
	  page_index = get_page_index_mkk(current); //Определяем номер страницы, на котором находится буффер current

	  if(page_index - previous_index <= 1) {
	  //Если current занимает не больше одной страницы, то
	    if(page == NULL) {
		    //Мы нашли что будет располагаться на странице page
		    page = current;
	    }
	    
	    ++count;
	  }
	  else {
          //Если current занимает больше 1 страницы, то мы сбрасываем счётчик до 1, а в страницу добавляем current, чтобы при повторном прохождении цикла не зайти в (if(page == NULL))
	    page = current;
	    count = 1;
	  }

	  if(count == pages) {
          //Если счётчик сравнялся с необходимым количеством страниц, то мы завершаем цикл
	    break;
	  }
	  
	  //Берём следующий буфер, при этом не забывая о предыдущем
	  previous = current;
	  current = current->next;
	  previous_index = page_index;
  }

  if(count < pages) {
  //Если счётчик оказался меньше необходимого количества страниц, то значит что на одну страницу буффер не вместится
	  page = NULL;
  }

  if(page != NULL) {
  //Если мы смогли что-то занести в страницу 
	  page_index = get_page_index_mkk(page); //Определяем номер страницы
	  memory_size_mkk[page_index] = size; //Страница под номером page_index разделена на буферы размером size (это уже некоторая степень 2)
	  current = (block_mkk*)((PBYTE_MKK)page + (pages - 1) * PAGE_SIZE_MKK); //Адрес текущего блока
		
	  if (previous != NULL) {	
	    previous->next = current->next;
	  }
	  else {
	    list_mkk[free_state] = current->next; //Вносим адрес свободного блока в list_mkk[free_state]
	  }
  }

  return page;
}

void free_page_mkk(block_mkk* block) {
  size_t index;
  size_t page_index = get_page_index_mkk(block); //Определяем номер страницы
  size_t block_count = memory_size_mkk[page_index] / PAGE_SIZE_MKK; //Определяем сколько страниц целиком заняты буффером block
  block_mkk* left = NULL; //Левый буффер
  block_mkk* right = NULL; //Правый буффер
  block_mkk* current = block; 

  while(current != NULL) {
	  if (current < block) { //Определение самого близкого к block левого буффера
	    left = current;
	  }
	  else {
	    if(current > block) { //Определение самого близкого к block правого буффера
		    right = current;

		    break;
	    }
	  }

	  current = current->next;
  }

  for(index = 1; index < block_count; ++index) {
	  block->next = (block_mkk*)((PBYTE_MKK)block + PAGE_SIZE_MKK); //Отделяем от буффера цельные страницы
	  block = block->next;
  }

  block->next = right;

  if(left != NULL) {
	  left->next = block; //Переопределяем связь левого с block, тем самым освободив все цельные страницы
  }
  else {
	  list_mkk[free_state] = block; //Утверждаем, что block свободен
  }
}

void split_page_mkk(block_mkk* block, size_t pow_index) {
  size_t index;
  size_t page_index = get_page_index_mkk(block); //Определяемномер страницы
  size_t block_size = 1 << pow_index; //Определяем размер блоков как 2^pow_index
  size_t block_count = PAGE_SIZE_MKK / block_size; //Количество блоков есть отношение общего размера страницы к размеру 1 блока

  list_mkk[pow_index] = block; //Сохраняем буфер block, как буффер размером 2^pow_index
  memory_size_mkk[page_index] = block_size; //На странице page_index хранится размер буфферов, на которые она поделена
	
  for(index = 1; index < block_count; ++index) {
	  block->next = (block_mkk*)((PBYTE_MKK)block + block_size); //Определяем связь буфферов на странице
	  block = block->next;
  }

  block->next = NULL;
}

size_t pow_of_size_mkk(size_t size) { //Округление вверх до степени 2
  size_t pow = 0;

  while(size > ((size_t)1 << pow)) {
	  ++pow;
  }

  return pow;
}

size_t get_pages_count_mkk(size_t size) { //Определение необходимого количества страниц
  return size / PAGE_SIZE_MKK + (size_t)(size % PAGE_SIZE_MKK != 0);
}

size_t get_page_index_mkk(block_mkk* block) { //Определение номера страницы, на которой находится block
  return (size_t)((PBYTE_MKK)block - (PBYTE_MKK)heap_mkk) / PAGE_SIZE_MKK;
}

size_t get_request_mkk() { //Опредение запрашиваемого за всё время объёма памяти
  return request_mkk;
}

size_t get_total_mkk() { //Определение использованного за всё время объёма памяти
  return total_mkk;
}
