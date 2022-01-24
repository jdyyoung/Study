#ifndef _RECOVERY_H_
#define _RECOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_SAVE_DIR "/tmp"

enum {
    ATTENTION,   // just letting you know
    COMPLAINT,   // could be a problem
    SERIOUS,     // serious problem (but trying to continue)
    RESTART_APP, // restarting the application should fix the problem
    RESTART_OS,  // restarting the OS should fix the problem
    RELOAD_OS,   // really bad news!!!
    DEBUGGING
};

//debug mask
#define SKB_SERIAL				(1 << 0)	/*0x1 */
#define SKB_MOTION			    (1 << 1)	/*0x2 */
#define SKB_STA				    (1 << 2)	/*0x4 */
#define SKB_AP				    (1 << 3)	/*0x8 */
#define SKB_VEDIO			    (1 << 4)	/*0x10*/
#define SKB_AUDIO				(1 << 5)	/*0x20*/
#define SKB_CFG			        (1 << 6)	/*0x40*/
#define SKB_BTN			        (1 << 7)	/*0x80*/
#define SKB_OTA			        (1 << 8)	/*0x100*/
#define SKB_WIFI				(1 << 9)	/*0x200*/
#define SKB_JPEG			    (1 << 10)	/*0x400*/
#define SKB_OTHER			    (1 << 11)	/*0x800*/

#define DBUG(level, comment...)\
	system_debug(level, __FUNCTION__, __LINE__,##comment)	
#define ALERT(error, comment...) \
    system_recovery(ATTENTION, __FUNCTION__, __LINE__, error, ##comment)
#define WARN(error, comment...) \
    system_recovery(COMPLAINT, __FUNCTION__, __LINE__, error, ##comment)
#define ERROR(error, comment...) \
    system_recovery(SERIOUS, __FUNCTION__, __LINE__, error, ##comment)

#if 0
#define RESTART(comment...) \
    system_recovery(RESTART_APP, __FUNCTION__, __LINE__, 0, ##comment)
#define REBOOT(error, comment...) \
    system_recovery(RESTART_OS, __FUNCTION__, __LINE__, error, ##comment)
#define REBOOT_OTHER_KERNEL(error, comment...) \
    system_recovery(RELOAD_OS, __FUNCTION__, __LINE__, error, ##comment)

// extern char* signal_string(int signo);
// extern void  watchdog_timeout(void);
// extern void  killapp(int errcode);
// extern int   init_signal_handlers(void (*callback)(int signo));
// extern int   system_boot_other_kernel(u32 bootlimit);
#endif

extern int   system_recovery(int severity, const char* function, int lineno, 
                             int error, const char* comment, ...);

// extern int   system_debug_level;
// extern int   system_console_level;
// extern bool  stdout_redirected;

extern int system_debug(int level, const char* function, int lineno,const char* comment, ...);
// extern int   system_debug_file(int level, const char* filename);
extern int   system_debug_level_update(int level);
extern int   system_console_level_update(int level);

extern void log_init_ext(const char* path);

#ifdef __cplusplus
}
#endif

#endif
