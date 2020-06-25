FLOATTYPE ?= ARBITRARY
PRECISION ?= 200
DEGREE    ?= 50
% : %.S
	riscv64-unknown-elf-gcc -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@
e.time : e.S
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -DDEGREE=$(DEGREE) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@

c.time : c.S lib.o crt.o
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< lib.o crt.o -o $@

%.out : %.S lib.o crt.o
	riscv64-unknown-elf-gcc -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< lib.o crt.o -o $@

lib.o : lib.c
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<
crt.o : crt.s
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<
