#!/bin/bash
t=`date +"%Y-%m-%d-%H-%M"`
gdb -ex 'target remote localhost:9998' -ex 'set logging file log.txt' -ex 'set logging on' -ex 'break boot' -ex 'c' ./kernel
