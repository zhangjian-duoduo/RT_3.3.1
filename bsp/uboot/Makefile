CROSS_COMPILE = arm-fullhanv3-linux-uclibcgnueabi-

JOBS=$(shell grep -c ^processor /proc/cpuinfo)
ifeq ($(JOBS),)
JOBS = 1
endif


help:
	@echo "usage:"
	@echo "    make FH8636"
	@echo "    make FH8852V201"
	@echo "    make FH8626V200"


FH8636:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	make fh8636_devboard_config; make all -j$(JOBS);

FH8852V201:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	make fh8852v201_devboard_config; make all -j$(JOBS);

FH8626V200:
	if [ ! -d uboot ]; then tar xf uboot.tar.gz; fi;\
	cd uboot; make clean; \
	export CROSS_COMPILE=$(CROSS_COMPILE); \
	export BUILD_DIR=./_build;\
	make fh8626v200_devboard_config; make all -j$(JOBS);



clean:
	cd uboot && make clean && rm -rf _build

distclean:
	rm -rf uboot