/* Copyright � 2015 HiTEM Engineering, Inc.  Skybell, Inc.
 * Proprietary information, NDA required to view or use this software.  
 * All rights reserved.
 */

#ifndef _FILENAMES_H_
#define _FILENAMES_H_

#ifndef MAX_PATHNAME
#define MAX_PATHNAME     512
#endif

#define FILE_ENCRYPTION 1

// could be a little smarter here and process the "/proc/cmdline" mtdparts= option
// to dynamically ascertain the partitions (TODO)
#define LOADER_DEV            "/dev/mtd0"
#define UBOOT_ENV_DEV         "/dev/mtd1"
#define KERNEL_A_DEV          "/dev/mtd2"
#define KERNEL_B_DEV          "/dev/mtd3"
#define ROOTFS_A_DEV          "/dev/mtd4"
#define ROOTFS_B_DEV          "/dev/mtd5"
#define ROOTFS_A_BLKDEV       "/dev/mtdblock4"
#define ROOTFS_B_BLKDEV       "/dev/mtdblock5"

//#define SKYBELL_ROOT          "/skybell"
#define SKYBELL_ROOT          "/tmp"
#define SKYBELL_CONF          SKYBELL_ROOT "/config"

#define SKYBELL_MISC          SKYBELL_ROOT "/misc"
#define SKYBELL_LOGS          SKYBELL_MISC "/logs"
//#define SKYBELL_OTA           SKYBELL_MISC "/ota"
#define SKYBELL_CORE          SKYBELL_MISC "/core"
#define SKYBELL_OOPS          SKYBELL_MISC "/oops"
#define SKYBELL_REBOOTS       SKYBELL_MISC "/reboot_cycles"
#define SKYBELL_CTEMPS        SKYBELL_MISC "/temperature.txt"
#define SKYBELL_OTEMPS        SKYBELL_MISC "/old-temperature.txt"
#define SKYBELL_CBATT         SKYBELL_MISC "/battery.txt"
#define SKYBELL_OBATT         SKYBELL_MISC "/old-battery.txt"
#define SKYBELL_CMOTION       SKYBELL_MISC "/motion.txt"
#define SKYBELL_OMOTION       SKYBELL_MISC "/old-motion.txt"
#define SKYBELL_REBOOT        SKYBELL_LOGS "/reboot.txt"
#define SKYBELL_RESTART       SKYBELL_LOGS "/restart.txt"
#define SKYBELL_SNAPSHOT      "/tmp/snapshot"
#define SKYBELL_VIDEOCAP      "/tmp/videocap"
#define SKYBELL_VBACKLOG      "/tmp/vbacklog"
#define SKYBELL_AUDIO_IN      SKYBELL_VIDEOCAP "/audio.in"
#define SKYBELL_AUDIO_OUT     SKYBELL_VIDEOCAP "/audio.out"
#define SKYBELL_META_DATA     "metadata.json"

#define SKYBELL_REBOOT_COUNT  SKYBELL_MISC "/reboot-count.bin"
#define SKYBELL_RESTART_COUNT SKYBELL_MISC "/restart-count.bin"

#define SKYBELL_OTA						 SKYBELL_MISC "/ota"
#define SKYBELL_OTA_TAR_GZ				 SKYBELL_MISC "/ota/new-ota-image.tar.gz"
#define SKYBELL_OTA_PATH_AND_FILE        SKYBELL_MISC "/ota/new-ota-image"
#define SKYBELL_OTA_FILE                 "new-ota-image"

//#define SKYBELL_OTA_PATH_AND_FILE        SKYBELL_OTA "/" SKYBELL_OTA_FILE
#define SKYBELL_SECURE_OTA_FILE          "secure_rootfs"
#define SKYBELL_SECURE_OTA_PATH_AND_FILE SKYBELL_OTA "/" SKYBELL_SECURE_OTA_FILE
#define SKYBELL_TEST_OTA_FILE            "test-ota-image"
#define SKYBELL_TEST_OTA_PATH_AND_FILE   SKYBELL_OTA "/" SKYBELL_TEST_OTA_FILE
#define SKYBELL_TORRENT_FILE             "rootfs.torrent"
#define SKYBELL_TORRENT_PATH_AND_FILE    SKYBELL_OTA "/" SKYBELL_TORRENT_FILE
#define SKYBELL_TORRENT_RETRY_FILE       SKYBELL_OTA "/" "torrent.retry"

//extract the sqfs image into /skybell/misc/ota because tmp may not be big enough to hold
#define SKYBELL_OTA_ROOTFS_NAME          "rootfs.sqfs"
#define SKYBELL_OTA_ROOTFS               SKYBELL_OTA "/" SKYBELL_OTA_ROOTFS_NAME
#define SKYBELL_OTA_SIG_ALG              "sha256"
#define SKYBELL_OTA_ROOTFS_SIG           SKYBELL_OTA_ROOTFS "." SKYBELL_OTA_SIG_ALG
#define SKYBELL_CERT_NAME                "skybell.cert.pem"
#define SKYBELL_OTA_X509_CERT            "/firmware" "/" SKYBELL_CERT_NAME
#define SKYBELL_OTA_X509_PUB_KEY         "/tmp" "/" SKYBELL_CERT_NAME

#define SKYBELL_TMP_OTA                  "/tmp/ota"
#define SKYBELL_TMP_OTA_ROOTFS           SKYBELL_TMP_OTA "/" SKYBELL_OTA_ROOTFS_NAME
#define SKYBELL_TMP_OTA_ROOTFS_SIG       SKYBELL_TMP_OTA_ROOTFS "." SKYBELL_OTA_SIG_ALG

#define SKYBELL_RINGTONE                 SKYBELL_CONF "/ringtone"
// #define SKYBELL_RINGTONE 					"/skybell/wav"

#define SKYBELL_OVERRIDE_SETTINGS_JSON          "/override_settings.json"

// pimp.hex -- STMicro application
// pimp-loader.hex -- STMicro loader
// pimp-loader-loader.hex -- STMicro special application that loads a loader 
#define SKYBELL_STMICRO_FIRMWARE                SKYBELL_CONF "/pimp.hex"
#define SKYBELL_STMICRO_LOADER                  SKYBELL_CONF "/pimp-loader.hex"
#define SKYBELL_STMICRO_LOADER_LOADER           SKYBELL_CONF "/pimp-loader-loader.hex"
#define SKYBELL_ROOTFS_STMICRO_FIRMWARE         "/firmware/pimp.hex"
#define SKYBELL_ROOTFS_STMICRO_LOADER           "/firmware/pimp-loader.hex"
#define SKYBELL_ROOTFS_STMICRO_LOADER_LOADER    "/firmware/pimp-loader-loader.hex"
#define SKYBELL_ROOTFS_OVERRIDE_SETTINGS        "/firmware" SKYBELL_OVERRIDE_SETTINGS_JSON

#ifdef SKYBELL_HD
#define SKYBELL_UBOOT_LOADER                    "NF2K_Output.bin"
#else
#define SKYBELL_UBOOT_LOADER                    "SN_Output.bin"
#endif             
#define SKYBELL_UBOOT_PATHNAME                  SKYBELL_CONF "/" SKYBELL_UBOOT_LOADER
#define SKYBELL_SECURE_UBOOT_PATHNAME           SKYBELL_CONF "/secure_" SKYBELL_UBOOT_LOADER
#define SKYBELL_LINUX_KERNEL                    "zImage"             
#define SKYBELL_LINUX_PATHNAME                  SKYBELL_MISC "/" SKYBELL_LINUX_KERNEL
#define SKYBELL_APPLICATION                     "skybell"             
#define SKYBELL_APP_LOAD_PATHNAME               SKYBELL_MISC "/" SKYBELL_APPLICATION
#define SKYBELL_APP_INSTALL_PATHNAME            SKYBELL_CONF "/" SKYBELL_APPLICATION
#define SKYBELL_MCU_BIN                         "zilog_img.bin"
#define SKYBELL_MCU_PATHNAME                    SKYBELL_MISC  "/mcu"
#define SKYBELL_VIENNA_TAR                      "vienna.tar.gz"
#define SKYBELL_VERIFY_OTA_FILENAME             "ota_verification"

// settings filename prefixes (note these don't include the suffix which is either ".json" or ".crc")
#define SKYBELL_SETTINGS                        SKYBELL_CONF "/settings"
#define SKYBELL_SETTINGS_BACKUP                 SKYBELL_CONF "/backup"
#define SKYBELL_SYSTEM_SETTINGS                 SKYBELL_CONF "/system_settings"
#define SKYBELL_SYSTEM_SETTINGS_BACKUP          SKYBELL_CONF "/system_backup"
#define SKYBELL_MEDIA_SETTINGS                  SKYBELL_CONF "/media_settings"
#define SKYBELL_MEDIA_SETTINGS_BACKUP           SKYBELL_CONF "/media_backup"
#define SKYBELL_OVERRIDE_SETTINGS               SKYBELL_CONF SKYBELL_OVERRIDE_SETTINGS_JSON

#define SKYBELL_REVERT                          SKYBELL_CONF   "/revert"
#define SKYBELL_REVERT_SETTINGS                 SKYBELL_REVERT "/settings"
#define SKYBELL_REVERT_SETTINGS_BACKUP          SKYBELL_REVERT "/backup"
#define SKYBELL_REVERT_SYSTEM_SETTINGS          SKYBELL_REVERT "/system_settings"
#define SKYBELL_REVERT_SYSTEM_SETTINGS_BACKUP   SKYBELL_REVERT "/system_backup"
#define SKYBELL_REVERT_MEDIA_SETTINGS           SKYBELL_REVERT "/media_settings"
#define SKYBELL_REVERT_MEDIA_SETTINGS_BACKUP    SKYBELL_REVERT "/media_backup"
#define SKYBELL_REVERT_OVERRIDE_SETTINGS        SKYBELL_REVERT SKYBELL_OVERRIDE_SETTINGS_JSON

// skybell UUID -- kept for all time?
#define SKYBELL_UUID_FILE                       SKYBELL_CONF "/uuid.bin"

// filename of invite_token received by mobile application through CoAP and used
#define SKYBELL_INVITE_TOKEN                    SKYBELL_CONF "/invite_token.json"
#define SKYBELL_PROVISION                       SKYBELL_CONF "/provision.json"
#define TMP_SKYBELL_PROVISION                   "/tmp/provision.json"
#define SKYBELL_RESOURCE                        SKYBELL_CONF "/resource.json"
#define SKYBELL_ACCESS_TOKEN                    SKYBELL_CONF "/access_token.json"
#define SKYBELL_DEVICE_ID                       SKYBELL_CONF "/device_id.json"

#define SKYBELL_DEVICE_NAME                     SKYBELL_CONF "/device_name.txt"

#define SKYBELL_HMAC_KEY                        SKYBELL_CONF "/hmac_key.txt"


typedef struct {
    struct list_head  list;
    char              name[128];
} FILE_LIST;

#endif
