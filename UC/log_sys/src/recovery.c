#include "lib.h"

#define LOG_TO_SYSLOG 0
#define PRINT_BUF_MAX_SIZE 1400


int system_log_level = XLOG_INFO;
int system_console_level = XLOG_INFO;

void log_init_ext(const char* path,LOG_DEVICE_INFO *info_param){
	log_init(path,info_param);

}

static void console_output(char* fmt, ...)
{
    va_list vargs;
    struct tm t;
    struct timeval tv;
    time_t now = time(NULL);

    localtime_r(&now, &t);
    gettimeofday(&tv, NULL);
    printf("%02d:%02d:%02d[.%lu] ", 
            t.tm_hour, t.tm_min, t.tm_sec,  tv.tv_usec);
    va_start(vargs, fmt); vprintf(fmt, vargs); va_end(vargs); fflush(stdout); 
}


static void log_output(char* fmt, ...)
{
    va_list vargs;

    // log everything to DBUG log
    va_start(vargs, fmt); __log_vprintf(debug_log, LOG_MAX_LEVEL, LOG_ANY_MASK, fmt, vargs); va_end(vargs);
    // if (strncmp(fmt, "DBUG", 4) != 0) {
    //     // log everything that does not begin with "DBUG" (e.g., "WARNING", "PROBLEM", "RESTART", "RELOAD")
    //     va_start(vargs, fmt); __log_vprintf(system_log, LOG_MAX_LEVEL, LOG_ANY_MASK, fmt, vargs); va_end(vargs);
    // }
    #if LOG_TO_SYSLOG
    va_start(vargs, fmt); vsyslog(LOG_EMERG, fmt, vargs); va_end(vargs); 
    #endif
}


static char* severity_string(int severity)
{
    static char unknown[8];

    switch (severity) {
    case XLOG_ERROR:     return "ERROR";
    case XLOG_WARN:      return "WARN";
    case XLOG_INFO:      return "INFO";
    case XLOG_DEBUG:     return "DEBUG";
    case XLOG_VERBOSE:   return "VERBOSE";
    default:
        snprintf(unknown, sizeof(unknown), "ERR%d", severity);
        return unknown;
    }
}


int system_recovery(int severity, const char* function, int lineno, int error, 
                    const char* comment, ...)
{
    va_list vargs;
    char buf[PRINT_BUF_MAX_SIZE];
    char* trim_buf;
    int len;

    errno = error;
    va_start(vargs, comment);
    if (error && severity < XLOG_DEBUG) {
        snprintf(buf, sizeof(buf), "%s(){E-%d'%m'}.%d: ", function, error, lineno); 
    } else {
        snprintf(buf, sizeof(buf), "%s().%d: ", function, lineno); 
    }
    len = strlen(buf);
    vsnprintf(buf+len, sizeof(buf)-len, comment, vargs);
    va_end(vargs);
    trim_buf = trim_eol(buf);

    if (severity <= system_console_level && !(severity >= XLOG_DEBUG &&  error == 0) ) {
        console_output("%s-%s\n", severity_string(severity), trim_buf); 
    }
    if(severity <= system_log_level  && !(severity >= XLOG_DEBUG &&  error == 0) ){
        log_output("%s-%s\n", severity_string(severity), trim_buf); 
    }
    
    sync();

    return 0;
}

int system_log_level_update(int level)
{
    int old = system_log_level;
    system_log_level = level;
    return old;
}


int system_console_level_update(int level)
{
    int old = system_console_level;
    system_console_level = level;
    return old;
}

