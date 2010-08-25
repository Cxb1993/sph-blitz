# show 'movie' 
# key n: next snapshots
# key p: previous snapshots

n=0
set grid
set title "Movie of traveling wave \n \n (press <n> for next image, <p> for previous image)"
set xlabel "position"
set ylabel "pressure"
plot [0.0:1.0][0.99:1.01] "punto.dat" u 1:4 every :::n::n w p pt 2  t sprintf("n=%.6i", n)

# do not go below zero
bind "p" "n=n>0?n-1:0; replot"

bind "n" "n=n+1; replot"
