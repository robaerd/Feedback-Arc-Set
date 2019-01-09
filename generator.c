//#include "supervisor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>

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

void extractVertFromEdge(char *edge, int *u, int *v)
{
  char *ptr;
  *u = strtol(edge, &ptr, 10);
  *v = strtol(ptr+1, NULL, 10);
}

int getAmountVertices (char **edges, int amountEdges)
{
  int max = 0;
  int i;
  int u,v;
  for(i = 0; i < amountEdges; i++){
	extractVertFromEdge(edges[i], &u, &v);
	if(u > max) max = u;
	if(v > max) max = v;
  }
  return max+1;
}

int validEdge(char *edge, int *vertices, int verticeAmount)
{
  //extract vertices from edge
  int u,v;
  extractVertFromEdge(edge, &u, &v);
  //printf("edge:%s\t u:%d v:%d\n", edge,u,v);
  int i;
  for(i = 0; i < verticeAmount; i++){
	if (*(vertices + i) == v) return 1;
	else if (*(vertices + i) == u) return 0;
  }
  //bottom should not be reached
  fprintf(stderr, "vertice not found in vertice array!\n");
  exit(EXIT_FAILURE);
}

int generateSolution(int *vertices, int verticeAmount, char **edges, int edgeAmount, char *solution)
{
  int k = 0;
  int i;
  for(i = 0; i < edgeAmount; i++){
    if(validEdge(edges[i], vertices, verticeAmount)){
	  strncat(solution, edges[i], 3);
	  strncat(solution, " ", 1);
	  }
  }
  return k; // returns size of solution
}


//circular buffer
#define BUF_LEN 8
int *buf; // points to shared memory mapped with mmap(2)
sem_t *free_sem, // tracks free space, initialized to BUF_LEN
  *used_sem; // tracks used space, initialized to 0

int write_pos = 0;
void circ_buf_write(int val) {
sem_wait(free_sem); // writing requires free space
buf[write_pos] = val;
sem_post(used_sem); // space is used by written data write_pos = (write_pos + 1) % BUF_LEN;
}


int main(int argc, char ** argv)
{
  int c;
  int edgeAmount;
  int verticeAmount;

  while((c = getopt(argc,argv,"")) != -1)
	switch(c) {
	case '?':
	  fprintf(stderr,"invalid option. only postitional arguments expected\n");
	  exit(EXIT_FAILURE);
	default:
	  break;
	}

    //set time seed for random number generator
  struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // using nano-seconds instead of seconds
    srand((time_t)ts.tv_nsec);

  int optI = optind;
  edgeAmount = argc - optI;
  char *edges[edgeAmount];
  if(optI < argc){
	int i;
	for (i = 0; i < argc-optI; i++){
	  edges[i] = *(argv + optI + i);
	  //problem: zahlen groesser 9 werden nicht gehandlet!
	  /* if (*(edges[i] +1) != '-' || !isdigit(*(edges[i])) || !isdigit(*(edges[i] + 2))){
		fprintf(stderr,"Wrong edge pattern: %s. Should be: u-v", edges[i]);
		exit(EXIT_FAILURE);
		}*/ 
	}
  }else {
	fprintf(stderr, "Invalid pattern!\n");
	exit(EXIT_FAILURE);
  }
  verticeAmount = getAmountVertices(edges, edgeAmount);
  int vertices[verticeAmount];
  printf("verticeamount: %d\n", verticeAmount);
  randperm(verticeAmount, vertices);

  //test
  char *solution = (char*) (malloc(sizeof(char) * 8*3));
  memset(solution, '\0', sizeof(char) * 8*3);
  int solutionSize = generateSolution(vertices, verticeAmount, edges,edgeAmount, solution);
  int t;
  //for(t = 0; t < solutionSize; t++)
  int z;
  for(z = 0; z < verticeAmount; z++) printf("%d,", vertices[z]);
  printf("solution: %s\n", solution);
}
