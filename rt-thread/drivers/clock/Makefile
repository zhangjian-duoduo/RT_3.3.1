include $(SDKROOT)/build/config.mk

LIBNAME := $(shell basename `pwd`)

# core
ifeq ($(CONFIG_CONFIG_ARCH_FH8862), y)
    CSRCS :=  clock.c fh_clockv2.c
else
    CSRCS :=  clock.c fh_clock.c
endif

include $(SDKROOT)/build/lib.mk
