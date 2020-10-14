# Makefile for unit tests

all.out : test_all.o
	riscv64-unknown-elf-ld -T link.ld $< -o $@
test_all.o : test_all.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fadd.out : fadd_p.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fadd_p.o : fadd_p.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fnmadd.out : fnmadd_p.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fnmadd_p.o : fnmadd_p.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_p_d.out : fcvt_p_d.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_p_d.o : fcvt_p_d.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_p_f.out : fcvt_p_f.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_p_f.o : fcvt_p_f.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_p_l.out : fcvt_p_l.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_p_l.o : fcvt_p_l.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_p_lu.out : fcvt_p_lu.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_p_lu.o : fcvt_p_lu.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_w_p.out : fcvt_w_p.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_w_p.o : fcvt_w_p.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_wu_p.out : fcvt_wu_p.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_wu_p.o : fcvt_wu_p.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

fcvt_x_y.out : fcvt_x_y.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
fcvt_x_y.o : fcvt_x_y.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

flp.out : flp.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
flp.o : flp.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

test_ldp_std.out : test_ldp_std.o
	 riscv64-unknown-elf-ld -T link.ld $< -o $@
test_ldp_std.o : test_ldp_std.S
	riscv64-unknown-elf-gcc -march=rv64ifd -mabi=lp64d -c $<

clean : ; rm -f *.out *.o
