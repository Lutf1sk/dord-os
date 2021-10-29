
OUT = fsroot/example.bin

OBJECTS = \
	example.o

CC_FLAGS = -I./ -I../libk/ -nostdlib -ffreestanding -fno-stack-protector -fpie -fpic -m32
LNK_FLAGS = -T linker.ld -melf_i386

include ../makefile.common

