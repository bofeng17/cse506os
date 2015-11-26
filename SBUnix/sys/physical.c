#include <sys/sbunix.h>
#include <sys/stdlib.h>
#include <sys/physical.h>

static uint64_t physfree = 0;
static uint32_t page_inuse_num = 0;
//static uint32_t num_for_init=0;
//static uint64_t kmalloc_base = 0; //the start of kmalloc usable physical address

extern uint32_t page_num;
//age_sp* page_struct_start=(page_sp*)(&kernofs+physfree);
//page_sp* page_struct_start=(page_sp*)(0xffffffff80350000);
//int page_num=((page_length)>>12);
extern page_sp* page_struct_start;
extern uint32_t page_index;

//each page is 4kb

int init_phy_page(uint32_t num, uint32_t page_num, uint32_t page_index) {
	if (num > page_num) {
		printf("ERROR: number is too big to init page");
	}

	//dprintf("struct start=%p\n", page_struct_start);

	page_sp* page_tmp;
	uint32_t i = 0;
	uint32_t index_ = page_index;

	while (i < page_num) {

		page_tmp = page_struct_start + i;

		if (i < num) {
			//printf("%d\n",i);
			page_tmp->info = PAGE_OCP;
			//printf("%x\n",page_tmp->info);
		} else if (i >= num) {
			page_tmp->info = PAGE_FREE;
		}

		page_tmp->index = index_;

		page_tmp->ref_count = -1;

		page_tmp->next = page_index + 1;

		i++;
		index_++;
		//(page_index)=(page_index)+1;
		page_inuse_num += num;

	}

	// printf("lalalala");
	return 0;
}

void set_used(uint32_t index) {
	page_sp* tmp = (page_sp*) (page_struct_start + index);
	tmp->info &= PAGE_OCP;
}

void set_free(uint32_t index) {
	page_sp* tmp = (page_sp*) (page_struct_start + index);
	tmp->info |= PAGE_FREE;
}

uint32_t find_first_free() {
	uint32_t i = 0;
	//uint32_t start=0;

	//printf("struct start :%x\n", page_struct_start);

	for (i = 0; i < page_num; i++) {
		//printf("page_num:%x",page_num);
		page_sp* tmp = (page_struct_start + i);
		if (tmp->info & PAGE_OCP) {
			//printf("find one! %x",i);
			continue;
		} else {
			//tmp->info=PAGE_OCP;
			//printf("find one! %x",i);
			return i;
		}
		//start=i;
	}

	//return start;
	return 0;
}

uint32_t find_free_pages(uint32_t num) {
	uint32_t i = 0, j = 0;
	//int number=0;
	//int start=0;

	for (i = 0; i < page_num - num + 1; i++) {
		if ((i + num) > page_num) {
			printf("ERROR: num overflows");
			break;
		}

		page_sp* tmp = (page_struct_start + i);

		if (!(tmp->info & PAGE_OCP)) {
			int number = 0;
			for (j = 0; j < num; j++) {
				if (!(tmp->info & PAGE_OCP)) {
					number++;

				}
			}

			if (number == num) {
				return i;
			}

		}

	}

	printf("we don't have enough physical memory to allocate");
	return 0;

	//return start;
}

uint64_t allocate_page() {
	uint32_t start = find_first_free();
	//return (uint64_t)(0xffffffff80200000+start);
	page_sp* tmp = page_struct_start + start;
	tmp->info = PAGE_OCP;
	//printf("see1: %x\n",tmp->index);
	return (uint64_t)((tmp->index) << 12);

}

uint64_t allocate_pages(uint32_t num) {
	uint32_t i;
	uint32_t start = find_free_pages(num);
	//return (uint64_t)(0xffffffff80200000+start);
	page_sp* tmp = (page_struct_start + start);
	//page_sp* temp=(page_struct_start+start);
	for (i = 0; i < num; i++) {
		page_sp* temp = (page_struct_start + i);
		temp->info = PAGE_OCP;
	}
	//return (uint64_t)(start<<12)
	return (uint64_t)((tmp->index) << 12);
}

uint64_t allocate_page_user() {
	uint32_t i = 8192;
	//uint32_t start=0;

	//printf("struct start :%x\n", page_struct_start);

	for (i = 8192; i < page_num; i++) {
		//printf("page_num:%x",page_num);
		page_sp* tmp = (page_struct_start + i);
		if (tmp->info & PAGE_OCP) {
			//printf("find one! %x",i);
			continue;
		} else {
			//tmp->info=PAGE_OCP;
			//printf("find one! %x",i);
			//return i;
			page_sp* tmp = page_struct_start + i;
			tmp->info = PAGE_OCP;
			tmp->ref_count = 0;
			return (uint64_t)((tmp->index) << 12);

		}
		//start=i;
	}

	//return start;
	printf("WARNING: all the physical pages are used!");
	return 0;
}

//get initial page numbers
uint32_t get_num_init(uint64_t kfree) {
	physfree = kfree;
	return ((physfree >> 12) + 256 + 1);
}

//get the start of kmalloc usable physical address
uint64_t get_kmalloc_base() {
	return get_num_init(physfree) << 12;
}

void phy_free(uint64_t addr) {
	uint32_t tmp = (addr >> 12);
	set_free(tmp);
}

page_sp *get_page_frame_descriptor(uint64_t phys_addr) {
	uint32_t index = (phys_addr >> 12) - page_index;
	page_sp* tmp = page_struct_start + index;
	return tmp;
}
