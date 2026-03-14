export CHIP_ID ?= HL
export RTT ?= 0
export STATIC ?= 1
export DEBUG ?= 0
export CROSS_COMPILE ?= arm-fullhan-linux-uclibcgnueabi-
export RTT_CROSS_COMPILE ?= arm-none-eabi-
export RTT_ARC_CROSS_COMPILE ?=arc-fullhan-elf32-
export RTOSV3 ?= 1
export FPGA_ENV ?= 0
export LOW_FPS ?= 0
export BUILD_MODE ?= 0
export LIB_ROOT_EN ?= 0
###### new add ###########
TOP_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

INSTALL_DIR ?= /home/nfs/opt/test
DSP_ROOT ?= $(HOME)/work/dsp
ISP_ROOT ?= $(HOME)/work/isp
LIBPATH=./


SENSOR_NAME?=$(shell pwd | sed 's/^\(.*\)[/]//')

ifeq ($(SENSOR_NAME),sensor_common)
SENSOR_NAME = sensor
endif

ifneq (,$(filter $(BUILD_MODE),1 5))
RTT_TARGET = $(LIBPATH)/lib$(SENSOR_NAME)_rtt.a
TARGET_BIN = $(RTT_TARGET)
endif
ifneq (,$(filter $(BUILD_MODE),2 3))
RTT_TARGET = $(LIBPATH)/lib$(SENSOR_NAME)_rtt.a
TARGET_BIN = $(RTT_TARGET)
RPC_RTT=1
endif
ifneq (,$(filter $(BUILD_MODE),0 4))
LINUX_TARGET = $(LIBPATH)/lib$(SENSOR_NAME).a
TARGET_BIN = $(LINUX_TARGET)
endif
CSRC= $(wildcard *.c)
TEMSRC= $(wildcard *.hex)
TEMSRC := $(patsubst %.hex,%.c,$(TEMSRC))
#CSRC+=../sensor_common/sensor_ops.c
#CSRC+=../sensor_common/clock_ops.c
EXTRA_CFLAGS += -Wall -Werror -DISP -DARMLIB
EXTRA_CFLAGS += -I$(ISP_ROOT)/include/isp \
		-I$(ISP_ROOT)/include/isp_ext \
		-I$(DSP_ROOT)/include \
		-I$(TOP_DIR)/sensor_common \

obj-m = $(patsubst %.ko,%.o,$(LINUX_TARGET))
$(patsubst %.ko,%,$(LINUX_TARGET))-y = $(patsubst %.c,%.o,$(CSRC))

ifeq ($(MS),1)
EXTRA_CFLAGS += -DMULTI_SENSOR=1
endif

ifeq ($(EMU),1)
EXTRA_CFLAGS += -DEMU
endif

ifeq ($(FPGA_ENV),1)
EXTRA_CFLAGS+=-DFPGA_ENV
endif

ifeq ($(LOW_FPS),1)
EXTRA_CFLAGS+=-DLOW_FPS
endif

ifeq ($(RTOSV3),1)
EXTRA_CFLAGS+=-DRTOSV3
endif

ifeq ($(LIB_ROOT_EN), 1)
EXTRA_CFLAGS += -I$(LIB_ROOT)/isp
EXTRA_CFLAGS += -I$(LIB_ROOT)/isp_ext
EXTRA_CFLAGS += -I$(LIB_ROOT)/dsp
EXTRA_CFLAGS += -I$(LIB_ROOT)/
EXTRA_CFLAGS += -I$(LIB_ROOT)/../../inc
endif

ifeq ($(RTT),1)
export BUILD_MODE=1
endif
#############for RTT############################################
ifneq ($(RTT_TARGET),)
CC = $(RTT_CROSS_COMPILE)gcc
AR = $(RTT_CROSS_COMPILE)ar
EXTRA_CFLAGS+=$(RTT_EXTRA_CFLAGS)
EXTRA_CFLAGS+=-DOS_RTT
EXTRA_CFLAGS+=-D${CHIP_ID}
ifneq ($(filter $(CHIP_ID),XGM XGM2 XGMPLUS),)
EXTRA_CFLAGS+=-DSTITCH_SUPPORT
endif

ifeq ($(RTT3),1)
EXTRA_CFLAGS+=-DUSE_RTT3_0
endif

ifeq ($(RPC_RTT),1)
export RTT_CROSS_COMPILE=$(RTT_ARC_CROSS_COMPILE)
export ARCH=ARC
EXTRA_CFLAGS+=-DRPC_RTT
endif

ifeq ($(ARCH),arm)
CFLAGS+= -mno-unaligned-access -ffunction-sections -fdata-sections -fno-omit-frame-pointer -mapcs
ifeq ($(A7),1)
CFLAGS+= -mcpu=cortex-a7
else
CFLAGS+= -mcpu=arm1176jzf-s
endif
ifeq ($(VFP),1)
	ifeq ($(A7),1)
	CFLAGS += -mfpu=neon-vfpv4 -mfloat-abi=hard
	else
	CFLAGS += -mfpu=vfp -mfloat-abi=softfp
	endif
else
	CFLAGS += -mfloat-abi=soft
endif
else # ($(ARCH),arc)
CFLAGS+= -mno-sdata -mA6 -mmul32x16 -mno-volatile-cache
AFLAGS = ' -c  -mno-sdata -mA6 -x assembler-with-cpp'
endif

ifeq ($(DEBUG),1)
CFLAGS += -O0 -gdwarf-2 -DRT_USING_NEWLIB
else
CFLAGS += -O2
endif
ifeq ($(RPC_RTT),1)
else
CFLAGS+=-fPIC
endif
CFLAGS+=$(EXTRA_CFLAGS)

ifeq ($(RPC_RTT),1)
CFLAGS += -I$(RPC_ROOT)/
CFLAGS += -I$(RPC_ROOT)/lib/inc/rt-thread
CFLAGS += -I$(RPC_ROOT)/lib/inc/rt-thread/libc/
CFLAGS += -I$(RPC_ROOT)/platform/inc
CFLAGS += -I$(RPC_ROOT)/drivers/inc
CFLAGS += -I$(RPC_ROOT)/
CFLAGS += -I$(RPC_ROOT)/lib/inc
else
ifeq ($(RTOSV3),1)
CFLAGS += -I$(RTT_ROOT)/
CFLAGS += -I$(RTT_ROOT)/out/rtconfig/
CFLAGS += -I$(RTT_ROOT)/kernel/include
CFLAGS += -I$(RTT_ROOT)/drivers/arch/drivers
CFLAGS += -I$(RTT_ROOT)/drivers/arch/
CFLAGS += -I$(RTT_ROOT)/drivers/include
CFLAGS += -I$(RTT_ROOT)/shell
CFLAGS += -I$(RTT_ROOT)/platform/cpu/arm/armv6
CFLAGS += -I$(RTT_ROOT)/platform/cpu/arm/common
CFLAGS += -I$(RTT_ROOT)/fs/include
CFLAGS += -I$(RTT_ROOT)/kernel/libc/compilers/newlib
CFLAGS += -I$(RTT_ROOT)/platform/include
CFLAGS += -I$(RTT_ROOT)/compat/include
# CFLAGS += -I$(RTT_ROOT)/components/drivers/spi
else
CFLAGS += -I$(RTT_ROOT)/
CFLAGS += -I$(RTT_ROOT)/include
CFLAGS += -I$(RTT_ROOT)/libcpu/arm/armv6
CFLAGS += -I$(RTT_ROOT)/libcpu/arm/common
CFLAGS += -I$(RTT_ROOT)/components/dfs
CFLAGS += -I$(RTT_ROOT)/components/dfs/include
CFLAGS += -I$(RTT_ROOT)/components/dfs/filesystems/devfs
CFLAGS += -I$(RTT_ROOT)/components/drivers/include
CFLAGS += -I$(RTT_ROOT)/components/drivers/spi
CFLAGS += -I$(RTT_ROOT)/components/finsh
ifeq ($(RTT3),1)
CFLAGS += -I$(RTT_ROOT)/components/libc/compilers/newlib
CFLAGS += -I$(RTT_ROOT)/components/libc/libdl
else
CFLAGS += -I$(RTT_ROOT)/components/libc/newlib
CFLAGS += -I$(RTT_ROOT)/components/libdl
endif

CFLAGS += -I$(RTT_ROOT)/components/libc/pthreads
CFLAGS += -I$(SDK_ROOT)/platform/startup/gcc/
CFLAGS += -I$(SDK_ROOT)/drivers
CFLAGS += -I$(RTT_ROOT)/../
endif
endif
endif


ifneq ($(LINUX_TARGET),)
CC = $(CROSS_COMPILE)gcc
AR = $(LINUX_CROSS_COMPILE)ar
EXTRA_CFLAGS += -DOS_LINUX
EXTRA_CFLAGS+=-D${CHIP_ID}
ifneq ($(filter $(CHIP_ID),XGM XGM2 XGMPLUS),)
EXTRA_CFLAGS+=-DSTITCH_SUPPORT
endif
CFLAGS += $(EXTRA_CFLAGS)
ifneq ($(ARM_CORTEXA7_VFP),)
CFLAGS  += -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
LDFLAGS += -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
endif
endif

ifeq ($(STATIC),0)
ifneq ($(LINUX_TARGET),)
CFLAGS +=-fPIC
TARGET_CP =$(basename $(TARGET_BIN)).so
else
TARGET_CP =$(basename $(TARGET_BIN)).a
endif
else
TARGET_CP =$(basename $(TARGET_BIN)).a
endif

###### new add ###########
OBJFILES := $(patsubst %.c,%.o,$(CSRC))

all: lib gen-fmt-md prebuild-paraFile build-paraFile

lib: $(TARGET_BIN)

ifeq ($(STATIC), 1)
$(TARGET_BIN): $(OBJFILES)
	$(AR) -rcs $(TARGET_CP) $^
else
ifneq ($(LINUX_TARGET),)
$(TARGET_BIN): $(OBJFILES)
	$(Q)$(CC) -shared -fPIC -o $(TARGET_CP) $^ $(LDFLAGS)
else
$(TARGET_BIN): $(OBJFILES)
	$(AR) -rcs $(TARGET_CP) $^
endif
endif

####################################################

ifneq ($(RTT_TARGET),)
install:
	@if [ ! -d "$(RTT_INSTALL_DIR)/sensors" ]; then mkdir $(RTT_INSTALL_DIR)/sensors; fi
ifeq ($(MS),1)
	@if [ ! -d "$(RTT_INSTALL_DIR)/sensors/multiple" ]; then mkdir $(RTT_INSTALL_DIR)/sensors/multiple; fi
	-cp -rf $(TARGET_CP) $(RTT_INSTALL_DIR)/sensors/multiple
else
	-cp -rf $(TARGET_CP) $(RTT_INSTALL_DIR)/sensors
endif
else
install:
	-cp -rf $(TARGET_CP) $(INSTALL_DIR)/
endif

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

.PHONY: build-headfile

gen-fmt-md:
	if [ -f '../gen_support_fmt.py' ];then python3 ../gen_support_fmt.py $(SENSOR_NAME);fi

prebuild-paraFile:
	mkdir -p parameters
	python3 ../prebuildHeadFile.py $(SENSOR_NAME) parameters

build-paraFile: $(TEMSRC)
%.c: %.hex
	mkdir -p parameters
	python3 ../hex2headfile.py $^ $@ parameters


.PHONY: delete-headfile

delete-paraFile:
	rm -rf parameters

clean: delete-paraFile
	find $(PWD) -name "*.o" | xargs rm -f
	find $(PWD) -name "*.d" | xargs rm -f
	find $(PWD) -name "*.a" | xargs rm -f
	find $(PWD) -name "*.so" | xargs rm -f