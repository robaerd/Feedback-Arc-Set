#include "generator.h"
#include "error.h"


static void swap(unsigned int * , unsigned int *);

static int validEdge(edge, unsigned int *, unsigned int);

void randperm (unsigned int n, unsigned int *vertices)
{
  unsigned int i;
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
  char *endPtr;
  unsigned u,v;
  if(!isdigit(*edgeStr)) error_exit("wrong edge fromat. Format: <digit>-<digit>. digit must be a positive value.");
  v = strtoul(edgeStr, &ptr, 10);
  if(*ptr != '-' || !isdigit(*(ptr+1))) error_exit("wrong edge fromat. Format: <digit>-<digit>. digit must be a positive value.");
  u = strtoul(ptr+1, &endPtr, 10);
  if(*endPtr != '\0') error_exit("wrong edge fromat. Format: <digit>-<digit>. digit must be a positive value.");
  //integer to edge struct
  iniEdges->u = u;
  iniEdges->v = v;
  //change vertex if any vertex in provided edge is higher than highes vertex ever provided
  if(u>v && u > *vertexAmount)
	*vertexAmount = u;
  else if (v>u && v > *vertexAmount)
	*vertexAmount = v;
}

unsigned int generateSolution(unsigned int *vertices, unsigned int verticeAmount, edge *edges, int edgeAmount, edge solution[MAX_SOL_EDGES])
{
  unsigned int k = 0;
  int i;
  for(i = 0; i < edgeAmount; i++){
	if(k > MAX_SOL_EDGES) return 9;
    if(validEdge(edges[i], vertices, verticeAmount))
	  solution[k++] = edges[i];
  }
  return k; // returns size of solution
}


int validEdge(edge Edge, unsigned int *vertices, unsigned int verticeAmount)
{
  unsigned int i;
  for(i = 0; i < verticeAmount; i++)
	if (*(vertices + i) == Edge.v) return 1;
	else if (*(vertices + i) == Edge.u) return 0;

  //bottom only reached in case of error
  error_exit("vertice from edge not part of vertice array.");
  exit(EXIT_FAILURE);
}


void swap(unsigned int *firstNum , unsigned int *secondNum)
{
  unsigned int temp = *firstNum;
  *firstNum = *secondNum;
  *secondNum = temp;
}
