#include <stdio.h>
#include <iostream>
#include "../include/lib.h"

using namespace std;

int main(int argc, char const *argv[])
{
	cout << "Введите: [key] [arg1] ... [argN]\n";
	cout << "Если вы хотите посчитать площадь фигуры: 1 [first side] [second size]\n";
	cout << "Если вы хотите отсортировать массив: 2 [size] [array[0]] [array[1]] ... [array[size-1]]\n";
	int key;
	while(cin >> key){
		if (key == 1){
			os_float A,B;
			cin >> A >> B;
			cout << Square(A,B) << "\n"; 
		} 
		if (key == 2){
			os_int size;
			cin >> size;
			os_int a[size];
			os_int *b;
			for (int i=0; i<size; i++)
				cin >> a[i];
			b = Sort(a, size);
			for (int i=0; i<size; i++)
				cout << b[i] << " ";
			cout << "\n";
		}
		if (key != 1 and key != 2){
			cout << "\nНеправильрный ключ\n";
		}
		cout << "\nВведите: [key] [arg1] ... [argN]\n";
        	cout << "Если вы хотите посчитать площадь фигуры: 1 [first side] [second size]\n";
        	cout << "Если вы хотите отсортировать массив: 2 [size] [array[0]] [array[1]] ... [array[size-1]]\n";
	}
}
