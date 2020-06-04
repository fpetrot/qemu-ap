#!/bin/sh

# TESTS for riscv in qemu

#make e_test_double.out
#make b.out
#make cholesky-asm.out
#make newton_double.out
make d.out

for i in {1..10}
do
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel e_test_double.out) 2>> riscv_e_double.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel b.out) 2>> riscv_e_mpfr.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel b.out) 2>> riscv_e_mpfr_prec.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel b.out) 2>> riscv_e_double2.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel cholesky-asm.out) 2>> riscv_cholesky_double.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel cholesky-asm.out) 2>> riscv_cholesky_mpfr.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel cholesky-asm.out) 2>> riscv_cholesky_mpfr_prec.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel cholesky-asm.out) 2>> riscv_cholesky_mpfr_prec2.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel cholesky-asm.out) 2>> riscv_cholesky_mpfr_prec3.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel newton_double.out) 2>> riscv_newton_double.txt
	#(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel d.out) 2>> riscv_newton_mpfr.txt
	(time ../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -kernel d.out) 2>> riscv_newton_mpfr_prec.txt
done

make clean

