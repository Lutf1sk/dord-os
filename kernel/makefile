
OUT = kernel.bin

OBJECTS = \
	kernel.o \
	ps2.o \
	mouse.o \
	entry.o \
	com.o \
	terminal.o \
	pic.o \
	gdt.o \
	idt.o \
	isr.o \
	interrupts.o \
	keyboard.o \
	ide.o \
	pci.o \
	pci_class.o \
	debug_io.o \
	pit.o \
	pc_speaker.o \
	dfs.o \
	acpi.o \
	vga.o

LNK_LIBS = ../bin/libk.a

CC_FLAGS = -I../libk/ -nostdlib -ffreestanding -fno-stack-protector -fno-pie -fno-pic -m32
LNK_FLAGS = -T linker.ld -melf_i386
ASM_FLAGS = -felf32

include ../makefile.common

