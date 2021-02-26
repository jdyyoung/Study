#ifndef _UTILS_H_
#define _UTILS_H_

extern bool  file_exists(char* filename, size_t* filesize);
extern int   delete_files(const char* dirname, char* match);
extern time_t monotonic_time(void);
extern char* trim_bol(char* line);
extern char* trim_eol(char* line);
extern void  add_to_file(char* filename, void* data, int nbytes, BOOL timestamp);

//-----------------------------------------------
#if 0

#include <MsgBroker/msg_broker.h>

#define UTILS_ROOTFS_  "nand-rootfs-"
#define UTILS_ROOTFS_A UTILS_ROOTFS_ "A"
#define UTILS_ROOTFS_B UTILS_ROOTFS_ "B"

#define UTILS_FILE_COPY_COMMAND "/bin/cp"
#define UTILS_INSMOD_COMMAND "/sbin/insmod"
#define UTILS_RMMOD_COMMAND "/sbin/rmmod"
#define UTILS_DF_COMMAND "/bin/df"
#define UTILS_MOUNT_COMMAND "/bin/mount"
#define UTILS_UMOUNT_COMMAND "/bin/umount"

#define UTILS_CMDLINE_PROCFILE "/proc/cmdline"



extern void  generate_sigsegv(void);
extern int   do_remote_system(char* filename, char* fmt, ...);
extern int   do_system(char* fmt, ...);
extern char  upper_case(char data);
extern bool  is_zero(u8* data, int sizeof_data);
extern bool  spaces(char* s);
extern u16   tohex(char* digits, int nbytes, BOOL* failed);
extern char* bin_to_hexstr(char* dst, size_t dst_len, const uint8_t *src, size_t src_len);
extern size_t hexstr_to_bin(uint8_t *dst, size_t dst_len, const char *src);
extern char* local_strncpy(char* dst, const char* src, size_t nbytes);
extern char* onoff_to_string(int on);
extern int   string_to_onoff(char* string);
extern char* lmh_to_string(u8 level);
extern int   string_to_lmh(char* string);
extern char* lmh_to_string2(u8 level);
extern int string_to_lmh2(char* string);
extern char* index_to_string(char* filename, int index, 
                             char* buffer, int buffer_size, char* notfound);
extern int   string_to_index(char* filename, char* string);
extern char* time_string(char* ascii, time_t time);
extern char* seconds_to_pretty_string(char* buffer, long seconds);
extern int   string_to_month(char* string);
extern int   set_rtc(void);
extern time_t time_of_build(char* date, char* time);
extern u32   get_maxfd(void);
extern int   set_core_limits(u32 dump_limit);
extern void  enable_core_dumps(u32 dump_limit, char* pathname);
extern void  dump_memory(char* banner, u32 addr, u8* data, int count);
extern void  dump_file(char* filename, char* header);
extern int   read_from_file(char* filename, size_t filesize, unsigned char* databuf);
extern void  dump_to_file(char* filename, void* data, int nbytes);
extern int   read_file_counter(char* filename);
extern int   increment_file_counter(char* filename);
extern int   temp_filename(char* filename, int maxlen);
extern int   file_count(char* pathname, char* match, struct list_head* hits);
extern int   delete_oldest_file(char* dirname, char* file_deleted);
extern int   copy_file(char* src_file, char* dst_file);
extern int   upload_file(char* url, char* filename, BOOL* uploaded);
extern int   newdir(char* dirname);
//extern int   mount_squashfs(char* device, char* mount_point, int retry);
extern bool  linux_cmdline(char* cmdline, int maxlen);
extern bool  rootfs_A(void);
extern bool  rootfs_B(void);
extern char  which_rootfs(void);
extern char  failed_rootfs(void);
extern u16   get_rootfs_version(void);
extern char* skybell_hwrev(void);
extern void  get_versions(int, char*, char*, char*, char*, char*);
extern int   insmod(char* module_pathname, char* options);
extern int   rmmod(char* module_pathname);
extern int   percent_disk_used(char* filesystem);
extern bool  read_only(char* filesystem);
//extern bool  uboot_has_misc_format(void);
extern int   html_escape(char* buf, size_t size, const char* str, const char* escaped);
extern int   html_unescape(char* buf, size_t size, const char* str);
extern int   url_encode(u8* source, u8* dest, int sizeof_dest);
extern i64   msec_monotonic_time(void);
extern long  get_sys_uptime(void);
extern long  get_net_uptime(void);
extern long  get_app_uptime(void);
extern void  set_app_start_time(void);
extern void  set_net_start_time(void);
extern void  set_net_down(void);
extern int   get_year(void);
extern void  get_memory_usage(u64* total, u64* used, u64* mem_free, u64* shared, u64* buffers, u64* cached);
extern void  loadavg(char* result, int sizeof_result);
extern void  close_pipe(int* pipe);

extern void  recover_file_system(int partition, int ubi_no, char* ubi_name, char* mount_point);

extern int   delayed_reboot(const char* reason, int delay_in_seconds);
extern char* extract_url_from_file(char* filename, char* url_string, int sizeof_url_string);

extern const char* skybell_device_filename(void);

extern int   compute_md5sum(char* filename, char* result);
extern int   read_md5sum_file(char* filename, char* result, int sizeof_result);

extern void  firewall_disabled(void);
extern void  firewall_restore(void);
extern void  firewall_hangup(void);
extern void  firewall_off(void);
extern void  firewall_on(void);

extern int compare_two_files(char const *file1, char const *file2, char *tmpfile, BOOL *result);

extern int get_gpio_hw_ver();
extern int MsgBroker_SendMsg_exp(const char* path, MsgContext* msg_context);
//-----------------------------------------------
#endif

#endif
