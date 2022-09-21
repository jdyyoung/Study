#ifndef _RECOVERY_H_
#define _RECOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_SAVE_DIR "/tmp"

/**
 * @brief Log level
 *
 */
enum {
    XLOG_NONE,       /*!< No log output */
    XLOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    XLOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    XLOG_INFO,       /*!< Information messages which describe normal flow of events */
    XLOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    XLOG_VERBOSE,     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
};

typedef struct{
    unsigned int fw_ver;
    unsigned int hw_ver;
    unsigned int mcu_ver;
    char device_sn[32];
    char device_mac[32];
    unsigned int bytes_per_file;
    int max_files;
}LOG_DEVICE_INFO;

#define DBUG(level, comment...)\
    system_recovery(XLOG_DEBUG, __FUNCTION__, __LINE__, level, ##comment)
#define ALERT(error, comment...) \
    system_recovery(XLOG_INFO, __FUNCTION__, __LINE__, error, ##comment)
#define WARN(error, comment...) \
    system_recovery(XLOG_WARN, __FUNCTION__, __LINE__, error, ##comment)
#define ERROR(error, comment...) \
    system_recovery(XLOG_ERROR, __FUNCTION__, __LINE__, error, ##comment)

extern int   system_recovery(int severity, const char* function, int lineno, 
                             int error, const char* comment, ...);

extern int   system_log_level_update(int level);
extern int   system_console_level_update(int level);
extern void log_init_ext(const char* path,LOG_DEVICE_INFO *info_param);

#ifdef __cplusplus
}
#endif

#endif
