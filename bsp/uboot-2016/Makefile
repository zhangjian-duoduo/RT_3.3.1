CROSS_COMPILE = arm-fullhanv3-linux-uclibcgnueabi-

JOBS=$(shell grep -c ^processor /proc/cpuinfo)
ifeq ($(JOBS),)
JOBS = 1
endif


help:
	@echo "usage:"
	@echo "    make FH8856V300"
	@echo "    make FH8858V300"
	@echo "    make FH8852V310"
	@echo "    make FH8856V310"
	@echo "    make FH8858V310"

	@echo "    make menuconfig"

FH8856V300:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	if [ ! -e .config ]; then make fh8856v300_defconfig; fi; \
	make all -j$(JOBS) ; 

FH8858V300:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	if [ ! -e .config ]; then make fh8858v300_defconfig; fi; \
	make all -j$(JOBS) ; 

FH8852V310:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	if [ ! -e .config ]; then make fh8852v310_defconfig; fi; \
	make all -j$(JOBS) ; 

FH8856V310:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	if [ ! -e .config ]; then make fh8856v310_defconfig; fi; \
	make all -j$(JOBS) ; 

FH8858V310:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	if [ ! -e .config ]; then make fh8858v310_defconfig; fi; \
	make all -j$(JOBS) ; 



menuconfig:
	@if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi
	@cd uboot; \
    if [ ! -e .config ]; then \
        echo "run [cd uboot; make <chip>_defconfig] first !"; \
        exit 1; \
    fi; \
    make menuconfig

clean:
	cd uboot && make clean

distclean:
	rm -rf uboot