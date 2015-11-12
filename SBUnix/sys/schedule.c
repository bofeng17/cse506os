#include <sys/sbunix.h>
#include <sys/process.h>

extern task_struct* front;
extern task_struct* end;

void
schedule ()
{

  task_struct *prev = front;
  task_struct* current = front->next;

  //dprintf ("prev thread name:%s \n", prev->task_name);

  //dprintf ("current thread name:%s\n", current->task_name);
//  if (current != prev)
//    {
  front = front->next;
  end = end->next;
  // dprintf ("begin switch from %s to %s\n", prev->task_name,current->task_name);
  context_switch (prev, current);
  //  dprintf ("finish switch from %s to %s\n", prev->task_name,current->task_name);

//    }

}
