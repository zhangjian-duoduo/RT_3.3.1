SDKROOT?=~/sdkv3/Moria
include $(SDKROOT)/build/config.mk
-include .config

all: make_sensor_hex

ifeq ($(CONFIG_CONFIG_ARCH_FH8862), y)
export CONFIG_ARCH_FH8862 = y
export chip = FH8862
LOCAL_LIBS += -lnna_rtt -lnnpost_rtt
else
export CONFIG_ARCH_FH8862 = n
endif

ifeq ($(CONFIG_CONFIG_ARCH_FH885xV310), y)
export CONFIG_ARCH_FH885xV310 = y
export chip = FH885xV310
LOCAL_LIBS += -lnna_rtt -lnnpost_rtt
else
export CONFIG_ARCH_FH885xV310 = n
endif

ifeq ($(CONFIG_CONFIG_ARCH_FH885xV300), y)
export CONFIG_ARCH_FH885xV300 = y
export chip = FH885xV310
LOCAL_LIBS += -lnna_rtt -lnnpost_rtt
else
export CONFIG_ARCH_FH885xV300 = n
endif

ifeq ($(CONFIG_CONFIG_ARCH_FH8636_FH8852V20X), y)
ifeq ($(CONFIG_CONFIG_CHIP_FH8626V200), y)
export CONFIG_CHIP_FH8626V200 = y
export chip = FH8626V200
export CONFIG_ARCH_FH8636_FH8852V20X = n
LOCAL_LIBS += -lnna_rtt -lnnpost_rtt
else
export CONFIG_CHIP_FH8626V200 = n
export CONFIG_ARCH_FH8636_FH8852V20X = y
export chip = FH8636_FH8852V20X
LOCAL_LIBS += -lnna_rtt -lnnpost_rtt
endif
else
export CONFIG_ARCH_FH8636_FH8852V20X = n
endif

SAMPNAME := $(shell basename `pwd`)

SAMP_SRCS := $(wildcard startup/*.c)
LOCAL_CFLAGS += -I$(shell pwd)

SAMP_SRCS += $(wildcard common/dsp/*.c)
SAMP_SRCS += $(wildcard common/isp/*.c)
SAMP_SRCS += $(wildcard common/usb_update/*.c)
SAMP_SRCS += $(wildcard common/bind/src/*.c)
SAMP_SRCS += $(wildcard common/isp/start_sensor/start_sensor_v2.c)

LOCAL_CFLAGS = -I./
LOCAL_CFLAGS += -Icommon
LOCAL_CFLAGS += -Icommon/inc
LOCAL_CFLAGS += -Icommon/usb_update
LOCAL_CFLAGS += -Icomponents/
LOCAL_CFLAGS += -I$(shell pwd)

ifeq ($(CONFIG_APP_USB_DEVICE_UAC),y)
    SAMP_SRCS += $(wildcard usb_audio/*.c)
    LOCAL_CFLAGS += -Iusb_audio/
endif

ifeq ($(CONFIG_APP_USB_DEVICE_CDC),y)
    SAMP_SRCS += $(wildcard usb_vcom/*.c)
    LOCAL_CFLAGS += -Iusb_vcom/
endif

ifeq ($(CONFIG_APP_USB_DEVICE_HID),y)
    SAMP_SRCS += $(wildcard usb_hid/*.c)
    LOCAL_CFLAGS += -Iusb_hid/
endif
# overlay code for chip before ZT
ifeq ($(CONFIG_APP_USB_DEVICE_UVC), y)
    SAMP_SRCS += $(wildcard usb_video/*.c)
    LOCAL_CFLAGS += -Iusb_video/
endif

ifeq ($(CONFIG_FH_APP_OPEN_OVERLAY), y)
    SAMP_SRCS += $(wildcard common/overlay/*.c)
    LOCAL_CFLAGS += -Icommon/overlay/
endif

# Crosshair feature (requires OVERLAY to be enabled)
ifeq ($(CONFIG_FH_APP_OVERLAY_CROSSHAIR), y)
    LOCAL_CFLAGS += -DUVC_ENABLE_CROSSHAIR=1
endif

ifeq ($(CONFIG_FH_APP_OPEN_NN_DETECT), y)
    SAMP_SRCS += $(wildcard common/fh_nn_detect/sample_nna_detect.c)
    LOCAL_CFLAGS += -Icommon/fh_nn_detect/
endif

ifeq ($(CONFIG_FH_APP_OPEN_NN_C2D), y)
    SAMP_SRCS += $(wildcard common/fh_nn_detect/sample_nna_detect.c)
    LOCAL_CFLAGS += -Icommon/fh_nn_detect/
endif

ifeq ($(CONFIG_FH_APP_OPEN_NN_FACE), y)
    SAMP_SRCS += $(wildcard common/fh_nn_detect/sample_nna_detect.c)
    LOCAL_CFLAGS += -Icommon/fh_nn_detect/
endif

LOCAL_CFLAGS += -D__RTTHREAD_OS__

LDFLAGS += -L$(SDKROOT)/lib/$(SOC_LINK)/lib/sensors

LOCAL_LIBS += -lm -lsensor_rtt -lvb_mpi_arm_rtt

ifeq ($(CONFIG_FH_USING_JXF22_MIPI),y)
LOCAL_LIBS += -ljxf22_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_JXF23_MIPI),y)
LOCAL_LIBS += -ljxf23_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_JXF23_DVP),y)
LOCAL_LIBS += -ljxf23_dvp_rtt
endif

ifeq ($(CONFIG_FH_USING_JXF23_IR),y)
LOCAL_LIBS += -ljxf23_mipi_ir_rtt
endif

ifeq ($(CONFIG_FH_USING_JXF32_MIPI),y)
LOCAL_LIBS += -ljxf32_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,jxf32_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_JXF37_MIPI),y)
LOCAL_LIBS += -ljxf37_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_JXH62_MIPI),y)
LOCAL_LIBS += -ljxh62_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_SC2231_DVP_MIPI),y)
LOCAL_LIBS += -lsc2231_dvp_rtt
endif

ifeq ($(CONFIG_FH_USING_OV2732_MIPI),y)
LOCAL_LIBS += -lov2732_mipi_rtt
endif

# !!!! ATTENTION !!!!
# for HL and later, MUST add local link options: -Wl,-u,$(SENSOR_NAME)_sensor_if

ifeq ($(CONFIG_FH_USING_OVOS05_MIPI),y)
LOCAL_LIBS += -lovos05_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ovos05_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_OVOS04C10_MIPI),y)
LOCAL_LIBS += -lovos04c10_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ovos04c10_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_OVOS02K_MIPI),y)
LOCAL_LIBS += -lovos02k_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ovos02k_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_GC2083_MIPI),y)
LOCAL_LIBS += -lgc2083_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,gc2083_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_OVOS02D_MIPI),y)
LOCAL_LIBS += -lovos02d_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ovos02d_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_OVOS08_MIPI),y)
LOCAL_LIBS += -lovos08_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ovos08_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_OV2735_MIPI),y)
LOCAL_LIBS += -lov2735_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,ov2735_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_GC4623_MIPI),y)
LOCAL_LIBS += -lgc4623_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_GC4633_MIPI),y)
LOCAL_LIBS += -ljGC4633_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_PS5270_MIPI),y)
LOCAL_LIBS += -lps5270_mipi_rtt
endif

ifeq ($(CONFIG_FH_USING_MN34425_MIPI),y)
LOCAL_LIBS += -lmn34425_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,mn34425_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_SC3335_MIPI),y)
LOCAL_LIBS += -lsc3335_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,sc3335_mipi_sensor_if
endif

ifeq ($(CONFIG_FH_USING_IMX415_MIPI),y)
LOCAL_LIBS += -limx415_mipi_rtt
LOCAL_LINKFLAGS += -Wl,-u,imx415_mipi_sensor_if
endif

include $(SDKROOT)/build/apps.mk

menuconfig: menuconfig_rtt make_sensor_hex

menuconfig_rtt:
	@$(SDKROOT)/build/kconfig-frontends/kconfig-mconf Kconfig
	@python $(SDKROOT)/build/tools/cfg2hdr.py .config rtconfig_app.h

make_sensor_hex:
	@chmod +x common/script/sensor_hex_rtt.py
	@common/script/sensor_hex_rtt.py rtconfig_app.h $(chip) $(SDKROOT)
	@chmod 644 common/script/sensor_hex_rtt.py
