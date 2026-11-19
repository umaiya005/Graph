#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100
#define MAX_EDGES 5000
#define INF 999999

typedef struct {
    int u, v, w;
} Edge;

int FindParent(int parent[], int x) {
    if (parent[x] != x)
        parent[x] = FindParent(parent, parent[x]);
    return parent[x];
}

int CompareEdges(const void *a, const void *b) {
    return ((const Edge *)a)->w - ((const Edge *)b)->w;
}

int FindMinCost(int vertices, int minCost[vertices],
                int included[vertices]) {
    int minimum = INF;
    int minVertex = -1;

    for (int i = 0; i < vertices; i++) {
        if (included[i] == 0 && minCost[i] < minimum) {
            minimum = minCost[i];
            minVertex = i;
        }
    }
    return minVertex;
}

int BuildPath(int parent[], int destination, int path[]) {
    int count = 0;
    int current = destination;

    while (current != -1) {
        path[count++] = current;
        current = parent[current];
    }

    for (int i = 0; i < count / 2; i++) {
        int temp = path[i];
        path[i] = path[count - 1 - i];
        path[count - 1 - i] = temp;
    }
    return count;
}

void PrintPath(int path[], int count, int cost) {
    for (int i = 0; i < count; i++) {
        printf("%d", path[i]);
        if (i < count - 1)
            printf(" -> ");
    }
    printf(" (Cost: %d)\n", cost);
}

void Dijkstra(int vertices, int graph[vertices][vertices],
              int source, int destination) {
    int distance[vertices], parent[vertices], included[vertices];

    for (int i = 0; i < vertices; i++) {
        distance[i] = INF;
        parent[i] = -1;
        included[i] = 0;
    }
    distance[source] = 0;

    for (int i = 0; i < vertices; i++) {
        int currentVertex = FindMinCost(vertices, distance, included);
        if (currentVertex == -1)
            break;

        included[currentVertex] = 1;

        for (int j = 0; j < vertices; j++) {
            if (graph[currentVertex][j] != 0 &&
                included[j] == 0 &&
                distance[currentVertex] != INF &&
                distance[currentVertex] + graph[currentVertex][j] < distance[j]) {

                distance[j] = distance[currentVertex] + graph[currentVertex][j];
                parent[j] = currentVertex;
            }
        }
    }

    if (distance[destination] == INF) {
        printf("Not Possible\n");
        return;
    }

    int path[vertices];
    int count = BuildPath(parent, destination, path);
    PrintPath(path, count, distance[destination]);
}

void FindTreePath(int vertices, int tree[vertices][vertices],
                  int source, int destination) {
    int queue[vertices], parent[vertices], visited[vertices];
    int front = 0, rear = 0;

    for (int i = 0; i < vertices; i++) {
        parent[i] = -1;
        visited[i] = 0;
    }

    queue[rear++] = source;
    visited[source] = 1;

    while (front < rear) {
        int currentVertex = queue[front++];

        for (int j = 0; j < vertices; j++) {
            if (tree[currentVertex][j] != 0 && visited[j] == 0) {
                visited[j] = 1;
                parent[j] = currentVertex;
                queue[rear++] = j;
            }
        }
    }

    if (visited[destination] == 0) {
        printf("Not Possible\n");
        return;
    }

    int path[vertices];
    int count = BuildPath(parent, destination, path);

    int cost = 0;
    for (int i = 0; i < count - 1; i++)
        cost += tree[path[i]][path[i + 1]];

    PrintPath(path, count, cost);
}

int Kruskal(int vertices, Edge edges[], int edgeCount,
            int tree[vertices][vertices]) {
    int parent[vertices];
    int count = 0;

    for (int i = 0; i < vertices; i++) {
        parent[i] = i;
        for (int j = 0; j < vertices; j++)
            tree[i][j] = 0;
    }

    qsort(edges, edgeCount, sizeof(Edge), CompareEdges);

    for (int i = 0; i < edgeCount; i++) {
        int parentU = FindParent(parent, edges[i].u);
        int parentV = FindParent(parent, edges[i].v);

        if (parentU != parentV) {
            tree[edges[i].u][edges[i].v] = edges[i].w;
            tree[edges[i].v][edges[i].u] = edges[i].w;
            parent[parentU] = parentV;
            count++;

            if (count == vertices - 1)
                break;
        }
    }
    return count == vertices - 1;
}

int Prim(int vertices, int graph[vertices][vertices],
         int tree[vertices][vertices]) {
    int parents[vertices], minCost[vertices], included[vertices];

    for (int i = 0; i < vertices; i++) {
        parents[i] = -1;
        minCost[i] = INF;
        included[i] = 0;
        for (int j = 0; j < vertices; j++)
            tree[i][j] = 0;
    }
    minCost[0] = 0;

    for (int i = 0; i < vertices; i++) {
        int currentVertex = FindMinCost(vertices, minCost, included);
        if (currentVertex == -1)
            return 0;

        included[currentVertex] = 1;

        for (int j = 0; j < vertices; j++) {
            if (graph[currentVertex][j] != 0 &&
                included[j] == 0 &&
                graph[currentVertex][j] < minCost[j]) {

                minCost[j] = graph[currentVertex][j];
                parents[j] = currentVertex;
            }
        }
    }

    for (int i = 1; i < vertices; i++) {
        tree[parents[i]][i] = graph[parents[i]][i];
        tree[i][parents[i]] = graph[i][parents[i]];
    }
    return 1;
}

void ShowKruskal(int vertices, Edge edges[], int edgeCount,
                 int source, int destination) {
    int tree[vertices][vertices];

    if (Kruskal(vertices, edges, edgeCount, tree))
        FindTreePath(vertices, tree, source, destination);
    else
        printf("Not Possible\n");
}

void ShowPrim(int vertices, int graph[vertices][vertices],
              int source, int destination) {
    int tree[vertices][vertices];

    if (Prim(vertices, graph, tree))
        FindTreePath(vertices, tree, source, destination);
    else
        printf("Not Possible\n");
}

void RunAll(int vertices, int graph[vertices][vertices],
            Edge edges[], int edgeCount,
            int source, int destination) {
    Edge copy[MAX_EDGES];
    for (int i = 0; i < edgeCount; i++)
        copy[i] = edges[i];

    printf("Dijkstra Path: ");
    Dijkstra(vertices, graph, source, destination);

    printf("Kruskal Path: ");
    ShowKruskal(vertices, copy, edgeCount, source, destination);

    printf("Prim Path: ");
    ShowPrim(vertices, graph, source, destination);
}

int main() {
    FILE *file = fopen("input.txt", "r");

    if (file == NULL) {
        printf("File not found!\n");
        return 1;
    }

    int vertices, edgeCount;

    if (fscanf(file, "%d %d", &vertices, &edgeCount) != 2 ||
        vertices <= 0 || vertices > MAX_VERTICES ||
        edgeCount < 0 || edgeCount > MAX_EDGES) {
        printf("Invalid input!\n");
        fclose(file);
        return 1;
    }

    Edge edges[MAX_EDGES];
    int graph[vertices][vertices];

    for (int i = 0; i < vertices; i++)
        for (int j = 0; j < vertices; j++)
            graph[i][j] = 0;

    for (int i = 0; i < edgeCount; i++) {
        if (fscanf(file, "%d %d %d",
                   &edges[i].u, &edges[i].v, &edges[i].w) != 3) {
            printf("Invalid input!\n");
            fclose(file);
            return 1;
        }

        int u = edges[i].u, v = edges[i].v;

        if (u < 0 || u >= vertices || v < 0 || v >= vertices ||
            u == v || edges[i].w <= 0) {
            printf("Invalid Edge!\n");
            fclose(file);
            return 1;
        }

        graph[u][v] = edges[i].w;
        graph[v][u] = edges[i].w;
    }

    int source, destination, removeU, removeV;

    if (fscanf(file, "%d %d", &source, &destination) != 2 ||
        fscanf(file, "%d %d", &removeU, &removeV) != 2) {
        printf("Invalid input!\n");
        fclose(file);
        return 1;
    }
    fclose(file);

    if (source < 0 || source >= vertices ||
        destination < 0 || destination >= vertices ||
        removeU < 0 || removeU >= vertices ||
        removeV < 0 || removeV >= vertices) {
        printf("Invalid Vertex!\n");
        return 1;
    }

    printf("\nOriginal Graph Results:\n");
    RunAll(vertices, graph, edges, edgeCount, source, destination);

    printf("\nAfter Removing Edge (%d,%d):\n", removeU, removeV);

    Edge newEdges[MAX_EDGES];
    int newEdgeCount = 0;
    int newGraph[vertices][vertices];

    for (int i = 0; i < vertices; i++)
        for (int j = 0; j < vertices; j++)
            newGraph[i][j] = 0;

    for (int i = 0; i < edgeCount; i++) {
        if (!((edges[i].u == removeU && edges[i].v == removeV) ||
              (edges[i].u == removeV && edges[i].v == removeU))) {

            newEdges[newEdgeCount] = edges[i];

            newGraph[edges[i].u][edges[i].v] = edges[i].w;
            newGraph[edges[i].v][edges[i].u] = edges[i].w;

            newEdgeCount++;
        }
    }

    RunAll(vertices, newGraph, newEdges, newEdgeCount, source, destination);

    return 0;
}
