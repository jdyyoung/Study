#ifndef _LIB_H_
#define _LIB_H_

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#ifdef SKYBELL_HD
#include <sys/sysinfo.h>
#endif
#include <sys/stat.h>
#include <linux/reboot.h>
#include <linux/netlink.h>
#include <linux/input.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <dirent.h>
#include <time.h>
#include <termios.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <mtd/mtd-abi.h>
#include <mtd/mtd-user.h>

#include "list.h"
#include "mutex.h"
#include "log.h"
#include "typedef.h"
#include "recovery.h"
#include "crc32.h"
// #include "filenames.h"
#include "utils.h"


#endif
