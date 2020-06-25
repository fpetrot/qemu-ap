#!/bin/zsh

# Make it false to skip stuff !
if true ; then
   apfile=$(mktemp --tmpdir ap.XXXX)
   dbfile=$(mktemp --tmpdir db.XXXX)
   for i in $(seq 10 10 100); 
   do
      for p in 53 80 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000;
      do
         grep " $i " trial-*/ap-res-e-$p | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i precision $p time" >> $apfile
      done
      grep -w ^$i trial-*/double-res-e | awk 'BEGIN {i=0.0; k=0} {k = k +1; i += $NF} END {printf("%f\n",  i/k)}' | xargs echo "degree $i time" >> $dbfile
   done

   echo "import sys"
   echo "import math"
   echo "import numpy as np"
   echo "import matplotlib.pyplot as plt"
   echo "from array import *"

   # Format must be here so that all texts inherit from it, strangely enough
   echo "plt.rc('text', usetex=True)"
   echo "plt.rc('font', family='serif')"

   echo "marks = [ '.', ',', 'o', 'v', '^', '<', '>', '1', '2', '3', '4', 's', 'p', '*', 'h', 'H', '+', 'x', 'D', 'd', '|', '_']"
   echo "d = [$(awk '{print $NF}' $dbfile | paste -s -d ,)]"
   echo "t = [[] for x in xrange(4001)]" # This sucks like hell ! Should probably use a hmap

   for p in 53 80 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000;
   do
      echo -n "t[$p] = ["
      v=$(grep "precision $p " $apfile | awk '{print $NF}' | paste -s -d ",")
      echo "$v]"
   done
   echo "m = 0"
   echo "i = [" $(seq 10 10 100 | paste -s -d ',') "]"

   echo "fig = plt.figure()"
   echo "ax = fig.add_subplot(111)"

   # Handle the double specifically
   echo "plt.plot(i, d, marker=marks[m], c=np.random.rand(3,), label=\"double\")"
   echo "m = m + 1"

   for p in 53 80 100 200 300 400 500 600 700 800 900 1000 2000 3000 4000;
   do
      echo "plt.plot(i, t[$p], marker=marks[m], c=np.random.rand(3,), label=\"precision $p\")"
      echo "m = m + 1"
   done
   #echo "plt.title(r'Computation of \$e\$ using Taylor expansion')"
   echo "plt.xlabel(r'Expansion degree')"
   echo "plt.ylabel(r'Simulation wall-clock time (s)')"
   echo "plt.xticks(i)"
   echo "plt.yscale('log')"
   echo "plt.legend(loc=7, bbox_to_anchor=(1, 0.65), prop={'size': 7})" # https://matplotlib.org/api/_as_gen/matplotlib.pyplot.legend.html#matplotlib.pyplot.legend
   echo 'for k, v in zip(i,t[4000]) :'
   echo '    ax.text(k, 1.1*v, "%1.2f" %v, ha="center")'
   echo 'for k, v in zip(i,d) :'
   echo '    ax.text(k, 1.1*v, "%1.2f" %v, ha="center")'
   echo "plt.tight_layout(pad=0.4, w_pad=0.5, h_pad=1.0)"
   echo "plt.savefig('e.pdf')"
fi
