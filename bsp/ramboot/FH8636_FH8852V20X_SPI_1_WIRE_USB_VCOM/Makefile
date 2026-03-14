CROSS ?= arm-fullhanv3-linux-uclibcgnueabi-

CC      = $(CROSS)gcc
AR      = $(CROSS)ar
LD      = $(CROSS)ld
NM      = $(CROSS)nm
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy

CFLAGS = -g -Wall -Wstrict-prototypes -fomit-frame-pointer -ffreestanding \
 		 -fno-strict-aliasing -fno-common -fno-builtin  -msoft-float -pipe \
		 -nostdinc -isystem $(shell $(CC) -print-file-name=include)
CFLAGS += -Os -ffunction-sections

OBJS_DIR = .objs

all: user.o extract_objs
	$(LD) -Bstatic -Tlink.lds $(OBJS_DIR)/start.o --start-group user.o $(filter-out $(OBJS_DIR)/start.o,$(wildcard $(OBJS_DIR)/*.o)) --end-group \
	      -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) \
	      -lgcc -g --gc-sections -o RamBoot_elf -Map bin_map
	$(OBJCOPY) --gap-fill=0xff -O binary -S RamBoot_elf RamBoot.bin
	$(OBJDUMP) -D -m arm RamBoot_elf > RamBoot.dis
	$(NM) -n RamBoot_elf > systemramboot.map

extract_objs:
	mkdir -p $(OBJS_DIR); cd $(OBJS_DIR); $(AR) x ../libramboot.a;

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf RamBoot_elf bin_map RamBoot.dis RamBoot.bin systemramboot.map *.o $(OBJS_DIR)
