#!/bin/bash

nohup raspistill -tl 500 -t 0 -bm -w 640 -h 480 -o ~/archive/a%05d.jpg -l ~/Vision2016/latest.jpg &
./highgoal.bin
