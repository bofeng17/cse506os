# cse506os

##Makefile Changes
 Three changes
 - add '-fno-stack-protector' gcc compile option so that we can compile it on local linux machine
 - add 'make qemu' command to run the kermel
 - add 'make debug' for debugging. Qemu starts without do anything, except waiting for gdb connection. After connected gdb, set break points and continue. "**gdb -ex 'target remote localhost:9998' -ex 'break boot' -ex 'call boot()' ./kernel** "is a quick way to do this

##Dev Notes

1.Simplify git operations
- Just use sh pull.sh (or ./pull.sh) to pull down latest repo
- Use sh push.sh (or ./push.sh) to push your repo, when propmting for commit msg, type your info and press enter
2.When debugging,use dprintf if you want to print results. If you define variables, surrounding your code between #if DEBUG and #endif,eg:
    #if DEBUG
		page_sp* page_tmp = (page_sp*) (page_struct_start) + 100;
		dprintf("see: %x\n", page_tmp->index);
	#endif

##Stages

###First stage
 
 memory management 
- physical memory
- virtual memory

##References

###Knowledge
- [Operating Systems Development - Physical Memory](http://www.brokenthorn.com/Resources/OSDev17.html)
- [Operating System Development Series](http://www.brokenthorn.com/Resources/OSDevIndex.html)
- [内存管理的那些事儿](http://edsionte.com/techblog/%E5%86%85%E5%AD%98%E7%AE%A1%E7%90%86)
- [LinuxMM](http://linux-mm.org/LinuxMM)

###Coding
- [sbunix](https://github.com/zhtlancer/sbunix/tree/shcho-dev)
- [sam8dec/cse506-SBUnix](https://github.com/sam8dec/cse506-SBUnix)
- [arpitsardhana/SBUnix](https://github.com/arpitsardhana/SBUnix)
