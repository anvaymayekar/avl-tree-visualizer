#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Window dimensions
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 680
#define CONTROL_PANEL_HEIGHT 140
#define FOOTER_HEIGHT 90

// Colors - Modern palette
#define COLOR_BACKGROUND RGB(248, 249, 250)
#define COLOR_NODE RGB(59, 130, 246)
#define COLOR_NODE_HIGHLIGHT RGB(239, 68, 68)
#define COLOR_NODE_SEARCH RGB(34, 197, 94)
#define COLOR_TEXT RGB(255, 255, 255)
#define COLOR_EDGE RGB(148, 163, 184)
#define COLOR_CONTROL_BG RGB(255, 255, 255)
#define COLOR_BUTTON RGB(59, 130, 246)
#define COLOR_BUTTON_HOVER RGB(37, 99, 235)
#define COLOR_TITLE RGB(15, 23, 42)
#define COLOR_SUBTITLE RGB(100, 116, 139)
#define COLOR_FOOTER_BG RGB(241, 245, 249)
#define COLOR_FOOTER_TEXT RGB(71, 85, 105)
#define COLOR_ACCENT RGB(168, 85, 247)

// Node visualization
#define NODE_RADIUS 28
#define LEVEL_HEIGHT 90
#define MIN_HORIZONTAL_GAP 20

// Timing
#define HIGHLIGHT_DURATION 1000

#endif // COMMON_H