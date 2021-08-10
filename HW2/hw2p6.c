/*
    Reference:
    1. https://iq.opengenus.org/leftist-heap/
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define N 100001

enum
{
    HIGHEST, FRONT, BACK, deHIGHEST, deFRONT, deBACK
};

// structures
typedef struct
{
    int rs;
    int rd;
    void (*execute)(int rs, int rd);
} operation;

typedef struct
{
    int state;
    int line;
    bool removed;
} info;

typedef struct node
{
    int height;
    int dist;
    // deque attribute
    struct node* prev;
    struct node* next;
    // heap attribute
    struct node* left;
    struct node* right;
} node;

typedef struct
{
    int id;
    node* root;
    node* front;
    node* back;
} line;

// initializations
void init_infos(int n_packages);
void init_lines(int n_lines);

// operations
void execute_push(int rs, int rd);
void execute_merge(int rs, int rd);

// change state
void chgstate(int id, int instruction);

// node maintenance
node* new_node(int height);

// heap maintenance
void mergeheap(line* line_s, line* line_d);
node* mergenroot(node* h1, node* h2);
node* merge(node* h_larger, node* h_smaller);
void swap_child(node* h);

void insert_deque(line* line, node* new_package);
void insert_heap(line* line, node* new_package);

// remove elements
void pop_front(line* line);
void pop_back(line* line);
void extract_max(line* line);
void pop(int height);

// utilities
void release_memory(int n_lines);

// variables
operation ops[2*N];
info infos[N];
line* lines[N];
int records[N];

int main(void)
{
    int n_tasks, n_packages, n_operations, n_lines;
    char operation[10];
    scanf("%d", &n_tasks);
    while (n_tasks--)
    {
        // get number of packages, operations, lines
        scanf("%d%d%d", &n_packages, &n_operations, &n_lines);

        // get operations
        for (int i = 0; i < n_operations; i++)
        {
            scanf("%s%d%d", operation, &ops[i].rs, &ops[i].rd);
            ops[i].execute = (strcmp(operation, "push") == 0) ? execute_push : execute_merge;
        }

        // get records
        int rec_back;
        for (rec_back = 0; rec_back < n_packages; rec_back++)
        {
            scanf("%d", &records[rec_back]);
        }

        // initializations
        init_infos(n_packages);
        init_lines(n_lines);

        // check possibility
        int rec_front = 0;
        for (int i = 0; i < n_operations; i++)
        {
            ops[i].execute(ops[i].rs, ops[i].rd);
            while (infos[records[rec_front]].state)
            {
                pop(records[rec_front]);
                rec_front++;
            }
        }
        
        // print result
        if (rec_front == rec_back)
        {
            printf("possible\n");
        }
        else
        {
            printf("impossible\n");
        }

        // release memory
        release_memory(n_lines);
    }
    
    return 0;
}

// initializations
void init_infos(int n_packages)
{
    for (int i = 0; i <= n_packages; i++)
    {
        infos[i].state = 0;
        infos[i].line = -1;
        infos[i].removed = false;
    }
}

void init_lines(int n_lines)
{
    for (int i = 0; i < n_lines; i++)
    {
        lines[i] = malloc(sizeof(line));
        lines[i]->id = i;
        lines[i]->root = NULL;
        lines[i]->front = NULL;
        lines[i]->back = NULL;
    }
}

// operations
void execute_push(int rs, int rd)
{
    // update state
    infos[rs].line = rd;

    // insert
    insert_deque(lines[rd], new_node(rs));
    insert_heap(lines[rd], new_node(rs));
}

void execute_merge(int rs, int rd)
{
    mergeheap(lines[rs], lines[rd]);
}

// change state
void chgstate(int id, int instruction)
{
    switch (instruction)
    {
        case HIGHEST: infos[id].state |= 1 << 2; break;
        case FRONT: infos[id].state |= 1 << 1; break;
        case BACK: infos[id].state |= 1 << 0; break;
        case deHIGHEST: infos[id].state &= ~(1 << 2); break;
        case deFRONT: infos[id].state &= ~(1 << 1); break;
        case deBACK: infos[id].state &= ~(1 << 0); break;
    }
}

// node maintenance
node* new_node(int height)
{
    node* new = malloc(sizeof(node));
    new->height = height;
    new->dist = 0;
    new->prev = NULL;
    new->next = NULL;
    new->left = NULL;
    new->right = NULL;

    return new;
}

// heap maintenance
void mergeheap(line* line_s, line* line_d)
{
    // if source is empty, do nothing
    if (line_s->root == NULL)
        return ;
    
    // if source is not empty, merge it to destination
    // change line id
    infos[line_s->root->height].line = line_d->id;  // !latest error!
    infos[line_s->front->height].line = line_d->id;  // !latest error!
    infos[line_s->back->height].line = line_d->id;  // !latest error!

    // deque: connect
    if (line_d->front == NULL) // if destination is empty
    {
        line_d->front = line_s->front;  // !latest error!
        line_d->back = line_s->back;
    }
    else // if destination is not empty
    {
        chgstate(line_d->back->height, deBACK);  // destination: old back out!
        chgstate(line_s->front->height, deFRONT);  // source: old front out!

        line_d->back->next = line_s->front;
        line_s->front->prev = line_d->back;
        line_d->back = line_s->back;
    }

    line_s->front = NULL;
    line_s->back = NULL;

    // heap: merge
    line_d->root = mergenroot(line_s->root, line_d->root);
    line_s->root = NULL;
}

node* mergenroot(node* h1, node* h2)
{
    if (h1 == NULL)
        return h2;
    if (h2 == NULL)
        return h1;
    if (h1->height > h2->height)
        return merge(h1, h2);
    else
        return merge(h2, h1);
}

node* merge(node* h_larger, node* h_smaller)
{
    // update state
    chgstate(h_smaller->height, deHIGHEST);

    // merge them
    if (h_larger->left == NULL)
    {
        h_larger->left = h_smaller;
    }
    else
    {
        h_larger->right = mergenroot(h_larger->right, h_smaller);
        if (h_larger->left->dist < h_larger->right->dist)
        {
            swap_child(h_larger);
        }
        h_larger->dist = h_larger->right->dist + 1;
    }

    return h_larger;
}

void swap_child(node* h)
{
    node* tmp = h->left;
    h->left = h->right;
    h->right = tmp;
}

void insert_deque(line* line, node* new_package)
{
    if (line->front == NULL)
    {
        line->front = new_package;
        line->back = new_package;
        chgstate(new_package->height, FRONT);
        chgstate(new_package->height, BACK);
    }
    else
    {
        line->back->next = new_package;
        new_package->prev = line->back;
        line->back = new_package;
        chgstate(new_package->prev->height, deBACK);
        chgstate(new_package->height, BACK);
    }
}

void insert_heap(line* line, node* new_package)
{
    line->root = mergenroot(line->root, new_package);
    chgstate(line->root->height, HIGHEST);
}

// remove elements
void pop_front(line* line)
{
    node* to_remove;
    while (line->front != NULL && infos[line->front->height].removed == true)
    {
        to_remove = line->front;
        chgstate(line->front->height, deFRONT);
        line->front = line->front->next;
        if (line->front == NULL)
        {
            line->back = NULL;
        }
        else
        {
            line->front->prev = NULL;
            chgstate(line->front->height, FRONT);
            infos[line->front->height].line = infos[to_remove->height].line;  // !latest error!
        }
        free(to_remove);
    }
}

void pop_back(line* line)
{
    node* to_remove;
    while (line->back != NULL && infos[line->back->height].removed == true)
    {
        to_remove = line->back;
        chgstate(line->back->height, deBACK);
        line->back = line->back->prev;
        if (line->back == NULL)
        {
            line->front = NULL;
        }
        else
        {
            line->back->next = NULL;
            chgstate(line->back->height, BACK);
            infos[line->back->height].line = infos[to_remove->height].line;  // !latest error!
        }
        free(to_remove);
    }
}

void extract_max(line* line)
{
    node* to_remove;
    while (line->root != NULL && infos[line->root->height].removed == true)
    {
        to_remove = line->root;
        chgstate(line->root->height, deHIGHEST);
        line->root = mergenroot(line->root->left, line->root->right);
        if (line->root != NULL)
        {
            chgstate(line->root->height, HIGHEST);
            infos[line->root->height].line = infos[to_remove->height].line;  // !latest error!
        }
        free(to_remove);
    }
}

void pop(int height)
{
    // update status
    infos[height].removed = true;

    // remove item
    if ((infos[height].state & (1<<2)))
        extract_max(lines[infos[height].line]);
    
    if ((infos[height].state & (1<<1)))
        pop_front(lines[infos[height].line]);
    else if ((infos[height].state & (1<<0)))
        pop_back(lines[infos[height].line]);
    
    // update status
    infos[height].line = -1;
    infos[height].state = 0;
}

// utilities
void release_memory(int n_lines)
{
    node *curr = NULL, *nxt = NULL;
    for (int i = 0; i < n_lines; i++)
    {
        // remove deque
        curr = lines[i]->front;
        while (curr != NULL)
        {
            nxt = curr->next;
            free(curr);
            curr = nxt;
        }
    }
}
