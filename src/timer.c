#include "timer.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

// 创建计时器上下文：演示“结构体+初始化+互斥锁”
TimerContext* create_timer_context(void) {
    TimerContext *ctx = (TimerContext*)malloc(sizeof(TimerContext));
    if (ctx) {
        ctx->current_ms = 0;
        ctx->stop_flag = 0;
        pthread_mutex_init(&ctx->mutex, NULL);
    }
    return ctx;
}

// 释放计时器上下文：对应 create，教学强调“成对释放”和销毁锁
void free_timer_context(TimerContext *ctx) {
    if (ctx) {
        pthread_mutex_destroy(&ctx->mutex);
        free(ctx);
    }
}

// 获取当前时间：演示“加锁读共享变量”
long long get_current_time(TimerContext *ctx) {
    long long ms;
    pthread_mutex_lock(&ctx->mutex);
    ms = ctx->current_ms;
    pthread_mutex_unlock(&ctx->mutex);
    return ms;
}

// 设置停止标志：线程间通信用布尔标志
void set_stop_flag(TimerContext *ctx, int flag) {
    pthread_mutex_lock(&ctx->mutex);
    ctx->stop_flag = flag;
    pthread_mutex_unlock(&ctx->mutex);
}

// 获取停止标志
int get_stop_flag(TimerContext *ctx) {
    int flag;
    pthread_mutex_lock(&ctx->mutex);
    flag = ctx->stop_flag;
    pthread_mutex_unlock(&ctx->mutex);
    return flag;
}

// 计时线程函数：教学重点“线程里循环 + 睡眠 + 更新时间 + 检查退出条件”
void* timer_thread_func(void *arg) {
    TimerContext *ctx = (TimerContext*)arg;
    struct timeval start, now;
    gettimeofday(&start, NULL);

    while (!get_stop_flag(ctx)) {
        usleep(50 * 1000); // 50ms

        gettimeofday(&now, NULL);
        long long ms =
            (now.tv_sec - start.tv_sec) * 1000LL +
            (now.tv_usec - start.tv_usec) / 1000LL;

        pthread_mutex_lock(&ctx->mutex);
        ctx->current_ms = ms;
        pthread_mutex_unlock(&ctx->mutex);
    }
    return NULL;
}
