#ifndef DISPLAY_H
#define DISPLAY_H

#include "lrc_parser.h"

// 显示相关函数声明
void print_line_with_highlight_utf8(const char *text, int highlight_chars);
void display_window(const LrcContext *ctx, long long now_ms);

#endif