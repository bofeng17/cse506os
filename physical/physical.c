#include <stdio.h>
#include <stdlib.h>


#define PAGE_OCP 0x0001
#define PAGE_FREE 0x0000

static uint32_t page_inuse_num=0;

//each page is 4kb
struct page{
	uint64_t info;
	uint64_t next;
	uint64_t reserved1;
	uint64_t reserved2;
}__attribute__((packed)) *page_sp;


int init_phy_page(int num)
{
	if(num<page_num)
	{
		printf("ERROR: number is too big to init page");
	}

	page_sp* page_tmp;
	int i=0;

	while(i<page_num)
	{
		page_tmp=0xffffffff80200000+i;

	if(i<num)
	{
		page_tmp=PAGE_OCP;
	}
	else if(i>=num)
	{
		page_tmp=PAGE_FREE;
	}

	page_tmp->next=page_index_+1;

	i++;
	page_index_++;
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
		page_sp* tem=0xffffffff80200000+i;
		if(tmp->info&PAGE_OCP)
		{
			break;
		}
		elseif(tmp->info&PAGE_FREE)
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
	int number;
	int start=0;

	for(i=0;i<page_num-num+1;i++)
	{
		if((i+num)>page_num)
		{
			printf("ERROR: num overflows");
			break;
		}
        
		page_sp* tem=0xffffffff80200000+i;

		for(j=0;j<num;j++)
		{
			number=0;
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

		if(number=num)
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
	int start=find_free_page();
	return (uint64_t)(0xffffffff80200000+start);
}

uint64_t allocate_pages(int num)
{
	int start=find_free_pages(num);
	return (uint64_t)(0xffffffff80200000+start);
}







