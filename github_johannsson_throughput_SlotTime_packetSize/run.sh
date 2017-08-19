#!/bin/bash

  echo "*******************************"
  echo "Starting simulation 1/3"
for i in {1..10}
do

  echo "-------------------------------"
  echo "starting batch n=$i"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i" &>logs/r"$i".txt
  echo "1/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --slot=12" &>logs/r"$i"_s12.txt
  echo "2/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --slot=25" &>logs/r"$i"_s25.txt
  echo "3/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --slot=30" &>logs/r"$i"_s30.txt
  echo "4/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --slot=50" &>logs/r"$i"_s50.txt
  echo "5/5"
  echo "n=$i finished"
  echo "-------------------------------"
  echo ""
done

echo "*******************************"
echo "Starting simulation 2/3"
for i in {1..10}
do

  echo "-------------------------------"
  echo "starting batch n=$i"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --ps=1000" &>logs/r"$i"_p1000.txt
  echo "1/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --ps=800" &>logs/r"$i"_p800.txt
  echo "2/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --ps=600" &>logs/r"$i"_p600.txt
  echo "3/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --ps=400" &>logs/r"$i"_p400.txt
  echo "4/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=$i --ps=200" &>logs/r"$i"_p200.txt
  echo "5/5"
  echo "n=$i finished"
  echo "-------------------------------"
  echo ""
done

echo "*******************************"
echo "Starting simulation 3/3"
for i in {1..5}
do
  if [ $i -eq 1 ]
  then
    s="12"
  fi
  if [ $i -eq 2 ]
  then
    s="20"
  fi
  if [ $i -eq 3 ]
  then
    s="25"
  fi
  if [ $i -eq 4 ]
  then
    s="30"
  fi
  if [ $i -eq 5 ]
  then
    s="50"
  fi
  echo $s

  echo "-------------------------------"
  echo "starting batch n=$i"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=1 --ps=1000 --slot=$s" &>logs/r_p1000_s"$s".txt
  echo "1/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=1 --ps=800 --slot=$s" &>logs/r_p800_s"$s".txt
  echo "2/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=1 --ps=600 --slot=$s" &>logs/r_p600_s"$s".txt
  echo "3/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=1 --ps=400 --slot=$s" &>logs/r_p400_s"$s".txt
  echo "4/5"
  ./waf --cwd=logs --run "src/ET4394_ns3_Johannsson --n=1 --ps=200 --slot=$s" &>logs/r_p200_s"$s".txt
  echo "5/5"
  echo "n=$i finished"
  echo "-------------------------------"
  echo ""
done

echo "Generating plots"
python plot_slot.py
python plot_packetSize.py
python plot_packetSize_slot.py
echo "Finished"
