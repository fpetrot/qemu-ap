#!/bin/zsh

# Let's not do the computation again !
if true ; then

   for turn in $(seq 1 10); 
   do
      out=trial-$turn
      rm -rf $out
      mkdir $out

      for i in $(seq 10 10 100); 
      do
         touch e.S
         make FLOATTYPE=DOUBLE PRECISION=1 DEGREE=$i e.time
         echo -n "double iterations $i " >> $out/double-e
         (time ../../build-ap/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel e.time) 2>> $out/double-e
      done


      for i in $(seq 10 10 100); 
      do
         for p in 53 80 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000;
         do
            touch e.S
            make FLOATTYPE=ARBITRARY PRECISION=$p DEGREE=$i e.time
            echo -n "ap iterations $i prec $p " >> $out/ap-e
            (time ../../build-ap/riscv64-softmmu/qemu-system-riscv64 -nographic -machine virt -bios none -m 256M -kernel e.time) 2>> $out/ap-e
         done
      done
   done
fi

if true ; then
   for turn in $(seq 1 10); 
   do
      out=trial-$turn
      cd $out
      awk '{print $3, $14}' double-e | sed -e 's/,/./g' > double-res-e
      awk '{print $5, $3, $16}' ap-e | sed -e 's/,/./g' > ap-res-e

      for p in 53 80 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000;
      do
         grep -w ^$p ap-res-e > ap-res-e-$p
      done

      for i in $(seq 10 10 100);
      do
         grep " $i " ap-res-e > ap-prec-$i
      done
      cd ..
   done
fi
