set terminal png  enhanced lw 2 font Helvetica 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.png"
set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Throuhput [Mbps]"
set yrange [0:1]

plot "throughput.data" with lines title "WiFi Throughput vs. Distance"

#-------------------------------------------------------
set out "wifi-delay.png"
set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Delay [ms]"
set yrange [0:1]

plot "delay.data" with lines title "WiFi Delay vs. Distance"

#-------------------------------------------------------
set out "wifi-loss.png"
set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:1]

plot "loss.data" with lines title "WiFi PLR vs. Distance"
