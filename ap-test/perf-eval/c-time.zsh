#!/bin/zsh

if true ; then
	for turn in $(seq 1 10);
	do
		out=trial_c-$turn
		rm -rf $out
		mkdir $out

		for n in 10 30 50 70 90 120 150 200 250 300 350 400 500 600 700 800 900 1000;
		do
			touch c.S
			make FLOATTYPE=DOUBLE PRECISION=1 N=$n c.time
			echo -n "matrix size $n " >> $out/double-c
			(time ../../build-ap/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel c.time) 2>> $out/double-c
		done

		for n in 10 30 50 70 90 120 150 200 250 300 350 400 500 600 700 800 900 1000;
		do
			touch c.S
			make FLOATTYPE=ARBITRARY PRECISION=100 N=$n c.time
			echo -n "matrix size $n " >> $out/ap-c
			(time ../../build-ap/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel c.time) 2>> $out/ap-c
		done
	done
fi

if true ; then
	for turn in $(seq 1 10);
	do
		out=trial_c-$turn
		cd $out
		awk '{print $3, $15}' double-c | sed -e 's/,/./g' > double-res-c
		awk '{print $3, $15}' ap-c | sed -e 's/,/./g' > ap-res-c
		
		for n in 10 30 50 70 90 120 150 200 250 300 350 400 500 600 700 800 900 1000;
		do
			grep -w ^$n double-res-c > double-matrix-$n
			grep -w ^$n ap-res-c > ap-matrix-$n
		done
		cd ..
	done
fi
