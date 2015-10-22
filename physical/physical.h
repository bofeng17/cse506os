#define PAGE_OCP 0x0001
#define PAGE_FREE 0x0000
#define page_num 10000 //to be changed
//number of total pages which is memory_length >>12    which is smap->length>>12
#define page_index 10000//to be changed
//start of page index which is smap->base >>12

struct page{
	uint64_t info;
	uint64_t next;
	uint64_t index;
	uint64_t reserved;
}__attribute__((packed));

typedef struct page page_sp;


int init_phy_page();
uint32_t find_first_free();
uint32_t find_free_pages(int);
uint64_t allocate_page();
uint64_t allocate_pages();
void set_used(int);
void set_free(int);
int check_usable(uint64_t);