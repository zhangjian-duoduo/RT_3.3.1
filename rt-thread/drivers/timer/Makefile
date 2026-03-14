include $(SDKROOT)/build/config.mk

LIBNAME := $(shell basename `pwd`)

# core
ifeq ($(CONFIG_RT_USING_SMP),y)
CSRCS := $(wildcard arch-timer.c)
endif
CSRCS += $(wildcard fh_timer.c)

include $(SDKROOT)/build/lib.mk
