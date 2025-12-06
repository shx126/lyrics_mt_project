#include "player.h"
#include <stdio.h>
#include <stdlib.h>

// 使用 system 在后台播放音乐
void start_mplayer_background(const char *song) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "mplayer \"%s\" >/dev/null 2>&1 &", song);
    system(cmd);
}