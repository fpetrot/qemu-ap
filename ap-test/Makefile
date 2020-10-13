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
	riscv64-unknown-elf-gcc -Os -march=rv64ifd -mabi=lp64d -c $<

d.out : newton_insn.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
newton_insn.o : newton_insn.S
	riscv64-unknown-elf-gcc -Os -march=rv64ifd -mabi=lp64d -c $<

crt.o : crt.s
	riscv64-unknown-elf-gcc -c $<

e_float_double.out : e_float_double.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
e_float_double.o : e_float_double.c
	riscv64-unknown-elf-gcc -DRISCV -mcmodel=medany -Os -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -Werror -ffreestanding -c $<

newton_float_double.out : newton_float_double.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
newton_float_double.o : newton_float_double.c
	riscv64-unknown-elf-gcc -DRISCV -mcmodel=medany -Os -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -Werror -ffreestanding -c $<


.PHONY: cholesky
cholesky : cholesky.log cholesky.time
	cat $^
%.log %.time : %.out
	{ time ../build_vp/riscv64-softmmu/qemu-system-riscv64 -machine virt -bios none -nographic -kernel $< > $*.log ; } 2> $*.time
cholesky.out : cholesky.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
cholesky.o : cholesky.c
	riscv64-unknown-elf-gcc -DRISCV -DDUMP_ARRAY -mcmodel=medany -Os -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<
cholesky.s : cholesky.c
	riscv64-unknown-elf-gcc -mcmodel=medany -Os -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -fverbose-asm -S -DRISCV cholesky.c
a : a.o crt.o
	riscv64-unknown-elf-ld -T link.ld $^ -o $@
a.o : a.c
	riscv64-unknown-elf-gcc -g -mcmodel=medany -Os -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -fverbose-asm -c $<

lib.o : lib.c
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<

c.out : gadd.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
gadd.o : gadd.s
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

# Small file to check conversions and fpreg/gpreg moves.
tests.s : tests.c
	riscv64-unknown-elf-gcc -S $<

clean : ; rm -f *.out *.time *.log test_all.o tests.s e_test_insn.o cholesky.o e_test_double.o newton_double.o lib.o crt.o