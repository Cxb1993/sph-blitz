#! /bin/bash

#eps solid for solid (not dashed) lines, momochrome (instead of color),... 
#cp ../../src/outdata/prtl$1.dat prtl$1.dat

gnuplot << EOF
set terminal postscript eps enhanced color  "Helvetica" 26

set output "U_profiles$1.eps"

set key 0.005,0.07
set grid;
set xtics 0.002
set xlabel "Velocity u"; set ylabel "Position y";
plot [0.0:][-0.5:0.5]  'exact$1.dat' u 2:1 title 'Exact' with line lt 2 lw 8,'simuForPlot$1.dat' u 2:1 title 'Simulation' lt 1 lw 8 with points;

set terminal png large
set output "U_profiles$1.png"
replot

reset

EOF
