gdb -ex 'target remote localhost:9998' -ex 'break boot' -ex 'call boot()' ./kernel
