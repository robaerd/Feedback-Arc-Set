#include "generator.h"
//#include "circularBuffer.h"

void swap(unsigned int *firstNum , unsigned int *secondNum)
{
  unsigned int temp = *firstNum;
  *firstNum = *secondNum;
  *secondNum = temp;

}

void randperm (unsigned int n, unsigned int *vertices)
{
  unsigned int i;
  for(i = 0; i < n; i++)
	vertices[i] = i;

  for(i = n; i > 0; i--) {
	unsigned int r = rand() % (i);
	if (r == (i-1)) continue;
   swap(&vertices[i-1], &vertices[r]);
  }
}

//also delivers amount of vertices
void extractEdgeFromString(char *edgeStr, edge *iniEdges, unsigned int *vertexAmount)
{
  //Edge in string format to integer
  char *ptr;
  unsigned u,v;
  v = strtoul(edgeStr, &ptr, 10);
  u = strtoul(ptr+1, NULL, 10);
  //integer to edge struct
  iniEdges->u = u;
  iniEdges->v = v;
  //change vertex if any vertex in provided edge is higher than highes vertex ever provided
  if(u>v && u > *vertexAmount)
	*vertexAmount = u;
  else if (v>u && v > *vertexAmount)
	*vertexAmount = v;
}


int validEdge(edge Edge, unsigned int *vertices, unsigned int verticeAmount)
{
  unsigned int i;
  for(i = 0; i < verticeAmount; i++)
	if (*(vertices + i) == Edge.v) return 1;
	else if (*(vertices + i) == Edge.u) return 0;

  //bottom should not be reached
  fprintf(stderr, "verticeAmount:%d\n edges:", verticeAmount);
  unsigned int j; for(j = 0; j< verticeAmount; j++) fprintf(stderr, "%d-%d ",Edge.v, Edge.u);
  fprintf(stderr, "\nvertice not found in vertice array!\n");
  exit(EXIT_FAILURE);
}

unsigned int generateSolution(unsigned int *vertices, unsigned int verticeAmount, edge *edges, int edgeAmount, edge solution[MAX_SOL_EDGES])
{
  unsigned int k = 0;
  int i;
  //fprintf(stderr,"verticeAmount: %d\n", verticeAmount);
  for(i = 0; i < edgeAmount; i++){
	if(k > MAX_SOL_EDGES) return 9;
    if(validEdge(edges[i], vertices, verticeAmount)){
	  // strncat(solution, edges[i], 3);
	  //strncat(solution, " ", 1);
	  solution[k] = edges[i];
	  k++;
	  }
  }
  //unsigned int l;
  //for(l = 0; l < k; l++) printf("%d-%d", solution[l].v,solution[l].u);
  return k; // returns size of solution
}

