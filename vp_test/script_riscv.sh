#!/bin/sh

# TEST of e double/mpfr for riscv in qemu
make e_test_double.out
#make b.out

for i in {1..10}
do
	(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel e_test_double.out) 2>> riscv_e_double.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel b.out) 2>> riscv_e_mpfr.txt
done

make clean

