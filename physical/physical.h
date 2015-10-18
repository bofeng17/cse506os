#define PAGE_OCP 0x0001
#define PAGE_FREE 0x0000
#define page_num 10000

struct page{
	uint64_t info;
	uint64_t next;
	uint64_t reserved1;
	uint64_t reserved2;
}__attribute__((packed)) *page_sp;


int init_phy_page();
uint32_t find_first_free();
uint32_t find_free_pages(int);
uint64_t allocate_page();
uint64_t allocate_pages();