// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

// Number of vertices in the graph and number of threads of pthread
//#define V 9
#define NUM_THREADS 8

int *dist;// The output array, dist[i] will hold the shortest distance from src to i

bool *sptSet; // sptSet[i] will true if vertex i is included in shortest path tree or shortest distance from src to i is finalized

int V;

int i, v, count;

struct Graph {
    int nVertices;
    int **w;
};

struct Graph *createRandomGraph(int nVertices, int nArestas, int seed) {

    int k,v;
    srandom(seed);

    struct Graph *graph = (struct Graph *) malloc( sizeof(struct Graph) );
    graph->nVertices = nVertices;
    graph->w = (int **) malloc( sizeof(int *)  * nVertices );
    for (v=0; v<nVertices; v++) {
        graph->w[v] = (int *) malloc( sizeof(int) * nVertices );
        for (k=0; k<nVertices; k++)
            graph->w[v][k] = 0; // Division prevents overflows
    }

    for (k=0; k<nArestas; k++) {
        int source = random() % nVertices;
        int dest   = random() % nVertices;
        while (source == dest)
            dest = random() % nVertices;

        int w      = 1 + (random() % 10);
        graph->w[source][ dest ] = w;
        graph->w[dest][ source ] = w;
    }

    return graph;
}

//Create pthread
pthread_t threads[NUM_THREADS];

struct Graph *graph;

//Struct to be passed as an argument to the threads
struct data{
int u;
int tid;

};
//Function to be performed by each thread to update the distance vector
void *update(void *param){

//Get the values passed as arguments
struct data *val= (struct data *)param;
int u=val->u;
int tid=val->tid;

int current;

// Update dist[current] only if is not in sptSet, there is an edge from
// u to current, and total weight of path from src to  current through u is
// smaller than value of dist[current]
for(current=tid; current<V;current+=NUM_THREADS){
if (!sptSet[current] && graph->w[u][current] && dist[u] != INT_MAX){
    if(dist[u]+graph->w[u][current] < dist[current]){
        dist[current] = dist[u] + graph->w[u][current];
        }
    }
}
pthread_exit(NULL);
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int minDistance(int dist[], bool sptSet[], int V)
{
// Initialize min value
int min = INT_MAX, min_index;
//Get the minimum distance and the vertex that has the minimum distance
for (v = 0; v < V; v++)
    if (sptSet[v] == false && dist[v] <= min)
        min = dist[v], min_index = v;
return min_index;
}

// A utility function to print the constructed distance array
int printSolution(int dist[], int n)
{

printf("Vertex   Distance from Source\n");
for (i = 0; i < n; i++)
    printf("%d \t\t %d\n", i, dist[i]);

printf(" \n");
return 0;

}

// Funtion that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(struct Graph *graph, int src, int V)
{

// Initialize all distances as INFINITE and stpSet[] as false
for (i = 0; i < V; i++)
    dist[i] = INT_MAX, sptSet[i] = false;

// Distance of source vertex from itself is always 0
dist[src] = 0;

// Find shortest path for all vertices
for(count=0;count<V;count++)
{
    // Pick the minimum distance vertex from the set of vertices not
    // yet processed. u is always equal to src in first iteration.
    int u = minDistance(dist, sptSet, V);

    // Mark the picked vertex as processed
    sptSet[u] = true;

    //Create structure
    struct data *val= (struct data *)malloc(NUM_THREADS * sizeof(struct data));

   //Set val values and create threads
    for (v = 0; v < NUM_THREADS; v++){
        val[v].tid=v;
        val[v].u=u;
            pthread_create(&threads[v], NULL, update,  val+v);

    }
    //Join
    for(v=0;v<NUM_THREADS;v++){
        pthread_join(threads[v], NULL);
    }
}

// Print the constructed distance array
//printSolution(dist, V);
}

//Main
int main(int argc, char** argv){

  int i = 1, v;
  int nVertices = atoi(argv[1]);
  int nArestas  = (nVertices*nVertices)/2;
  int seed = i;

  V = nVertices;

  graph = createRandomGraph(nVertices, nArestas, seed);

  dist = (int *)malloc(nVertices*sizeof(int));
  sptSet = (bool *)malloc(nVertices*sizeof(bool));


  //struct timeval tv;
  //gettimeofday(&tv, 0);
  //long t1 = tv.tv_sec*1000 + tv.tv_usec/1000;
  struct timeval t1;
  gettimeofday(&t1, 0);

  //Apply algorithm
  dijkstra(graph, 0, nVertices);

  struct timeval t2;
  gettimeofday(&t2, 0);

  printf("%f\n", (t2.tv_sec*1000. + t2.tv_usec/1000.) - (t1.tv_sec*1000. + t1.tv_usec/1000.));

  for (v=0; v<nVertices; v++)
     free(graph->w[v]);
  free(graph->w);
  free(graph);

  return 0;
}
