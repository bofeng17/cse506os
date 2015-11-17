gdb -ex 'target remote localhost:9998' -ex 'set logging on' -ex 'break boot' -ex 'call boot()' ./kernel
