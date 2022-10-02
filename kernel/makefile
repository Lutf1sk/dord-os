
OUT = kernel.bin

OBJECTS = \
	debug_io.o \
	kernel.o \
	pmman.o \
	proc.o \
	fs.o \
	x86/entry.o \
	x86/gdt.o \
	x86/idt.o \
	x86/interrupts.o \
	x86/syscall.o \
	x86/isr.o \
	x86/proc.o \
	drivers/acpi.o \
	drivers/apic.o \
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

CC_FLAGS = -O2 -mno-avx -masm=intel -I./ -I../libk/ -nostdlib -ffreestanding -fno-stack-protector -fno-pie -fno-pic -m32
LNK_FLAGS = -T linker.ld -melf_i386
ASM_FLAGS = -felf32

BIN_DIR = ../bin/

DEPS = $(patsubst %.o,%.d,$(OBJECTS))

ASM = nasm
CC = cc
LNK = ld

-include $(DEPS)

all: $(BIN_DIR)/$(OUT)

$(BIN_DIR)/$(OUT): $(OBJECTS) x86/trampoline.asm $(LNK_LIBS)
	@$(ASM) -fbin x86/trampoline.asm -o x86/trampoline.bin
	@objcopy -O elf32-i386 -B i386 -I binary x86/trampoline.bin trampoline.o
	$(LNK) -o "$(BIN_DIR)/$(OUT)" $(OBJECTS) $(LNK_FLAGS) $(LNK_LIBS) trampoline.o

%.o: %.c
	@echo Compiling $<...
	@$(CC) $(CC_FLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	@$(CC) $(CC_FLAGS) -o $@ -c $<

%.o: %.asm
	@echo Compiling $<...
	@$(ASM) $(ASM_FLAGS) -M -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	@$(ASM) $(ASM_FLAGS) -o $@ $<

.PHONY: clean all
clean:
	-rm "$(BIN_DIR)/$(OUT)" $(OBJECTS) $(DEPS)

