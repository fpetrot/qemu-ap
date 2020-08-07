#!/bin/zsh

if true ; then
	for turn in $(seq 1 10);
	do
		out=trial_prec-$turn
		rm -rf $out
		mkdir  $out
		
		for i in $(seq 10 10 100);
		do
			for j in 1 5 10 15 20 25 30;
			do
				touch e_prec.S
				make FLOATTYPE=ARBITRARY PRECISION=53 PREC_STEP=$j DEGREE=$i e_prec.time
				echo -n "ap degree $i prec 53 step $j " >> $out/ap-e_prec
				# ATTENTION au nom du dossier build, a modifier
				(time ../../build/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel e_prec.time) 2>> $out/ap-e_prec
			done
		done
	done
fi

if true ; then
	for turn in $(seq 1 10);
	do
		out=trial_prec-$turn
		cd $out
		awk '{print $7, $3, $19}' ap-e_prec  | sed -e 's/,/./g' > ap-res-e_prec

		for j in 1 5 10 15 20 25 30;
		do
			grep -w ^$j ap-res-e_prec > ap-res-e_prec-$j
		done

		for i in $(seq 10 10 100);
		do
			grep " $i " ap-res-e_prec > ap-degree-$i
		done
		cd ..
	done
fi
