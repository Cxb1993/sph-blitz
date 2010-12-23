#! /bin/bash

#eps solid for solid (not dashed) lines, momochrome (instead of color),... 


gnuplot << EOF
set terminal postscript eps enhanced color "Helvetica" 26
set output "suplen_influence.eps"

set key bottom right;
set grid;
set mxtics 0.25;
set xtics 0.5;

set xlabel "Ratio l_s/dx"; set ylabel "Periodical time T";
plot [1:5.5][0.5:1.0] 'supLen_dx_influence_on_T' u 1:3 title 'Exact' with line lt 2 lw 8, 'supLen_dx_influence_on_T' u 1:2 title 'Simulation' with points lt 1 lw 8;

set terminal png
set output "suplen_influence.png"
replot

reset

EOF