all: dbr bootloader

U_DRIVE_FILE=/dev/disk2s1

dbr:
	nasm dbr_boot.asm
	cat dbr_pre dbr_boot > dbr
	# dd if=dbr of=${U_DRIVE_FILE} bs=512 count=1

bootloader:
	nasm bootloader.asm
	# dd if=bootloader of=${U_DRIVE_FILE} bs=512 oseek=2 count=2

clean:
	rm -rf dbr dbr_boot bootloader

