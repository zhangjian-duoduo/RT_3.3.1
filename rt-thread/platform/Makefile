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
ifeq ($(CONFIG_CONFIG_CHIP_FH8852V201), y)
    CSRCS += $(wildcard fh8852v201/fastboot/*.c)
endif
ifeq ($(CONFIG_CONFIG_CHIP_FH8852V310), y)
    CSRCS += $(wildcard fh8852v310/fastboot/*.c)
endif
ifeq ($(CONFIG_CONFIG_CHIP_FH8856V310), y)
    CSRCS += $(wildcard fh8856v310/fastboot/*.c)
endif
ifeq ($(CONFIG_CONFIG_CHIP_FH8858V310), y)
    CSRCS += $(wildcard fh8858v310/fastboot/*.c)
endif
endif

include $(SDKROOT)/build/lib.mk
