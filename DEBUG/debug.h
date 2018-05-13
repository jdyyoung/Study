#ifndef __DEBUG_H__
#define __DEBUG_H__

#define EL(x,y...)  {printf("[*** %s : %d] ",__FILE__,__LINE__); printf(x,##y); printf("\n");}

enum LOG_LEVEL{
L_ERROR   = 0,
L_WARN    = 1,
L_INFO    = 2,
L_DEBUG   = 3,
L_VERBOSE = 4,
};

extern int log_level;

#define LOGCOM(level,x,y...)		\
do{									\
	if(level <= log_level) printf(x,##y);  \
}while(0)

#define LOGE(x, y...)   LOGCOM( L_ERROR,  x, ##y)
#define LOGW(x, y...)   LOGCOM( L_WARN,   x, ##y)
#define LOGI(x, y...)   LOGCOM( L_INFO,   x, ##y)
#define LOGD(x, y...)   LOGCOM( L_DEBUG,  x, ##y)
#define LOGV(x, y...)   LOGCOM( L_VERBOSE,x, ##y)

#define LOG(x, y...)    LOGI(x, ##y)

#ifdef DEBUG
#define CONFIG_DEBUG_TIME
#define FUNC()          LOGI("[%s:%d] %s\n",__FILE__,__LINE__,__func__);
#define FUNS(tag...)    LOGI("[%s:%d] %s : %s\n",__FILE__,__LINE__,__func__, tag);
#else
#define FUNC()		    do{} while (0);
#define FUNS(tag...)	do{} while (0);
#endif


#undef CONFIG_DEBUG_TIME
#ifdef CONFIG_DEBUG_TIME

int show_time(const char* tag);
void show_time_close(void);

#define SHOW_TIME(tag...) 	\
do{			\
	show_time(tag);	\
}while(0)
#else /* CONFIG_DEBUG_TIME */
#define SHOW_TIME(tag...)	do{} while (0)

#endif /* CONFIG_DEBUG_TIME */

#endif /* __DEBUG_H__ */
