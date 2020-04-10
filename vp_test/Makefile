a.out: marie_test_e.o link.ld
	riscv64-unknown-elf-ld -T link.ld marie_test_e.o
marie_test_e.o : marie_test_e.s
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c marie_test_e.s
a.dis: a.out
	riscv32-unknown-elf-objdump -D a.out > a.dis

# Small file to check conversions and fpreg/gpreg moves.
tests.s : tests.c
	riscv32-unknown-elf-gcc -S $<

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

clean : ; rm -f a.out marie_test_e.o a.dis tests.s mpfr-test mpfr-test.o