#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *firstNum , int *secondNum)
{
  int temp = *firstNum;
  *firstNum = *secondNum;
  *secondNum = temp;

}

void randperm (int n, int *vertices)
{
  int i;
  for(i = 0; i < n; i++)
	vertices[i] = i;
	

  for(i = n-1; i >= 0; i--) {
	int r = rand() % (i+1);
	//printf("i: %d, r: %d\n", i, r);
	if (r == i) continue;
   swap(&vertices[i], &vertices[r]);
  }
}

