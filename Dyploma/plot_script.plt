set terminal pngcairo size 1400,700 enhanced font 'Verdana,10'
set output 'performance_chart.png'
set title 'Algorithm Time Performance per pattern'
set xlabel 'Source file (pattern)'
set ylabel 'Time (ms)'
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9
set xtics rotate by -45 scale 0
set key outside right top vertical
plot 'plot_data.txt' using 2:xtic(1) title 'Knuth-Morris-Pratt', \
     '' using 3 title 'Boyer-Moore', \
     '' using 4 title 'Rabin-Karp', \
     '' using 5 title 'Bitap'
