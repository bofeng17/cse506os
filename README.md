# cse506os

##Makefile Changes
 Three changes
 - add '-fno-stack-protector' gcc option so that we can compile it on local linux machine
 - make qemu to run the kermel
 - make debug for debug. Qemu starts without do anything, except waiting for gdb connection. After connected gdb, set break points and continue.

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

###Coding
- [sbunix](https://github.com/zhtlancer/sbunix/tree/shcho-dev)
- [sam8dec/cse506-SBUnix](https://github.com/sam8dec/cse506-SBUnix)
- [arpitsardhana/SBUnix](https://github.com/arpitsardhana/SBUnix)
