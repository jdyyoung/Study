#/bin/sh

for i in $(seq 1 8);do
	rm -rf /mnt/diskc/xdc01_log/log_test$i
	sync
	mkdir /mnt/diskc/xdc01_log/log_test$i
	sync
	/usr/bin/log_test$i &
	sleep 1
done
