#ifndef COLOR_GRAPH_H
#define COLOR_GRAPH_H

typedef struct {
    int nVertices;      // The total number of vertices
    int nEdges;         // Keep track of how many edges are in this graph 
    int** adjMatrix;    // Dynamic matrix of adjacent vertices (allocated at runtime)
} Graph;

void initGraph(Graph* graph, int vertices);
void addEdge(Graph *graph, int u, int v);
void deleteGraph(Graph* graph);
int* greedyColor(Graph* g);
double CLOCK();

#endif /*COLOR_GRAPH_H*/