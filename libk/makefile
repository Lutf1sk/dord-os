
LIB = libk.a

OBJECTS = \
	memory.o \
	spinlock.o \
	io.o

CC_FLAGS = -nostdlib -ffreestanding -fno-stack-protector -fno-pie -m32
LIB_FLAGS = --target=elf32-i386

include ../makefile.common

