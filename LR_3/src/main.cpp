#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
//#include <time.h>
using namespace std;

typedef struct thread_data{
	int counter_first;
	int counter_second;
	int plays;
	int tour;
	int experements;
	int wins_first;
	int wins_second;
} thread_data;

void* thread_func(void *arg){
	thread_data *tdata = (thread_data *)arg;
	unsigned int seed;
	seed = pthread_self();

	int first;
	int second;
	for (int counter1 = 0; counter1 < tdata->experements; counter1++){
		first = tdata->counter_first;
	        second = tdata->counter_second;
		for (int counter2 = 0; counter2 < tdata->plays - tdata->tour + 1; counter2++){
			first += rand_r(&seed)%6 + 1;
			first += rand_r(&seed)%6 + 1;
			second += rand_r(&seed)%6 + 1;
			second += rand_r(&seed)%6 + 1;
		}
		if (first > second)
			tdata->wins_first++;
		if (second > first)
			tdata->wins_second++;
	}
	return 0;
}

int main(int argc, char *argv[]){
	if (argc != 2){
		cerr << "error: Program must have only 1 key\n";
	} else {
		//clock_t start_time, end_time;
		//float timer;
		//start_time = clock();
		int NUMBER_THREADS = atoi(argv[1]);
		//cout << "#Количество потоков = " << NUMBER_THREADS << "\n";	
		int counter_first, counter_second, plays, tour, experements, count_of_experements_for_one_thread;
      		float wins_first = 0, wins_second = 0;
	
		cout << "Введите:\n 1)Количество очков первого игрока: ";
		cin >> counter_first;
		cout << " 2)Количество очков второго игрока: ";
		cin >> counter_second;
		cout << " 3)Номер данного тура: ";
		cin >> tour;
		cout << " 4)Количество бросков костей: ";
		cin >> plays;
		cout << " 5)Количество экспериментов: ";
		cin >> experements; 
		
		count_of_experements_for_one_thread = experements / NUMBER_THREADS;
		//cout << "#Количество эксперементов для одного потока = " << count_of_experements_for_one_thread << "\n";
		
		thread_data tdata[NUMBER_THREADS];
		
		for (int i = 0; i < NUMBER_THREADS; i++){
			tdata[i].counter_first = counter_first;
			tdata[i].counter_second = counter_second;
			tdata[i].plays = plays;
			tdata[i].tour = tour;
			if (i == NUMBER_THREADS - 1)
				tdata[i].experements = count_of_experements_for_one_thread + experements % NUMBER_THREADS;
			else 
				tdata[i].experements = count_of_experements_for_one_thread;
			tdata[i].wins_first = 0;		
			tdata[i].wins_second = 0;
		}
	
		pthread_t thread[NUMBER_THREADS];
	
		for (int i = 0; i < NUMBER_THREADS; i++){
			if(pthread_create(&thread[i], NULL, thread_func, &tdata[i]) != 0){
				cerr << "error: Cannot create thread # " << i << "\n";
		      		break;
			}
		}
		
		for (int i = 0; i < NUMBER_THREADS; i++){
			if(pthread_join(thread[i], NULL) != 0){
				cerr << "error: Cannot join thread # " << i << "\n";
				break;
			}
			thread_data *result = &tdata[i];
			wins_first += result->wins_first;
			wins_second += result->wins_second;
		}
		cout << "Вероятность победы первого игрока: " << wins_first/experements << "\n";
	       	cout << "Вероятность победы второго игрока: " << wins_second/experements << "\n";
		//end_time = clock()`;
		//timer = end_time - start_time;
		//cout << "Time: " << timer / CLOCKS_PER_SEC << "\n";
	}
}
