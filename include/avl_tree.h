#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "common.h"

// AVL Node structure
typedef struct AVLNode
{
    int key;
    int height;
    int balance_factor;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

// Rotation types for visualization
typedef enum
{
    ROTATION_NONE,
    ROTATION_LL,
    ROTATION_RR,
    ROTATION_LR,
    ROTATION_RL
} RotationType;

// Operation types
typedef enum
{
    OP_NONE,
    OP_INSERT,
    OP_SEARCH,
    OP_DELETE
} OperationType;

// Global variables for tracking rotations and operations
extern RotationType g_last_rotation;
extern AVLNode *g_rotation_node;
extern AVLNode *g_found_node;
extern OperationType g_last_operation;

// Function prototypes
AVLNode *create_node(int key);
int height(AVLNode *node);
int balance_factor(AVLNode *node);
void update_height(AVLNode *node);
AVLNode *rotate_right(AVLNode *y);
AVLNode *rotate_left(AVLNode *x);
AVLNode *balance_node(AVLNode *node);
AVLNode *insert_node(AVLNode *root, int key);
AVLNode *find_min(AVLNode *node);
AVLNode *delete_node(AVLNode *root, int key);
AVLNode *search_node(AVLNode *root, int key);
void free_tree(AVLNode *root);
int count_nodes(AVLNode *root);

#endif // AVL_TREE_H