#!/bin/sh

allSousrceLedOff(){
	echo 0 > /sys/class/leds/opt-led/brightness
	echo 0 > /sys/class/leds/bt-led/brightness
	echo 0 > /sys/class/leds/aux-led/brightness
	echo 0 > /sys/class/leds/wifi-led/brightness
}

wifiSourceLed(){
	allSousrceLedOff
	echo 1 > /sys/class/leds/wifi-led/brightness
}

optSourceLed(){
	allSousrceLedOff
	echo 1 > /sys/class/leds/opt-led/brightness
}

auxSourceLed(){
	allSousrceLedOff
	echo 1 > /sys/class/leds/aux-led/brightness
}

muteOnoff(){
	muteState=`cat /sys/class/leds/mute-led/brightness`
	if [ $muteState = 0 ];then
		echo 1 >  /sys/class/leds/mute-led/brightness
	else
		echo 0 >  /sys/class/leds/mute-led/brightness
	fi
}
btSourceLed(){
	allSousrceLedOff
	echo 1 > /sys/class/leds/bt-led/brightness
}

btLedQuickBlink(){
	allSousrceLedOff
	echo timer > /sys/class/leds/bt-led/trigger
	echo 100 > /sys/class/leds/bt-led/delay_on
	echo 100 > /sys/class/leds/bt-led/delay_off
}

btLedSlowBlink(){
	allSousrceLedOff
	echo timer > /sys/class/leds/bt-led/trigger
	echo 1000 > /sys/class/leds/bt-led/delay_on
	echo 1000 > /sys/class/leds/bt-led/delay_off
}

echo "$0:\$1=$1"
case $1 in
	"wifi") wifiSourceLed ;;
	"optical") optSourceLed ;;
	"line-in") auxSourceLed ;;
	"mute") muteOnoff ;;
	"bt") btSourceLed ;;
	"btMatching") btLedQuickBlink ;;
	"btBroken") btLedSlowBlink ;;
	*)echo "#####arg error:gpio led no $1 control######";;
esac

