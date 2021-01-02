####################################################
# Generic makefile -
# for compiling and linking C++ projects on Linux 
# Author: xxx Check: bob
###############################################################################
# define tools
#
SHELL = /bin/sh
##SHELL = /bin/bash
#export CROSS?=arm-linux-
export CC:=/opt/vtcs_toolchain/vienna/usr/bin/arm-linux-gcc
export RA:=/opt/vtcs_toolchain/vienna/usr/bin/arm-linux-ar
#
RM-F := rm -f
ECHO = echo
###############################################################################
# define flags
#
CURDIR := $(shell pwd)
TOPDIR = ${CURDIR}
SRCDIR = ${TOPDIR}/src
LIBTGTDIR = ${TOPDIR}/lib

TARGET := srtp.bin
LIBTGTNAME = libsrtp.a
LIBTGT := ${LIBTGTDIR}/${LIBTGTNAME}

LIBDIR := ${TOPDIR}/../crypto


LIBS := ${LIBDIR}/libcryptomodule.a
#LIBS += ${LIBDIR}/libCommon.a

INC_DIR = ${TOPDIR}/inc
COMMON_DIR = ${TOPDIR}/../crypto/include
#CACHE_DIR = ${TOPDIR}/../cache/inc

INC_PATH := -I${INC_DIR} \
            -I${COMMON_DIR} \
      #      -I${CACHE_DIR}

DFLAGS ?= -D_LINUX_
CFLAGS := -g -Wall -O3 -DHAVE_CONFIG_H
#-lpthread
#CPPFLAGS := $(CFLAGS)
CFLAGS += ${INC_PATH}
CFLAGS += -MMD
CXXFLAGS := $(CFLAGS)
CXXFLAGS += ${INC_PATH}
CXXFLAGS += -MMD

###############################################################################
# define the work dir and files
#
SRVSRC := $(wildcard *.c) $(wildcard $(addsuffix /*.c, $(SRCDIR)))
SRVOBJ := $(patsubst %.c,%.o,$(SRVSRC))
SRVDEP := $(patsubst %.o,%.d,$(SRVOBJ))
#MISSING_DEPS := $(filter-out $(wildcard $(SRVDEP)),$(SRVDEP))
#MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))

###############################################################################
#
.PHONY : all deps objs clean veryclean rebuild info

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

-include $(SRVDEP)
$(TARGET) : $(SRVOBJ)
	$(CC) -o $(TARGET) $(SRVOBJ) -lpthread -ldl -lm $(LIBS)
$(SRVOBJ):%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread -ldl -lm $(LIBS)

$(LIBTGT): $(SRVOBJ)
	$(RA) -ruv $@ $^

info:
	@echo $(SRVSRC)
	@echo $(SRVOBJ)
	@echo $(SRVDEP)
#	@echo $(MISSING_DEPS)
#	@echo $(MISSING_DEPS_SOURCES)

###############################################################################

