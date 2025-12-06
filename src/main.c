#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "lrc_parser.h"
#include "display.h"
#include "timer.h"
#include "player.h"

// === 教学提示：这里的全局指针在多个函数/线程间共享，方便大家专注流程，不用传很多参数 ===
static LrcContext *lrc_ctx = NULL;
static TimerContext *timer_ctx = NULL;
static volatile int g_should_exit = 0;

// 显示线程函数
void* display_thread_func(void *arg) {
    LrcContext *ctx = (LrcContext*)arg;
    
    while (!g_should_exit && !get_stop_flag(timer_ctx)) {
        long long ms = get_current_time(timer_ctx);
        
        display_window(ctx, ms);

        // 教学提示：超过最后一句时间 + 5s 自动收尾，演示“线程内自行判断退出条件”
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

    // 教学提示：初始化数据上下文，后续模块（解析、计时）都需要
    lrc_ctx = create_lrc_context();
    timer_ctx = create_timer_context();
    
    if (!lrc_ctx || !timer_ctx) {
        fprintf(stderr, "内存分配失败\n");
        goto cleanup;
    }

    // 教学提示：调用解析模块，失败要及时退出
    if (load_lrc(lrc_ctx, lrc) != 0 || lrc_ctx->line_count == 0) {
        fprintf(stderr, "载入歌词失败或内容为空。\n");
        goto cleanup;
    }

    printf("歌词共 %d 行。\n", lrc_ctx->line_count);
    printf("将使用 mplayer 播放音乐并同步滚动歌词。\n");
    printf("按回车开始播放...\n");
    getchar();

    // 教学提示：启动外部播放器并记录 pid，后面好“善后”
    pid_t mplayer_pid = start_mplayer_background(song);
    if (mplayer_pid <= 0) {
        fprintf(stderr, "启动 mplayer 失败。\n");
        goto cleanup;
    }

    // 教学提示：一个线程计时，一个线程显示，主线程做控制
    pthread_t tid_timer, tid_display;
    pthread_create(&tid_timer, NULL, timer_thread_func, timer_ctx);
    pthread_create(&tid_display, NULL, display_thread_func, lrc_ctx);

    // 教学提示：非阻塞 stdin，避免 getchar 卡住主循环
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags != -1) {
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }

    // 教学提示：主循环的两个退出条件——时间到或用户回车
    const long long end_ms = lrc_ctx->last_time_ms + 3000;
    while (!g_should_exit && !get_stop_flag(timer_ctx)) {
        long long now = get_current_time(timer_ctx);
        if (now > end_ms) {
            g_should_exit = 1;
            break;
        }

        int ch = getchar();
        if (ch == '\n') {
            g_should_exit = 1;
            break;
        }
        usleep(100 * 1000);
    }

    // 到时间或收到退出信号时，通知计时器线程退出
    set_stop_flag(timer_ctx, 1);
    g_should_exit = 1;

    // 停止音乐播放（演示“资源收尾”）
    stop_mplayer(mplayer_pid);

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
