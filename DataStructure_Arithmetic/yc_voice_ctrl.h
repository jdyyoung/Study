#ifndef _yc_voice_ctrl_h_
#define _yc_voice_ctrl_h_

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#include "ak_common.h"
#include "ak_global.h"
#include "ak_ao.h"
#include "ak_adec.h"
#include "ak_demux.h"
#include "ak_ai.h"

typedef void (*FUNC)();

//初始化声音播放
int init_voice_ctrl();
//关闭声音播放
int exit_voice_ctrl(void);

//播放AI整段音频
int voice_add_play_ai(const char *file_name, FUNC cb);
//播放整段音频
int voice_add_play(const char *file_name);
//播放有间隔音频
int voice_add_play_range(const char *file_name, unsigned long begin_ms, unsigned long end_ms);
//停止声音播放
void voice_stop_play();
//设置系统音量
void voice_set_volume(int volume);
//声音+
void voice_up();
//声音减
void voice_down();
//声音暂停
void voice_pause();

int voice_is_playing();
#endif
