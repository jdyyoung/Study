/* Copyright � 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */

#include "lib.h"

//#define DUMP_MEMORY
char hex[] = "0123456789abcdef";

int delete_files(const char* dirname, char* match)
{
    DIR* dirp = opendir(dirname);
    int  deleted = 0;
    char filename[MAX_PATHNAME];

    printf( "delete_files(%s, %s)", dirname, match? match : "(null)");
    if (dirp == NULL)
        return -1;

    for (;;) {
        struct dirent  this;
        struct dirent* pthis;
        int rc = readdir_r(dirp, &this, &pthis);
        if (rc || pthis == NULL)
            break;

        if (strcmp(this.d_name, ".") == 0 || strcmp(this.d_name, "..") == 0)
            continue;
        if (match != NULL && strstr(this.d_name, match) == NULL)
            continue;
        
        snprintf(filename, sizeof(filename), "%s/%s", dirname, this.d_name);
        unlink(filename);
        sync();
        ++deleted;
    }

    closedir(dirp);
    return deleted;
}

bool file_exists(char* filename, size_t* filesize)
{
    if (filename) {
        struct stat sbuf;
        int rc = stat(filename, &sbuf);
        if (rc == 0 && filesize)
            *filesize = sbuf.st_size; 
        return rc == 0;
    }
    return FALSE;
}

time_t monotonic_time(void)
{
    struct timespec ts;
    return clock_gettime(CLOCK_MONOTONIC, &ts) == 0? ts.tv_sec : 0;
//    struct timeval ts; //bob add
//    return gettimeofday(&ts, NULL) == 0? ts.tv_sec : 0;  //bob add
}

char* trim_bol(char* line)
{
    if (line) {
        for (; *line; ++line) {
            switch (*line) {
            case '\n': case '\r': case '\t': case ' ': continue;
            }
            break;
        }
    }
    return line;
}


char* trim_eol(char* line)
{
    if (line) {
        int oleng, leng;
        for (oleng = leng = strlen(line); leng > 0; --leng) {
            switch (line[leng-1]) {
            case '\n': case '\r': case '\t': case ' ': continue;
            }
            break;
        }
        if (leng != oleng)
            line[leng] = '\0';
    }
    return line;
}

void add_to_file(char* filename, void* data, int nbytes, BOOL timestamp)
{
    FILE* file = fopen(filename, "ae");
    if (file != NULL) {
        if (timestamp) {
            time_t now = time(NULL); 
            struct tm t;
            localtime_r(&now, &t);
            fprintf(file, "%d/%02d/%02d-%02d:%02d:%02d ", 
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec);
        }
        fwrite(data, 1, nbytes, file);
        if (((char*)data)[nbytes-1] != '\n')
            fwrite("\n", 1, 1, file);
        fclose(file); 
        sync();
    }
}

//--------------------------------------------------------------
#if 0

void generate_sigsegv(void)
{
    void (*sigsegv)(void) = (void*)1;
    ALERT(0, "ABOUT TO GENERATE SIGSEGV!!!\n"); fflush(stdout);
    sigsegv();
    ALERT(0, "SHOULD NOT GET HERE!!!\n"); fflush(stdout);
}


int do_remote_system(char* filename, char* fmt, ...)
{
    va_list vargs;
    char cmd[MAX_PATHNAME];
    int fd, rc, error;
    int saved_stdout;
    int saved_stderr;

    va_start(vargs, fmt);
    vsnprintf(cmd, sizeof(cmd), fmt, vargs);
    va_end(vargs);

    fd = open(filename, O_CREAT|O_TRUNC|O_WRONLY|O_CLOEXEC, 0666);
    if (fd < 0) {
        ERROR(errno, "unable to create file '%s'", filename);
        return -1;
    }

    ui_lock(); 
    stdout_redirected++;

    saved_stdout = dup(1);
    saved_stderr = dup(2);
    dup2(fd, 1);
    dup2(fd, 2);

    errno = 0;
    rc = system(cmd);
    error = errno;

    dup2(saved_stdout, 1);
    dup2(saved_stderr, 2);
    close(saved_stdout);
    close(saved_stderr);

    stdout_redirected--;
    ui_unlock();

    if (rc == -1) {
        errno = error;
        WARN(error, "do_system(%s): rc=%d, WIFEXITED(rc)=%d, WEXITSTATUS(rc)=%d", 
             cmd, rc, error, WIFEXITED(rc), WEXITSTATUS(rc));
        close(fd);
        return -error;
    }

    // note: this is the "exit(#);" or "main(): return(#);" lower 8 bits of
    // the return code (>=0)
    if (WIFEXITED(rc) && WEXITSTATUS(rc)) {
        WARN(error, "do_system(%s): WEXITSTATUS(%d)=%d", cmd, rc, WEXITSTATUS(rc));
        close(fd);
        return WEXITSTATUS(rc);
    }
    close(fd);
    return 0;
}


int do_system(char* fmt, ...)
{
    va_list vargs;
    char cmd[MAX_PATHNAME*2];
    char* cmdp = cmd;
    int rc, error;

    va_start(vargs, fmt);
    rc = vsnprintf(cmd, sizeof(cmd), fmt, vargs);
    va_end(vargs);

    if (rc > sizeof(cmd)) {
        ALERT(0, "Using heap because cmdlen=%d and max=%d!\n", rc, sizeof(cmd));
        cmdp = malloc(rc+8);
        if (cmdp == NULL) {
            return -ENOMEM;
        }

        va_start(vargs, fmt);
        vsnprintf(cmdp, rc+8, fmt, vargs);
        va_end(vargs);
    }

    #if 0
    if (strncmp(cmdp, "curl", 4) == 0) {
        ALERT(0, "cmdlen=%d and max=%d!\n", rc, sizeof(cmd));
        ALERT(0, "do_system(%s)\n", cmdp);
        dump_memory("CURL", 0, cmdp, strlen(cmdp));
    }
    #endif

    errno = 0; 
    DBUG(5, "system(cmdp):%s\n",cmdp);
    rc = system(cmdp);
    error = errno;

    if (rc == -1) {
        errno = error;
        WARN(error, "do_system(%s): rc=%d, WIFEXITED(rc)=%d, WEXITSTATUS(rc)=%d", 
             cmdp, rc, error, WIFEXITED(rc), WEXITSTATUS(rc));
        if (cmd != cmdp) free(cmdp);
        return -error; 
    }

    // note: this is the "exit(#);" or "main(): return(#);" lower 8 bits of
    // the return code (>=0)
    if (WIFEXITED(rc) && WEXITSTATUS(rc)) {
        WARN(error, "do_system(%s): WEXITSTATUS(%d)=%d", cmd, rc, WEXITSTATUS(rc));
        if (cmd != cmdp) free(cmdp);
        return WEXITSTATUS(rc);
    }

    if (cmd != cmdp) free(cmdp);
    return 0;
}


char upper_case(char data)
{
    return islower(data)? toupper(data) : data;
}


bool is_zero(u8* data, int sizeof_data)
{
    int i;
    for (i = 0; i < sizeof_data; ++i) {
        if (data[i] != 0) 
            return FALSE;
    }
    return TRUE;
}


bool spaces(char* s)
{
    while (*s == ' ')
        ++s;
    return *s == '\0';
}


u16 tohex(char* digits, int nbytes, BOOL* failed)
{
    u16 accum, digit;
    int i;

    for (accum = 0, i = 0; i < nbytes; ++i) {
        if (digits[i] >= '0' && digits[i] <= '9') {
            digit = digits[i] - '0';
        } else
        if (digits[i] >= 'A' && digits[i] <= 'F') {
            digit = (digits[i] - 'A') + 10;
        } else {
            *failed = TRUE;
            return 0xFFFF;
        }
        accum = (accum<<4) + digit;
    }

    return accum;
}


char* bin_to_hexstr(char* dst, size_t dst_len, const uint8_t *src, size_t src_len)
{
    size_t dst_str_max_length = 2 * src_len;
    char* dst_ptr = dst;
    char* dst_end = dst + dst_len;
    int i;

    if (dst == NULL || src == NULL || dst_len < dst_str_max_length || src_len == 0)
        return NULL;

    for (i = 0; i < src_len; i++)
        dst_ptr += snprintf(dst_ptr, dst_end - dst_ptr, "%02X", src[i]);

    *dst_ptr = '\0';
    return dst;
}


size_t hexstr_to_bin(uint8_t *dst, size_t dst_len, const char *src)
{
    if (src == NULL || dst == NULL ){
        WARN(0,"src == NULL || dst == NULL\n");
        return 0;
    } 

    size_t len = strlen(src);
    if(len == 0 || (len % 2) != 0){
        WARN(0,"len == 0 || (len % 2) != 0\n");
        return 0;
    }

    //Check the content of the string is only hexadecimal digits
    const char *p;
    for (p = src; *p != '\0'; p++) {
        if (('0' <= *p && *p <= '9') || ('a' <= *p && *p <= 'f') || ('A' <= *p && *p <= 'F'))
            continue;
        return 0;
    }

    len = len / 2;
    if (len > dst_len)
        len = dst_len;

    int i = 0; 
    for (i = 0; i < len; i++)
        sscanf(&src[i*2], "%2hhx", &dst[i]);

    return i;
}

char* local_strncpy(char* dst, const char* src, size_t nbytes)
{
    if (dst && nbytes > 0) {
        strncpy(dst, src, nbytes);
        dst[nbytes-1] = '\0';
    }
    return dst;
}


char* onoff_to_string(int on)
{
    return on? "true" : "false"; // changed from "ON" "OFF" to "true" "false" for JSON
}


int string_to_onoff(char* string)
{
    return strcasecmp(string, "ON") == 0 ||
           strcasecmp(string, "TRUE") == 0;
}


char* lmh_to_string2(u8 level)
{
    switch(level){
    case VOL_LEVEL_OFF:
        return "OFF";
    case VOL_LEVEL_LOW:
        return "LOW";
    case VOL_LEVEL_MED:
        return "MED";
    case VOL_LEVEL_HIGH:
        return "HIGH";
    default:
        return "ERROR";
    }
}


//chime volume level
int string_to_lmh2(char* string)
{
    if (strcasecmp(string, "0")      == 0 ||
        strcasecmp(string, "OFF")    == 0)
        return VOL_LEVEL_OFF;
    if (strcasecmp(string, "1")      == 0 ||
        strcasecmp(string, "L")      == 0 ||
        strcasecmp(string, "LOW")    == 0)
        return VOL_LEVEL_LOW;
    if (strcasecmp(string, "3")      == 0 ||
        strcasecmp(string, "H")      == 0 ||
        strcasecmp(string, "HIGH")    == 0)
        return VOL_LEVEL_HIGH;
    //MED is the default cause
    return VOL_LEVEL_MED;
}


char* index_to_string(char* filename, int index, 
                      char* buffer, int buffer_size, char* notfound)
{
    int lineno = 0;
    char line[1024];
    FILE* file;

    local_strncpy(buffer, notfound, buffer_size);

    file = fopen(filename, "re");
    if (file == NULL) {
        WARN(errno, "unable to open file '%s'", filename);
        return buffer;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (lineno == index) {
            local_strncpy(buffer, trim_eol(line), buffer_size);
            break;
        }
        ++lineno;
    }

    fclose(file);
    return buffer;
}


int string_to_index(char* filename, char* string)
{
    int lineno = 0;
    int leng = strlen(string);
    char line[1024];
    FILE* file;

    file = fopen(filename, "re");
    if (file == NULL) {
        WARN(errno, "unable to open file '%s'", filename);
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncasecmp(string, line, leng) == 0) {
            fclose(file);
            return lineno;
        }
        ++lineno;
    }

    fclose(file);
    return -1;
}


char* time_string(char* ascii, time_t time)
{
    struct tm t;
    gmtime_r(&time, &t);

    sprintf(ascii, "%d-%02d-%02dT%02d:%02d:%02d.000Z", 
            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec);
    return ascii;
}


static long pretty_units(char** line, long secs, long unit_secs, char* unit_name)
{
    int count = secs / unit_secs;
    if (count) {
        *line += sprintf(*line, "%d %s%s", count, unit_name, count > 1? "s":"");
        secs %= unit_secs;
        if (secs) {
            strcat(*line, ", ");
            *line += 2;
        }
    }

    return secs;
}


char* seconds_to_pretty_string(char* buffer, long seconds)
{
    char* line = buffer;

    buffer[0] = '\0';
    seconds = pretty_units(&line, seconds, 365*24*60*60, "year");
    seconds = pretty_units(&line, seconds, 24*60*60, "day");
    seconds = pretty_units(&line, seconds, 60*60, "hour");
    seconds = pretty_units(&line, seconds, 60, "minute");
    if (seconds)
        line += sprintf(line, "%ld seconds", seconds);
    return buffer;
}


int set_rtc(void)
{
    return do_system("hwclock -w");
}


int string_to_month(char* string)
{
    if (strncasecmp(string, "Jan", 3) == 0) return 1;
    if (strncasecmp(string, "Feb", 3) == 0) return 2;
    if (strncasecmp(string, "Mar", 3) == 0) return 3;
    if (strncasecmp(string, "Apr", 3) == 0) return 4;
    if (strncasecmp(string, "May", 3) == 0) return 5;
    if (strncasecmp(string, "Jun", 3) == 0) return 6;
    if (strncasecmp(string, "Jul", 3) == 0) return 7;
    if (strncasecmp(string, "Aug", 3) == 0) return 8;
    if (strncasecmp(string, "Sep", 3) == 0) return 9;
    if (strncasecmp(string, "Oct", 3) == 0) return 10;
    if (strncasecmp(string, "Nov", 3) == 0) return 11;
    if (strncasecmp(string, "Dec", 3) == 0) return 12;
    return -1;
}


time_t time_of_build(char* date, char* time)
{
    char month[16];
    int  day, year, hour, minute, second;
    struct tm t = {0};

    sscanf(date, "%s %d %d", month, &day, &year);
    sscanf(time, "%d:%d:%d", &hour, &minute, &second);
  
    t.tm_year  = year - 1900;
    t.tm_mon   = string_to_month(month) - 1;
    t.tm_mday  = day;
    t.tm_hour  = hour;
    t.tm_min   = minute;
    t.tm_sec   = second;
    return mktime(&t);
}


u32 get_maxfd(void)
{
    struct rlimit maxfd;
    
    if (getrlimit(RLIMIT_NOFILE, &maxfd) < 0) {
        WARN(errno, "getrlimit(RLIMIT_NOFILE)");
        return -1;
    }
    return (u32)maxfd.rlim_cur;
}


int set_core_limits(u32 dump_limit)
{
    struct rlimit core_limits;
    
    if (getrlimit(RLIMIT_CORE, &core_limits) < 0) {
        WARN(errno, "getrlimit(RLIMIT_CORE)");
        return -1;
    }

    if ((u32)core_limits.rlim_cur < dump_limit) {
        core_limits.rlim_cur = dump_limit;
        if ((u32)core_limits.rlim_max < dump_limit)
            core_limits.rlim_max = dump_limit;
        if (setrlimit(RLIMIT_CORE, &core_limits) < 0) {
            ERROR(errno, "setrlimit(RLIMIT_CORE)");
            return -1;
        }
    } else {
        ALERT(0, "core_limits.rlim_cur(0x%x) > dump_limit(0x%x)\n",
              core_limits.rlim_cur, dump_limit);
    }

    return 0;
}


void enable_core_dumps(u32 dump_limit, char* pathname)
{
    char core_pattern[32];

    if (set_core_limits(dump_limit))
        return;

    // the following magic will direct all core file dumps to the 'pathname'
    // directory, (probably /skybell/core) where "%e-%t-%s-%p" signifies:
    //     %e -- executable filename (e.g. skybell)
    //     %t -- time of dump (epoch seconds)
    //     %s -- number of signal causing dump
    //     %p -- PID of dumped process
    // 
    // Note: the core file dump process will not create directories, and the
    // existing directories must be writable (and obviously there must be
    // sufficient space in the filesystem).  The one problem with the current
    // approach is that it can potentially fill up the file system because each
    // core file has a unique name -- in the production system we may want to
    // just specify the name of the executable and maybe the signal number so
    // multiple dumps will overwrite the previous core file -- this could be
    // qualified with the 'development' boolean to cause the two distinct
    // behaviors.
    // 
    unlink(pathname);
    snprintf(core_pattern, sizeof(core_pattern), "%%e-v%s-%%s-%%t", PROG_VERSION);
    do_system("echo %s/%s > /proc/sys/kernel/core_pattern", pathname, core_pattern);
}

void dump_memory(char* banner, u32 addr, u8* data, int count)
{
#ifdef DUMP_MEMORY
    int offset = 0;
    int i;

    for (; count >= 16; count -= 16, offset += 16) {
        printf("%s_%08lx: ", banner, addr + offset);
        for (i=0; i<16; ++i)
            printf("%02x ", data[offset+i]);
        for (i=0; i<16; ++i) {
            char ch = data[offset+i];
            printf("%c", ch >= ' ' && ch <= '~'? ch : '?');
        }
        printf("\n");
    }

    if (count) {
        printf("%s_%08lx: ", banner, addr + offset);
        for (i=0; i<count; ++i)
            printf("%02x ", data[offset+i]);
        for (; i<16; ++i)
            printf("   ");
        for (i=0; i<count; ++i) {
            char ch = data[offset+i];
            printf("%c", ch >= ' ' && ch <= '~'? ch : '?');
        }
        printf("\n");
    }
#else
    return;
#endif
}


void dump_file(char* filename, char* header)
{
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        char data[1024];
        size_t nbytes;
        u32    offset = 0;

        do {
            nbytes = fread(data, 1, sizeof(data), file); 
            dump_memory(header, offset, (u8*)data, nbytes);
            offset += nbytes;
        } while (nbytes == sizeof(data));

        fclose(file);
    }
}


int read_from_file(char* filename, size_t filesize, unsigned char* data)
{
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        size_t nbytes = fread(data, 1, filesize, file);
        fclose(file);

        data[filesize] = '\0';
        return nbytes == filesize?  0 : -1;
    }

    return -1;
}


void add_to_file(char* filename, void* data, int nbytes, BOOL timestamp)
{
    FILE* file = fopen(filename, "ae");
    if (file != NULL) {
        if (timestamp) {
            time_t now = time(NULL); 
            struct tm t;
            localtime_r(&now, &t);
            fprintf(file, "%d/%02d/%02d-%02d:%02d:%02d ", 
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec);
        }
        fwrite(data, 1, nbytes, file);
        if (((char*)data)[nbytes-1] != '\n')
            fwrite("\n", 1, 1, file);
        fclose(file); 
        sync();
    }
}


void dump_to_file(char* filename, void* data, int nbytes)
{
    FILE* file = fopen(filename, "we");
    if (file != NULL) {
        fwrite(data, 1, nbytes, file);
        fclose(file);
        sync();
    }
}


int read_file_counter(char* filename)
{
    int counter = 0;
    int fd = open(filename, O_RDWR|O_CREAT|O_CLOEXEC, 0666);
    if (fd >= 0) {
        if (read(fd, (void*)&counter, sizeof(counter)) != sizeof(counter))
            counter = 0;
        close(fd);
    }

    return counter;
}


int increment_file_counter(char* filename)
{
    int counter = 0;

    int fd = open(filename, O_RDWR|O_CREAT|O_CLOEXEC, 0666);
    if (fd < 0)
        return 0;

    if (read(fd, (void*)&counter, sizeof(counter)) != sizeof(counter))
        counter = 0;
    ++counter;

    lseek(fd, 0L, SEEK_SET);
    write(fd, (void*)&counter, sizeof(counter));
    close(fd);
    sync();

    return counter;
}


int temp_filename(char* filename, int maxlen)
{
    return snprintf(filename, maxlen, "/tmp/skybell-%08lx", random());
}


int file_count(char* pathname, char* match, struct list_head* hits)
{
    int count = 0;
    DIR* dirp = opendir(pathname);
    if (dirp == NULL) {
        WARN(errno, "opendir(%s)", pathname);
        return -1;
    }

    for (;;) {
        struct dirent  this;
        struct dirent* pthis;
        int rc = readdir_r(dirp, &this, &pthis);
        if (rc) {
            WARN(errno, "readdir_r(%s)", pathname);
            closedir(dirp);
            return -1;
        }

        // denotes end of directory
        if (pthis == NULL) 
            break;

        if (this.d_type==DT_REG && 
            (match == NULL || strstr(this.d_name, match) != NULL)) {
            if (hits != NULL) {
                FILE_LIST *file = (FILE_LIST *)HEAP_ALLOC(sizeof(FILE_LIST)); 
                if (file) {
                    local_strncpy(file->name, this.d_name, sizeof(file->name));
                    list_add_tail(&file->list, hits);
                }
            }
            ++count; 
        }
    }
    closedir(dirp);

    return count;
}

int delete_oldest_file(char* dirname, char* file_deleted)
{
    // disard the oldest file (by name) in the PENDING_PATHNAME directory  
    // return TRUE if a log file was deleted

    time_t oldest_time = time(NULL);
    char oldest[MAX_PATHNAME];
    char pathname[MAX_PATHNAME];
    int  rc;
    DIR* dirp;

    dirp = opendir(dirname);
    if (dirp == NULL) {
        WARN(errno, "opendir(%s)", dirname);
        return -1;
    }

    memset(oldest, 0, sizeof(oldest));
    for (;;) {
        struct dirent  this;
        struct dirent* pthis;

        rc = readdir_r(dirp, &this, &pthis);
        if (rc) {
            WARN(errno, "readdir_r(%s)", dirname);
            break;
        }

        // denotes end of directory
        if (pthis == NULL) 
            break;

        if (this.d_type == DT_REG) {
            struct stat stbuf;
            snprintf(pathname, sizeof(pathname), "%s/%s", dirname, this.d_name);
            if (oldest[0] == '\0' || 
                (stat(pathname, &stbuf) == 0 && stbuf.st_mtime < oldest_time)) {
                strcpy(oldest, this.d_name);
                oldest_time = stbuf.st_mtime;
            }
        }
    }
    closedir(dirp);

    rc = -1;
    if (oldest[0]) {
        snprintf(pathname, sizeof(pathname), "%s/%s", dirname, oldest);
        rc = unlink(pathname);
        if (rc == 0)
            strcpy(file_deleted, pathname);
        sync();
    }

    return rc;
}


int copy_file(char* src_file, char* dst_file)
{
    int rc = 0;

    unlink(dst_file); sync();
    if (file_exists(src_file, NULL)) {
        rc = do_system("%s %s %s", UTILS_FILE_COPY_COMMAND, src_file, dst_file); 
        if (rc)
            return rc;
        sync();
    }
    return 0;
}


int upload_file(char* url, char* filename, BOOL* uploaded)
{
    if (file_exists(filename, NULL)) {
        int rc;

        if (url == NULL)
            url = cfg.stage ? get_url_log() : CFG_DEFAULT_LOG_URI_PROD;
        rc = coap_system_https_upload_request(url, filename); 
        if (rc) {
            ALERT(0, "%s.%d:Unable to upload '%s'\n", __FUNCTION__,__LINE__,filename);
            unlink(filename);
          	  sync();
            return rc;
        }

        *uploaded = TRUE;
        ALERT(0, "UPLOADED OK----------------'%s'!\n", filename);
        unlink(filename);
        sync();
    }

    return 0;
}


// newdir
//
// Create the directory if it does not exist.  
// Returns:
//     -1 failure
//      0 success
// 
int newdir(char* dirname)
{
    int rc = mkdir(dirname, 0777);
    if (rc == -1 && errno != EEXIST) {
        WARN(errno, "mkdir(%s)", dirname);
        return -1;
    }

    sync();
    return 0;
}


bool linux_cmdline(char* cmdline, int maxlen)
{
    char* result;
    FILE* file;

    memset(cmdline, 0, maxlen);
    file = fopen(UTILS_CMDLINE_PROCFILE, "re");
    if (file == NULL) {
        ERROR(errno, "fopen(%s, 'r')", UTILS_CMDLINE_PROCFILE);
        return FALSE;
    }

    result = fgets(cmdline, maxlen, file);
    fclose(file);
    return result != NULL;
}


// return TRUE if rootfs-a (and kernel-a) are currently active
//        FALSE if not active
//
bool rootfs_A(void)
{
    static bool A = -1;
    if (A == -1) {
        char  result[MAX_PATHNAME];
        char* kernel;

        linux_cmdline(result, sizeof(result));
        kernel = strstr(result, "nand-misc"); // get beyond "mtdparts"
        if (kernel != NULL) {
            if ((kernel = strstr(kernel, "nand-kernel-")) != NULL) {
                A = kernel[12] == 'A';
                return A;
            }
        }
        if (ubootenv_getvalue("bootcmd", result, sizeof(result))) 
            return FALSE;
        A = strstr(result, UTILS_ROOTFS_A) != NULL;
    }
    return A;
}


// return TRUE if rootfs-b (and kernel-b) are currently active
//        FALSE if not active
//
bool rootfs_B(void)
{
    static bool B = -1;
    if (B == -1) {
        char  result[MAX_PATHNAME];
        char* kernel;

        linux_cmdline(result, sizeof(result));
        kernel = strstr(result, "nand-misc"); // get beyond "mtdparts"
        if (kernel != NULL) {
            if ((kernel = strstr(kernel, "nand-kernel-")) != NULL) {
                B = kernel[12] == 'B';
                return B;
            }
        }
        if (ubootenv_getvalue("bootcmd", result, sizeof(result))) 
            return FALSE;
        B = strstr(result, UTILS_ROOTFS_B) != NULL;
    }
    return B;
}


char which_rootfs(void)
{
    return rootfs_A()? 'A':'B';
}


// this routine is called when uboot environment bootcount > bootlimit
// there are two cases here depending on how recent uboot is:
//   1. there is a "nand-rootfs-" and "nand-kernel-" in the linux command line
//      in which case we know exactly what rootfs/kernel we are currently using
//      and the one we are not using failed because u-boot booted "altbootcmd"
//   2. older u-boot, no "nand-rootfs" or "nand-kernel": in this case the kernel
//      and rootfs we used is reflected in "altbootcmd" *NOT* "bootcmd"
//
// return 'A' if the 'A' rootfs/kernel failed, 'B' if the 'B' rootfs/kernel failed
//
char failed_rootfs(void)
{
    char cmdline[MAX_PATHNAME];
    char* kernel;

    linux_cmdline(cmdline, sizeof(cmdline));
    kernel = strstr(cmdline, "nand-misc"); // get beyond "mtdparts"
    if (kernel != NULL) {
        if ((kernel = strstr(kernel, "nand-kernel-")) != NULL) {
            return kernel[12] == 'A'? 'B':'A'; // report other partition
        }
    }

    // since the linux cmdline did not have the "nand-kernel-" string,
    // the rootfs_A() routine is using u-boot variables which will report
    // the partition from "bootcmd" eventhough we know u-boot used 
    // "altbootcmd" because bootcount > bootlimit  -- I know a little confusing...
    return which_rootfs(); // report the partition from bootcmd (which is currently inactive)
}

u16 get_rootfs_version(void)
{
    static u16 cached_version = 0;

    if (cached_version == 0) {
        struct list_head hits; 

        INIT_LIST_HEAD(&hits);
        if (file_count("/firmware", "version", &hits) > 0) {
            struct list_head *this;
            struct list_head *next;
            FILE_LIST *item = NULL;
            char version[32];

            list_for_each_safe(this, next, &hits) {
                item = list_entry(this, FILE_LIST, list);
                local_strncpy(version, item->name, sizeof(version));
                list_del(&item->list);
                heap_free(item);
            }
            if (item != NULL) {
                char* period = strchr(version, '.');
                if (period != NULL) 
                    cached_version = atoi(period+1);
            }
        }
    }

    return cached_version;
}


char* skybell_hwrev(void)
{
    char* hwrev = strstr(stmicro_hwrev, "HDW ");
    return hwrev == NULL? stmicro_hwrev : hwrev + 4;
}

#define V_UBOOT "U-Boot 2015.01-Vienna-2.3 Build D3-ver1.0 (Dec 07 2019 - 15:11:08)"
const char* mcu_crc = "cc14";
void get_versions(int maxstring, char* v_stmicro, char* v_uboot, char* v_linux, char* v_rootfs, char* v_skybell)
{
    char filename[MAX_PATHNAME];
    u16  version;

    stmicro_crc = strtoul(mcu_crc, NULL, 16);
    // get stmicro version
    snprintf(v_stmicro, maxstring, "v%d.%03d (CRC %04x)", stmicro_version/256, stmicro_version%256, stmicro_crc);
    //memcpy(v_cypress, cypress_version(), maxstring);

    // get uboot version -- this is the uboot environment variable 'ver'
    if (ubootenv_getvalue("ver", v_uboot, maxstring)) 
        local_strncpy(v_uboot, V_UBOOT, maxstring);

    // get linux version -- this is the string returned by 'uname -a'
    local_strncpy(v_linux, "UNAVAIL", maxstring);
    if (temp_filename(filename, sizeof(filename)) > 0) {
        if (do_system("uname -a > %s", filename) == 0) {
            FILE* file = fopen(filename, "re");
            if (file != NULL) {
                char  line[128];
                char* ok = fgets(line, sizeof(line), file);
                fclose(file);
                unlink(filename);
                if (ok) 
                    local_strncpy(v_linux, trim_bol(trim_eol(line)), maxstring);
            }
        }
    }

    // get the rootfs version -- this is the suffix of the '/firmware/version.*' file
    // note that the skybell and linux versions can be derived from this (not uboot)
    version = get_rootfs_version();
    snprintf(v_rootfs, maxstring, "v%d.%03d", version/1000, version%1000);

    //take from ver.h generated by cmake instead
    // get the skybell version -- this is the easy one!
    //snprintf(v_skybell, maxstring, "v%d.%03d (%s %s)", VERSION/1000, VERSION%1000, __DATE__, __TIME__);
    snprintf(v_skybell, maxstring, "v%s %s-%s (%s %s)", PROG_VERSION, PROG_GIT_BRANCH, PROG_GIT_COMMIT_HASH, __DATE__, __TIME__);
}


int insmod(char* module_pathname, char* options)
{
    if (options == NULL) {
        options = "";
    }

    return do_system("%s %s %s", UTILS_INSMOD_COMMAND, module_pathname, options);
}


int rmmod(char* module_pathname)
{
    return do_system("%s %s", UTILS_RMMOD_COMMAND, module_pathname);
}


int percent_disk_used(char* filesystem)
{
    struct statvfs svfs;
    int rc = statvfs(filesystem, &svfs);

    if (rc != 0) {
        WARN(errno, "statvfs(%s)", filesystem);
        return -1;
    }
    return (svfs.f_blocks - svfs.f_bfree) * 100 / svfs.f_blocks;
}


// return TRUE if the 'filesystem' is read-only
// note: statvfs() f_flags read-only bit did not accurately reflect the file system state
//
bool read_only(char* filesystem)
{
    struct statvfs svfs;
    int rc = statvfs(filesystem, &svfs);
    if (rc != 0) {
        WARN(errno, "statvfs(%s)", filesystem);
        return -1;
    }
    return (svfs.f_flag & ST_RDONLY) ? TRUE : FALSE;
}


#if 0
// deprecated 2017/2/22 -- skybell_init now reformats /skybell/misc if read-only,
// so we don't need to load a special uboot anymore

// any date "JUL 21 2016" or more recent has this feature
bool uboot_has_misc_format(void)
{
    char ver[MAX_PATHNAME];

    if (ubootenv_getvalue("ver", ver, sizeof(ver)) == 0) {
        int y, m, d;
        char* when = strchr(ver, '(');
        char* year;
        char* day;

        if (when == NULL)
            return FALSE;
        when += 1;

        day = strchr(when, ' ');
        if (day == NULL)
            return FALSE;
        day += 1;

        year = strchr(day, ' ');
        if (year == NULL)
            return FALSE;
        year += 1;

        m = string_to_month(when);
        d = strtoul(day, NULL, 10);
        y = strtoul(year, NULL, 10);

        printf("%s(): year=%d, month=%d, day=%d\n", __FUNCTION__, y, m, d);
        fflush(stdout);

        if (y == 2016 && (m > 7 || (m==7 && d>=21)))
            return TRUE;
        return y > 2016;
    }

    return FALSE;
}
#endif


// Escape characters in string using HTML escape character encoding convention.
// For example, the input string "hello&" will encode to "hello&#38;".
//  buf         buffer to receive escaped string
//  size        size of buffer (note: escaped strings are always larger then the
//              original)
//  str         string to escape
//  escaped     string of characters to be escaped (note: '&' will always
//              be escaped and needn't be included in this string)
// This routine returns the size (including null) of the escaped string, or
// -1 if the buffer size was too small and resulted in truncation.
// Regardless of return code, the output will always be null terminated.
// 
int html_escape(char* buf, size_t size, const char* str, const char* escaped)
{
    int i = 0;
    int n = 0;
    const char defaults[] = { "&" };

    while ((str[i] != '\0') && (n < size)) {
        if ((strchr(escaped, str[i]) == NULL) &&
            (strchr(defaults, str[i]) == NULL)) {
            buf[n++] = str[i++];
        }
        else {
            if (n + 6 >= size) {
                buf[n] = '\0';
                return -1;
            }
            buf[n++] = '&';
            buf[n++] = '#';
            n += snprintf(&buf[n], 3, "%d", (unsigned)str[i++]);
            buf[n++] = ';';
        }
    }

    if (n >= size) {
        buf[size - 1] = '\0';
        return -1;
    }

    buf[n++] = '\0';

    return n;
}


// Unescape HTML escape "&#nn;" encoded characters in a string.
//  buf         buffer to receive unescaped string
//  size        size of buffer
//  str         string to unescape
// This routine returns the size (including null) of the unescaped string, or
// -1 if the buffer size was too small and resulted in truncation.
// Regardless of return code, the output will always be null terminated.
// 
int html_unescape(char* buf, size_t size, const char* str)
{
    int i = 0;
    int n = 0;
    int value;
    char *end;

    while ((str[i] != '\0') && (n < size)) {
        if ((str[i] == '&') && (str[i + 1] == '#')) {
            value = strtol(&str[i + 2], &end, 10);
            if ((*end == ';') && (value > 0) && (value < 256)) {
                buf[n++] = (char)value;
                i = end - str + 1;
            }
            else {
                buf[n++] = str[i++];
            }
        }
        else {
            buf[n++] = str[i++];
        }
    }

    if (n >= size) {
        buf[size - 1] = '\0';
        return -1;
    }

    buf[n++] = '\0';

    return n;
}


int url_encode(u8* source, u8* dest, int sizeof_dest)
{
    char *hex = "0123456789abcdef";
    int si, di = 0;

    for (si = 0; source[si] != '\0'; ++si) {
        u8 ch = source[si];
        if (('a' <= ch && ch <= 'z') || 
            ('A' <= ch && ch <= 'Z') || 
            ('0' <= ch && ch <= '9')) {
            dest[di] = ch;
            if (++di >= sizeof_dest) {
                return -1;
            }
        } else {
            if (di + 4 >= sizeof_dest)
                return -1;
            dest[di++] = '\\';
            dest[di++] = '%'; 
            dest[di++] = hex[ch >> 4];
            dest[di++] = hex[ch % 0xF];
        }
    }

    dest[di] = '\0';
    return 0;
}


i64 msec_monotonic_time(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
//    struct timeval ts; //bob add
//	if (gettimeofday(&ts, NULL) == 0) {  //bob add
        i64 msecs = ts.tv_sec;
        return (msecs*1000) + (ts.tv_nsec/1000000);
//        return (msecs*1000) + (ts.tv_usec/1000);
    }
    return 0;
}


// Get system uptime.
// This routine returns the Linux system uptime in seconds.
// 
long get_sys_uptime(void)
{
    struct sysinfo sinfo;
    sysinfo(&sinfo);
    return (long)sinfo.uptime;
}


static long app_start_time = 0;
static long net_start_time = -1;

long get_app_uptime(void) { return get_sys_uptime() - app_start_time; }
long get_net_uptime(void) { return net_start_time == -1? 0 : get_sys_uptime() - net_start_time; }
void set_app_start_time(void) { app_start_time = get_sys_uptime(); }
void set_net_start_time(void) { net_start_time = get_sys_uptime(); }
void set_net_down(void)       { net_start_time = -1; }


int get_year(void)
{
    struct tm t;
    time_t now = time(NULL);
    localtime_r(&now, &t);
    return t.tm_year + 1900;
}


void get_memory_usage(u64* total, u64* used, u64* mem_free, u64* shared, u64* buffers, u64* cached)
{
    FILE* file;
    char  filename[MAX_PATHNAME];
    char  line[256];

    *total = *used = *mem_free = *shared = *buffers = *cached = 0;

    if (temp_filename(filename, sizeof(filename)) <= 0) {
        ERROR(errno, "temp_filename()\n");
        return;
    }

    if (do_system("free > %s", filename)) {
        ERROR(errno, "do_system(free >& %s)\n", filename);
        return;
    }

    file = fopen(filename, "re");
    if (file == NULL) {
        ERROR(errno, "fopen(%s, 'r')", filename);
        return;
    }

    // format is first line header, second line "Mem: <total> <used> <free> <shared> <buffers> <cached>
    while (fgets(line, sizeof(line), file) != NULL) {
        char* ptr = strstr(line, "Mem:");
        if (ptr != NULL) {
            *total = strtoull(ptr + 5, &ptr, 10);
            *used  = strtoull(ptr, &ptr, 10);
            *mem_free = strtoull(ptr, &ptr, 10);
            *shared = strtoull(ptr, &ptr, 10);
            *buffers = strtoull(ptr, &ptr, 10);
            break;
        }
    }
    fclose(file);
    unlink(filename);
}


void loadavg(char* result, int sizeof_result)
{
    FILE* file;
    char  filename[MAX_PATHNAME];

    if (temp_filename(filename, sizeof(filename)) <= 0) {
        ERROR(errno, "temp_filename()\n");
        return;
    }

    if (do_system("cat /proc/loadavg > %s", filename)) {
        ERROR(errno, "cat /proc/loadavg > %s\n", filename);
        return;
    }

    file = fopen(filename, "re");
    if (file == NULL) {
        ERROR(errno, "fopen(%s, 'r')", filename);
    } else {
        if (fgets(result, sizeof_result, file) == NULL)
            memset(result, 0, sizeof_result);
        fclose(file);
        unlink(filename);

        trim_eol(result);
    }
}


void close_pipe(int* pipe)
{
    int i;

    for (i = 0; i < 2; ++i) {
        if (pipe[i] != -1) {
            close(pipe[i]);
            pipe[i] = -1;
        }
    }
}


typedef struct {
    char reason[128];
    int  delay_in_seconds;
} REBOOT_CONTEXT;

static int reboot_thread(THREAD* thread)
{
    REBOOT_CONTEXT* context = (REBOOT_CONTEXT*)thread->private_data;
    if (context != NULL) {
        ALERT(0, "REBOOTING IN %d SECONDS BECAUSE '%s'\n", context->delay_in_seconds, context->reason); 
        if (context->delay_in_seconds > 0) {
            sleep(context->delay_in_seconds); 
        }
        REBOOT(0, context->reason);
    } else {
        REBOOT(0, "CONTEXT WAS NULL!!!");
    }
    thread_cancel(thread);
    return 0;
}


int delayed_reboot(const char* reason, int delay_in_seconds)
{
    REBOOT_CONTEXT* context = (REBOOT_CONTEXT*) HEAP_ALLOC(sizeof(*context));
    context->delay_in_seconds = delay_in_seconds;
    local_strncpy(context->reason, reason, sizeof(context->reason));
    return thread_assign("delayed_reboot", reboot_thread, context) == NULL? -1 : 0;
}


typedef struct {
    char    url[1024];
} SNAPSHOT_URL;

static int url_json_to_bin(SNAPSHOT_URL* dst_bin, u8* src_json, int sizeof_json)
{
    JSON_ITEM items[] = {
      JSON_DECODE_STRING( "url", SNAPSHOT_URL, (*dst_bin), url),
    };
    JSON json = {.count = ARRAY_SIZE(items), .item = items};

    int rc = json_decode(&json, (char *)src_json, sizeof_json, dst_bin, sizeof(*dst_bin), TRUE); 
    if (rc) {
        WARN(rc, "json_decode(url)=%d\n", rc);
        return -1;
    } 

    return 0;
}


char* extract_url_from_file(char* filename, char* url_string, int sizeof_url_string)
{
    FILE* file;
    SNAPSHOT_URL url;

    DBUG(2, "%s(%s)...\n", __FUNCTION__, filename);

    memset(&url, 0, sizeof(url));
    memset(url_string, 0, sizeof_url_string);

    file = fopen(filename, "re");
    if (file == NULL)
        ALERT(errno, "Unable to open file '%s'\n", filename); 
    else {
        u8 payload[1400];
        size_t nbytes = fread(payload, 1, sizeof(payload), file); 
        fclose(file);

        if (cfg.debug_level > 2) {
            printf("%s(%s): nbytes=%d\n", __FUNCTION__, filename, nbytes); 
            dump_memory("URL", 0, payload, nbytes);
        }

        if (nbytes > 0) {
            int rc = url_json_to_bin(&url, payload, nbytes);
            if (rc == 0) { 
                // success!
                memcpy(url_string, url.url, sizeof_url_string);
                url_string[sizeof_url_string-1] = '\0';
                return url_string;
            }
        }
    }

    return "UNKNOWN-URL";
}


const char* skybell_device_filename(void)
{
    return SKYBELL_DEVICE_NAME;
}


int compute_md5sum(char* filename, char* result)
{
    return do_system("busybox md5sum %s > %s", filename, result);
}


int read_md5sum_file(char* filename, char* result, int sizeof_result)
{
    char* space;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        ERROR(errno, "fopen(%s, 'r')", filename);
        return -1;
    }

    fgets(result, sizeof_result, file);
    fclose(file);

    space = strchr(result, ' ');
    if (space != NULL)
        *space = '\0';
    return 0;
}


//compare two files character by characters
//result = FALSE (files are different)
//result = TRUE (files are same)
int compare_two_files(char const *file1, char const *file2, char *tmpfile, BOOL *result){
    FILE *fp1, *fp2;

    fp1 = fopen(file1, "re");
    if(fp1 == NULL){
        return -1;
    }
    fp2 = fopen(file1, "re");
    if(fp2 == NULL){
        fclose(fp1);
        return -1;
    }


    do_system("cmp %s %s > %s", file1, file2, "/tmp/cmp.tmp");

    struct stat sbuf;
    int rc = stat("/tmp/cmp.tmp", &sbuf);
    if(rc == 0){
        *result = (sbuf.st_size > 0) ? FALSE : TRUE;
    }else{
        fclose(fp1);
        fclose(fp2);
        return -1;
    }

    fclose(fp1);
    fclose(fp2);

    return 0;
}


static int firewall_count = 0;
static BOOL fw_disabled = FALSE;


void firewall_disabled(void)
{
    ALERT(0, "firewall_count=%d, fw_disabled=%d\n", firewall_count, fw_disabled);
    fw_disabled = TRUE;
    //privileges_restore(__FUNCTION__, __LINE__);
    do_system("sh /etc/iptables/iptables.default");
    //privileges_drop(__FUNCTION__, __LINE__);
}


void firewall_restore(void)
{
    ALERT(0, "firewall_count=%d, fw_disabled=%d\n", firewall_count, fw_disabled);
    fw_disabled = FALSE;
    if (firewall_count == 0) {
        //privileges_restore(__FUNCTION__, __LINE__);
        do_system("sh /etc/iptables/iptables.skybell");
        //privileges_drop(__FUNCTION__, __LINE__);
    }
}


void firewall_hangup(void)
{
    ALERT(0, "firewall_count=%d, fw_disabled=%d\n", firewall_count, fw_disabled);
    if (!fw_disabled && firewall_count == 0) {
        //privileges_restore(__FUNCTION__, __LINE__);
        do_system("sh /etc/iptables/iptables.skybell");
        //privileges_drop(__FUNCTION__, __LINE__);
    }
}


void firewall_off(void)
{
    ALERT(0, "firewall_count=%d, fw_disabled=%d\n", firewall_count, fw_disabled);
    if (firewall_count++ == 0) {
        //privileges_restore(__FUNCTION__, __LINE__);
        do_system("sh /etc/iptables/iptables.default");
        //privileges_drop(__FUNCTION__, __LINE__);
    }
}


void firewall_on(void)
{
    ALERT(0, "firewall_count=%d, fw_disabled=%d\n", firewall_count, fw_disabled);

    if (firewall_count < 0)
        firewall_count = 0;
    if (firewall_count > 0) 
        firewall_count -= 1;

    if (!fw_disabled && firewall_count == 0) {
        //privileges_restore(__FUNCTION__, __LINE__);
        do_system("sh /etc/iptables/iptables.skybell");
        //privileges_drop(__FUNCTION__, __LINE__);
    }
}


//GPIO Pin No.
#define GPIO_T5 57
#define GPIO_W6 58
#define GPIO_T6 59
// #define GPIO_V6 60
#define GPIO_U6 61
#define DIR_OUT 1
#define DIR_IN  0

static unsigned int get_gpio_value(unsigned int gpio_no){
    unsigned int val = 0xff;
    gpio_export(gpio_no);
    gpio_set_dir(gpio_no, DIR_IN);
    gpio_get_value(gpio_no, &val);
    gpio_unexport(gpio_no);

    // printf("get gpio_%d value is %d\n",gpio_no ,val);
    return val;
}

int get_gpio_hw_ver()
{
    static BOOL hw_ver_cached = FALSE;
    static unsigned char hw_ver = 0xFF;

    if( ! hw_ver_cached){
        hw_ver_cached = TRUE;
        if( ! get_gpio_value(GPIO_T5)){
            hw_ver = hw_ver & 0xF7;
        }
        if( ! get_gpio_value(GPIO_W6)){
            hw_ver = hw_ver & 0xFB;
        }
        if( ! get_gpio_value(GPIO_T6)){
            hw_ver = hw_ver & 0xFD;
        }
        if(!get_gpio_value(GPIO_U6)){
            hw_ver = hw_ver & 0xFE;
        }
        hw_ver = ~ hw_ver;
    }

    DBUG(0,"HW VERSION:v0.%02d\n",hw_ver);
    return hw_ver;
}

#define ERROR_COUNT_MAX 3
static u16 MsgBroker_SendMsg_error_count = 0;
int MsgBroker_SendMsg_exp(const char* path, MsgContext* msg_context){
    int ret;
    if (MsgBroker_SendMsg(path, msg_context) == S_OK){
        MsgBroker_SendMsg_error_count = 0;
        ret = S_OK; 
    }
    else{
        DBUG(0,"%d %s() : send command error",__LINE__,__func__);
        MsgBroker_SendMsg_error_count++;
        if(MsgBroker_SendMsg_error_count >= ERROR_COUNT_MAX){
            REBOOT(0,"MsgBroker_SendMsg_error_count more than ERROR_COUNT_MAX");
        }
        ret = S_FAIL;
    }
    return ret;
}


//--------------------------------------------------------------
#endif
