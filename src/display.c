#include "display.h"
#include "utils.h"
#include <stdio.h>

// 教学提示：逐字高亮打印一行，演示“先数字符，再切片输出”
void print_line_with_highlight_utf8(const char *text, int highlight_chars) {
    int total_bytes = strlen(text);

    // 1. 先算整行有多少个"字符"
    int char_count = 0;
    for (int i = 0; i < total_bytes; ) {
        unsigned char c = (unsigned char)text[i];
        int char_len = 1;
        if      ((c & 0x80) == 0x00) char_len = 1;
        else if ((c & 0xE0) == 0xC0) char_len = 2;
        else if ((c & 0xF0) == 0xE0) char_len = 3;
        else if ((c & 0xF8) == 0xF0) char_len = 4;

        i += char_len;
        char_count++;
    }

    if (highlight_chars < 0) highlight_chars = 0;
    if (highlight_chars > char_count) highlight_chars = char_count;

    // 2. 计算前 highlight_chars 个"字符"所对应的字节数
    int highlight_bytes = utf8_bytes_for_chars(text, highlight_chars);

    printf("\033[1;32m");
    fwrite(text, 1, highlight_bytes, stdout);   // 高亮部分按字节输出
    printf("\033[0m");

    // 3. 剩下的部分原样输出
    fwrite(text + highlight_bytes, 1, total_bytes - highlight_bytes, stdout);
}

// 显示歌词窗口（包含上一行、当前行、下一行…）
void display_window(const LrcContext *ctx, long long now_ms) {
    if (ctx->line_count == 0) return;

    // 教学提示：找到当前行 index（线性扫描够用，便于学生理解）
    int idx = 0;
    while (idx + 1 < ctx->line_count &&
           now_ms >= ctx->lines[idx + 1].time_ms) {
        idx++;
    }

    // 当前行起止时间
    int line_start = ctx->lines[idx].time_ms;
    int line_end;
    if (idx + 1 < ctx->line_count)
        line_end = ctx->lines[idx + 1].time_ms;
    else
        line_end = line_start + 4000; // 最后一行默认持续 4 秒

    int line_len = strlen(ctx->lines[idx].text);
    int highlight_chars = 0;

    if (now_ms <= line_start) {
        highlight_chars = 0;
    } else if (now_ms >= line_end) {
        highlight_chars = line_len;
    } else {
        double ratio = (double)(now_ms - line_start) /
                       (double)(line_end - line_start);
        highlight_chars = (int)(ratio * line_len + 0.5);
    }

    clear_screen(); // 教学提示：用 ANSI 转义清屏，模拟“刷新”效果

    int win = 9;
    int half = win / 2;
    int start = idx - half;
    if (start < 0) start = 0;
    int end = start + win - 1;
    if (end >= ctx->line_count) end = ctx->line_count - 1;

    for (int i = start; i <= end; ++i) {
        if (i == idx) {
            printf("> ");
            print_line_with_highlight_utf8(ctx->lines[i].text, highlight_chars);
            printf("\n");
        } else {
            printf("  %s\n", ctx->lines[i].text);
        }
    }

    printf("\n[时间] %.2f 秒（回车退出）\n", now_ms / 1000.0);
}
