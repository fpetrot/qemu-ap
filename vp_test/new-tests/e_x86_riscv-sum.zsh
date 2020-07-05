#!/bin/zsh

if true ; then
	apfile_riscv=$(mktemp --tmpdir ap_riscv.XXXX)
	dbfile_riscv=$(mktemp --tmpdir db_riscv.XXXX)
	apfile_x86=$(mktemp --tmpdir ap_x86.XXXX)
	dbfile_x86=$(mktemp --tmpdir db_x86.XXXX)

	for i in $(seq 10 10 100);
	do
		grep -w ^$i trial_riscv-*/ap-res-e_riscv | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i time" >> $apfile_riscv
		grep -w ^$i trial_riscv-*/double-res-e_riscv | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i time" >> $dbfile_riscv
		grep -w ^$i trial_x86-*/ap-res-e_x86 | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i time" >> $apfile_x86
		grep -w ^$i trial_x86-*/double-res-e_x86 | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i time" >> $dbfile_x86
	done

	echo "import sys"
	echo "import math"
	echo "import numpy as np"
	echo "import matplotlib.pyplot as plt"
	echo "from array import *"

	# Format must be here so that all texts inherit from it, strangely enough
	echo "plt.rc('text', usetex=True)"
	echo "plt.rc('font', family='serif')"

	echo "marks = [ '.', ',', 'o', 'v', '^', '<', '>', '1', '2', '3', '4', 's', 'p', '*', 'h', 'H', '+', 'x', 'D','d', '|', '_']"

	echo "ap_riscv = [$(awk '{print $NF}' $apfile_riscv | paste -s -d ,)]"
	echo "d_riscv = [$(awk '{print $NF}' $dbfile_riscv | paste -s -d ,)]"
	echo "ap_x86 = [$(awk '{print $NF}' $apfile_x86 | paste -s -d ,)]"
	echo "d_x86 = [$(awk '{print $NF}' $dbfile_x86 | paste -s -d ,)]"

	echo "m = 0"
	echo "i = [" $(seq 10 10 100 | paste -s -d ',') "]"

	echo "fig = plt.figure()"
	echo "ax = fig.add_subplot(111)"

	echo "plt.plot(i, ap_riscv, marker=marks[m], c=np.random.rand(3,), label=\"mpfr RISCV\")"
	echo "m = m + 1"
	echo "plt.plot(i, d_riscv, marker=marks[m], c=np.random.rand(3,), label=\"double RISCV\")"
	echo "m = m + 1"
	echo "plt.plot(i, ap_x86, marker=marks[m], c=np.random.rand(3,), label=\"mpfr x86\")"
	echo "m = m + 1"
	echo "plt.plot(i, d_x86, marker=marks[m], c=np.random.rand(3,), label=\"double x86\")"
	echo "m = m + 1"

	echo "plt.xlabel(r'Expansion degree')"
	echo "plt.ylabel(r'Simulation wall-clock time (s)')"
	echo "plt.xticks(i)"
	#echo "plt.yscale('log')"
	echo "plt.legend(loc=7, bbox_to_anchor=(1, 0.65), prop={'size': 7})"
	echo "plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)"
	echo "plt.savefig('e_x86_riscv.pdf')"
fi
