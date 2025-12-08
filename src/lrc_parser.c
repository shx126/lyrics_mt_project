#include "lrc_parser.h"

// 教学提示：解析 [mm:ss.xxx] → 毫秒，演示“字符串拆分 + atoi”
// 额外处理：毫秒超过 3 位则截断，避免异常时间戳
int parse_time_ms(const char *str) {
    const char *colon = strchr(str, ':');
    if (!colon || colon == str) return 0;

    // 提取分
    char min_buf[8] = {0};
    size_t min_len = (size_t)(colon - str);
    if (min_len >= sizeof(min_buf)) return 0;
    strncpy(min_buf, str, min_len);

    // 提取秒和小数部分
    const char *sec_start = colon + 1;
    const char *dot = strchr(sec_start, '.');
    char sec_buf[8] = {0};
    size_t sec_len = dot ? (size_t)(dot - sec_start) : strlen(sec_start);
    if (sec_len == 0 || sec_len >= sizeof(sec_buf)) return 0;
    strncpy(sec_buf, sec_start, sec_len);

    int m = atoi(min_buf);
    int s = atoi(sec_buf);
    int ms = (m * 60 + s) * 1000;

    // 处理小数部分（最多 3 位毫秒）
    if (dot && *(dot + 1) != '\0') {
        char frac_buf[4] = {0};              // 只保留前三位
        size_t frac_len = strlen(dot + 1);
        if (frac_len > 3) frac_len = 3;
        strncpy(frac_buf, dot + 1, frac_len);

        int frac = atoi(frac_buf);
        if (frac_len == 1) frac *= 100;      // 0.x → x * 100
        else if (frac_len == 2) frac *= 10;  // 0.xx → xx * 10
        ms += frac;                          // 0.xxx → 直接作为毫秒
    }

    return ms;
}

// 教学提示：解析一行 LRC，如 "[01:23.45]这一刻"
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

// 按时间排序插入（简单插入排序）：教学说明“边读边排”
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

// 载入 LRC 文件：教学说明“文件循环读取 + 调用解析 + 存入上下文”
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

// 创建/释放歌词上下文：演示“malloc + 默认值”
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
