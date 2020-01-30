a.out: gadd.o link.ld
	riscv64-unknown-elf-ld -T link.ld gadd.o
gadd.o : gadd.s
	riscv64-unknown-elf-gcc -march=rv64i -c gadd.s
a.dis: a.out
	riscv32-unknown-elf-objdump -D a.out > a.dis
