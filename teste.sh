#!/bin/bash

echo "ADDCAR A 50 50 0.04 95" > fifo
sleep 0.1
echo "ADDCAR A 55 50 100 95" > fifo
sleep 0.1
echo "ADDCAR A 70 50 0.04 95" > fifo
sleep 0.1

#echo "START RACE" > fifo
#sleep 1

echo "ADDCAR B 77 50 0.04 95" > fifo
sleep 0.1
echo "ADDCAR B 88 50 0.04 95" > fifo
sleep 0.1
echo "ADDCAR B 99 50 0.04 95" > fifo
sleep 0.1

#echo "START RACE" > fifo
#sleep 1

echo "ADDCAR C 81 50 0.04 95" > fifo
sleep 0.1
echo "ADDCAR C 91 50 0.04 95" > fifo
sleep 0.1
echo "ADDCAR C 36 50 0.04 95" > fifo

