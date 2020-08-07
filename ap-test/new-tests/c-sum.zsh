#!/bin/zsh

if true ; then
	apfile=$(mktemp --tmpdir ap-c.XXXX)
	dbfile=$(mktemp --tmpdir db-c.XXXX)
	for n in 10 30 50 70 90 120 150 200 250 300 350 400 500 600 700 800 900 1000;
	do
		grep -w ^$n trial_c-*/double-res-c | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "matrix size $n time" >> $dbfile
		grep -w ^$n trial_c-*/ap-res-c | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "matrix size $n time" >> $apfile
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

	echo "d = [$(awk '{print $NF}' $dbfile | paste -s -d ,)]"
	echo "ap = [$(awk '{print $NF}' $apfile | paste -s -d ,)]"

	echo "m = 0"
	echo "i = [10,30,50,70,90,120,150,200,250,300,350,400,500,600,700,800,900,1000]"

	echo "fig = plt.figure()"
	echo "ax = fig.add_subplot(111)"

	echo "plt.plot(i, d, marker=marks[m], c=np.random.rand(3,), label=\"double\")"
	echo "m = m + 1"	
	echo "plt.plot(i, ap, marker=marks[m], c=np.random.rand(3,), label=\"mpfr\")"
	echo "m = m + 1"

	echo "plt.xlabel(r'Matrix size')"
	echo "plt.ylabel(r'Simulation wall-clock time (s)')"
	#echo "plt.xticks(i)"
	echo "plt.yscale('log')"
	echo "plt.legend(loc=7, bbox_to_anchor=(1, 0.65), prop={'size': 7})" # https://matplotlib.org/api/_as_gen/matplotlib.pyplot.legend.html#matplotlib.pyplot.legend
	echo "plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)"
	echo "plt.savefig('c.pdf')"
fi
