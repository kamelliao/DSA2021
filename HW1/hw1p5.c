/*
    Reference:
    https://www.geeksforgeeks.org/xor-linked-list-a-memory-efficient-doubly-linked-list-set-2/
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define CAPCITY 1000

typedef struct train
{
    int label;
    struct train *npx;
} Train;

typedef struct rail
{
    Train* head;
    Train* tail;
} Rail;

Rail* station[CAPCITY];

Train* XOR(Train* t1, Train* t2);
Rail* new_rail();
void show_rails(int num_rails);
void delete_rails(int num_rails);

bool is_empty(Rail* rail);
void enter(Rail* rail, int label);
void leave(Rail* rail);
void migrate(Rail* rail_s, Rail* rail_d);

int main(void)
{
    // get input
    int num_rails, num_logs;
    scanf("%d%d", &num_rails, &num_logs);

    // initialize rails
    for (int i = 0; i < num_rails; i++)
    {
        station[i] = new_rail();
    }

    // get logs
    char query[10];
    int rail, rail2, label;
    for (int i = 0; i < num_logs; i++)
    {
        scanf("%s", query);

        if (strcmp(query, "enter") == 0)
        {
            scanf("%d%d", &rail, &label);
            enter(station[rail], label);
        }
        else if (strcmp(query, "leave") == 0)
        {
            scanf("%d", &rail);
            leave(station[rail]);
        }
        else if (strcmp(query, "migrate") == 0)
        {
            scanf("%d%d", &rail, &rail2);
            migrate(station[rail], station[rail2]);
        }
        else
        {
            printf("Error query!\n");
        }
    }

    // print results
    show_rails(num_rails);

    // release memory
    delete_rails(num_rails);

    return 0;
}

Train* XOR(Train* t1, Train* t2)
{
  return (Train*)((uintptr_t)t1 ^ (uintptr_t)t2);
}

Rail* new_rail()
{
    Rail* rail = (Rail*) malloc(sizeof(Rail));
    rail->tail = NULL;
    rail->head = NULL;
    
    return rail;
}

void show_rails(int num_rails)
{
    for (int i = 0; i < num_rails; i++)
    {
        if (is_empty(station[i]))
        {
            printf("\n");
        }
        else
        {
            Train* curr = station[i]->tail;
            Train* prev = NULL;
            Train* next = NULL;
            while (curr != NULL)
            {
                printf("%d ", curr->label);
                next = XOR(prev, curr->npx);
                prev = curr;
                curr = next;
            }
            printf("\n");
        }
    }
}

void delete_rails(int num_rails)
{
    for (int i = 0; i < num_rails; i++)
    {
        Train* curr = station[i]->head;
        Train* prev = NULL;
        Train* next = NULL;
        while (curr != NULL)
        {
            next = XOR(prev, curr->npx);
            prev = curr;
            free(curr);
            curr = next;
        }
    }
}

bool is_empty(Rail* rail)
{
    return (rail->head == NULL);
}

void enter(Rail* rail, int label)
{
    // create a new train
    Train* new_train = (Train*) malloc(sizeof(Train));
    new_train->label = label;
    new_train->npx = XOR(rail->head, NULL);  // rail->head might be NULL

    // enter into rail
    if (rail->head == NULL)
    {
        rail->tail = new_train;
    }
    else
    {
        rail->head->npx = XOR(rail->head->npx, new_train);  // link old head to new head
    }
    rail->head = new_train;
}

void leave(Rail* rail)
{
    if (!is_empty(rail))
    {
        Train* to_leave = rail->head;  // temporarily store old head
        rail->head = XOR(rail->head->npx, NULL);  // set new head (might be NULL)
        if (!is_empty(rail))
        {
            rail->head->npx = XOR(XOR(rail->head->npx, to_leave), NULL);  // remove link to old head
        }
        free(to_leave);
    }
}

void migrate(Rail* rail_s, Rail* rail_d)
{
    // migrate to destination
    if (!is_empty(rail_s))
    {
        if (!is_empty(rail_d))
        {
            rail_d->head->npx = XOR(rail_d->head->npx, rail_s->head);  // pass the baton
            rail_s->head->npx = XOR(rail_s->head->npx, rail_d->head);  // catch the baton
            rail_d->head = rail_s->tail;  // give away the throne of head
        }
        else
        {
            rail_d->head = rail_s->tail;
            rail_d->tail = rail_s->head;
        }
        
        // empty source rail
        rail_s->head = NULL;
        rail_s->tail = NULL;
    }
}