##README

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
FILE (5)
-----------------file_base
VMA (4)
-----------------vma_base
USERPT (3)
-----------------userpt_base
MM (2)
-----------------mm_base
KSTACK (1)
-----------------kstack_base
TASK (0)
-----------------kmalloc (task_base) 0MB

```