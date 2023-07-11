set terminal eps
set output "wifi_project_de.eps"
set title "Wifi Project - IFPB"
set xlabel "Time (s)"
set ylabel "Mean Delay (ms)"

set xrange [0:4]
set label '1.004818' at 1.000000,1.004818 offset 0.5,0.5 font ',10'
set label '0.980516' at 2.000000,0.980516 offset 0.5,0.5 font ',10'
set label '0.653376' at 3.000000,0.653376 offset 0.5,0.5 font ',10'
plot "-"  title "Mean Delay" with linespoints pointtype 13 pointsize 1 pointinterval 0.5 lc rgb 'red'
0 -nan
1 1.00482
2 0.980516
3 0.653376
e
