#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <time.h>
#include "allocator_list.h"
#include "allocator_mkk.h"

using namespace std;

//Запросы содержат адрес и размер запрашиваемого места
typedef struct request_structure {
  void* address;
  size_t bytes;
} request;

//Нужно для переработки char* в число
size_t parse_size(const char* string) {
  size_t size = 0;

  while(*string != '\0') {
	  if(*string < '0' || *string > '9') {
	    return 0;
	  }

	  size = size * 10 + *string - '0';
	  ++string;
  }

  return size;
}

int main(int argument_count, char* argument_vector[]) {
  const size_t NUMBER_REQUESTS = 1000;
  const size_t MAX_BYTES = 5000;
  clock_t first_time;
  clock_t second_time;
  size_t first_index; //size_t - беззнаковый тип, созданспециально для хранения размера оюъектов любых типов
  size_t second_index;
  size_t third_index;
  size_t argument;
  size_t query = 0;
  size_t total = 0;
  size_t* permute = (size_t*)malloc(sizeof(size_t) * NUMBER_REQUESTS);
  request* requests = (request*)malloc(sizeof(request) * NUMBER_REQUESTS);

  srand((unsigned int)time(0)); //Выполняем инициализацию генератора случайных чисел rand

  if(argument_count < 2) {
	  cout << "Usage: " << argument_vector[0] << " <SIZE>\n";
	  return 0;
  }

  argument = parse_size(argument_vector[1]);

  //Инициализация алгоритма аллокации списка свободных блоков(первого подходящего) argument битами
  if(!initialization_list(argument)) {
	  cout << "Error. No memory\n";
	  return 0;
  }
  //Инициализация алгоритма аллокации Мак-Кьюзи-ККэрэлса argument битами
  if(!initialization_mkk(argument)) {
	  cout << "Error. No memory\n";
	  return 0;
  }

  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  requests[first_index].bytes = 1 + rand() % MAX_BYTES; //Псевдослучайным образом определяем размеры запросов
	  permute[first_index] = first_index; //permute хранит индексы массива request и нужен будет для того чтобы доставать псевдослучайный запрос
  }

  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  second_index = rand() % NUMBER_REQUESTS;
	  third_index = rand() % NUMBER_REQUESTS;
	  argument = permute[second_index];
	  permute[second_index] = permute[third_index];
	  permute[third_index] = argument;
	  //Поменяли местами permute[second_index] и permute[third_index]
  }

  cout << "Alloc requests: " << NUMBER_REQUESTS;
  //Требуется произвести NUMBER_REQUESTS аллокаций памяти
  cout << "\nBytes: 1 to " << MAX_BYTES;
  //Необходимое место: от 1 до MAX_BYTES
  cout << "\n\nAllocator LIST:\n";
  //Характеристики алгоритма аллокации: список свободных блоков(первое подходящее) 
 
  //Замеряем скорость выделения блоков
  first_time = clock();

  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  requests[first_index].address = malloc_list(requests[first_index].bytes);
	  //Для каждого запроса пытаемся найти свободный блок размером >= размера запроса (request[first_index].bytes)
  }
	
  second_time = clock();
  //Заканчиваем замерять скорость выделения блоков 
  //И параллельно начинаем замерять скорость освобождения блоков

  printf("Alloc time: %lf\n", (double)(second_time - first_time) / CLOCKS_PER_SEC);
  //Разница между концом и началом замеров скорости выделения блоков и есть скорость выделения блоков (/CLOCKS_PER_SEC нужен так как необходимо миллисекунды перевести в секунжы)
  
  query = get_request_list(); //Счётчик количества запрашиваемого места в общем
  total = get_total_list(); //Счётчик количества используемого места в общем

  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  if(requests[permute[first_index]].address == NULL) {
	    continue;
	  }
	  //Вот и раскрывается сакральный смысл permute (нужен для того чтобы более точно и качественно определить скорость освобождения блоков) 
	  list_free(requests[permute[first_index]].address);
	  //Для каждого запроса освобождаем блоки
  }
	
  first_time = clock();
  //Заканчиваем замерять скорость освобождения блоков

  printf("Free time: %lf\n", (double)(first_time - second_time) / CLOCKS_PER_SEC);
  //Разница между концом и началом замеров скорости освобождения блоков и есть скорость освобождения блоков (printf нужен для того чтобы число выводилось целиком, а не в естественной форме)
  cout << "Usage factor: " << (long double)query / total << "\n\n";
  //Фактор использования определяется отношением количества запрашиваемой памяти к количеству использованной
  cout << "Allocator MKK\n";
  //Характеристики алгоритма аллокации: Мак-Кьюзи-Кэрэлса

  //Замеряем скорость выделения блоков
  first_time = clock();
	
  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  requests[first_index].address = malloc_mkk(requests[first_index].bytes);
	  //Для каждого запроса пытаемся либо выбрать на странице буфер размером 2^n (n - некоторое целое число, такое, что 2^n ближайшая степень 2 (>=) к requests[first_index].bytes)
	  //Либо выделить некоторое количество страниц (у которых размер тоже 2^m, где m - некоторое целое число (>=0)) и на последней странице также выбрать буфер 
  }
	
  second_time = clock();
  //Заканчиваем замерять скорость выделения блоков
  //И параллельно начинаем замерять скорость освобождения блоков

  printf("Alloc time: %lf\n", (double)(second_time - first_time) / CLOCKS_PER_SEC);
  //Разница между концом и началом замеров скорости освобождения блоков и есть скорость освобождения блоков (printf нужен для того чтобы число выводилось целиком, а не в естественной форме)

  query = get_request_mkk(); //Счётчик количества запрашиваемого места в общем
  total = get_total_mkk(); //Счётчик количества используемого места в общем

  for(first_index = 0; first_index < NUMBER_REQUESTS; ++first_index) {
	  if(requests[permute[first_index]].address == NULL) {
	    continue;
	  }
	  free_mkk(requests[permute[first_index]].address);
	  //Для каждого запроса освобождаем блоки
  }
	
  first_time = clock();

  printf("Free time: %lf\n", (double)(first_time - second_time) / CLOCKS_PER_SEC);
  //Разница между концом и началом замеров скорости освобождения блоков и есть скорость освобождения блоков (printf нужен для того чтобы число выводилось целиком, а не в естественной форме)
  cout << "Usage factor: " << (long double)query / total << "\n";
  //Фактор использования определяется отношением количества запрашиваемой памяти к количеству использованной	

  destroy_list();
  destroy_mkk();

  free(requests);
  free(permute);

  return 0;
}
