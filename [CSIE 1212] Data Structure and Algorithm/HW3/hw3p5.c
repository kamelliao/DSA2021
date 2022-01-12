/*
    Reference:
    1. https://cp-algorithms.com/data_structures/treap.html
    2. https://tioj.ck.tp.edu.tw/uploads/attachment/5/35/9.pdf
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define INT_MIN -2147483648

enum
{
    INSERT = 1, DELETE = 2, INCREASE = 3, QUERY = 4, REVERSE = 5, REMOVE = 6
};

typedef struct node node;
struct node
{
    int fix;
    int prior;
    int incr;
    int larg;
    int size;
    bool rev;
    
    node *l;
    node *r;
};

// treap maintenance
node* merge(node* a, node* b);
void split(node* root, node** l, node** r, int tgt_key, int acc);
node* new_node(int prior);
node* insert(node* root, int key, int prior);
node* delete(node* root, int key);

// some utilities
void swap_child(node* root);

// update informations
void push(node* root);
void pull(node* root);
int size(node* root);
int largest(node* root);

// operations
node* increase(node* root, int lower, int upper, int value);
node* reverse(node* root, int lower, int upper);
node* query(node* root, int lower, int upper);
int find(node* root, int max, int acc);
node* rmlarg(node* root);

int main(void)
{
    int n_books, n_operations;
    scanf("%d%d", &n_books, &n_operations);

    node* root = NULL;
    int priority;
    for (int i = 1; i <= n_books; i++)
    {
        scanf("%d", &priority);
        root = insert(root, i, priority);
    }

    
    int l, r, p, k, operation;
    while (n_operations--)
    {
        scanf("%d", &operation);
        switch (operation)
        {
            case INSERT:
            {
                scanf("%d%d", &p, &k);
                root = insert(root, k, p);
                break;
            }
            case DELETE:
            {
                scanf("%d", &k);
                root = delete(root, k);
                break;
            }
            case INCREASE:
            {
                scanf("%d%d%d", &l, &r, &p);
                root = increase(root, l, r, p);
                break;
            }
            case QUERY:
            {
                scanf("%d%d", &l, &r);
                root = query(root, l, r);
                break;
            }
            case REVERSE:
            {
                scanf("%d%d", &l, &r);
                root = reverse(root, l, r);
                break;
            }
            case REMOVE:
            {
                root = rmlarg(root);
                break;
            }
            default:
            {
                printf("Invalid Operation!\n");
                break;
            }
        }
    }
    
    return 0;
}

node* merge(node* l, node* r)
{
    push(l);
    push(r);
    if (l == NULL)
        return r;
    if (r == NULL)
        return l;
    if (l->fix > r->fix)
    {
        l->r = merge(l->r, r);
        pull(l);
        return l;
    }
    else
    {
        r->l = merge(l, r->l);
        pull(r);
        return r;
    }
}

void split(node* root, node** l, node** r, int tgt_key, int acc)
{
    if (root == NULL)
    {
        *l = NULL;
        *r = NULL;
        return ;
    }
    
    int cur_key = acc + size(root->l) + 1;
    if (cur_key <= tgt_key)
    {
        *l = root;
        push(*l);
        split(root->r, &((*l)->r), r, tgt_key, cur_key);
        pull(*l);
    }
    else
    {
        *r = root;
        push(*r);
        split(root->l, l, &((*r)->l), tgt_key, acc);
        pull(*r);
    }
}

node* new_node(int prior)
{
    node* new = malloc(sizeof(node));
    new->fix = rand();
    new->prior = prior;
    new->incr = 0;
    new->larg = prior;
    new->size = 1;
    new->rev = false;
    new->l = NULL;
    new->r = NULL;

    return new;
}

node* insert(node* root, int key, int prior)
{
    // create new node
    node* new = new_node(prior);

    // insert it
    if (root == NULL)
        return new;
    
    node *tmp_l, *tmp_r;
    split(root, &tmp_l, &tmp_r, key, 0);
    root = merge(tmp_l, new);
    root = merge(root, tmp_r);

    return root;
}

node* delete(node* root, int key)
{
    node *tmp_l, *tmp_tgt, *tmp_r;
    split(root, &tmp_l, &tmp_tgt, key-1, 0);
    split(tmp_tgt, &tmp_tgt, &tmp_r, 1, 0);
    free(tmp_tgt);
    root = merge(tmp_l, tmp_r);

    return root;
}

// some utilities
void swap_child(node* root)
{
    node* tmp = root->l;
    root->l = root->r;
    root->r = tmp;
}

// update informations
void push(node* root)
{
    if (!root)
        return;

    // hand down to children
    if (root->l)
    {
        root->l->prior += root->incr;
        root->l->incr += root->incr;
        root->l->larg += root->incr;

        if (root->rev)
        {
            swap_child(root->l);
            root->l->rev ^= root->rev;
        }
    }
    if (root->r)
    {
        root->r->prior += root->incr;
        root->r->incr += root->incr;
        root->r->larg += root->incr;
        
        if (root->rev)
        {
            swap_child(root->r);
            root->r->rev ^= root->rev;
        }
    }

    root->incr = 0;
    root->rev = false;
}

void pull(node* root)
{
    if (!root)
        return;

    // update largest
    root->larg = max(root->prior, max(largest(root->l), largest(root->r)));
    
    // update size
    root->size = size(root->l) + size(root->r) + 1;
}

int size(node* root)
{
    if (!root)
        return 0;
    return root->size;
}

int largest(node* root)
{
    if (!root)
        return INT_MIN;
    return root->larg;
}

// operations
node* increase(node* root, int lower, int upper, int value)
{
    if (!root)
        return NULL;

    node *tmp_l, *tmp_tgt, *tmp_r;
    split(root, &tmp_l, &tmp_tgt, lower-1, 0);
    split(tmp_tgt, &tmp_tgt, &tmp_r, upper-lower+1, 0);

    tmp_tgt->prior += value;
    tmp_tgt->incr += value;
    tmp_tgt->larg += value;

    root = merge(tmp_l, tmp_tgt);
    root = merge(root, tmp_r);

    return root;
}

node* reverse(node* root, int lower, int upper)
{
    if (!root)
        return NULL;

    node *tmp_l, *tmp_tgt, *tmp_r;
    split(root, &tmp_l, &tmp_tgt, lower-1, 0);
    split(tmp_tgt, &tmp_tgt, &tmp_r, upper-lower+1, 0);

    tmp_tgt->rev ^= true;
    swap_child(tmp_tgt);

    root = merge(tmp_l, tmp_tgt);
    root = merge(root, tmp_r);

    return root;
}

node* query(node* root, int lower, int upper)
{
    if (!root)
        return NULL;

    node *tmp_l, *tmp_tgt, *tmp_r;
    split(root, &tmp_l, &tmp_tgt, lower-1, 0);
    split(tmp_tgt, &tmp_tgt, &tmp_r, upper-lower+1, 0);

    int largest = tmp_tgt->larg;

    root = merge(tmp_l, tmp_tgt);
    root = merge(root, tmp_r);

    printf("%d\n", largest);

    return root;
}

int find(node* root, int max, int acc)
{
    push(root);
    int cur_key = acc + size(root->l) + 1;
    if (root->l && (max == largest(root->l) || max == root->l->prior))
        return find(root->l, max, acc);
    else if (max == root->prior)
        return cur_key;
    else if (root->r)
        return find(root->r, max, cur_key);
}

node* rmlarg(node* root)
{
    if (!root)
        return NULL;

    int tgt_key = find(root, largest(root), 0);
    root = delete(root, tgt_key);

    return root;
}
