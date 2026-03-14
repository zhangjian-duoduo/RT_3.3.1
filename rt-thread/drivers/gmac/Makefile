include $(SDKROOT)/build/config.mk

LIBNAME := $(shell basename `pwd`)

ifeq ($(CONFIG_CONFIG_CHIP_FH8862), y)
CSRCS := fh_qos_gmac_main.c fh_qos_gmac_phyt.c fh_qos_gmac_ethtool.c
else
CSRCS := fh_gmac.c fh_gmac_phyt.c
endif

LOCAL_CFLAGS := -I./


include $(SDKROOT)/build/lib.mk
