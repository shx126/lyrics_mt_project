#include "lrc_parser.h"

// 解析 [mm:ss.xx] → 毫秒
int parse_time_ms(const char *str) {
    int m = 0, s = 0, x = 0;
    int n = sscanf(str, "%d:%d.%d", &m, &s, &x);
    int ms = (m * 60 + s) * 1000;
    if (n == 3) {
        if (x < 100) ms += x * 10;
        else ms += x;
    }
    return ms;
}

// 解析一行 LRC，如 "[01:23.45]这一刻"
int parse_lrc_line(const char *line, int *time_ms, char *text_out) {
    const char *L = strchr(line, '[');
    const char *R = strchr(line, ']');
    if (!L || !R || R <= L + 1) return 0;

    char tbuf[32] = {0};
    int len = R - L - 1;
    if (len <= 0 || len >= (int)sizeof(tbuf)) return 0;
    strncpy(tbuf, L + 1, len);
    tbuf[len] = '\0';

    *time_ms = parse_time_ms(tbuf);
    strcpy(text_out, R + 1);
    trim_newline(text_out);

    if (text_out[0] == '\0') return 0;   // 没有文本就忽略
    return 1;
}

// 按时间排序插入（简单插入排序）
void insert_sorted(LrcContext *ctx, int t, const char *text) {
    int i = ctx->line_count;
    while (i > 0 && t < ctx->lines[i - 1].time_ms) {
        ctx->lines[i] = ctx->lines[i - 1];
        i--;
    }
    ctx->lines[i].time_ms = t;
    strncpy(ctx->lines[i].text, text, MAX_LINE_LEN - 1);
    ctx->lines[i].text[MAX_LINE_LEN - 1] = '\0';
    ctx->line_count++;
}

// 载入 LRC 文件
int load_lrc(LrcContext *ctx, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        perror("打开 LRC 失败");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int t;
    char text[MAX_LINE_LEN];

    ctx->line_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (parse_lrc_line(line, &t, text)) {
            if (ctx->line_count >= MAX_LRC_LINES) break;
            insert_sorted(ctx, t, text);
        }
    }
    fclose(fp);

    if (ctx->line_count > 0)
        ctx->last_time_ms = ctx->lines[ctx->line_count - 1].time_ms;
    else
        ctx->last_time_ms = 0;

    return 0;
}

// 创建歌词上下文
LrcContext* create_lrc_context(void) {
    LrcContext *ctx = (LrcContext*)malloc(sizeof(LrcContext));
    if (ctx) {
        ctx->line_count = 0;
        ctx->last_time_ms = 0;
    }
    return ctx;
}

// 释放歌词上下文
void free_lrc_context(LrcContext *ctx) {
    free(ctx);
}