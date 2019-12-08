set terminal png  enhanced lw 2 font Helvetica 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.png"
set title "Average Throughput vs No. of Users"
set xlabel "Users"
set xrange [0:60]
set ylabel "Throuhput [Mbps]"
set yrange [0:2]

plot "1usrTP.data" with lines title "1 User", \
     "5usrTP.data" with lines title "5 Users", \
     "10usrTP.data" with lines title "10 Users", \
     "20usrTP.data" with lines title "20 Users", \
     "50usrTP.data" with lines title "50 Users"
#-------------------------------------------------------
set out "wifi-delay.png"
set title "Average Delay vs No. of Users"
set xlabel "Users"
set xrange [0:60]
set ylabel "Delay [ms]"
set yrange [0:2000]

plot "1usrdelay.data" with lines title "1 User", \
     "5usrdelay.data" with lines title "5 Users", \
     "10usrdelay.data" with lines title "10 Users", \
     "20usrdelay.data" with lines title "20 Users", \
     "50usrdelay.data" with lines title "50 Users"

#-------------------------------------------------------
set out "wifi-loss.png"
set title "Average vs No. of Users"
set xlabel "Users"
set xrange [0:60]
set ylabel "Packet Loss Ratio [%]"
set yrange [0:5]

plot "1usrloss.data" with lines title "1 User", \
     "5usrloss.data" with lines title "5 Users", \
     "10usrloss.data" with lines title "10 Users", \
     "20usrloss.data" with lines title "20 Users", \
     "50usrloss.data" with lines title "50 Users"
