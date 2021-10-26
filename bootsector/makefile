OUT = ../bin/bootsector.bin

OBJECTS = panic.o \
	  boot.o \
	  drive.o


ASM_FLAGS = -felf32
LNK_FLAGS = -melf_i386 -T linker.ld

include ../makefile.common

