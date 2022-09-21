#ifndef _LOG_H_
#define _LOG_H_

#include "recovery.h"
#include "lib.h"
#include "typedef.h"

// #define LOG_SAVE_DIR "/tmp"

#ifndef LOG_PATHNAME
#define LOG_PATHNAME "logs"
#endif

#ifndef MAX_PATHNAME
#define MAX_PATHNAME     512
#endif

#define LOG_VERSION          "log-config-000"
#define LOG_MAXFILES         1000
#define LOG_DEFAULT_LEVEL    0
#define LOG_DEFAULT_MASK     0xFFFFFFFF
#define LOG_DEFAULT_DIRFLAGS 0777

#define LOG_ANY_MASK         0xFFFFFFFF
#define LOG_MAX_LEVEL        100000000

// #define SKYBELL_MISC "/tmp/misc"
#define PROG_VERSION "2.2.16"
// #define MAX_PATHNAME     512


typedef struct {
    char version[16];

    // wrap to next file after log_printf()/log_data() exceed this size 
    // (NOTE: no imposed size limit)
    size_t bytes_per_file;

    // log files are "000" to "nnn" where "nnn" is max_files-1
    // "000" is the oldest file, "nnn" the most recent --
    // max_files is truncated to LOG_MAXFILE
    int max_files;

    // if true add ASCII timestamp to log_printf()s (*NOT* log_data())
    bool timestamp;

    // 32-bit mask, log_printf()/log_data() must have overlapping 
    // "mask" bit set to log (defaults to LOG_DEFAULT_MASK)
    unsigned long mask;

    // log_printf()/log_data() level must be >= this level to log --
    // (defaults to LOG_DEFAULT_LEVEL) -- so level 0 here will 
    // display everything, the higher the level in log_printf()/log_data
    // the more important the data
    int level;
} LOG_CONFIG;


// extern void* system_log;
extern void* debug_log;


// general use:
//   0. log_init("<skybell-root-pathname>");
//   1. void* context = log_open("<log-dir-name>");
//   2. if (context == NULL)  
//          context = log_create("<log-dir-name>", nbytes, nfiles, true);
//   3. if (context) {
//           LOG_CONFIG config; 
//           log_getconfig(context, &config); 
//           // change level, mask, etc. 
//           log_setconfig(context, &config);
//      }
//   4. log_printf(context, 0, LOG_DEFAULT_MASK, 
//                 "hello there this is %s %s\n", "greg", "today"); ***OR***
//      log_data(context, 0, LOG_DEFAULT_MASK, data, nbytes);
//   5. log_close(context);  // when finished logging for now
//   6. log_delete("<log-dir-name>"); // delete log and associated data
//

extern void  __log_vprintf(const void* log, int level, unsigned long mask, 
                         const char* msg, va_list ap);
extern void  log_vprintf(const void* log, int level, unsigned long mask, 
                         const char* msg, va_list ap);
extern void  log_printf(const void* log, int level, unsigned long mask, 
                        const char* msg, ...);
extern void  log_data(const void* log, int level, unsigned long mask, 
                      const void* data, size_t nbytes);
extern int   log_getconfig(const void* log, LOG_CONFIG* config);
extern int   log_setconfig(const void* log, LOG_CONFIG* config);
extern void* log_open(const char* logname);
extern int   log_close(void* log);
extern int   log_create(const char* logname, 
                        size_t bytes_per_file, int max_files, bool timestamp);
extern int   log_delete(const char* logname);
extern int   log_archive(const char* archive_name);
// extern int   log_zap(void);
extern void* log_restart(char* logname, size_t bytes_per_file, 
                         int max_files, bool timestamp);
// extern void  log_init(char* rootpath, void (*restore)(void));
extern void  log_init(const char* rootpath,LOG_DEVICE_INFO *info_param);
#endif
