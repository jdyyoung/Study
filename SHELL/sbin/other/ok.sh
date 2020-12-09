#!/bin/sh

tar -cvf avs-sugr-package20180211.tar -C /
tar -cvf server-sense.tar -C /

tar -cvf led_key.tar -C /
tar -cvf ntp.tar -C /

cp /etc/init.d/S91avs-login /etc/rc5.d/
cp /etc/init.d/S70sense /etc/rc5.d/
