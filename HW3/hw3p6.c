/*
    Reference: 無
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define NUM 100000
#define END -1

int* graph[NUM];
int ptr[NUM] = {0};
int all_neighbors = 0;

int edges[4*NUM][2];
int n_edges = 0;

int queue[NUM];
int head = 0, tail = 0;

void enqueue(int idx)
{
    queue[tail] = idx;
    tail = (++tail) % NUM;
}

int dequeue()
{
    int item = queue[head];
    head = (++head) % NUM;
    
    return item;
}

bool is_empty()
{
    return (head == tail);
}

int neighbor(int idx)
{
    return (graph[idx][ptr[idx]]);
}

void check_neighbor(int idx)
{
    int nbr = neighbor(idx);
    if (nbr == END)
        return;
    if (neighbor(nbr) != idx)
        return;

    edges[n_edges][0] = (idx <= nbr) ? idx : nbr;
    edges[n_edges][1] = (idx > nbr) ? idx : nbr;
    n_edges++;
    
    ptr[idx]++;
    ptr[nbr]++;

    if (neighbor(idx) != END)
        enqueue(idx);
    if (neighbor(nbr) != END)
        enqueue(nbr);
}

int main(void)
{
    // input
    int n_vertices;
    scanf("%d", &n_vertices);

    int n_neighbors, id;
    for (int i = 0; i < n_vertices; i++)
    {
        scanf("%d", &n_neighbors);
        all_neighbors += n_neighbors;
        graph[i] = malloc((n_neighbors + 1) * sizeof(int));
        for (int j = 0; j < n_neighbors; j++)
        {
            scanf("%d", &id);
            graph[i][j] = id - 1;
        }
        graph[i][n_neighbors] = END;
    }

    // check
    for (int k = 0; k < n_vertices; k++)
    {
        enqueue(k);
        while (!is_empty())
        {
            check_neighbor(dequeue());
        }
    }

    // print
    if (n_edges != (all_neighbors / 2))
    {
        printf("No\n");
    }
    else
    {
        printf("Yes\n");
        for (int i = 0; i < n_edges; i++)
        {
            printf("%d %d\n", edges[i][0] + 1, edges[i][1] + 1);
        }
    }

    return 0;
}