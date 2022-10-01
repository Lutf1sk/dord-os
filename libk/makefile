
LIB = libk.a

OBJECTS = \
	memory.o \
	spinlock.o \
	psf.o \
	ppm.o \
	io.o

CC_FLAGS = -I./ -nostdlib -ffreestanding -fno-stack-protector -fno-pie -m32
LIB_FLAGS = --target=elf32-i386

include ../makefile.common

