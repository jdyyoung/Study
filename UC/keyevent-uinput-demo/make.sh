#!/bin/sh

arm-linux-gnueabihf-gcc -o getevent getevent.c
arm-linux-gnueabihf-gcc -o test_send test_send.c uinput.c
