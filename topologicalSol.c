/**
 * @file topologicalSol.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 09.01.19
 *
 * @brief Contains all edge-vertice-graph dependent functionalitys.
 *
 * extractEdgeFromString(..) is used to parse the string containing all edges into the edge struct
 * generateSolution(..) finds a feedback arc set with 8 or less edges.
 * randperm(..) shuffles the provided vertex indices
 *
 */

#include "generator.h"
#include "error.h" /* provides error_exit() and error_errno_exit() */

/**
 * @brief swaps the values of two unsinged integer pointers
 */
static void swap(unsigned int * , unsigned int *);

/**
 * @brief checks if the edge fulfills all the requirements of the topological sorting. return 1 if it does, otherwise 0,
 */
static int validEdge(edge, unsigned int *, unsigned int);

/**
 * @brief shuffles provided vertices
 * @param n amount of vertices
 * @param vertices int array containing all vertex indices
 */
void randperm (unsigned int n, unsigned int *vertices)
{
  unsigned int i;
  for(i = n; i > 0; i--) {
	unsigned int r = rand() % (i);
	if (r == (i-1)) continue;
    swap(&vertices[i-1], &vertices[r]);
  }
}

/**
 * @brief extracts (parses) a string containing edges. Also provides vertex amount
 * @param edgeStr the initilial string containing all edges provided by the positional arguments
 * @param iniEdges edge array where the parsed edges are stored
 * @param vertexAmount the amount of vertices is assigned to vertexAmount
 */
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

/**
 * @brief generates a valid feedback arc set
 * @param vertices int array containing all vertices
 * @param verticeAmount amount of vertices
 * @param edges the initilial parsed edge array provided by the positional arguments
 * @param amaount of the provided edges by positional arguments
 * @param solution generated feedback arc set consisting of edges
 * @return returns size of the generated feedback arc set
 */
unsigned int generateSolution(unsigned int *vertices, unsigned int verticeAmount, edge *edges, int edgeAmount, edge solution[MAX_SOL_EDGES])
{
  unsigned int k = 0;
  int i;
  for(i = 0; i < edgeAmount; i++){
	if(k > MAX_SOL_EDGES) return (MAX_SOL_EDGES+1);
    if(validEdge(edges[i], vertices, verticeAmount))
	  solution[k++] = edges[i];
  }
  return k; // returns size of solution
}

/**
 * @brief checks if the edge fulfills all the requirements of the topological sorting.
 * @param Edge edge to be checked if it is valid.
 * @param vertices int array containing all vertices
 * @param verticeAmount amount of vertices
 * @return return 1 if the edge fulfills the requirements of the topological sorting, otherwise 0.
 */
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

/**
 * @brief swaps the values of two unsinged integer pointers
 * @param firstNum unsigned integer being swaped with secondNum
 * @param secondNum unsigned integer being swaped with firstNum
 */
void swap(unsigned int *firstNum , unsigned int *secondNum)
{
  unsigned int temp = *firstNum;
  *firstNum = *secondNum;
  *secondNum = temp;
}
