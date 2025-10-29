#include "avl_tree.h"

// GUI globals
extern HWND g_hInput, g_hInsert, g_hSearch, g_hDelete, g_hStatus;
extern AVLNode *g_root;
extern double g_last_execution_time;
extern DWORD g_highlight_start;

// Calculate horizontal position for nodes
int calculate_x_position(AVLNode *root, AVLNode *target, int left, int right)
{
    if (root == NULL || target == NULL)
        return (left + right) / 2;

    if (root == target)
    {
        return (left + right) / 2;
    }

    int mid = (left + right) / 2;

    if (target->key < root->key)
    {
        return calculate_x_position(root->left, target, left, mid - MIN_HORIZONTAL_GAP);
    }
    else
    {
        return calculate_x_position(root->right, target, mid + MIN_HORIZONTAL_GAP, right);
    }
}

// Get node level (depth)
int get_node_level(AVLNode *root, AVLNode *target, int level)
{
    if (root == NULL)
        return -1;
    if (root == target)
        return level;

    int left_level = get_node_level(root->left, target, level + 1);
    if (left_level != -1)
        return left_level;

    return get_node_level(root->right, target, level + 1);
}

// Draw a single node with enhanced visuals
void draw_node(HDC hdc, AVLNode *node, int x, int y, BOOL highlight, BOOL is_search)
{
    // Select colors based on state
    HBRUSH hBrush;
    if (is_search && highlight)
    {
        hBrush = CreateSolidBrush(COLOR_NODE_SEARCH);
    }
    else if (highlight)
    {
        hBrush = CreateSolidBrush(COLOR_NODE_HIGHLIGHT);
    }
    else
    {
        hBrush = CreateSolidBrush(COLOR_NODE);
    }

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // Draw shadow for depth
    HBRUSH hShadowBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hShadowPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hShadowPen);
    SelectObject(hdc, hShadowBrush);

    SetBkMode(hdc, TRANSPARENT);
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 40, 0};
    Ellipse(hdc, x - NODE_RADIUS + 3, y - NODE_RADIUS + 3,
            x + NODE_RADIUS + 3, y + NODE_RADIUS + 3);

    // Draw main circle with border
    SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(30, 40, 50));
    SelectObject(hdc, hPen);

    Ellipse(hdc, x - NODE_RADIUS, y - NODE_RADIUS,
            x + NODE_RADIUS, y + NODE_RADIUS);

    // Draw key value
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, COLOR_TEXT);
    HFONT hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    char text[32];
    sprintf(text, "%d", node->key);

    RECT textRect = {x - NODE_RADIUS, y - NODE_RADIUS + 3,
                     x + NODE_RADIUS, y + 5};
    DrawText(hdc, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Draw balance factor with better styling
    sprintf(text, "BF:%d", node->balance_factor);
    SetTextColor(hdc, RGB(240, 240, 255));
    HFONT hSmallFont = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    SelectObject(hdc, hSmallFont);

    RECT bfRect = {x - NODE_RADIUS, y + 3, x + NODE_RADIUS, y + NODE_RADIUS - 3};
    DrawText(hdc, text, -1, &bfRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hSmallFont);
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hShadowBrush);
    DeleteObject(hPen);
    DeleteObject(hShadowPen);
}

// Draw edge between nodes with improved styling
void draw_edge(HDC hdc, int x1, int y1, int x2, int y2)
{
    HPEN hPen = CreatePen(PS_SOLID, 3, COLOR_EDGE);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Recursive tree drawing
void draw_tree_recursive(HDC hdc, AVLNode *node, AVLNode *root,
                         int left, int right, int level)
{
    if (node == NULL)
        return;

    int x = calculate_x_position(root, node, left, right);
    int y = CONTROL_PANEL_HEIGHT + 60 + level * LEVEL_HEIGHT;

    // Draw edges to children first
    if (node->left)
    {
        int child_x = calculate_x_position(root, node->left, left, x - MIN_HORIZONTAL_GAP);
        int child_y = y + LEVEL_HEIGHT;
        draw_edge(hdc, x, y + NODE_RADIUS, child_x, child_y - NODE_RADIUS);
    }

    if (node->right)
    {
        int child_x = calculate_x_position(root, node->right, x + MIN_HORIZONTAL_GAP, right);
        int child_y = y + LEVEL_HEIGHT;
        draw_edge(hdc, x, y + NODE_RADIUS, child_x, child_y - NODE_RADIUS);
    }

    // Determine if node should be highlighted
    BOOL highlight = FALSE;
    BOOL is_search = FALSE;
    DWORD current_time = GetTickCount();
    if (current_time - g_highlight_start < HIGHLIGHT_DURATION)
    {
        if (node == g_found_node)
        {
            highlight = TRUE;
            is_search = TRUE;
        }
        else if (node == g_rotation_node)
        {
            highlight = TRUE;
        }
    }

    // Draw the node
    draw_node(hdc, node, x, y, highlight, is_search);

    // Recursively draw children
    draw_tree_recursive(hdc, node->left, root, left, x - MIN_HORIZONTAL_GAP, level + 1);
    draw_tree_recursive(hdc, node->right, root, x + MIN_HORIZONTAL_GAP, right, level + 1);
}

// Main tree drawing function
void draw_tree(HDC hdc, AVLNode *root)
{
    if (root == NULL)
    {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(148, 163, 184));
        HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        RECT rect = {0, CONTROL_PANEL_HEIGHT, WINDOW_WIDTH,
                     WINDOW_HEIGHT - FOOTER_HEIGHT};
        DrawText(hdc, "Tree is empty. Insert nodes to visualize the AVL tree structure.", -1,
                 &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        return;
    }

    draw_tree_recursive(hdc, root, root, 50, WINDOW_WIDTH - 50, 0);
}

// Draw control panel with modern styling
void draw_control_panel(HDC hdc)
{
    // Background with gradient effect
    HBRUSH hBrush = CreateSolidBrush(COLOR_CONTROL_BG);
    RECT rect = {0, 0, WINDOW_WIDTH, CONTROL_PANEL_HEIGHT};
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // Shadow border
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(226, 232, 240));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 0, CONTROL_PANEL_HEIGHT - 1, NULL);
    LineTo(hdc, WINDOW_WIDTH, CONTROL_PANEL_HEIGHT - 1);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Title with icon effect
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, COLOR_TITLE);
    HFONT hFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    TextOut(hdc, 20, 15, "AVL Tree Visualizer", 19);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    // Stats section with better formatting
    char statsText[512];
    int node_count = count_nodes(g_root);
    int tree_height = height(g_root);

    if (g_last_execution_time > 0)
    {
        const char *op_names[] = {"", "INSERT", "SEARCH", "DELETE"};
        sprintf(statsText, "Operation: %s  |  Time: %.6f ms  |  Nodes: %d  |  Height: %d",
                op_names[g_last_operation],
                g_last_execution_time * 1000,
                node_count,
                tree_height);
    }
    else
    {
        sprintf(statsText, "Ready  |  Nodes: %d  |  Height: %d  |  Waiting for operation...",
                node_count, tree_height);
    }

    if (g_last_rotation != ROTATION_NONE)
    {
        char rotText[128];
        const char *rotName[] = {"", "LL-Rotation", "RR-Rotation", "LR-Rotation", "RL-Rotation"};
        sprintf(rotText, "  |  Balance: %s", rotName[g_last_rotation]);
        strcat(statsText, rotText);
    }

    HFONT hStatsFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    hOldFont = (HFONT)SelectObject(hdc, hStatsFont);
    SetTextColor(hdc, COLOR_SUBTITLE);

    TextOut(hdc, 20, 52, statsText, strlen(statsText));

    SelectObject(hdc, hOldFont);
    DeleteObject(hStatsFont);
}

// Draw footer with better visibility
void draw_footer(HDC hdc)
{
    // Background
    HBRUSH hBrush = CreateSolidBrush(COLOR_FOOTER_BG);
    RECT rect = {0, WINDOW_HEIGHT - FOOTER_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT};
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // Top border with accent
    HPEN hPen = CreatePen(PS_SOLID, 2, COLOR_ACCENT);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 0, WINDOW_HEIGHT - FOOTER_HEIGHT, NULL);
    LineTo(hdc, WINDOW_WIDTH, WINDOW_HEIGHT - FOOTER_HEIGHT);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Footer text with better spacing
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, COLOR_FOOTER_TEXT);
    HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    const char *line1 = "This microproject was developed under the curriculum of Data Structures and Algorithms in C";
    const char *line2 = "as a self-learning activity by Anvay Mayekar ( SYECS1 - 26 ), instructed by Professor Anita Nalawade.";

    RECT textRect1 = {20, WINDOW_HEIGHT - FOOTER_HEIGHT + 18,
                      WINDOW_WIDTH - 20, WINDOW_HEIGHT - FOOTER_HEIGHT + 38};
    DrawText(hdc, line1, -1, &textRect1, DT_CENTER | DT_TOP | DT_SINGLELINE);

    RECT textRect2 = {20, WINDOW_HEIGHT - FOOTER_HEIGHT + 42,
                      WINDOW_WIDTH - 20, WINDOW_HEIGHT - FOOTER_HEIGHT + 62};
    DrawText(hdc, line2, -1, &textRect2, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // Add decorative element
    SetTextColor(hdc, COLOR_ACCENT);
    HFONT hIconFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    SelectObject(hdc, hIconFont);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hIconFont);
}