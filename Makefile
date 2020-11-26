C_SOURCES = $(wildcard kernel/*.c kernel/lisp/*.c drivers/*.c cpu/*.c libc/*.c test/*.c)
HEADERS = $(wildcard kernel/*.h kernel/lisp/*.h drivers/*.h cpu/*.h libc/*.h test/*.h)
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 

CC = i386-elf-gcc
GDB = i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

all: os-image.bin kernel.elf

# First rule is run by default
os-image.bin: boot/boot_sec.bin kernel.bin
	cat $^ > $@

kernel.bin: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x7e00 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x7e00 $^

run: os-image.bin
	qemu-system-i386 -curses -hda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -curses -s -hda os-image.bin -S &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o kernel/lisp/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o test/*.o

stop:
	ps | grep qemu-system-i386 | sed  '/grep/d' | awk '{print $$1}' | xargs kill
