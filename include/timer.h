#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>

// 计时器上下文结构
typedef struct {
    long long current_ms;      // 当前时间（毫秒）
    int stop_flag;             // 停止标志
    pthread_mutex_t mutex;     // 互斥锁
} TimerContext;

// 函数声明
TimerContext* create_timer_context(void);
void free_timer_context(TimerContext *ctx);
long long get_current_time(TimerContext *ctx);
void set_stop_flag(TimerContext *ctx, int flag);
int get_stop_flag(TimerContext *ctx);
void* timer_thread_func(void *arg);

#endif