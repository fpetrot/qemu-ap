# Makefile for unit tests

SRCS=$(wildcard *.S)
OBJS=$(SRCS:%.S=%.o)
EXES=$(OBJS:%.o=%.out)

%.out : %.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
%.o : %.S
	riscv64-unknown-elf-gcc -gdwarf -march=rv64ifd -mabi=lp64d -c $<

all : $(EXES)

clean :
	rm -f $(OBJS) $(EXES)
