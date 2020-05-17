SHELL=/bin/bash  # To have access to 'time'

a.out: marie_test_e.o link.ld
	riscv64-unknown-elf-ld -T link.ld marie_test_e.o
marie_test_e.o : marie_test_e.s
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c marie_test_e.s
a.dis: a.out
	riscv32-unknown-elf-objdump -D a.out > a.dis
b.out : e_test_insn.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
e_test_insn.o : e_test_insn.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

d.out : newton_insn.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
newton_insn.o : newton_insn.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

crt.o : crt.s
	riscv64-unknown-elf-gcc -c $<

e_test_double.out : e_test_double.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
e_test_double.o : e_test_double.c
	riscv64-unknown-elf-gcc -DRISCV -mcmodel=medany -O3 -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -Werror -ffreestanding -c $<

newton_double.out : newton_double.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
newton_double.o : newton_double.c
	riscv64-unknown-elf-gcc -DRISCV -mcmodel=medany -O3 -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -Werror -ffreestanding -c $<


.PHONY: cholesky
cholesky : cholesky.log cholesky.time
	cat $^
%.log %.time : cholesky.out
	{ time ../build_vp/riscv64-softmmu/qemu-system-riscv64 -machine virt -bios none -nographic -kernel $< > $*.log ; } 2> $*.time
cholesky.out : cholesky.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
cholesky.o : cholesky.c
	riscv64-unknown-elf-gcc -DRISCV -DDUMP_ARRAY -mcmodel=medany -Os -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<
cholesky.s : cholesky.c
	riscv64-unknown-elf-gcc -mcmodel=medany -Os -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -fverbose-asm -S -DRISCV cholesky.c

cholesky-asm.out : cholesky-asm.o lib.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
cholesky-asm.o : cholesky-asm.S
	riscv64-unknown-elf-gcc -g -march=rv64imfd -c $<
lib.o : lib.c
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<

c.out : gadd.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
gadd.o : gadd.s
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

# Small file to check conversions and fpreg/gpreg moves.
tests.s : tests.c
	riscv64-unknown-elf-gcc -S $<

ifeq ($(shell echo $$LOGNAME), petrot) 
MPFR=$(HOME)/Developpement/mpfr-4.0.2
else # Marie
MPFR=$(HOME)/Documents/Ensimag/MasterDI/Stage/Frederic_Petrot/qemu/variable_precision_floating_point/qemu-vp/mpfr-4.0.2
endif

CFLAGS=-I$(MPFR)/src/
LDFLAGS=-L$(MPFR)/build/src/.libs/ $(MPFR)/build/src/.libs/libmpfr.so -lgmp -lm
mpfr-test : mpfr-test.o
	gcc -o $@ $< $(LDFLAGS)
mpfr-test.o : mpfr-test.c

clean : ; rm -f *.out  test_all.o tests.s mpfr-test mpfr-test.o e_test_insn.o cholesky.o e_test_double.o newton_double.o
