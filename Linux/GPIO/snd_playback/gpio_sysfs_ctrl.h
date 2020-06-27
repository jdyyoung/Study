/**
 *
 * Copyright (c) 2015 VATICS INC.
 * All Rights Reserved.
 *
 * @brief		GPIO sysfs control interface header file.
 * @author		Max Chang
 * @date		2015/10/30
 *
 */
#ifndef _GPIO_SYSFS_CTRL_H
#define _GPIO_SYSFS_CTRL_H

#define SYSFS_GPIO_DIR 			"/sys/class/gpio"
#define MAX_BUF 				64

#define GPIO_EDGE_RISING		"rising"

#ifdef __cplusplus
extern "C" {
#endif

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);

int gpio_set_dir(unsigned int gpio, unsigned int out_flag);

int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);

int gpio_set_edge(unsigned int gpio, const char *edge);

int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
