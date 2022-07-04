# Configuration
BUILD_DIR = Build
SRC_DIR = Kernel/src

DISK_IMG = pros.img

QEMU = qemu-system-i386

QEMU_FLAGS = -serial stdio -m 32 -d int -D log.txt -monitor /dev/stdout

# Script related
G++ = /home/maciek/Projects/pros4/Toolchain/i686/bin/i686-elf-g++
AS = /home/maciek/Projects/pros4/Toolchain/i686/bin/i686-elf-as
LD = /home/maciek/Projects/pros4/Toolchain/i686/bin/i686-elf-ld

#G++ = $(CXX)
#AS = $(AS)
#LD = $(LD)

LINKER_SCRIPT = Kernel/linker.ld

SOURCES_CPP = $(shell find $(SRC_DIR)/ -name "*.cpp")
OBJECTS_CPP = $(SOURCES_CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

SOURCES_S = $(shell find $(SRC_DIR)/ -name "*.S")
OBJECTS_S = $(SOURCES_S:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

INCLUDES = Kernel/include
INCLUDE_PARAMS = $(INCLUDES:%=-I%)

G++_OPTIONS = $(INCLUDE_PARAMS) -m32 -c -O2  -mno-red-zone -ggdb -Wno-write-strings -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wall -Wextra  -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-permissive
AS_OPTIONS = --32
LD_OPTIONS = -m elf_i386 -Map=kernel.map

all: kernel

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(G++) $(G++_OPTIONS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(AS) $(AS_OPTIONS) -o $@ $^

kernel: $(OBJECTS_CPP) $(OBJECTS_S)
	$(LD) $(LD_OPTIONS) -r $(OBJECTS_CPP) -o $(BUILD_DIR)/kernelc.o
	$(LD) $(LD_OPTIONS) -r $(OBJECTS_S) -o $(BUILD_DIR)/kernels.o
	$(LD) $(LD_OPTIONS) -T $(LINKER_SCRIPT) -o $(BUILD_DIR)/pros.bin $(BUILD_DIR)/kernelc.o $(BUILD_DIR)/kernels.o

iso: kernel
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(BUILD_DIR)/pros.bin iso/boot/pros.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "pros" {' 				 >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/pros.bin'    	 >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --xorriso=/home/maciek/xorriso-1.5.4/xorriso/xorriso --output=$(BUILD_DIR)/pros.iso iso
	rm -rf iso

build: clean kernel

run:
	#$(QEMU) $(QEMU_FLAGS) $(BUILD_DIR)/pros.iso
	$(QEMU) $(QEMU_FLAGS) -kernel $(BUILD_DIR)/pros.bin
	# -drive file=$(DISK_IMG)

qemu:
	$(QEMU) $(QEMU_FLAGS) $(BUILD_DIR)/pros.iso
	# -drive file=$(DISK_IMG)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf iso

clear: clean

.PHONY: clean
