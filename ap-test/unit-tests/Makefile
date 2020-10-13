# Makefile for unit tests

a.out : test_all.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
test_all.o : test_all.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<
