#ifndef COLOR_GRAPH_H
#define COLOR_GRAPH_H

typedef struct {
    int nVertices;      // The total number of vertices
    int nEdges;         // Keep track of how many edges are in this graph 
    int** adjMatrix;    // Dynamic matrix of adjacent vertices (allocated at runtime)
} Graph;
double CLOCK();
void initGraph(Graph* graph, int vertices);
void addEdge(Graph *graph, int u, int v);
void deleteGraph(Graph* graph);
int isAdj(Graph* g, int u, int v);
int conflicts_exist(int *result, Graph* g);
int get_color(int v, int* result, Graph* g);
int* parallelGraph(Graph* g); 


#endif /*COLOR_GRAPH_H*/