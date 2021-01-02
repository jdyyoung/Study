



```C
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

recover_file_system(8, 2, "ubi2_0", SKYBELL_MISC);
```



```shell
flash_eraseall /dev/mtd6 > /dev/null 2>&1
sync
ubiformat /dev/mtd6 -y > /dev/null 2>&1
ubiattach /dev/ubi_ctrl -m 6 > /dev/null 2>&1
ubimkvol /dev/ubi0 -m -N "ubi0_conf" > /dev/null 2>&1
mount -t ubifs ubi0:ubi0_conf /skybell/config

sleep 1

flash_eraseall /dev/mtd7 > /dev/null 2>&1
sync
ubiformat /dev/mtd7 -y > /dev/null 2>&1
ubiattach /dev/ubi_ctrl -m 7 > /dev/null 2>&1
ubimkvol /dev/ubi1 -m -N "ubi1_misc" > /dev/null 2>&1
mount -t ubifs ubi1:ubi1_misc /skybell/misc

mount  /dev/mmcblk0p1   /mnt/sd
sleep 1
#unzip /mnt/sd/skybell-config.zip -d /skybell/config
tar -zxvf /mnt/sd/skybell-config.tar.gz -C /skybell/config
sync

#unzip /mnt/sd/skybell-misc.zip -d /skybell/misc
tar -zxvf /mnt/sd/skybell-misc.tar.gz -C /skybell/misc
sync

```

---

```shell
ubiattach /dev/ubi_ctrl -m 6 > /dev/null 2>&1
mount -t ubifs ubi0:ubi0_conf /skybell/config
sleep 1;

ubiattach /dev/ubi_ctrl -m 7 > /dev/null 2>&1
mount -t ubifs ubi1:ubi1_misc /skybell/misc
```

