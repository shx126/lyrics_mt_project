#ifndef PLAYER_H
#define PLAYER_H

// 播放器相关函数声明
#include <sys/types.h>
pid_t start_mplayer_background(const char *song);
void stop_mplayer(pid_t pid);

#endif
