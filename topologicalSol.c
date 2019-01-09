#include "generator.h"
//#include "circularBuffer.h"

void swap(unsigned int *firstNum , unsigned int *secondNum)
{
  unsigned int temp = *firstNum;
  *firstNum = *secondNum;
  *secondNum = temp;

}

void randperm (int n, unsigned int *vertices)
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

//also delivers amount of vertices
void extractEdgeFromString(char *edgeStr, edge iniEdges, int *vertexAmount)
{
  //Edge in string format to integer
  char *ptr;
  int u,v;
  u = strtol(edgeStr, &ptr, 10);
  v = strtol(ptr+1, NULL, 10);
  //integer to edge struct
  iniEdges.u = u;
  iniEdges.v = v;
  //change vertex if any vertex in provided edge is higher than highes vertex ever provided
  if(u>v && u > *vertexAmount)
	*vertexAmount = u;
  else if (v>u && v > *vertexAmount)
	*vertexAmount = u;
}

/*
void extractVertFromEdge(char *edge, int *u, int *v)
{
  char *ptr;
  *u = strtol(edge, &ptr, 10);
  *v = strtol(ptr+1, NULL, 10);
}
*/

int validEdge(edge Edge, unsigned int *vertices, int verticeAmount)
{
  int i;
  for(i = 0; i < verticeAmount; i++)
	if (*(vertices + i) == Edge.v) return 1;
	else if (*(vertices + i) == Edge.u) return 0;

  //bottom should not be reached
  fprintf(stderr, "vertice not found in vertice array!\n");
  exit(EXIT_FAILURE);
}

int generateSolution(unsigned int *vertices, int verticeAmount, edge *edges, int edgeAmount, edge solution[MAX_SOL_EDGES])
{
  int k = 0;
  int i;
  for(i = 0; i < edgeAmount; i++){
    if(validEdge(edges[i], vertices, verticeAmount)){
	  // strncat(solution, edges[i], 3);
	  //strncat(solution, " ", 1);
	  solution[k] = edges[i];
	  k++;
	  }
  }
  return k; // returns size of solution
}

