set terminal png  enhanced lw 2 font Helvetica 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.png"
set title "Average Throughput Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Throuhput [Mbps]"
set yrange [0:2]

plot "1usrTP.data" with lines title "1 User", \
     "5usrTP.data" with lines title "5 Users", \
     "10usrTP.data" with lines title "10 Users", \
     "15usrTP.data" with lines title "15 Users", \
     "20usrTP.data" with lines title "20 Users"

#-------------------------------------------------------
set out "wifi-delay.png"
set title "Average Delay Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Delay [ms]"
set yrange [0:520]

plot "1usrdelay.data" with lines title "1 User", \
     "5usrdelay.data" with lines title "5 Users", \
     "10usrdelay.data" with lines title "10 Users", \
     "15usrdelay.data" with lines title "15 Users", \
     "20usrdelay.data" with lines title "20 Users"

#-------------------------------------------------------
set out "wifi-loss.png"
set title "Average Loss Over Distance"
set xlabel "Distance (m)"
set xrange [0:210]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:100]

plot "1usrloss.data" with lines title "1 User", \
     "5usrloss.data" with lines title "5 Users", \
     "10usrloss.data" with lines title "10 Users", \
     "15usrloss.data" with lines title "15 Users", \
     "20usrloss.data" with lines title "20 Users"
