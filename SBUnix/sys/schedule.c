#include <sys/sbunix.h>
#include <sys/process.h>
#include <sys/virmm.h>

extern task_struct* end;
extern task_struct* current;

task_struct *prev;
void
schedule ()
{

  prev = current;
  current = current->next;

//  if (current->pid == 0)
//    {
//      current = current->next;
//    }

  clear_zombie (current);

  while (current->task_state != TASK_READY)
    {
      current = current->next;
      if (current == prev)
	break;
    }

  //dprintf ("prev thread name:%s \n", prev->task_name);

  //dprintf ("current thread name:%s\n", current->task_name);
//  if (current != prev)
//    {
//  front = front->next;
  end = prev;
  // dprintf ("begin switch from %s to %s\n", prev->task_name,current->task_name);
  context_switch (prev, current);
  //  dprintf ("finish switch from %s to %s\n", prev->task_name,current->task_name);

//    }

}

void
clear_zombie (task_struct* zombie)
{
  if (zombie->task_state == TASK_ZOMBIE)
    {
      current = zombie->next;
      prev->next = zombie->next;
      zombie->next = NULL;
      kfree (zombie, TASK);
    }
}
