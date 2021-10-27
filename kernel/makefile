
OUT = kernel.bin

OBJECTS = \
	debug_io.o \
	kernel.o \
	x86/entry.o \
	x86/gdt.o \
	x86/idt.o \
	x86/interrupts.o \
	x86/isr.o \
	drivers/acpi.o \
	drivers/com.o \
	drivers/dfs.o \
	drivers/ide.o \
	drivers/keyboard.o \
	drivers/mouse.o \
	drivers/pci.o \
	drivers/pci_class.o \
	drivers/pc_speaker.o \
	drivers/pic.o \
	drivers/pit.o \
	drivers/ps2.o \
	drivers/terminal.o \
	drivers/vga.o \


LNK_LIBS = ../bin/libk.a

CC_FLAGS = -I./ -I../libk/ -nostdlib -ffreestanding -fno-stack-protector -fno-pie -fno-pic -m32
LNK_FLAGS = -T linker.ld -melf_i386
ASM_FLAGS = -felf32

include ../makefile.common

