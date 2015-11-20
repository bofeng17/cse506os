#include <sys/process.h>
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3

#define PAGE_SIZE 0x1000

#define PH_TYPE_X 0x1
#define PH_TYPE_W 0x2
#define PH_TYPE_R 0x4

struct elf_header
{
  unsigned char ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;

};

typedef struct elf_header elf_h;

struct program_header
{
  uint32_t p_type;
  uint32_t p_flag;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

typedef struct program_header pgm_h;

void
test_elf ();
void
load_elf (task_struct*, void*);

