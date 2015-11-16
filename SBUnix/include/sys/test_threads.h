#include <sys/sbunix.h>
#include <sys/stdio.h> //kernel should not include user header files
#include <sys/stdlib.h>//kernel should not include user header files
//#include <sys/process.h>

void
func_a ()
{
  int i = 0;
  while (i < 3)
    {
      dprintf (" aaaaaaa thread %d \n", i++);
      schedule ();
//      __asm__ __volatile__ ("hlt");
    }
  exit (0);
}

void
func_b ()
{
  int i = 0;
  while (i < 6)
    {
      dprintf (" bbbbbbbb thread %d \n", i++);
      schedule ();

//      __asm__ __volatile__ ("hlt");
    }
  exit (0);
}

void
func_c ()
{
  int i = 0;
  while (1)
    {
      dprintf (" cccccccc thread %d \n", i++);
      schedule ();
//      __asm__ __volatile__ ("hlt");
    }
}

void
print_threads (task_struct* front)
{
  while (1)
    {
      dprintf ("pid of %s is: %d\n", front->task_name, front->pid);
      front = front->next;
      if (front->pid == 0)
	break;
    }
}
