#! /bin/bash

#eps solid for solid (not dashed) lines, momochrome (instead of color),... 
cp ../../src/outdata/prtl$1.dat prtl$1.dat

gnuplot << EOF
set terminal postscript eps enhanced color  "Helvetica" 26

set output "U_profiles$1.eps"


set grid;
set xlabel "U"; set ylabel "length y";
plot [0.0:][0.0:]  'exact*.dat' u 2:1 title 'exact' with line lt 2 lw 8,'prtl$1.dat' u 3:2 title 'simu' lt 1 with points;

set terminal png large
set output "U_profiles$1.png"
replot

reset

EOF
