##README

###Detailed Usage
see README file (not this one)

### useful scripts
- use sh run.sh (or ./run.sh) to make and run
- use sh up.sh to upload files to sever
- use sh db.sh open qemu debug window
- use sh gdb.sh begin gdb debugging

###kmalloc memory organization

```
----------------- 32MB

upper kenerl memory

-----------------
FILE (6)
-----------------file_base
VMA (5)
-----------------vma_base
USERPT (4)
-----------------userpt_base
MM (3)
-----------------mm_base
KSTACK (2)
-----------------kstack_base
TASK (1)
-----------------task_base
KERNPT(0)
-----------------kmalloc base (physfree 4k aligned)  

```
