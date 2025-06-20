set terminal pngcairo size 1000,600 enhanced font 'Verdana,10'
set output 'performance_chart_algo.png'
set title 'Average Time per Algorithm'
set xlabel 'Algorithm'
set ylabel 'Average Time (ms)'
set style data histogram
set style fill solid border -1
set boxwidth 0.7
set xtics rotate by -45 scale 0
plot 'plot_data_algo.txt' using 2:xtic(1) title 'Average Time'
