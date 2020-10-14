#!/bin/zsh

if true ; then
	for turn in $(seq 1 10);
	do
		# RISCV
		out=trial_riscv-$turn
		rm -rf $out
		mkdir $out

		for i in $(seq 10 10 100);
		do
			touch e.S
			make FLOATTYPE=DOUBLE PRECISION=1 DEGREE=$i e.time
			echo -n "double iterations $i " >> $out/double-e_riscv
			(time ../../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel e.time) 2>> $out/double-e_riscv
		done

		for i in $(seq 10 10 100);
		do
			touch e.S
			make FLOATTYPE=ARBITRARY PRECISION=200 DEGREE=$i e.time
			echo -n "ap iterations $i prec 200 " >> $out/ap-e_riscv
			(time ../../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel e.time) 2>> $out/ap-e_riscv
		done

		# x86
		out_x86=trial_x86-$turn
		rm -rf $out_x86
		mkdir $out_x86

		for i in $(seq 10 10 100);
		do
			make e_x86
			echo -n "double iterations $i " >> $out_x86/double-e_x86
			(time ./e_x86 $i) 2>> $out_x86/double-e_x86
		done

		for i in $(seq 10 10 100);
		do
			make e_x86_mpfr
			echo -n "ap iterations $i prec 200 " >> $out_x86/ap-e_x86
			(time ./e_x86_mpfr $i) 2>> $out_x86/ap-e_x86
		done

	done
fi

if true ; then
	for turn in $(seq 1 10);
	do
		out=trial_riscv-$turn
		cd $out
		awk '{print $3, $15}' double-e_riscv | sed -e 's/,/./g' > double-res-e_riscv
		awk '{print $3, $17}' ap-e_riscv | sed -e 's/,/./g' > ap-res-e_riscv
		cd ..

		out_x86=trial_x86-$turn
		cd $out_x86
		awk '{print $3, $12}' double-e_x86 | sed -e 's/,/./g' > double-res-e_x86
		awk '{print $3, $14}' ap-e_x86 | sed -e 's/,/./g' > ap-res-e_x86
		cd ..
	done
fi
