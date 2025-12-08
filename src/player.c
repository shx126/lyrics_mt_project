#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 教学提示：通过 shell 启动 mplayer，拿到子进程 pid，后续好停止播放
pid_t start_mplayer_background(const char *song) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "mplayer \"%s\" >/dev/null 2>&1 & echo $!", song);

    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;

    char buf[64] = {0};
    if (!fgets(buf, sizeof(buf), fp)) {
        pclose(fp);
        return -1;
    }

    pclose(fp);
    return (pid_t)atoi(buf);
}

void stop_mplayer(pid_t pid) {
    if (pid > 0) {
        kill(pid, SIGTERM);
    }
}
