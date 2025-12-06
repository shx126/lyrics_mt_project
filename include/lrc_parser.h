#ifndef LRC_PARSER_H
#define LRC_PARSER_H

#include "utils.h"

// 歌词行结构体
typedef struct {
    int time_ms;                // 此行开始时间（毫秒）
    char text[MAX_LINE_LEN];    // 歌词内容
} LrcLine;

// 歌词数据上下文
typedef struct {
    LrcLine lines[MAX_LRC_LINES];
    int line_count;
    int last_time_ms;
} LrcContext;

// 函数声明
int parse_time_ms(const char *str);
int parse_lrc_line(const char *line, int *time_ms, char *text_out);
void insert_sorted(LrcContext *ctx, int t, const char *text);
int load_lrc(LrcContext *ctx, const char *file);
LrcContext* create_lrc_context(void);
void free_lrc_context(LrcContext *ctx);

#endif