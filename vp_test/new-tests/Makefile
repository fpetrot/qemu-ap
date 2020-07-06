FLOATTYPE ?= ARBITRARY
PRECISION ?= 200
DEGREE    ?= 50
PREC_STEP ?= 1
% : %.S
	riscv64-unknown-elf-gcc -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@
e_prec.time : e_prec.S
	riscv64-unknown-elf-gcc -D$(FLOATTYPE) -DPRECISION=$(PRECISION) -DPREC_STEP=$(PREC_STEP) -DDEGREE=$(DEGREE) -g -march=rv64imfd -mabi=lp64d -T link.ld -ffreestanding -nostdlib $< -o $@
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


ifeq ($(shell echo $$LOGNAME), petrot)
MPFR=$(HOME)/Developpement/mpfr-4.0.2
else # Marie
MPFR=$(HOME)/Documents/Ensimag/MasterDI/Stage/Frederic_Petrot/qemu/variable_precision_floating_point/qemu-vp/mpfr-4.0.2
endif

# Tests to be executed in x86 machine
CFLAGS=-I$(MPFR)/src/
LDFLAGS=-L$(MPFR)/build/src/.libs/ $(MPFR)/build/src/.libs/libmpfr.so -lgmp -lm
e_x86_mpfr : e_x86_mpfr.o
	gcc -O2 -o $@ $< $(LDFLAGS)
e_x86_mpfr.o : e_x86_mpfr.c

e_x86 : e_x86.o
	gcc -O2 -o $@ $<
e_x86.o : e_x86.c
