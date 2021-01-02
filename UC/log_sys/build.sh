#!/bin/bash
gcc crc32.c log.c main.c mutex.c utils.c recovery.c -lpthread -o log_sys_test
