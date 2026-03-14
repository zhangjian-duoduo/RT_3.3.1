include $(SDKROOT)/build/config.mk

# LIBNAME := board
LIBNAME := $(shell basename `pwd`)

# core
CSRCS := $(wildcard $(SOC_NAME)/*.c)
CSRCS += $(wildcard common/*.c)
CSRCS += $(wildcard loader/*.c)
CSRCS += $(wildcard startup/*.c)

CSRCS += $(wildcard $(SOC_NAME)/$(BOARD_NAME)/*.c)

ifeq ($(CONFIG_FH_FAST_BOOT), y)
    CSRCS += $(wildcard fastboot/*.c)
endif

include $(SDKROOT)/build/lib.mk
