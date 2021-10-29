
all:
	make all -C libk
	make all -C kernel
	make all -C bootsector
	make all -C write_img
	make all -C stage2
	make all -C prog
	bin/write_img -odordos.img -bbin/bootsector.bin bin/stage2.bin bin/kernel.bin bin/fsroot/*

clean:
	make clean -C libk
	make clean -C write_img
	make clean -C bootsector
	make clean -C kernel
	make clean -C stage2
	make clean -C prog

bochs: all
	bochs -f bochs.conf -q

run: all
	qemu-system-i386 -mem-prealloc -m 512M -soundhw pcspk -serial file:/dev/stdout -drive format=raw,file=dordos.img
