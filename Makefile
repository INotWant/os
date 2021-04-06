C_SOURCES = $(wildcard kernel/*.c kernel/lisp_asm/*.c drivers/*.c cpu/*.c libc/*.c)
ASM_SOURCES = $(wildcard kernel/lisp_asm/*.asm test/lisp_asm/*.asm)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 
A_OBJ = ${ASM_SOURCES:.asm=.o}

CC = i386-elf-gcc
LD = i386-elf-ld
GDB = i386-elf-gdb

CFLAGS = $(CFLAG)
CFLAGS += -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

A_FLAGS = $(AFLAG)

all: os-image.bin kernel.elf
	# $(MAKE) -C u_boot

# First rule is run by default
os-image.bin: boot/boot_sec.bin kernel.bin
	cat $^ > $@

kernel.bin: boot/kernel_entry.o ${OBJ} ${A_OBJ}
	${LD} -m elf_i386 -o $@ -Ttext 0x8400 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ} ${A_OBJ}
	${LD} -m elf_i386 -o $@ -Ttext 0x8400 $^

run: os-image.bin
	qemu-system-i386 -curses -hda os-image.bin

debug: os-image.bin kernel.elf
	qemu-system-i386 -curses -s -hda os-image.bin -S &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $(A_FLAGS) $< -f elf -o $@

%.bin: %.asm
	nasm $(A_FLAGS) $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o kernel/lisp/*.o kernel/lisp_asm/*.o evaluator/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o test/*.o test/lisp_asm/*.o
	$(MAKE) -C u_boot clean

stop:
	ps | grep qemu-system-i386 | sed  '/grep/d' | awk '{print $$1}' | xargs kill