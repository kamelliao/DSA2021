/*
    Reference:
    1. https://courses.engr.illinois.edu/cs374/sp2020/B/hw/hw_05_extra.pdf
    2. 柯晨緯
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "generator.h"
#define N 3000000

enum Position
{
    P, Q, R
};

typedef struct
{
    int tag;
    int coord[3];
} line;

uint64_t intersections = 0;
line lines[N];

int Qs[N], Rs[N];
int leftq[N], leftr[N], rightq[N], rightr[N];

int cmp(const void *a ,const void *b)
{
    if (((line *)a)->coord[P] > ((line *)b)->coord[P])
    {
        return 1;
    }
    else if (((line *)a)->coord[P] < ((line *)b)->coord[P])
    {
        return -1;
    }
    else
    {
        if (((line *)a)->coord[R] >= ((line *)b)->coord[R])
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
}

void merge(int lower, int mid, int upper)
{
    // number of elements of the left part and the right part
    int n1 = mid - lower + 1, n2 = upper - mid;

    // collect elements
    int i, j;
    for (i = 0; i < n1; i++)
    {
        leftq[i] = Qs[lower + i];
        leftr[i] = Rs[lower + i];
    }

    for (j = 0; j < n2; j++)
    {
        rightq[j] = Qs[mid + j + 1];
        rightr[j] = Rs[mid + j + 1];
    }

    leftq[n1] = INT_MAX;
    leftr[n1] = INT_MAX;
    rightq[n2] = INT_MAX;
    rightr[n2] = INT_MAX;

    // count intersect
    uint64_t n_intersect = 0;
    int a = 0, b = 0;
    for (a = 0; a < n1; a++)
    {
        while (leftr[a] >= rightq[b])
        {
            b++;
        }
        n_intersect += b;
    }

    // merge Q
    i = 0, j = 0;
    for (int k = lower; k <= upper; k++)
    {
        // compare
        if (leftq[i] <= rightq[j])
        {
            Qs[k] = leftq[i];
            i++;
        }
        else
        {
            Qs[k] = rightq[j];
            j++;
        }
    }

    // merge R
    i = 0, j = 0;
    for (int k = lower; k <= upper; k++)
    {
        // compare
        if (leftr[i] <= rightr[j])
        {
            Rs[k] = leftr[i];
            i++;
        }
        else
        {
            Rs[k] = rightr[j];
            j++;
        }
    }

    // update total intersections if pos==BTM(which represents 1)
    intersections += n_intersect;
}

void mergesort(int lower, int upper)
{
    if (lower < upper) // no need to sort 1 vs. 1
    {
        int mid = ((lower + upper) >> 1);
        mergesort(lower, mid);
        mergesort(mid + 1, upper);
        merge(lower, mid, upper);
    }
}

int main(void)
{
    generator.init();
    int t = generator.getT();
    while (t--)
    {
        int n, *p, *q, *r;
        generator.getData(&n, &p, &q, &r);

        // set lines
        intersections = 0;
        for (int i = 0; i < n; i++)
        {
            lines[i].tag = i;
            lines[i].coord[P] = p[i];
            lines[i].coord[Q] = (q[i] < r[i]) ? q[i] : r[i];
            lines[i].coord[R] = (q[i] >= r[i]) ? q[i] : r[i];
        }

        // check intersection
        qsort(lines, n, sizeof(lines[0]), cmp);
        for (int i = 0; i < n; i++)
        {
            Qs[i] = lines[i].coord[Q];
            Rs[i] = lines[i].coord[R];
        }
        
        mergesort(0, n-1);

        // print result
        printf("%lld\n", intersections);
    }
}
