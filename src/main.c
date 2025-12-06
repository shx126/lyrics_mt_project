#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "utils.h"
#include "lrc_parser.h"
#include "display.h"
#include "timer.h"
#include "player.h"

// 全局变量（上下文）
static LrcContext *lrc_ctx = NULL;
static TimerContext *timer_ctx = NULL;
static volatile int g_should_exit = 0;

// 信号处理函数
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n接收到 Ctrl+C，正在退出...\n");
        g_should_exit = 1;
        if (timer_ctx) {
            set_stop_flag(timer_ctx, 1);
        }
    }
}

// 显示线程函数
void* display_thread_func(void *arg) {
    LrcContext *ctx = (LrcContext*)arg;
    
    while (!g_should_exit) {
        long long ms = get_current_time(timer_ctx);
        
        display_window(ctx, ms);

        // 超过最后一行一段时间后自动退出
        if (ms > ctx->last_time_ms + 5000) {
            set_stop_flag(timer_ctx, 1);
            g_should_exit = 1;
            break;
        }

        usleep(100 * 1000); // 100ms 刷新
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("用法: %s song.mp3 lyrics.lrc\n", argv[0]);
        return 1;
    }

    const char *song = argv[1];
    const char *lrc  = argv[2];

    // 设置信号处理
    signal(SIGINT, signal_handler);

    // 初始化上下文
    lrc_ctx = create_lrc_context();
    timer_ctx = create_timer_context();
    
    if (!lrc_ctx || !timer_ctx) {
        fprintf(stderr, "内存分配失败\n");
        goto cleanup;
    }

    // 加载歌词
    if (load_lrc(lrc_ctx, lrc) != 0 || lrc_ctx->line_count == 0) {
        fprintf(stderr, "载入歌词失败或内容为空。\n");
        goto cleanup;
    }

    printf("歌词共 %d 行。\n", lrc_ctx->line_count);
    printf("将使用 mplayer 播放音乐并同步滚动歌词。\n");
    printf("按回车开始播放...\n");
    getchar();

    // 启动音乐
    start_mplayer_background(song);

    // 启动线程
    pthread_t tid_timer, tid_display;
    pthread_create(&tid_timer, NULL, timer_thread_func, timer_ctx);
    pthread_create(&tid_display, NULL, display_thread_func, lrc_ctx);

    // 等待线程结束
    pthread_join(tid_timer, NULL);
    pthread_join(tid_display, NULL);

    printf("播放结束。\n");

cleanup:
    // 清理资源
    if (lrc_ctx) free_lrc_context(lrc_ctx);
    if (timer_ctx) free_timer_context(timer_ctx);
    
    return 0;
}