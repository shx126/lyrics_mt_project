#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

// 常量定义
#define MAX_LINE_LEN   256
#define MAX_LRC_LINES  1024

// 工具函数声明
void trim_newline(char *s);
int utf8_bytes_for_chars(const char *s, int n_chars);
void clear_screen(void);

#endif