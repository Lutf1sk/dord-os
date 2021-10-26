OUT = ../bin/stage2.bin

OBJECTS = stage2.o \
	memory_map.o \
	a20.o \
	panic.o \
	ide.o \
	stage2_c.o \
	debug_io.o

ASM_FLAGS = -felf32
CC_FLAGS = -I../libk -m32 -fno-pie -ffreestanding -nostdlib -fno-stack-protector
LNK_FLAGS = -melf_i386 -T linker.ld

LNK_LIBS = ../bin/libk.a

include ../makefile.common

