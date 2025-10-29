#include "avl_tree.h"

// Global variables for visualization
RotationType g_last_rotation = ROTATION_NONE;
AVLNode *g_rotation_node = NULL;
AVLNode *g_found_node = NULL;
OperationType g_last_operation = OP_NONE;

// Create a new AVL node
AVLNode *create_node(int key)
{
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (node == NULL)
        return NULL;

    node->key = key;
    node->height = 1;
    node->balance_factor = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Get height of node
int height(AVLNode *node)
{
    return node ? node->height : 0;
}

// Calculate balance factor
int balance_factor(AVLNode *node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

// Update height and balance factor
void update_height(AVLNode *node)
{
    if (node == NULL)
        return;

    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = (left_h > right_h ? left_h : right_h) + 1;
    node->balance_factor = left_h - right_h;
}

// Right rotation (LL case)
AVLNode *rotate_right(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    update_height(y);
    update_height(x);

    g_last_rotation = ROTATION_LL;
    g_rotation_node = x;

    return x;
}

// Left rotation (RR case)
AVLNode *rotate_left(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    update_height(x);
    update_height(y);

    g_last_rotation = ROTATION_RR;
    g_rotation_node = y;

    return y;
}

// Balance the node
AVLNode *balance_node(AVLNode *node)
{
    if (node == NULL)
        return NULL;

    update_height(node);
    int bf = balance_factor(node);

    // Left-Left case
    if (bf > 1 && balance_factor(node->left) >= 0)
    {
        return rotate_right(node);
    }

    // Left-Right case
    if (bf > 1 && balance_factor(node->left) < 0)
    {
        g_last_rotation = ROTATION_LR;
        node->left = rotate_left(node->left);
        AVLNode *result = rotate_right(node);
        g_rotation_node = result;
        return result;
    }

    // Right-Right case
    if (bf < -1 && balance_factor(node->right) <= 0)
    {
        return rotate_left(node);
    }

    // Right-Left case
    if (bf < -1 && balance_factor(node->right) > 0)
    {
        g_last_rotation = ROTATION_RL;
        node->right = rotate_right(node->right);
        AVLNode *result = rotate_left(node);
        g_rotation_node = result;
        return result;
    }

    return node;
}

// Insert a node
AVLNode *insert_node(AVLNode *root, int key)
{
    // Standard BST insertion
    if (root == NULL)
    {
        return create_node(key);
    }

    if (key < root->key)
    {
        root->left = insert_node(root->left, key);
    }
    else if (key > root->key)
    {
        root->right = insert_node(root->right, key);
    }
    else
    {
        // Duplicate keys not allowed
        return root;
    }

    // Balance the node
    return balance_node(root);
}

// Find minimum value node
AVLNode *find_min(AVLNode *node)
{
    while (node && node->left != NULL)
    {
        node = node->left;
    }
    return node;
}

// Delete a node
AVLNode *delete_node(AVLNode *root, int key)
{
    if (root == NULL)
        return NULL;

    // Standard BST deletion
    if (key < root->key)
    {
        root->left = delete_node(root->left, key);
    }
    else if (key > root->key)
    {
        root->right = delete_node(root->right, key);
    }
    else
    {
        // Node found
        if (root->left == NULL || root->right == NULL)
        {
            AVLNode *temp = root->left ? root->left : root->right;

            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
            {
                *root = *temp;
            }
            free(temp);
        }
        else
        {
            AVLNode *temp = find_min(root->right);
            root->key = temp->key;
            root->right = delete_node(root->right, temp->key);
        }
    }

    if (root == NULL)
        return NULL;

    // Balance the node
    return balance_node(root);
}

// Search for a node
AVLNode *search_node(AVLNode *root, int key)
{
    if (root == NULL || root->key == key)
    {
        return root;
    }

    if (key < root->key)
    {
        return search_node(root->left, key);
    }
    else
    {
        return search_node(root->right, key);
    }
}

// Free the entire tree
void free_tree(AVLNode *root)
{
    if (root == NULL)
        return;

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

// Count total nodes
int count_nodes(AVLNode *root)
{
    if (root == NULL)
        return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}