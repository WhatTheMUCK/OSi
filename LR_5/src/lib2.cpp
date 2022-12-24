#include <stdio.h>
#include <iostream>
#include "../include/types.h"

using namespace std;

extern "C" os_float Square(os_float A, os_float B)
{
	cout << "\nThe area of a right triangle: ";
        return A * B / 2;
}

void quickSort (os_int *array, os_int low, os_int high)
{
  os_int i = low;
  os_int j = high;
  os_int pivot = array[(i + j) / 2];
  os_int temp;

  while (i <= j)
    {
      while (array[i] < pivot)
	i++;
      while (array[j] > pivot)
	j--;
      if (i <= j)
	{
	  temp = array[i];
	  array[i] = array[j];
	  array[j] = temp;
	  i++;
	  j--;
	}
    }
  if (j > low)
    quickSort (array, low, j);
  if (i < high)
    quickSort (array, i, high);
}

extern "C" os_int *Sort(os_int *array, os_int size)
{
    quickSort(array, 0, size-1);
    cout << "\nSorting an array using Hoare sorting: ";
    return array;
}
