#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "color_graph.h"

// initialize clock
double CLOCK() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec*1e-6);
}

// initialize the graph with a given number of vertices
void initGraph(Graph* graph, int vertices) {
    graph->nVertices = vertices;
    graph->adjMatrix = (int**) malloc(sizeof(int*) * (vertices));
    for(int i = 0; i < vertices; i++) {
        // initialize each array
        graph->adjMatrix[i] = (int *) malloc(sizeof(int) * vertices); 
        memset(graph->adjMatrix[i], 0, sizeof(int) * vertices);
    }
}

// add an edge between vertices u and v
void addEdge(Graph *graph, int u, int v) {
    graph->adjMatrix[u][v] = 1;
    graph->adjMatrix[v][u] = 1;
    graph->nEdges++;
}

// frees up the memory allocated for the adjacent array
void deleteGraph(Graph* graph) {
    for(int i = 0; i < graph->nVertices; i++) {
        free(graph->adjMatrix[i]);
    }
    free(graph->adjMatrix);
}

int isAdj(Graph* g, int u, int v) {
    return (g->adjMatrix[u][v] == 1 && g->adjMatrix[v][u] == 1);
}

// Function to check if conflicts exist in the coloring
int conflicts_exist(int *result, Graph* g) {
    int conflict = 0;
    int vert = g->nVertices;
    int u; int v;
    #pragma omp parallel for reduction(|:conflict)
    for (u = 0; u < vert; u++) {
        for (v = 0; v < vert; v++) {
            if (isAdj(g, u, v) && result[u] == result[v]) {
                conflict |= 1;
            }
        }
    }
    return conflict;
}

// find smallest available color for given vertex
int get_color(int v, int* result, Graph* g) {
    int used_colors[g->nVertices];
    memset(used_colors, 0, sizeof(used_colors));

    int vert = g->nVertices;

    // Mark colors used by neighbors
    for(int i = 0; i < vert; i++) {
        if(g->adjMatrix[v][i] == 1 && result[i] != -1) {
            used_colors[result[i]] = 1;
        }
    }

    // Find smallest available color
    for(int c = 0; c < vert; c++) {
        if(!used_colors[c]) return c;
    }

    // should not return this
    return -1; 
}

int* parallelGraph(Graph* g) {
    // check for base cases
    int vert = g->nVertices;

    // dynamically allocate memory for result array
    int* result = (int *) malloc(sizeof(int) * vert);

    // initialize result to {-1}
    #pragma omp parallel for
    for(int n = 0; n < vert; n++) {
        result[n] = -1;
    }

    #pragma omp parallel for 
    for(int u = 0; u < vert; u++) {
        result[u] = get_color(u, result, g);
    }

    while(conflicts_exist(result, g)) {
        #pragma omp parallel for
        for(int u = 0; u < vert; u++) {
            for(int v = 0; v < vert; v++) {
                if(g->adjMatrix[u][v] == 1 && result[u] == result[v]) {
                    result[u] = get_color(u, result, g);
                }
            }
        }
    }

    return result;
}

int main(int argc, char** argv) {
    int opt;    // option int
    int nThreads =  16; // default 5 philosophers

    // get user arguments
    while((opt = getopt(argc, argv, "t:h")) != -1) {
        int temp;
        switch (opt) {
            case 't':
                temp = atoi(optarg);
                if (temp < 0){
                    printf("Invalid input for Number of Philosophers (Threads). Default: %d\n", nThreads);
                } else {
                    nThreads = temp;
                }
                break;
            case 'h':
                printf("Usage: %s [-t threads] [-h]\n", argv[0]);
                printf("  -t threads  Set the max number of threads\n");
                printf("  -h          Display this help message\n");
                return 0;
            default:
                fprintf(stderr, "Usage: %s [-t threads] [-h]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    // Set max number of threads to use
    omp_set_num_threads(nThreads);

    // // O(N^2) if color
    // Graph graph;
    // int vert = 6; // Number of vertices

    // // Creating graph 1
    // initGraph(&graph, vert);
    // addEdge(&graph, 0, 4);
    // addEdge(&graph, 0, 5);
    // addEdge(&graph, 0, 2);

    // addEdge(&graph, 1, 4);
    // addEdge(&graph, 1, 5);

    // addEdge(&graph, 2, 3);
    // addEdge(&graph, 2, 4);

    // addEdge(&graph, 5, 4);

    // printf("Parallel Graph Coloring using OpenMP:\n");
    // printf("Graph1(%d, %d):\n", graph.nVertices, graph.nEdges);

    // // benchmark start
    // double t1, total;
    // t1 = CLOCK();

    // int* result = parallelGraph(&graph);

    // // benchmark stop
    // total = CLOCK() - t1;

    //  // print result
    // for(int i = 0; i < vert; i++) {
    //     printf("Node %d -> Color %d\n", i, result[i]);
    // }
    // printf("Time elapsed: %lf ms\n", total);

    // // free allocated memory
    // deleteGraph(&graph);
    // free(result);
    
    // Graph 2
    Graph graph;
    int vert = 8; // Number of vertices

    // Creating graph 2
    initGraph(&graph, vert);
    addEdge(&graph, 0, 1);
    addEdge(&graph, 0, 2);
    addEdge(&graph, 0, 6);
    addEdge(&graph, 0, 7);

    addEdge(&graph, 1, 4);
    addEdge(&graph, 1, 5);
    addEdge(&graph, 1, 7);

    addEdge(&graph, 2, 3);
    addEdge(&graph, 2, 4);

    addEdge(&graph, 3, 4);
    addEdge(&graph, 3, 5);

    addEdge(&graph, 6, 7);

    printf("Parallel Graph Coloring using OpenMP:\n");
    printf("Graph2(%d, %d):\n", graph.nVertices, graph.nEdges);

    // benchmark start
    double t1, total;
    t1 = CLOCK();

    int* result = parallelGraph(&graph);

    // benchmark stop
    total = CLOCK() - t1;

     // print result
    for(int i = 0; i < vert; i++) {
        printf("Node %d -> Color %d\n", i, result[i]);
    }
    printf("Time elapsed: %lf ms\n", total);

    // free allocated memory
    deleteGraph(&graph);
    free(result);
    return 0;
}