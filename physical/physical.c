#include <stdio.h>
#include <stdlib.h>
#include <sys/physical.h>


static uint32_t page_inuse_num=0;

//each page is 4kb


int init_phy_page(int num)
{
	if(num<page_num)
	{
		printf("ERROR: number is too big to init page");
	}

	page_sp* page_tmp;
	int i=0;
	uint64_t index_=page_index;

	while(i<page_num)
	{
		page_tmp=(page_sp*)(0xffffffff80200000+i);

	if(i<num)
	{
		page_tmp->info=PAGE_OCP;
	}
	else if(i>=num)
	{
		page_tmp->info=PAGE_FREE;
	}

	page_tmp->index=index_;

	page_tmp->next=page_index+1;

	i++;
	index_++;
	//(page_index)=(page_index)+1;
	page_inuse_num+=num;

    }
return 0;
}

uint32_t find_first_free()
{
	int i=0;
	int start=0;

	for(i=0;i<page_num;i++)
	{
		page_sp* tmp=(page_sp*)(0xffffffff80200000+i);
		if(tmp->info&PAGE_OCP)
		{
			break;
		}
		else if(tmp->info&PAGE_FREE)
		{
			tmp->info=PAGE_OCP;
		}
		start=i;
	}
	
	return start;
}

uint32_t find_free_pages(int num)
{
	int i=0,j=0;
	int number=0;
	int start=0;

	for(i=0;i<page_num-num+1;i++)
	{
		if((i+num)>page_num)
		{
			printf("ERROR: num overflows");
			break;
		}
        
		page_sp* tmp=(page_sp*)(0xffffffff80200000+i);

		for(j=0;j<num;j++)
		{
			//number=0;
			if(tmp->info==0)
				{
					tmp->info=PAGE_OCP;
					number++;
				}
			else
				{
                    break;
				}

			
		}

		if(number==num)
			{
				start=i;
			}

		else
		{
			printf("we don't have enough physical memory to allocate");
			return 0;
		}

	}

	return start;
}

uint64_t allocate_page()
{
	int start=find_first_free();
	//return (uint64_t)(0xffffffff80200000+start);
	return (uint64_t)(start<<12)

}

uint64_t allocate_pages(int num)
{
	int start=find_free_pages(num);
	//return (uint64_t)(0xffffffff80200000+start);
	return (uint64_t)(start<<12)
}







