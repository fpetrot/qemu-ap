#!/bin/zsh

if true ; then
	apfile=$(mktemp --tmpdir ap_prec.XXXX)
	for i in $(seq 10 10 100);
	do
		for j in 1 5 10 15 20 25 30;
		do
			grep " $i " trial_prec-*/ap-res-e_prec-$j | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i step $j time" >> $apfile
		done
	done

	# Precision step en abscisse
	if false ; then
		echo "import sys"
		echo "import math"
		echo "import numpy as np"
		echo "import matplotlib.pyplot as plt"
		echo "from array import *"

		# Format must be here so that all texts inherit from it, strangely enough
		echo "plt.rc('text', usetex=True)"
		echo "plt.rc('font', family='serif')"

		echo "marks = [ '.', ',', 'o', 'v', '^', '<', '>', '2', '2', '3', '4', 's', 'p', '*', 'h', 'H', '+', 'x', 'D','d', '|', '_']"

		echo "t = [[] for x in range(101)]"
		for i in $(seq 10 10 100);
		do
			echo -n "t[$i] = ["
			v=$(grep "degree $i " $apfile | awk '{print $NF}' | paste -s -d ",")
			echo "$v]"
		done

		echo "m = 0"
		echo "i = [1,5,10,15,20,25,30]"

		echo "fig = plt.figure()"
		echo "ax = fig.add_subplot(111)"

		for i in $(seq 10 10 100);
		do
			echo "plt.plot(i, t[$i], marker=marks[m], c=np.random.rand(3,), label=\"degree $i\")"
			echo "m = m + 1"
		done
		echo "plt.xlabel(r'Precision step')"
		echo "plt.ylabel(r'Simulation wall-clock time (s)')"
		#echo "plt.ylim([0.03, 0.05])"
		echo "plt.xticks(i)"
		echo "plt.legend(loc=7, bbox_to_anchor=(1, 0.65), prop={'size': 7})"
		#echo 'for k, v in zip(i,t[100]) :'
		#echo '    ax.text(k, 1.1*v, "%1.2f" %v, ha="center")'
		echo "plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)"
		echo "plt.savefig('e_prec.pdf')"
	fi

	# Degree en abscisse
	if true ; then
		echo "import sys"
		echo "import math"
		echo "import numpy as np"
		echo "import matplotlib.pyplot as plt"
		echo "from array import *"

		# Format must be here so that all texts inherit from it, strangely enough
		echo "plt.rc('text', usetex=True)"
		echo "plt.rc('font', family='serif')"

		echo "marks = [ '.', ',', 'o', 'v', '^', '<', '>', '2', '2', '3', '4', 's', 'p', '*', 'h', 'H', '+', 'x', 'D','d', '|', '_']"

		echo "t = [[] for x in range(31)]"
		for j in 1 5 10 15 20 25 30;
		do
			echo -n "t[$j] = ["
			v=$(grep "step $j " $apfile | awk '{print $NF}' | paste -s -d ",")
			echo "$v]"
		done

		echo "m = 0"
		echo "i = [10,20,30,40,50,60,70,80,90,100]"
		echo "a = [0.037600,0.037600,0.036700,0.037300,0.038100,0.037000,0.037500,0.037300,0.037000,0.037200]"
		echo "b = [0.038500,0.038900,0.040900,0.042000,0.042500,0.044300,0.044100,0.045500,0.046200,0.047700]"

		echo "fig = plt.figure()"
		echo "ax = fig.add_subplot(111)"

		for j in 1 5 10 15 20 25 30;
		do
			echo "plt.plot(i, t[$j], marker=marks[m], c=np.random.rand(3,), label=\"step $j\")"
			echo "m = m + 1"
		done
		echo "plt.plot(i, a, marker=marks[m], c=np.random.rand(3,), label=\"prec 53\")"
		echo "m = m + 1"
		echo "plt.plot(i, b, marker=marks[m], c=np.random.rand(3,), label=\"prec 4000\")"
		echo "m = m + 1"

		echo "plt.xlabel(r'Degree')"
		echo "plt.ylabel(r'Simulation wall-clock time (s)')"
		#echo "plt.ylim([0.03, 0.05])"
		echo "plt.xticks(i)"
		echo "plt.legend(loc=7, bbox_to_anchor=(1, 0.65), prop={'size': 7})"
		#echo 'for k, v in zip(i,t[100]) :'
		#echo '    ax.text(k, 1.1*v, "%1.2f" %v, ha="center")'
		echo "plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)"
		echo "plt.savefig('e_prec.pdf')"
	fi
fi
