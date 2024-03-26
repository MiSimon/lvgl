LVGL_PATH := $(CURDIR)/../..
ARCH = x86_64
CC = clang
$(info LVGL_PATH is set to: $(LVGL_PATH))

CSRCS += $(shell find $(LVGL_PATH)/src/ -type f -name '*.c')

CSRCS += test.c
CSRCS += lv_efi.c
CFLAGS = -Werror -Wall  -I$(LVGL_PATH) -I$(CURDIR) \
	-target $(ARCH)-unknown-windows -ffreestanding -fshort-wchar  -mno-red-zone  -fno-stack-protector -fno-stack-check -mno-red-zone \
	-DLV_BUILD_TEST=1 \
	-DLV_CONF_SKIP=1 \
	-DLV_KCONFIG_IGNORE=1 \
	-DLV_COLOR_DEPTH=32 \
	-DLV_USE_LOG=1 \
	-DLV_LOG_PRINTF=0 \
	-DLV_USE_STDLIB_MALLOC=LV_STDLIB_BUILTIN \
	-DLV_USE_STDLIB_STRING=LV_STDLIB_BUILTIN \
	-DLV_USE_STDLIB_SPRINTF=LV_STDLIB_BUILTIN \
	-DLV_STDINT_INCLUDE=\<lv_efi_types.h\> \
	-DLV_STDINT_INCLUDE=\<lv_efi_types.h\> \
	-DLV_STDDEF_INCLUDE=\<lv_efi_types.h\> \
	-DLV_STDBOOL_INCLUDE=\<lv_efi_types.h\> \
	-DLV_INTTYPES_INCLUDE=\<lv_efi_types.h\> \
	-DLV_LIMITS_INCLUDE=\<lv_efi_types.h\> \
	-DLV_STDARG_INCLUDE=\<lv_efi_types.h\>

LDFLAGS = -target $(ARCH)-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link

COBJS := $(patsubst $(LVGL_PATH)%,objs/%, $(CSRCS:.c=.o))

test_file: $(COBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

objs/%.o: $(LVGL_PATH)%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -r $(CURDIR)/objs
	rm test_file

run: test_file 
	mkdir -p $(CURDIR)/.run/
	cp test_file $(CURDIR)/.run/
	echo fs0:\test_file > $(CURDIR)/.run/startup.nsh
	qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd -nodefaults -vga std -drive format=raw,file=fat:rw:$(CURDIR)/.run/