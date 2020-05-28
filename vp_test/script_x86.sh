#!/bin/sh

# TEST of e double/mpfr for x86
make e_test_x86
make e_test_x86_mpfr

: > res.txt
: > res_mpfr.txt


for i in 100 500 1000 5000 10000 50000 100000 500000 1000000 5000000
do
	for j in {1..10}
	do
		(time ./e_test_x86 $i) 2>> res.txt
		(time ./e_test_x86_mpfr $i) 2>> res_mpfr.txt
	done
done

cat res.txt | grep real > x86.txt
cat res_mpfr.txt | grep real > x86_mpfr.txt

sed -e "s/real.*m//" -e "s/s/ /" -e "s/,/./" x86.txt > x86_e_double.txt
sed -e "s/real.*m//" -e "s/s/ /" -e "s/,/./" x86_mpfr.txt > x86_e_mpfr.txt

rm x86.txt
rm x86_mpfr.txt

make clean
