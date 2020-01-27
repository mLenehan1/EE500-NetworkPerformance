set terminal png  enhanced lw 2 font Helvetica 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.png"
set title "Average Throughput Over Transmission Interval"
set xlabel "Transmission Interval (s)"
set xrange [0:0.01]
set ylabel "Throuhput [Mbps]"
set yrange [0:20]

plot "throughput.data" with lines title "WiFi Throughput vs. txInterval"

#-------------------------------------------------------
set out "wifi-delay.png"
set title "Average Delay Over Transmission Interval"
set xlabel "Transmission Interval (s)"
set xrange [0:0.01]
set ylabel "Delay [ms]"
set yrange [0:300]

plot "delay.data" with lines title "WiFi Delay vs. txInterval"

#-------------------------------------------------------
set out "wifi-loss.png"
set title "Average Loss Over Transmission Interval"
set xlabel "Transmission Interval (s)"
set xrange [0:0.01]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:40]

plot "loss.data" with lines title "WiFi PLR vs. txInterval"
