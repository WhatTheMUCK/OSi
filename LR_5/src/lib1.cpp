#include <stdio.h>
#include <iostream>
#include "../include/types.h"

using namespace std;

extern "C" os_float Square(os_float A, os_float B)
{
	cout << "\nRectangle area: "; 
	return A * B;
}

extern "C" os_int *Sort(os_int *array, os_int size)
{
  int i, j;
  for (i = 0; i < size - 1; i++)
    {
      for (j = 0; j < size - 1 - i; j++)
	{
	  if (array[j] > array[j + 1])
	    {
	      os_int temp = array[j + 1];
	      array[j + 1] = array[j];
	      array[j] = temp;
	    }
	}
    }
  cout << "\nSorting an array with a bubble: ";
  return array;
}
