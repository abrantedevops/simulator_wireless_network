set terminal eps
set output "wifi_project_tr.eps"
set title "Wifi Project - IFPB"
set xlabel "Time (s)"
set ylabel "Throughtput (Mbps)"

set xrange [0:5]
set label '9.042' at 1.000000,9.042297 offset 0.5,0.5 font ',10'
set label '9.047' at 2.000000,9.046829 offset 0.5,0.5 font ',10'
set label '9.041' at 3.000000,9.041061 offset 0.5,0.5 font ',10'
plot "-"  title "Throughtput" with linespoints pointtype 7 pointsize 1 pointinterval 0.5 lc rgb 'blue'
0 0
1 9.0423
2 9.04683
3 9.04106
e
