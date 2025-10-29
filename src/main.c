#include "avl_tree.h"
// author: @anvaymayekar
// Forward declarations
void draw_tree(HDC hdc, AVLNode *root);
void draw_control_panel(HDC hdc);
void draw_footer(HDC hdc);

// Global variables
HWND g_hInput, g_hInsert, g_hSearch, g_hDelete, g_hStatus;
AVLNode *g_root = NULL;
double g_last_execution_time = 0.0;
DWORD g_highlight_start = 0;

// Control IDs
#define ID_INPUT 101
#define ID_INSERT 102
#define ID_SEARCH 103
#define ID_DELETE 104

// Measure execution time with high precision
double measure_time(clock_t start, clock_t end)
{
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Get input value from edit control
BOOL get_input_value(HWND hWnd, int *value)
{
    char buffer[32];
    GetWindowText(g_hInput, buffer, sizeof(buffer));

    if (strlen(buffer) == 0)
    {
        MessageBox(hWnd, "Please enter a number.", "Input Required",
                   MB_OK | MB_ICONWARNING);
        return FALSE;
    }

    *value = atoi(buffer);

    if (*value == 0 && strcmp(buffer, "0") != 0)
    {
        MessageBox(hWnd, "Please enter a valid number.", "Invalid Input",
                   MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

// Handle insert operation
void handle_insert(HWND hWnd)
{
    int value;
    if (!get_input_value(hWnd, &value))
        return;

    // Check for duplicate
    if (search_node(g_root, value) != NULL)
    {
        MessageBox(hWnd, "This value already exists in the tree!\nAVL trees don't allow duplicates.",
                   "Duplicate Value", MB_OK | MB_ICONWARNING);
        return;
    }

    g_last_rotation = ROTATION_NONE;
    g_rotation_node = NULL;
    g_last_operation = OP_INSERT;

    clock_t start = clock();
    g_root = insert_node(g_root, value);
    clock_t end = clock();

    g_last_execution_time = measure_time(start, end);
    g_highlight_start = GetTickCount();

    SetWindowText(g_hInput, "");
    SetFocus(g_hInput);
    InvalidateRect(hWnd, NULL, TRUE);

    // Set timer for unhighlighting
    SetTimer(hWnd, 1, HIGHLIGHT_DURATION, NULL);
}

// Handle search operation
void handle_search(HWND hWnd)
{
    int value;
    if (!get_input_value(hWnd, &value))
        return;

    g_last_rotation = ROTATION_NONE;
    g_rotation_node = NULL;
    g_found_node = NULL;
    g_last_operation = OP_SEARCH;

    clock_t start = clock();
    AVLNode *found = search_node(g_root, value);
    clock_t end = clock();

    g_last_execution_time = measure_time(start, end);

    if (found)
    {
        g_found_node = found;
        g_highlight_start = GetTickCount();

        char msg[256];
        sprintf(msg, "Node %d found successfully!\n\n"
                     "Details:\n\n"
                     "Height: %d\n"
                     "Balance Factor: %d\n"
                     "Search Time: %.6f ms",
                value, found->height, found->balance_factor,
                g_last_execution_time * 1000);
        MessageBox(hWnd, msg, "Search Result - Found", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        char msg[256];
        sprintf(msg, "Node %d not found in the tree.\n\n"
                     "Search Time: %.6f ms\n\n"
                     "The value does not exist in the current tree structure.",
                value, g_last_execution_time * 1000);
        MessageBox(hWnd, msg, "Search Result - Not Found", MB_OK | MB_ICONINFORMATION);
    }

    SetWindowText(g_hInput, "");
    SetFocus(g_hInput);
    InvalidateRect(hWnd, NULL, TRUE);

    SetTimer(hWnd, 1, HIGHLIGHT_DURATION, NULL);
}

// Handle delete operation
void handle_delete(HWND hWnd)
{
    int value;
    if (!get_input_value(hWnd, &value))
        return;

    // Check if node exists before deletion
    if (search_node(g_root, value) == NULL)
    {
        MessageBox(hWnd, "Node not found in the tree.\nCannot delete a non-existent value.",
                   "Delete Failed", MB_OK | MB_ICONWARNING);
        return;
    }

    g_last_rotation = ROTATION_NONE;
    g_rotation_node = NULL;
    g_found_node = NULL;
    g_last_operation = OP_DELETE;

    clock_t start = clock();
    g_root = delete_node(g_root, value);
    clock_t end = clock();

    g_last_execution_time = measure_time(start, end);
    g_highlight_start = GetTickCount();

    char msg[256];
    sprintf(msg, "Node %d deleted successfully!\n\n"
                 "Deletion Time: %.6f ms\n\n"
                 "The tree has been rebalanced automatically.",
            value, g_last_execution_time * 1000);
    MessageBox(hWnd, msg, "Delete Result", MB_OK | MB_ICONINFORMATION);

    SetWindowText(g_hInput, "");
    SetFocus(g_hInput);
    InvalidateRect(hWnd, NULL, TRUE);

    SetTimer(hWnd, 1, HIGHLIGHT_DURATION, NULL);
}

// Custom button drawing for modern look
void draw_custom_button(HDC hdc, RECT rect, const char *text, BOOL is_hovered)
{
    // Button background with rounded effect
    HBRUSH hBrush = CreateSolidBrush(is_hovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON);
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // Button border
    HPEN hPen = CreatePen(PS_SOLID, 2, is_hovered ? COLOR_BUTTON_HOVER : COLOR_BUTTON);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw rounded rectangle effect
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 8, 8);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Button text - WHITE for visibility on blue background
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT hFont = CreateFont(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    DrawText(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // Create input field with modern styling and black text
        g_hInput = CreateWindowEx(
            WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER,
            20, 90, 180, 35,
            hWnd, (HMENU)ID_INPUT, NULL, NULL);

        // Set black text color for input
        HDC hdcInput = GetDC(g_hInput);
        SetTextColor(hdcInput, RGB(0, 0, 0));
        SetBkColor(hdcInput, RGB(255, 255, 255));
        ReleaseDC(g_hInput, hdcInput);

        // Create buttons
        g_hInsert = CreateWindow(
            "BUTTON", "Insert",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
            220, 90, 110, 35,
            hWnd, (HMENU)ID_INSERT, NULL, NULL);

        g_hSearch = CreateWindow(
            "BUTTON", "Search",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
            340, 90, 110, 35,
            hWnd, (HMENU)ID_SEARCH, NULL, NULL);

        g_hDelete = CreateWindow(
            "BUTTON", "Delete",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
            460, 90, 110, 35,
            hWnd, (HMENU)ID_DELETE, NULL, NULL);

        // Set font for input with black text
        HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

        SendMessage(g_hInput, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Set focus to input
        SetFocus(g_hInput);

        break;
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;

        if (pDIS->CtlType == ODT_BUTTON)
        {
            const char *btnText = "";
            BOOL is_hovered = (pDIS->itemState & ODS_SELECTED);

            if (pDIS->CtlID == ID_INSERT)
                btnText = "Insert";
            else if (pDIS->CtlID == ID_SEARCH)
                btnText = "Search";
            else if (pDIS->CtlID == ID_DELETE)
                btnText = "Delete";

            draw_custom_button(pDIS->hDC, pDIS->rcItem, btnText, is_hovered);
        }
        return TRUE;
    }

    case WM_CTLCOLOREDIT:
    {
        // Set black text on white background for input box
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, RGB(0, 0, 0));
        SetBkColor(hdcEdit, RGB(255, 255, 255));
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_INSERT:
            handle_insert(hWnd);
            break;
        case ID_SEARCH:
            handle_search(hWnd);
            break;
        case ID_DELETE:
            handle_delete(hWnd);
            break;
        case ID_INPUT:
            // Handle Enter key in input field
            if (HIWORD(wParam) == EN_CHANGE)
            {
                // Could add real-time validation here
            }
            break;
        }
        break;
    }

    case WM_TIMER:
    {
        // Unhighlight after duration
        InvalidateRect(hWnd, NULL, TRUE);
        KillTimer(hWnd, 1);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Create double buffer for smooth rendering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        // Enable anti-aliasing
        SetBkMode(hdcMem, TRANSPARENT);

        // Fill background
        HBRUSH hBrush = CreateSolidBrush(COLOR_BACKGROUND);
        RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        FillRect(hdcMem, &rect, hBrush);
        DeleteObject(hBrush);

        // Draw components
        draw_control_panel(hdcMem);
        draw_tree(hdcMem, g_root);
        draw_footer(hdcMem);

        // Copy to screen
        BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcMem, 0, 0, SRCCOPY);

        // Cleanup
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_KEYDOWN:
    {
        // Handle keyboard shortcuts
        if (wParam == VK_RETURN)
        {
            handle_insert(hWnd);
        }
        else if (wParam == VK_F3)
        {
            handle_search(hWnd);
        }
        else if (wParam == VK_DELETE)
        {
            handle_delete(hWnd);
        }
        break;
    }

    case WM_DESTROY:
    {
        free_tree(g_root);
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

// Main entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Register window class
    const char CLASS_NAME[] = "AVLTreeVisualizer";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClass(&wc);

    // Calculate window size with frame
    RECT windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    AdjustWindowRect(&windowRect, style, FALSE);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    // Center window on screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // Create window
    HWND hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "AVL Tree Visualizer - Data Structures & Algorithms",
        style,
        posX, posY,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWnd == NULL)
    {
        MessageBox(NULL, "Failed to create window!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}