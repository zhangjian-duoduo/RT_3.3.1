MCONF_DIR := $(SDKROOT)/build/kconfig-frontends
MCONF := $(MCONF_DIR)/kconfig-mconf

TOOLS_DIR := $(SDKROOT)/build/tools/

menuconfig: menuprepare
	$(QUIET)$(MCONF) build/Kconfig
	$(QUIET)if [ ! -e $(OUT)/rtconfig ]; then mkdir -p $(OUT)/rtconfig; fi
	$(QUIET)python $(TOOLS_DIR)/cfg2hdr.py .config $(OUT)/rtconfig/rtconfig.h
	$(QUIET)python ./build/tools/mkscript.py

appconfig: menuprepare
	@echo "*"$(SAMPLE_DIR)"*"
	$(QUIET)cd $(SAMPLE_DIR);make menuconfig OS=RTT;cd -

menuprepare:
	$(QUIET)if [ ! -e $(MCONF) ]; then make -C $(MCONF_DIR); fi

.PHONY: menuconfig menuprepare
