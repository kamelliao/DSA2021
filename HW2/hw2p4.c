#include <stdio.h>
#include <stdlib.h>
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define MIN 0
#define MAX 1000000001

int count = 1;

typedef struct node
{
    int key;
    struct node* left;
    struct node* right;
} Node;

Node* new_node()
{
    Node* node = malloc(sizeof(Node));
    node->key = 0;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void traverse(Node* root, int lower, int upper)
{
    if (root->left)
    {
        if (root->left->key > lower && root->left->key < min(upper, root->key))
        {
            count++;
        }

        traverse(root->left, lower, min(upper, root->key));
    }

    if (root->right)
    {
        if (root->right->key > max(lower, root->key) && root->right->key < upper)
        {
            count++;
        }

        traverse(root->right, max(lower, root->key), upper);
    }
}

int main(void)
{
    int n;
    scanf("%d", &n);

    Node** tree = malloc(n * sizeof(Node*));
    for (int i = 0; i < n; i++)
    {
        tree[i] = new_node();
    }

    int key, left, right;
    for (int i = 0; i < n; i++)
    {
        scanf("%d%d%d", &key, &left, &right);
        tree[i]->key = key;
        tree[i]->left = (left == -1) ? NULL : tree[left-1];
        tree[i]->right = (right == -1) ? NULL : tree[right-1];
    }
    
    traverse(tree[0], MIN, MAX);
    printf("%d\n", count);

    // release memory
    for (int i = 0; i < n; i++)
    {
        free(tree[i]);
    }
    free(tree);
    return 0;
}