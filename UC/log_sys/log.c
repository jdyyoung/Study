/* Copyright � 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */
 #include "log.h"

typedef struct {
    MUTEX mutex;              // makes logging thread-safe and atomic
    FILE* file;              // if non-null, the active log file
    char name[MAX_PATHNAME]; // log name
    LOG_CONFIG config;       // configuration, saved in log directory
} LOG_CONTEXT;


static char log_rootpathname[MAX_PATHNAME];
static long log_session;
static int  logging = 0;
static void (*log_restore)(void) = NULL;


void* system_log = NULL;
void* debug_log = NULL;


// returns index of next file to log (or LOG_CONFIG.max_files if all of
// the files are full (-1 on error in which case logging is disabled)
//
static int next_file(LOG_CONTEXT* ctxt)
{
    DIR* dirp;
    char filename[MAX_PATHNAME];
    struct stat stbuf;
    int next;

    // search the log directory for the next file to write to (the largest
    // numbered file)
    //
    snprintf(filename, sizeof(filename), "%s/%s", log_rootpathname, ctxt->name);
    dirp = opendir(filename);
    if (dirp == NULL)
        return -1;

    for (next = -1;;) {
        struct dirent  this;
        struct dirent* pthis;
        int d_number;
        int rc = readdir_r(dirp, &this, &pthis);
        if (rc || pthis == NULL)
            break; // at end of directory

        // skip if not a log file
        if (this.d_name[0] < '0' || this.d_name[0] > '9' || this.d_name[1] != '\0')
            continue;

        // log files are # (integer from 0 to 9), if this log# is smaller
        // than a previous one disregard
        d_number = atoi(this.d_name);
        if (d_number > next)
            next = d_number;
    }

    // don't forget to close the directory (avoid memory leaks)
    closedir(dirp);

    // if there are no files, return 0 to write to the first file
    if (next == -1)
        return 0;

    // found a file, if this file is full return the next file
    snprintf(filename, sizeof(filename), "%s/%s/%01d", 
             log_rootpathname, ctxt->name, next);
    if (stat(filename, &stbuf) == 0)
        return stbuf.st_size < ctxt->config.bytes_per_file? next : next + 1;

    // TODO: should not happen -- return failure, should post a warning
    return -1;
}


// prepend the timestamp "2010/12/06-22:30:15" to the next log line
// if LONG_CONFIG.timestamp is enabled
//
static void timestamp(const LOG_CONTEXT* ctxt)
{
    if (ctxt->config.timestamp) {
        struct tm t;
        struct timeval tv;
        time_t now = time(NULL);
        localtime_r(&now, &t);
        gettimeofday(&tv, NULL);
        fprintf(ctxt->file, "%d/%02d/%02d-%02d:%02d:%02d[%lu.%lu] ", 
                t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                t.tm_hour, t.tm_min, t.tm_sec, tv.tv_sec, tv.tv_usec);
    }
}


// will return with LOG_CONTEXT.file set to non-null value where logging
// will resume (if LOG_CONTEXT.file is NULL on return logging is disabled)
//
static void new_file(LOG_CONTEXT* ctxt)
{
    int  next;
    char dst[MAX_PATHNAME];

    if (ctxt->file) {
        fclose(ctxt->file);
        ctxt->file = NULL;
    }

    next = next_file(ctxt);
    if (next == -1)
        return;

    if (next >= ctxt->config.max_files) {
        int s, delta = next - ctxt->config.max_files;
        char src[MAX_PATHNAME];

        // delete old files -- note file prune if caller changed config
        for (s = 0; s <= delta; ++s) {
            sprintf(src, "%s/%s/%01d", log_rootpathname, ctxt->name, s);
            unlink(src);
        }

        // move files down -- yuck but at least it is always pretty
        // for the operator viewing the log directory...
        for (; s < ctxt->config.max_files; ++s) {
            snprintf(src, sizeof(src), "%s/%s/%01d", 
                     log_rootpathname, ctxt->name, s);
            snprintf(dst, sizeof(dst), "%s/%s/%01d", 
                     log_rootpathname, ctxt->name, s - (delta+1));
            rename(src, dst);
        }

        // prune files if caller changed the configuration
        //
        for (; s < 10; ++s) {
            snprintf(src, sizeof(src), "%s/%s/%01d", 
                     log_rootpathname, ctxt->name, s);
            if (unlink(src))
                break;
        }

        // set to the last file
        //
        next = ctxt->config.max_files - 1;
    }

    // now open the appropriate log file -- truncate if it exists 
    // (should not be the case)
    //
    snprintf(dst, sizeof(dst), "%s/%s/%01d", 
             log_rootpathname, ctxt->name, next);
    ctxt->file = fopen(dst, "ae");

    // if (ctxt->file != NULL) {
        // char string[MAX_PATHNAME];
        // wifi_info_t wifi;
        // uuid_t uuid;
        // u32 addr;

        // if (wifi_get_info(&wifi, FALSE) != S_OK) {
            // memset(&wifi, 0, sizeof(wifi));
            // addr = 0;
        // } else {
            // addr = ntohl(wifi.ipv4.addr);
        // }

        // get the rootfs version -- this is the suffix of the '/firmware/version.*' file
        // note that the skybell and linux versions can be derived from this (not uboot)
        // timestamp(ctxt);
        // fprintf(ctxt->file, "v%s [%s/%s] -- rootfs-%c[version.%d]\n",
                // PROG_VERSION, __DATE__, __TIME__, which_rootfs(), get_rootfs_version());

        // display the device-id, UUID, serialno, MAC and IP address to help verify the log file
        // matches the device (when in doubt) 
        // skybell_uuid(&uuid);
        // uuid_tostring(uuid, string, sizeof(string));
        // fprintf(ctxt->file, "Device-ID: '%s', UUID: '%s'\n", skybell_device_id(), string);
        // fprintf(ctxt->file, "Serial-No: '%s', MAC: '" WIFI_MACSTR "' IP: '%ld.%ld.%ld.%ld'\n", 
                // skybell_serialno(), WIFI_MAC(wifi.mac),
                // (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF);
    // }
}


// append the next log line specifed by printf() "msg" and vargs
// if level >= LOG_CONFIG.level and mask has a bit set in LOG_CONFIG.mask
//
void __log_vprintf(const void* log, int level, unsigned long mask, 
                   const char* msg, va_list ap)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return;

    ++logging;
    if (level >= ctxt->config.level && (mask & ctxt->config.mask)) {
        if (ctxt->file == NULL || ftell(ctxt->file) >= ctxt->config.bytes_per_file) {
            new_file(ctxt);
        }

        if (ctxt->file) {
            timestamp(ctxt);
            vfprintf(ctxt->file, msg, ap);
            fflush(ctxt->file);
        }
    }
    --logging;
}


// append the next log line specifed by printf() "msg" and vargs
// if level >= LOG_CONFIG.level and mask has a bit set in LOG_CONFIG.mask
//
void log_vprintf(const void* log, int level, unsigned long mask, 
                 const char* msg, va_list ap)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return;

    MUTEX_LOCK(&ctxt->mutex);
    __log_vprintf(log, level, mask, msg, ap);
    MUTEX_UNLOCK(&ctxt->mutex);
}


// append the next log line specifed by printf() "msg" and vargs
// if level >= LOG_CONFIG.level and mask has a bit set in LOG_CONFIG.mask
//
void log_printf(const void* log, int level, unsigned long mask, 
                const char* msg, ...)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return;

    MUTEX_LOCK(&ctxt->mutex);
    if (level >= ctxt->config.level && (mask & ctxt->config.mask)) {
        if (ctxt->file == NULL || 
            ftell(ctxt->file) >= ctxt->config.bytes_per_file)
            new_file(ctxt);

        if (ctxt->file) {
            va_list vargs;
            timestamp(ctxt);
            va_start(vargs, msg);
            vfprintf(ctxt->file, msg, vargs);
            va_end(vargs);
            fflush(ctxt->file);
        }
    }
    MUTEX_UNLOCK(&ctxt->mutex);
}


// append the next log line of "nbytes" of raw binary "data"
// if level >= LOG_CONFIG.level and mask has a bit set in LOG_CONFIG.mask
//
void log_data(const void* log, int level, unsigned long mask, 
              const void* data, size_t nbytes)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return;

    MUTEX_LOCK(&ctxt->mutex);
    if (level >= ctxt->config.level && (mask & ctxt->config.mask)) {
        if (ctxt->file == NULL || 
            ftell(ctxt->file) >= ctxt->config.bytes_per_file)
            new_file(ctxt);

        if (ctxt->file) {
            fwrite(data, 1, nbytes, ctxt->file);
            fflush(ctxt->file);
        }
    }
    MUTEX_UNLOCK(&ctxt->mutex);
}


// get the LOG_CONFIG settings for the "log" void* pointer returned
// by a successful call to log_open() or log_create()
//
// returns 0 for success
//
int log_getconfig(const void* log, LOG_CONFIG* config)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return -1;
    MUTEX_LOCK(&ctxt->mutex);
    *config = ctxt->config;
    MUTEX_UNLOCK(&ctxt->mutex);
    return 0;
}


// set the LOG_CONFIG settings for the "log" void* pointer
// return by a successful call to log_open() or log_create().
//
// Note: if the bytes_per_file or max_files settings change
// the # of files will be pruned the next time a file fills
// to the bytes_per_file amount (any already closed log
// files will remain at the old bytes_per_file length)
//
// returns 0 for success
//
int log_setconfig(const void* log, LOG_CONFIG* config)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;
    char filename[MAX_PATHNAME];
    FILE* file;
    u32 crc;

    // if bogus log pointer return failure
    if (log == NULL || strlen(ctxt->name) == 0)
        return -1;

    MUTEX_LOCK(&ctxt->mutex);

    // set the new configuration and close any active log files
    // (so they can adhere to the new config settings)
    strcpy(config->version, LOG_VERSION);
    ctxt->config = *config;
    if (ctxt->file) {
        fclose(ctxt->file);
        ctxt->file = NULL;
    }

    // write the new log config to flash (persistent)
    snprintf(filename, sizeof(filename), "%s/%s/%s",
             log_rootpathname, ctxt->name, ctxt->name);
    file = fopen(filename, "we");
    if (file == NULL) {
        MUTEX_UNLOCK(&ctxt->mutex);
        return -1;
    }
    if (fwrite(&ctxt->config, sizeof(ctxt->config), 1, file) != 1) {
        fclose(file);
        MUTEX_UNLOCK(&ctxt->mutex);
        return -1;
    }

    // success!
    fclose(file);

    crc32_file(filename, &crc, 0);
    strcat(filename, ".crc");
    crc32_setcrc(filename, crc);

    MUTEX_UNLOCK(&ctxt->mutex);
    return 0;
}


// caller has previously performed a log_create() to create the log directory
// and is now calling log_open() after a system reboot, if this call fails 
// (NULL return) caller will probably perform a log_create() making the, 
// hopefully correct, assumption that the log_open() call failed because 
// the log directory does not exist
//
// logname is the name of the log directory, the pathname is provided during
// system initialization through the log_init() call -- log filenames are 
// integers from "000" to LOG_CONTEXT.max_files-1 where max_files cannot exceed
// 1000 -- the smallest numbered files are the oldest
//
// returns NULL on failure!
//
void* log_open(const char* logname)
{
    char dirname[MAX_PATHNAME];
    struct stat stbuf;
    FILE* file;
    LOG_CONTEXT* ctxt;
    u32 computed_crc, file_crc;

    // if bogus logname, return failure (NULL)
    if (logname == NULL || strlen(logname) == 0)
        return NULL;

    // if the log directory does not exist return failure (NULL)
    snprintf(dirname, sizeof(dirname), "%s/%s", log_rootpathname, logname);
    if (stat(dirname, &stbuf) || !S_ISDIR(stbuf.st_mode))
        return NULL;

    // read the log context file into heap context
    strncat(dirname, "/", strlen("/"));
    strncat(dirname, logname, strlen(logname));
    file = fopen(dirname, "re");
    if (file == NULL)
        return NULL;

    // allocate heap for the log context and initialize
    ctxt = (LOG_CONTEXT*) malloc(sizeof(*ctxt));
    if (fread(&ctxt->config, sizeof(ctxt->config), 1, file) != 1) {
        printf( "%s(%s)", __FUNCTION__, logname);
        fclose(file);
        free(ctxt);
        return NULL;
    }
    fclose(file);

    crc32_file(dirname, &computed_crc, 0);
    strcat(dirname, ".crc");
    crc32_getcrc(dirname, &file_crc);

    if (computed_crc != file_crc) {
        printf("%s(%s): computed_crc(0x%lx) != file_crc(0x%lx)", 
              __FUNCTION__, logname, (long unsigned int)computed_crc, (long unsigned int)file_crc);
        free(ctxt);
        return NULL;
    }

    // made it this far, return success (non-null)
    MUTEX_INIT(&ctxt->mutex, logname);
    strncpy(ctxt->name, logname, sizeof(ctxt->name)-1);
    return ctxt; 
}


// caller must do a log_close() after a log_open() to free the log
// contxt structure and terminate logging
//
int log_close(void* log)
{
    LOG_CONTEXT* ctxt = (LOG_CONTEXT*) log;
    int retry;

    if (log == NULL || strlen(ctxt->name) == 0)
        return -1;

    MUTEX_LOCK(&ctxt->mutex);
    memset(ctxt->name, 0, sizeof(ctxt->name));
    for (retry = 0; logging && retry < 5; ++retry)
        sleep(2);
    if (ctxt->file) {
        fclose(ctxt->file);
        ctxt->file = NULL;
    }
    MUTEX_UNLOCK(&ctxt->mutex);
    MUTEX_DESTROY(&ctxt->mutex);
    memset(ctxt, 0, sizeof(*ctxt));
    free(ctxt);
    return 0;
}


// called at the beginning of the world to create log directories
// the bytes_per_file, max_files and timestamp settings can be
// changed with a call to log_setconfig()
//
// returns 0 on success
//
int log_create(const char* logname, 
               size_t bytes_per_file, int max_files, bool timestamp)
{
    char dirname[MAX_PATHNAME];
    struct stat stbuf;
    FILE* file;
    int rc;
    LOG_CONFIG cfg;
    u32 crc;

    if (logname == NULL || strlen(logname) == 0)
        return -1;

    // if the file/directory name already exists return failure
    snprintf(dirname, sizeof(dirname), "%s/%s", log_rootpathname, logname);
    if (stat(dirname, &stbuf) == 0)
        return -1;

    // set log context to default values or values passed by caller 
    // and create the log directory
    rc = mkdir(dirname, LOG_DEFAULT_DIRFLAGS);
    if (rc && errno != EEXIST) {
        printf( "%s(%s): mkdir(%s)", __FUNCTION__, logname, dirname);
        return rc;
    }

    // create the log config file
    strncat(dirname, "/", strlen("/"));
    strncat(dirname, logname, strlen(logname));
    file = fopen(dirname, "w+e");
    if (file == NULL) {
        printf( "%s(%s): fopen(%s)", __FUNCTION__, logname, dirname);
        return -1;
    }

    strcpy(cfg.version, LOG_VERSION);
    cfg.bytes_per_file = bytes_per_file;
    cfg.max_files = max_files > LOG_MAXFILES? LOG_MAXFILES : max_files;
    cfg.mask = LOG_DEFAULT_MASK;
    cfg.level = LOG_DEFAULT_LEVEL;
    if (fwrite(&cfg, sizeof(cfg), 1, file) != 1) {
        fclose(file);
        printf( "%s(%s): fwrite(%s)", __FUNCTION__, logname, dirname);
        return -1;
    }

    // made it to the end, return success!
    fclose(file);

    crc32_file(dirname, &crc, 0);
    strcat(dirname, ".crc");
    crc32_setcrc(dirname, crc);

    return 0;
}


// delete the log directory and all log files
//
// returns 0 on success
//
int log_delete(const char* logname)
{
    char dirname[MAX_PATHNAME];
    struct stat stbuf;

    if (logname == NULL || strlen(logname) == 0)
        return -1;

    // if the log directory does not exist or is not a directory fail!
    snprintf(dirname, sizeof(dirname), "%s/%s", log_rootpathname, logname);
    if (stat(dirname, &stbuf) || !S_ISDIR(stbuf.st_mode))
        return -1;

    // delete all of the files in the log directory, then delete the 
    // empty directory returning the result of the delete
    delete_files(dirname, NULL);
    return rmdir(dirname); 
}


// create a tarball of the "/skybell/misc/logs" directory storing the result in "archive_name"
// NOTE: this routine locks the logging mutexes to avoid logging taking place while attempting
// to tar the files (which would change the content of the files during the tar).
// 
// returns 0 on success, non-zero on failure
//
int log_archive(const char* archive_name)
{
    LOG_CONTEXT* sys = system_log;
    LOG_CONTEXT* dbg = debug_log;
    char tar_command[MAX_PATHNAME];
    int rc;

    if (sys != NULL) MUTEX_LOCK(&sys->mutex); 
    if (dbg != NULL) MUTEX_LOCK(&dbg->mutex); 

    // note we intentionally did not use "do_system()" here because we have locked the log mutexes
    // and do_system() will try to use the logging system (which would be a deadly embrace).  So
    // we basically build our own do_system() here without logging!
    unlink(archive_name);
    snprintf(tar_command, sizeof(tar_command), "tar -czf %s -C %s %s", archive_name, SKYBELL_MISC, LOG_PATHNAME);

    rc = system(tar_command);
    if (WIFEXITED(rc) && WEXITSTATUS(rc)) {
        printf("%s(%s): system(%s): WEXITSTATUS(%d)=%d", __FUNCTION__, archive_name, tar_command, rc, WEXITSTATUS(rc));
        rc = WEXITSTATUS(rc);
    }

    if (dbg != NULL) MUTEX_UNLOCK(&dbg->mutex); 
    if (sys != NULL) MUTEX_UNLOCK(&sys->mutex); 

    return rc;
}


int log_zap(void)
{
    if (log_restore != NULL) {
        if (debug_log != NULL) {
            void* log = debug_log;
            debug_log = NULL;
            log_close(log);
            log_delete("debug");
        }
        if (system_log != NULL) {
            void* log = system_log;
            system_log = NULL;
            log_close(log);
            log_delete("system");
        }

        log_restore();
        return 0;
    }

    return -1;
}

static char which_rootfs(void)
{
    return 1? 'A':'B';
}

static u16 get_rootfs_version(void)
{
    static u16 cached_version = 8888;
    return cached_version;
}

// log_restart
//
// Called by various modules to either start logging for the first time
// or to resume logging (called at system initialization time).
// 
// Returns non-null if logging has been activated for "logname"
// 
void* log_restart(char* logname, size_t bytes_per_file, 
                  int max_files, bool timestamp)
{
    void* log;
    LOG_CONTEXT* ctxt;
    bool dirty = FALSE;

    log = log_open(logname);
    if (log == NULL) {
        if (log_create(logname, bytes_per_file, max_files, timestamp) < 0)
            printf("log_restart(%s): log_create()", logname);
        log = log_open(logname);
    }

    ctxt = (LOG_CONTEXT*) log;
    if (ctxt == NULL || strlen(ctxt->name) == 0)
        return NULL;

    MUTEX_LOCK(&ctxt->mutex);
    if (ctxt->config.bytes_per_file != bytes_per_file ||
        ctxt->config.max_files != max_files ||
        ctxt->config.timestamp != timestamp) {
        ctxt->config.bytes_per_file = bytes_per_file;
        ctxt->config.max_files = max_files;
        ctxt->config.timestamp = timestamp;
        dirty = TRUE;
    }
    MUTEX_UNLOCK(&ctxt->mutex);

    if (dirty)
        log_setconfig(log, &ctxt->config);

    log_printf(log, LOG_MAX_LEVEL, LOG_ANY_MASK,
               "%s(%s): session 0x%lx\n", __FUNCTION__, logname, log_session);
    log_printf(log, LOG_MAX_LEVEL, LOG_ANY_MASK, 
               "v%s [%s/%s] -- rootfs-%c[version.%d]\n",
               PROG_VERSION, __DATE__, __TIME__, which_rootfs(), get_rootfs_version());
    return log;
}


// create all of the component directories in log_rootpathname,
// this routine must be called if *ANY* logging is to take place
// 
void log_init(char* rootpath, void (*restore)(void))
{
    struct timeval tv;
    char* slash = log_rootpathname;

    log_restore = restore;
    gettimeofday(&tv, NULL);
    srandom(tv.tv_usec);
    log_session = random();

    snprintf(log_rootpathname, sizeof(log_rootpathname), "%s/%s",
             rootpath, LOG_PATHNAME);

    // create all of the component directories in the path 
    for (slash = strchr(slash+1, '/'); slash != NULL;
         slash = strchr(slash+1, '/')) {
        *slash = '\0';
        mkdir(log_rootpathname, LOG_DEFAULT_DIRFLAGS);
        *slash = '/';
    }

    // make the log directory
    mkdir(log_rootpathname, LOG_DEFAULT_DIRFLAGS);

    // if (!file_exists(log_rootpathname, NULL)) {
        // not good, have to reformat /skybell/misc
        // TODO
    // }

    if (log_restore) {
        log_restore(); 
    }
}
