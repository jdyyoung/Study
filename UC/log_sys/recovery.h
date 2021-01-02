/* Copyright � 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */

#ifndef _RECOVERY_H_
#define _RECOVERY_H_

enum {
    ATTENTION,   // just letting you know
    COMPLAINT,   // could be a problem
    SERIOUS,     // serious problem (but trying to continue)
    RESTART_APP, // restarting the application should fix the problem
    RESTART_OS,  // restarting the OS should fix the problem
    RELOAD_OS,   // really bad news!!!
    DEBUGGING
};


#define DBUG(level, comment...)\
	do {if ((level) < system_debug_level) system_debug(level, ##comment);} while (0)	
#define ALERT(error, comment...) \
    system_recovery(ATTENTION, __FUNCTION__, __LINE__, error, ##comment)
#define WARN(error, comment...) \
    system_recovery(COMPLAINT, __FUNCTION__, __LINE__, error, ##comment)
#define ERROR(error, comment...) \
    system_recovery(SERIOUS, __FUNCTION__, __LINE__, error, ##comment)
#define RESTART(comment...) \
    system_recovery(RESTART_APP, __FUNCTION__, __LINE__, 0, ##comment)
#define REBOOT(error, comment...) \
    system_recovery(RESTART_OS, __FUNCTION__, __LINE__, error, ##comment)
#define REBOOT_OTHER_KERNEL(error, comment...) \
    system_recovery(RELOAD_OS, __FUNCTION__, __LINE__, error, ##comment)

extern char* signal_string(int signo);
extern void  watchdog_timeout(void);
extern void  killapp(int errcode);
extern int   init_signal_handlers(void (*callback)(int signo));
extern int   system_boot_other_kernel(u32 bootlimit);
extern int   system_recovery(int severity, const char* function, int lineno, 
                             int error, const char* comment, ...);

extern int   system_debug_level;
extern int   system_console_level;
extern bool  stdout_redirected;

extern int   system_debug(int level, const char* comment, ...);
extern int   system_debug_file(int level, const char* filename);
extern int   system_debug_level_update(int level);
extern int   system_console_level_update(int level);

#endif
