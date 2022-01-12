/*
    Reference:
    NTU COOL討論版、TA
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define LOWER -100001
#define UPPER 100001

// define data structure
typedef struct node
{
    int* data;
    int* order;
    int size;
    int cut;
    int start;
    struct node* next;
} Node;

// functions
Node* new_node();
void merge(Node* left, Node* right);
void release_node(Node* node);
void push(Node* curr, int number, int order);
void intitial_seq(int n);
Node** allocate(int start, int end);
int binary_search(Node* list, int target);

// instructions
void query(int start, int end, int k);
void insert(int order, int element);
void delete(int order);
void reverse(int start, int end);

// claim variables
int LEN = 0;
int n_list = 0;
int n_tmp_nodes = 0;
int n_reverse = 0;
Node* head = NULL;
Node* tail = NULL;
Node* tmp_nodes[2];

int main(void)
{
    // get inputs
    int n, q;
    scanf("%d%d", &n, &q);
    LEN = (int) sqrt(n + q) + 1;

    // initil sequence
    intitial_seq(n);

    // get instructions
    char instruction[20];
    int start, end, k, element;
    for (int i = 0; i < q; i++)
    {
        scanf("%s", instruction);

        if (strcmp(instruction, "Query") == 0)
        {
            scanf("%d%d%d", &start, &end, &k);
            query(start, end, k);
        }
        else if (strcmp(instruction, "Insert") == 0)
        {
            scanf("%d%d", &start, &element);
            insert(start, element);
        }
        else if (strcmp(instruction, "Delete") == 0)
        {
            scanf("%d", &start);
            delete(start);
        }
        else if (strcmp(instruction, "Reverse") == 0)
        {
            n_reverse += 1;
            scanf("%d%d", &start, &end);
            reverse(start, end);
        }
        else
        {
            printf("Error: Invalid Instruction!\n");
        }
    }

    // release memory
    Node* curr = head;
    Node* to_free = NULL;
    while (curr != NULL)
    {
        to_free = curr;
        curr = curr->next;
        release_node(to_free);
    }

    return 0;
}

Node* new_node()
{
    Node* node = malloc(sizeof(Node));
    node->data = malloc(LEN * 2 * sizeof(int));
    node->order = malloc(LEN * 2 * sizeof(int));
    node->size = 0;
    node->start = 1;
    node->next = NULL;

    return node;
}

void merge(Node* left, Node* right)
{
    for (int i = 0; i < right->size; i++)
    {
        push(left, right->data[i], right->order[i] + right->start - left->start);
    }
    left->next = right->next;
    release_node(right);
}

void release_node(Node* node)
{
    free(node->data);
    free(node->order);
    free(node);
}

void push(Node* curr, int number, int order)
{
    if (curr->size == 0)
    {
        curr->data[curr->size] = number;
        curr->order[curr->size] = order;
    }
    else
    {
        int i;
        for (i = curr->size; i > 0; i--)
        {
            if (number >= curr->data[i-1])
            {
                break;
            }
            else
            {
                curr->data[i] = curr->data[i-1];
                curr->order[i] = curr->order[i-1];
            }
        }
        curr->data[i] = number;
        curr->order[i] = order;
    }
    curr->size++;
}

void intitial_seq(int n)
{
    head = new_node();
    head->start = 1;
    Node* curr = head;
    int new;
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &new);
        if (curr->size < LEN)
        {
            push(curr, new, i + 1 - curr->start);
        }
        else
        {
            curr->next = new_node();
            curr = curr->next;
            curr->start = i + 1;
            push(curr, new, i + 1 - curr->start);
        }
    }
}

Node** allocate(int start, int end)
{
    Node* curr = head;
    int count = 0;

    Node** lists = calloc(LEN + n_reverse*2, sizeof(Node*));
    n_list = 0;

    while (count < end && curr != NULL)
    {
        if (count + curr->size < start)
        {
            // pass
        }
        else if (count < start)
        {
            Node* subnode = new_node();

            for (int i = 0; i < curr->size; i++)
            {
                if (curr->order[i] + curr->start >= start && curr->order[i] + curr->start <= end)
                {
                    subnode->data[subnode->size] = curr->data[i];
                    subnode->order[subnode->size] = curr->order[i];
                    subnode->size++;
                }
            }

            lists[n_list++] = subnode;
            tmp_nodes[n_tmp_nodes++] = subnode;
        }
        else if (start <= count && count + curr->size <= end)
        {
            lists[n_list++] = curr;
        }
        else if (start <= count && count + curr->size > end)
        {
            Node* subnode = new_node();

            for (int i = 0; i < curr->size; i++)
            {
                if (curr->order[i] + curr->start <= end)
                {
                    subnode->data[subnode->size] = curr->data[i];
                    subnode->order[subnode->size] = curr->order[i];
                    subnode->size++;
                }
            }

            lists[n_list++] = subnode;
            tmp_nodes[n_tmp_nodes++] = subnode;
        }

        count += curr->size;
        curr = curr->next;
    }

    return lists;
}

int binary_search(Node* list, int target)
{
    int low = 0, high = list->size - 1, mid;
    while (low <= high)
    {
        mid = (low + high) / 2;

        if (list->data[mid] <= target && list->data[mid + 1] > target)
        {
            list->cut = mid;
            return mid;
        }
        else if (list->data[mid] <= target && list->data[mid + 1] <= target)
        {
            low = mid + 1;

            if (low == list->size)
            {
                list->cut = mid;
                return mid;
            }
        }
        else if (list->data[mid] > target)
        {
            high = mid - 1;

            if (high == -1)
            {
                list->cut = -1;
                return -1;
            }
        }
    }
}

void query(int start, int end, int k)
{
    // allocate
    Node** list = allocate(start, end);

    // get max and min
    int max = LOWER, min = UPPER;
    for (int i = 0; i < n_list; i++)
    {
        if (list[i]->data[0] < min)
        {
            min = list[i]->data[0];
        }

        if (list[i]->data[list[i]->size-1] > max)
        {
            max = list[i]->data[list[i]->size-1];
        }
    }

    // find
    int mid = (min + max) / 2;
    int count = 0, target = LOWER;
    int close_count = UPPER, close_target = UPPER;
    while (min <= max)
    {
        // set initial value
        mid = (min + max) / 2;
        count = 0;
        target = LOWER;

        // get count
        for (int i = 0; i < n_list; i++)
        {
            // get count
            count += (binary_search(list[i], mid) + 1);

            // get target element
            // aka the biggest at the cut position
            if (list[i]->cut >= 0 && list[i]->cut < list[i]->size && list[i]->data[list[i]->cut] >= target)
            {
                target = list[i]->data[list[i]->cut];
            }
        }

        // adjust range
        if (count == k)
        {
            break;  // BINGO !
        }
        else if (count < k)
        {
            min = mid + 1;
        }
        else if (count > k)
        {
            max = mid - 1;
        }

        if (count > k && count <= close_count && mid < close_target)
        {
            close_count = count;
            close_target = mid;
        }
    }

    if (count != k)
    {
        target = close_target;
    }

    // release tmp_nodes and list
    while (n_tmp_nodes > 0)
    {
        release_node(tmp_nodes[--n_tmp_nodes]);
    }
    free(list);

    printf("%d\n", target);
}

void insert(int order, int element)
{
    // find the node
    Node* curr = head;
    Node* prev = NULL;

    while (curr != NULL)
    {
        // condition: INSIDE
        if ((curr->start <= order && (curr->start + curr->size > order || (curr->start + curr->size == 1 && order == 1))) || (curr->start <= order && curr->next == NULL))
        {
            // increment existed order if >= target
            for (int i = 0; i < curr->size; i++)
            {
                if (curr->order[i] + curr->start >= order)
                {
                    curr->order[i]++;
                }
            }
            
            // insert the element
            int i;
            for (i = curr->size; i > 0; i--)
            {
                // insert element to the right position
                if (element >= curr->data[i-1])
                {
                    break;
                }
                else
                {
                    curr->data[i] = curr->data[i-1];
                    curr->order[i] = curr->order[i-1];
                }
            }

            curr->data[i] = element;
            curr->order[i] = order - curr->start;
            curr->size++;

            // split to half if size == LEN * 2
            if (curr->size == LEN * 2)
            {
                Node* left = new_node();
                Node* right = new_node();
                left->start = curr->start;
                right->start = curr->start + LEN;

                for (int i = 0; i < curr->size; i++)
                {
                    if (curr->order[i] < LEN)
                    {
                        push(left, curr->data[i], curr->order[i]);
                    }
                    else
                    {
                        push(right, curr->data[i], curr->order[i] - LEN);
                    }
                }
                
                // connect
                if (curr->start == 1)
                {
                    head = left;
                }
                else
                {
                    prev->next = left;
                }
                left->next = right;
                right->next = curr->next;
                release_node(curr);
                curr = right;
            }
        }
        // condition: exceed
        else if (curr->start > order)
        {
            curr->start++;
        }

        prev = curr;
        curr = curr->next;
    }
}

void delete(int order)
{
    // find the node
    Node* curr = head;
    Node* prev = NULL;

    while (curr != NULL)
    {
        // store the posiotn to delete
        int pos_todel;
        // condition: INSIDE
        if (curr->start <= order && curr->start + curr->size > order)
        {
            for (int i = 0; i < curr->size; i++)
            {
                // find the position to delete
                if (curr->order[i] + curr->start == order)
                {
                    pos_todel = i;
                }
                // decrement existed order if >= target
                else if (curr->order[i] + curr->start > order)
                {
                    curr->order[i]--;
                }
            }

            // delete the element
            for (int j = pos_todel; j < curr->size - 1; j++)
            {
                curr->data[j] = curr->data[j+1];
                curr->order[j] = curr->order[j+1];
            }
            curr->size--;

            // delete empty node
            if (curr->size == 0 && !(curr->start == 1 && curr->next == NULL))
            {
                Node* to_del;
                // if is first node
                if (curr->start == 1)
                {
                    to_del = curr;
                    prev = NULL;
                    curr = curr->next;
                    head = curr;
                }
                // if is not first node
                else
                {
                    to_del = curr;
                    prev->next = curr->next;
                    curr = curr->next;
                }
                release_node(to_del);
            }
            // no delete
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
        // condition: others
        else
        {
            // condition: exceed
            if (curr->start > order)
            {
                curr->start--;
            }
            // condition: NOT YET
            else
            {
                // pass
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

void reverse(int start, int end)
{
    Node* curr = head;
    Node* prev = NULL;
    Node* start_node = NULL;
    Node* end_node = NULL;
    Node* real_end_node = NULL;
    Node** list = calloc(LEN + n_reverse*2, sizeof(Node*));
    n_list = 0;
    while (curr != NULL && curr->start <= end)
    {
        // inner
        if (curr->start <= start && curr->start + curr->size > end)
        {
            for (int i = 0; i < curr->size; i++)
            {
                if (curr->start + curr->order[i] >= start && curr->start + curr->order[i] <= end)
                {
                    curr->order[i] = (end - curr->start) + (start - curr->start) - curr->order[i];
                }
            }
        }
        // full
        else if (curr->start >= start && curr->start + curr->size - 1 <= end)
        {
            for (int i = 0; i < curr->size; i++)
            {
                curr->order[i] = curr->size - 1 - curr->order[i];
            }
            list[n_list++] = curr;
        }
        // head
        else if (curr->start <= start && curr->start + curr->size > start)
        {
            Node* left = new_node();
            Node* right = new_node();
            left->start = curr->start;
            right->start = start;
            for (int i = 0; i < curr->size; i++)
            {
                if (curr->start + curr->order[i] < start)
                {
                    push(left, curr->data[i], curr->order[i]);
                }
                else
                {
                    push(right, curr->data[i], (curr->start + curr->size - start - 1) - (curr->order[i] + curr->start - start));
                }
            }
            // connect
            if (curr->start == 1)
            {
                head = left;
            }
            else
            {
                prev->next = left;
            }
            left->next = right;
            right->next = curr->next;
            release_node(curr);
            prev = left;
            curr = right;

            list[n_list++] = right;
        }
        // tail
        else if (curr->start <= end && curr->start + curr->size > end)
        {
            Node* left = new_node();
            Node* right = new_node();
            left->start = curr->start;
            right->start = end + 1;
            for (int i = 0; i < curr->size; i++)
            {
                if (curr->start + curr->order[i] <= end)
                {
                    push(left, curr->data[i], (end - curr->start) - curr->order[i]);
                }
                else
                {
                    push(right, curr->data[i], curr->order[i] - (end - curr->start + 1));
                }
            }
            // connect
            prev->next = left;
            left->next = right;
            right->next = curr->next;
            release_node(curr);
            curr = left;

            list[n_list++] = left;
        }

        // set start & end node
        if (n_list == 1)
        {
            start_node = prev;
        }
        
        if (n_list > 0)
        {
            end_node = curr;
            real_end_node = end_node->next;
        }

        prev = curr;
        curr = curr->next;
    }

    // connect
    if (start_node && end_node)
    {
        int i = n_list - 1;
        
        // merge fisrt segment
        if (start_node->size + list[i]->size < 2 * LEN)
        {
            list[i]->start = start_node->start + start_node->size;
            merge(start_node, list[i]);
            i--;
        }


        // connect
        curr = start_node;
        for (i = i; i >= 0; i--)
        {
            list[i]->start = curr->start + curr->size;
            curr->next = list[i];
            curr = curr->next;
        }

        
        // merge last segment
        if (real_end_node != NULL && (curr->size + real_end_node->size < 2 * LEN))
        {
            merge(curr, real_end_node);
        }
        else
        {
            curr->next = real_end_node;
        }
    }
    else if (end_node)
    {
        int i = n_list - 1;
        head = list[i];
        head->start = 1;
        curr = head;
        for (i = i - 1; i >= 0; i--)
        {
            list[i]->start = curr->start + curr->size;
            curr->next = list[i];
            curr = curr->next;
        }

        // merge last segment
        if (real_end_node != NULL && (curr->size + real_end_node->size < 2 * LEN))
        {
            merge(curr, real_end_node);
        }
        else
        {
            curr->next = real_end_node;
        }
    }
    free(list);
}

