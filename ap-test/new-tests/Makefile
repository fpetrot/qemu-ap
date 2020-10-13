FLOATTYPE ?= ARBITRARY
PRECISION ?= 200
DEGREE    ?= 50
PREC_STEP ?= 1
N ?= 40
% : %.S
	riscv64-unknown-elf-gcc -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@
e_prec.time : e_prec.S
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -DPREC_STEP=$(PREC_STEP) -DDEGREE=$(DEGREE) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@
e.time : e.S
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -DDEGREE=$(DEGREE) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@

c.time : c.S lib.o crt.o
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -DN=$(N) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< lib.o crt.o -o $@

%.out : %.S lib.o crt.o
	riscv64-unknown-elf-gcc -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< lib.o crt.o -o $@

lib.o : lib.c
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<
crt.o : crt.s
	riscv64-unknown-elf-gcc -mcmodel=medany -g -march=rv64imfd -mabi=lp64d -fno-math-errno -Wall -ffreestanding -c $<


# Tests to be executed on a x86 machine
# Be careful to put an argument when you execute them
CFLAGS=-I$(MPFR_DIR)/src/
LDFLAGS=-L$(MPFR_DIR)/build/src/.libs/ $(MPFR_DIR)/build/src/.libs/libmpfr.so -lgmp -lm
e_x86_mpfr : e_x86_mpfr.o
	gcc -O2 -o $@ $< $(LDFLAGS)
e_x86_mpfr.o : e_x86_mpfr.c

e_x86 : e_x86.o
	gcc -O2 -o $@ $<
e_x86.o : e_x86.c
