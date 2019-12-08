#!/bin/sh

DISTANCES="0 10 20 30 40 50 60 70 80 90 100 110 120 130 140 150"
DBPREFIX="P1QB"
TRIALS="1"
SIMTIME=20 #Default in wifi-example-sim.cc: The transmission Time approximately equals to the Simulation Running Time

echo WiFi Experiment Example

pCheck=`which sqlite3`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sqlite3 (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which gnuplot`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires gnuplot (wifi-example-sim does not)."
  exit 255
fi

pCheck=`which sed`
if [ -z "$pCheck" ]
then
  echo "ERROR: This script requires sed (wifi-example-sim does not)."
  exit 255
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/

if [ -e ../../P1QB.db ]
then
  echo "Kill data.db? (y/n)"
  read ANS
  if [ "$ANS" = "yes" -o "$ANS" = "y" ]
  then
    echo Deleting database
    rm ../../P1QB.db
  fi
fi

for trial in $TRIALS
do
  for distance in $DISTANCES
  do
    echo Trial $trial, distance $distance
    ../../waf --run "WiFi --distance=$distance --dbPrefix=$DBPREFIX --run=run-$trial-$DBPREFIX-$distance"
  done
done

#
#Another SQL command which just collects raw numbers of frames receved.
#
#CMD="select Experiments.input,avg(Singletons.value) \
#    from Singletons,Experiments \
#    where Singletons.run = Experiments.run AND \
#          Singletons.name='wifi-rx-frames' \
#    group by Experiments.input \
#    order by abs(Experiments.input) ASC;"

mv ../../P1QB.db .

CMD="select exp.input, tx.value, rx.value, delay.value \
    from Singletons rx, Singletons tx, Experiments exp, Singletons delay \
    where rx.run = tx.run AND \
          rx.run = exp.run AND \
          delay.run = exp.run AND \
          rx.variable='receiver-rx-packets' AND \
          tx.variable='sender-tx-packets' AND \
          delay.variable='delay-average' \
    group by exp.input \
    order by abs(exp.input) ASC;"
 


sqlite3 -noheader P1QB.db "$CMD" > wifi-default.temp
sed -i "s/|/ /g" wifi-default.temp
awk '{print $1 " " $4*8*1000/20/1000/1000}' wifi-default.temp >throughput.data
awk '{print $1 " " $5/1000000}' wifi-default.temp >delay.data
awk '{print $1 " " ($3-$4)/$3*100}' wifi-default.temp >loss.data

gnuplot wifi-p1qb.gnuplot

#Clean Directory
mv *.data *.db *.temp QB/Data
mv *.png QB/Images
echo "Done; data in wifi-default.data, plot in wifi-default.eps"
