####################################################
# Generic makefile 
# for compiling and linking C++ projects on Linux 
###############################################################################
# define tools
SHELL = /bin/sh
#SHELL = /bin/bash
#CROSS?=arm-openwrt-linux-
CROSS?=
CC:=${CROSS}gcc
AR:=${CROSS}ar
#export CC:=/opt/vtcs_toolchain/vienna/usr/bin/arm-linux-gcc
#export RA:=/opt/vtcs_toolchain/vienna/usr/bin/arm-linux-ar
RM-F := rm -f
ECHO = echo
###############################################################################

STATIC := 0

# define flags
CURDIR := $(shell pwd)
TOPDIR = ${CURDIR}
SRCDIR = ${TOPDIR}/src
LIBTGTDIR = ${TOPDIR}/lib
INSTALL_DIR = 

TARGET := log_test

INC_PATH := -I${TOPDIR}/inc
# INC_PATH := -I${TOPDIR}/inc \
#             -I${CRYPTODIR}/include \
# 			  -I${COAPDIR}/inc \
#             -I${SRTPDIR}/inc \
#             -I${WPADIR}/inc \
#             -I${BASE64DIR}/inc \
#             -I${IPERFDIR}/inc \
#             -I${CYBTLDRDIR}/inc \
#             -I${VTCSDIR}/include \
#             -I${INIPARSERDIR}/inc \
#             -I${AUDIOTKDIR}/inc \
#             -I${ALSADIR}/include \
#             -I${MSGBROKERDIR}/inc \
#             -I${WACDIR}/inc        \
#             -I${VTCSDIR}/include/curl \
#            -I${CURLDIR}/include \
           
            

DFLAGS ?= -D_LINUX_ -D_GNU_SOURCE
CFLAGS := -g -Wall  -O3 

#LIBTGTNAME = skybell.a
#LIBTGT := ${LIBTGTDIR}/${LIBTGTNAME}
ifeq ($(STATIC), 1)
LIBTGT := ${LIBTGTDIR}/libxlog.a
else
LIBTGT := ${LIBTGTDIR}/libxlog.so
CFLAGS += -fpic
endif

LDFLAGS := -L${LIBTGTDIR} -lxlog
CFLAGS += ${INC_PATH}
#CFLAGS += -MMD -DWITH_POSIX -DSKYBELL_HD_M3C -DMICRO_V3
#CFLAGS += -DWITH_POSIX -DSKYBELL_HD_M3C -DMICRO_V3
# CXXFLAGS := $(CFLAGS)
# CXXFLAGS += ${INC_PATH}
#CXXFLAGS += -MMD -DWITH_POSIX -DSKYBELL_HD_M3C -DMICRO_V3
#CXXFLAGS += -DWITH_POSIX -DSKYBELL_HD_M3C -DMICRO_V3

###############################################################################
# define the work dir and files
SRVSRC := $(wildcard *.c) $(wildcard $(addsuffix /*.c, $(SRCDIR)))
SRVOBJ := $(patsubst %.c,%.o,$(SRVSRC))
SRVDEP := $(patsubst %.o,%.d,$(SRVOBJ))
#MISSING_DEPS := $(filter-out $(wildcard $(SRVDEP)),$(SRVDEP))
#MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))

###############################################################################
#
.PHONY : all deps objs clean veryclean rebuild info install

all: $(TARGET)

lib: $(LIBTGT)

deps : $(SRVDEP)

objs : $(SRVOBJ)

clean :
	@${RM-F} ${TARGET}
	@$(RM-F) ${SRVOBJ}
	@$(RM-F) ${SRVDEP}

cleanlib:
	@${RM-F} ${LIBTGT}
	@$(RM-F) ${SRVOBJ}
	@$(RM-F) ${SRVDEP}

$(TARGET) : $(SRVOBJ)
	$(CC) -o $(TARGET) $(SRVOBJ)  -lpthread -ldl -lm $(LDFLAGS)
$(SRVOBJ):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

ifeq ($(STATIC), 1)
$(LIBTGT): $(SRVOBJ)
	$(AR) -ruv $@ $^
else
$(LIBTGT): $(SRVOBJ)
	$(CC) -shared -o $@ $^
endif

info:
	@echo $(SRVSRC)
	@echo $(SRVOBJ)
	@echo $(SRVDEP)

install:
	@cp $(LIBTGT) ../curl_lib/
	@cp ${TOPDIR}/inc/recovery.h ../omf_include

###############################################################################

