set terminal pngcairo size 1400,700 enhanced font 'Verdana,10'
set output 'performance_chart_file.png'
set title 'Average Time per File and Algorithm'
set xlabel 'Source file'
set ylabel 'Average Time (ms)'
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9
set xtics rotate by -45 scale 0
set key outside right top vertical
plot 'plot_data_file.txt' using 2:xtic(1) title 'Knuth-Morris-Pratt', \
     '' using 3 title 'Boyer-Moore', \
     '' using 4 title 'Rabin-Karp', \
     '' using 5 title 'Bitap'
