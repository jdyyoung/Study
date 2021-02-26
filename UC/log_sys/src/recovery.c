#include "lib.h"
// #include <signal.h>
// #include <ucontext.h>
// #include <linux/reboot.h>
// #include <sys/mount.h>

#define LOG_TO_SYSLOG 0
#define PRINT_BUF_MAX_SIZE 1400

// extern void reboot(int);

// static void (*sync_system)(int signo) = NULL;

// static struct termios console_termios;
// static bool           console_termios_saved = FALSE;

int system_debug_level = 1;
int system_console_level = 1;

bool stdout_redirected = FALSE;

// bool development = FALSE;

void log_init_ext(const char* path){
	log_init(path);

}
static void console_output(char* fmt, ...)
{
    if (!stdout_redirected) {
        va_list vargs;
        struct tm t;
        struct timeval tv;
        time_t now = time(NULL);

        localtime_r(&now, &t);
        gettimeofday(&tv, NULL);
        printf("%02d:%02d:%02d[%lu.%lu] ", 
                t.tm_hour, t.tm_min, t.tm_sec, tv.tv_sec, tv.tv_usec);
        va_start(vargs, fmt); vprintf(fmt, vargs); va_end(vargs); fflush(stdout); 
    }
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
    case ATTENTION:   return "ALERT";
    case COMPLAINT:   return "ISSUE";
    case SERIOUS:     return "*BAD*";
    case RESTART_APP: return "RESTART_APP";
    case RESTART_OS:  return "RESTART_OS";
    case RELOAD_OS:   return "RELOAD_OS";
    default:
        snprintf(unknown, sizeof(unknown), "ERR%d", severity);
        return unknown;
    }
}

#if 0
char* signal_string(int signo)
{
    return strsignal(signo);
}


void killapp(int errcode)
{
    console_output("%s(): sending kill(*, SIGKILL) followed by exit()!\n", __FUNCTION__);
    log_output("%s(): sending kill(*, SIGKILL) followed by exit()!\n", __FUNCTION__);

    sync();
    if (sync_system){
        sync_system(0);
    }

    // do_system("kill $(pidof udhcpc)");
    // do_system("kill $(pidof wpa_supplicant)");
    // kill(watchdog_getpid(), SIGKILL);

    // if (console_termios_saved) {
    //     console_termios_saved = FALSE;
    //     tcsetattr(0, TCSANOW, &console_termios);
    // }

    // the special case of 0x66 causes skybell_init to invoke the shell instead
    // of re-starting the app -- this is for development purposes (i.e., 
    // supports "exit" ui.c command
    // 
    if (errcode != 0x66) {
        kill(0, SIGKILL);
    }

    exit(errcode);
}


static void signal_handler(int signum, siginfo_t* info, void* ucptr) 
{
    // extern void ui_restore_console(void);
    char buf[64];

    console_output("CAUGHT SIGNAL '%s'!!! (si_signo=%d)\n", signal_string(signum), signum);
    log_output("CAUGHT SIGNAL '%s'!!! (si_signo=%d)\n", signal_string(signum), signum);

    if (info) {
        log_output("si_errno=%d, si_code=%d, si_addr=%p\n", 
                   info->si_errno, info->si_code, info->si_addr);
    }
    // ui_restore_console();


    snprintf(buf, sizeof(buf), "SIGNAL '%s'\n", signal_string(signum));
    add_to_file(SKYBELL_RESTART, buf, strlen(buf), TRUE); 
    killapp(2);
}


int init_signal_handlers(void (*callback)(int signo))
{
    struct sigaction action;

    if (!console_termios_saved)
        console_termios_saved = tcgetattr(0, &console_termios) == 0;

    memset(&action, 0, sizeof(action));
    action.sa_sigaction = signal_handler;
    action.sa_flags = SA_SIGINFO;

    sync_system = callback;

    // following signals do not default to dumping core,
    // take control of these signals in production or
    // development mode...
    sigaction(SIGHUP,  &action, NULL);
    sigaction(SIGINT,  &action, NULL);
    sigaction(SIGALRM, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    sigaction(SIGCONT, &action, NULL);
    sigaction(SIGTSTP, &action, NULL);
    sigaction(SIGTTOU, &action, NULL);
    sigaction(SIGPOLL, &action, NULL);
    sigaction(SIGPROF, &action, NULL);
    sigaction(SIGURG,  &action, NULL);
    sigaction(SIGIO,   &action, NULL);
    sigaction(SIGPWR,  &action, NULL);
    //AFR
    //sigaction(SIGSEGV, &action, NULL);

    // following signals dump core by default, if in development
    // mode, allow the core file to be dumped!
    //
    #if 0
    if (!development) {
        sigaction(SIGQUIT, &action, NULL);
        sigaction(SIGILL,  &action, NULL);
        sigaction(SIGABRT, &action, NULL);
        sigaction(SIGFPE,  &action, NULL);
        sigaction(SIGSEGV, &action, NULL);
        sigaction(SIGBUS,  &action, NULL);
        sigaction(SIGSYS,  &action, NULL);
        sigaction(SIGTRAP, &action, NULL);
        sigaction(SIGIOT,  &action, NULL);
    }
    #endif

    // following are signals to IGNORE
    action.sa_handler = SIG_IGN;        // ASSUMES sa_handler/sa_sigaction UNION
    sigaction(SIGPIPE, &action, NULL);  // to recovery from telnet disconnect
    sigaction(SIGIO,   &action, NULL);  // to recovery from telnet disconnect

    return 0;
}
#endif

int system_recovery(int severity, const char* function, int lineno, int error, 
                    const char* comment, ...)
{
    va_list vargs;
    char buf[PRINT_BUF_MAX_SIZE];
    char* trim_buf;
    int len;

    errno = error;
    va_start(vargs, comment);
    if (error) {
        snprintf(buf, sizeof(buf), "%s(){E-%d'%m'}.%d: ", function, error, lineno); 
    } else {
        snprintf(buf, sizeof(buf), "%s().%d: ", function, lineno); 
    }
    len = strlen(buf);
    vsnprintf(buf+len, sizeof(buf)-len, comment, vargs);
    va_end(vargs);
    trim_buf = trim_eol(buf);

    if (system_console_level > 0) {
        console_output("%s-%s\n", severity_string(severity), trim_buf); 
    }
    log_output("%s-%s\n", severity_string(severity), trim_buf); 
    sync();

    // switch (severity) {
    // case ATTENTION:
    // case COMPLAINT:
    // case SERIOUS:
    //     break;

    // case RESTART_APP:
    //     // note: this code assumes /etc/inittab::respawn is set to skybell so
    //     // by killing skybell the 'init' process should restart it...
    //     // Or, which is more likely skybell_init is running and it will restart skybell
    //     add_to_file(SKYBELL_RESTART, buf, strlen(buf), TRUE); 
    //     killapp(2);
    //     break;

    // case RELOAD_OS:
    //     // change the uBoot environment variables to boot the other kernel/rootfs
    //     // and return control to uBoot
    //     //ubootenv_toggle_bootcmd(5);

    //     // fall through to reboot the OS (in uBoot)
    // case RESTART_OS:
    //     add_to_file(SKYBELL_REBOOT, buf, strlen(buf), TRUE); 
    //     if (sync_system) 
    //         sync_system(0);
    //     //led_redalert(5); 2016/11/29 -- v1115 led sub-system uses mutexes that may be locked 
    //     printf(">>>system_recovery RESET OS   start\n"); //paul 20200312
    //     printf(">>>%s\n",buf); //paul 20200312
    //     printf(">>>system_recovery RESET OS   end\n"); //paul 20200312
        
    //     reboot(LINUX_REBOOT_CMD_RESTART);//paul 20200312
    //     break;
    // }

    return 0;
}


int system_debug(int level, const char* comment, ...)
{
    if (level <= system_debug_level) {
        va_list vargs;
        char buf[PRINT_BUF_MAX_SIZE];
        char* trim_buf;

        va_start(vargs, comment);
        vsnprintf(buf, sizeof(buf), comment, vargs);
        va_end(vargs);

        trim_buf = trim_eol(buf);
        if (system_console_level > 0 ) {
            console_output("DBUG%d: [%s]\n", level, trim_buf); 
        }
        log_output("DBUG%d: [%s]\n", level, trim_buf);
    }

    return 0;
}


int system_debug_level_update(int level)
{
    int old = system_debug_level;
    system_debug_level = level;
    return old;
}


int system_console_level_update(int level)
{
    int old = system_console_level;
    system_console_level = level;
    return old;
}

//------------------------------------------------------------
#if 0
int system_debug_file(int level, const char* filename)
{
    if (level <= system_debug_level) {
        FILE* file = fopen(filename, "re");
        if (file) {
            char  line[MAX_PATHNAME];
            while (fgets(line, sizeof(line), file) != NULL)
                system_debug(level, "%s", line);
            fclose(file);
        }
    }
    return 0;
}


void recover_file_system(int partition, int ubi_no, char* ubi_name, char* mount_point)
{
    int rc;

    ALERT(0, "partition=%d, ubi_no=%d, ubi_name='%s', mount_pount='%s'\n", partition, ubi_no, ubi_name, mount_point);

    rc = umount(mount_point);
    if (rc) {
        WARN(0, "umount(%s) returned %d\n", mount_point, rc);
    }

    do_system("/usr/bin/flash_eraseall /dev/mtd%d > /dev/null 2>&1", partition); 
    sync();

    do_system("/usr/bin/ubirmvol /dev/ubi%d -N %s > /dev/null 2>&1", ubi_no, ubi_name);
    do_system("/usr/bin/ubidetach /dev/ubi_ctrl -m %d > /dev/null 2>&1", partition);
    do_system("/usr/bin/ubiformat /dev/mtd%d -y > /dev/null 2>&1", partition);
    do_system("/usr/bin/ubiattach /dev/ubi_ctrl -m %d > /dev/null 2>&1", partition);
    do_system("/usr/bin/ubimkvol /dev/ubi%d -m -N %s > /dev/null 2>&1", ubi_no, ubi_name);
    do_system("/bin/mount -t ubifs ubi%d:%s %s > /dev/null 2>&1", ubi_no, ubi_name, mount_point);
    sync();
}


int system_boot_other_kernel(u32 bootlimit)
{
    // change the uBoot environment variables to boot the other kernel/rootfs
    // and return control to uBoot
    ubootenv_toggle_bootcmd(bootlimit);
    if (sync_system) 
        sync_system(0);
    led_redalert(5); 
    // reboot(LINUX_REBOOT_CMD_RESTART);
    return 0;
}
//------------------------------------------------------------
#endif
