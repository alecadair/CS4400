#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <elf.h>

/* Given the in-memory ELF header pointer as `ehdr` and a section
   header pointer as `shdr`, returns a pointer to the memory that
   contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

static void check_for_shared_object(Elf64_Ehdr *ehdr);
static void fail(char *reason, int err_code);
static void print_function_indeces(Elf64_Ehdr*);
static unsigned long process_0xe9_instr(char*);
static int get_four_values(char*);

Elf64_Ehdr *ehdr;


static Elf64_Shdr* dynsym_shdr;
static Elf64_Sym* syms;
static char* strs;
static Elf64_Shdr* reladyn_shdr;
static Elf64_Shdr* relaplt_shdr;
static Elf64_Rela* reladyn;
static Elf64_Rela* relaplt;

int get_four_values(char* mem_addr){
	int j;
	unsigned int offset = 0;
	for(j = 0; j < 4; j++){
		unsigned char val = *(mem_addr + j);
		unsigned int valcastint = (unsigned int)val;
		valcastint = valcastint <<(j << 3); //shift the cast 8 times j.
		offset |= valcastint;
	}
	return (int)offset;
}

Elf64_Shdr* section_by_index(Elf64_Ehdr* ehdr, int index){
    Elf64_Shdr *shdrs = (void*)ehdr+ehdr->e_shoff; //get the array of sec headers
    return (Elf64_Shdr*)&shdrs[index];
}

Elf64_Shdr * section_by_name(Elf64_Ehdr *ehdr, char*name){
    Elf64_Shdr *shdrs = (void*)ehdr+ehdr->e_shoff; //get the array of sec headers
    char* strs = (void*)ehdr+shdrs[ehdr->e_shstrndx].sh_offset; //get the string table
    int i;
    for(i = 0; i < ehdr->e_shnum;i++){
        if(strcmp(strs+shdrs[i].sh_name,name) == 0){
        	return (Elf64_Shdr*)&shdrs[i];
        }
    }
    return 0;
}

int main(int argc, char **argv) {
  int fd;
  size_t len;
  void *p;
//  Elf64_Ehdr *ehdr;

  if (argc != 2)
    fail("expected one file on the command line", 0);

  /* Open the shared-library file */
  fd = open(argv[1], O_RDONLY);
  if (fd == -1)
    fail("could not open file", errno);

  /* Find out how big the file is: */
  len = lseek(fd, 0, SEEK_END);

  /* Map the whole file into memory: */
  p = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (p == (void*)-1)
    fail("mmap failed", errno);

  /* Since the ELF file starts with an ELF header, the in-memory image
     can be cast to a `Elf64_Ehdr *` to inspect it: */
  ehdr = (Elf64_Ehdr *)p;

  /* Check that we have the right kind of file: */
  check_for_shared_object(ehdr);

  /* Add a call to your work here */

  print_function_indeces(ehdr);
  return 0;
}

static void process_machine_instruction(char* func_location){
	int j;
	int offset = get_four_values(func_location + 3);
	offset += 0x7+(unsigned long)func_location;
	offset &= 0xfff;

	/*iterate through .rela.dyn*/
	int rela_dyn_count;
	rela_dyn_count = reladyn_shdr->sh_size/sizeof(Elf64_Rela);
	unsigned long symbol_index = 0;
	for(j = 0; j < rela_dyn_count; j++){
		unsigned long relaVal = reladyn[j].r_offset & 0xfff;
		if(relaVal == (unsigned long)offset){
			symbol_index = ELF64_R_SYM(reladyn[j].r_info);
		}
	}
	char* index_name = strs+syms[symbol_index].st_name;
	printf("  %s\n",index_name);
}
/*
 * takes in the address of the 0xe9 instruction to be processed.
 * address is address of instruction
 */
static unsigned long process_0xe9_instr(char* mem_addr_e9){
	int new_offset = 0;
	char* newMem = mem_addr_e9 + 1;
	new_offset = (int)get_four_values(newMem);
	char* new_addr = mem_addr_e9 + new_offset + 0x5;
	int result = get_four_values(new_addr+2);
	unsigned long new_new_addr = (unsigned long)(new_addr + result + 0x6);
	return new_new_addr;
}

static void print_function_indeces(Elf64_Ehdr*ehdr){
	int i, j;

	dynsym_shdr = section_by_name(ehdr,".dynsym");
	syms = AT_SEC(ehdr, dynsym_shdr);
	strs = AT_SEC(ehdr, section_by_name(ehdr,".dynstr"));
	reladyn_shdr = section_by_name(ehdr,".rela.dyn");
	relaplt_shdr = section_by_name(ehdr,".rela.plt");
	reladyn = AT_SEC(ehdr,reladyn_shdr);
	relaplt = AT_SEC(ehdr,relaplt_shdr);
	int count = dynsym_shdr->sh_size/sizeof(Elf64_Sym);
	for(i = 0; i < count; i++){
		char* sym_name = strs+syms[i].st_name;
		if(ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC &&
				(sym_name[0] != '_' && sym_name[1] != '_' && sym_name[0] != '\0')){
			printf("%s\n",strs+syms[i].st_name);
			Elf64_Shdr *shdr = section_by_index(ehdr,syms[i].st_shndx);
			char* func_location = AT_SEC(ehdr,shdr) + syms[i].st_value - shdr->sh_addr;
			char* first_byte = func_location;
			char* second_byte = (func_location+1);
			for(j = 0; ((*first_byte&0xff) != 0xc3); j++){
				unsigned char first_val = (*first_byte);
				unsigned char second_val = (*second_byte);
				/*Process a movq instruction*/
				if(first_val == 0x48 && second_val == 0x8b){
					process_machine_instruction(first_byte);
					first_byte = first_byte + 7;
					second_byte = second_byte + 7;
				}
				else if((first_val == 0x63 || first_val == 0x89 || first_val == 0x8b)&&((second_val&0xc0) == 0xc0)){
					//mov is from one reg to another
					first_byte++;
					second_byte++;
				}
				else if(first_val == 0x48 && (second_val == 0x63 || second_val == 0x89 || second_val == 0x8b)){
					char third_val = (*(second_byte +1)&0xff);
					if(third_val&&0xc0 == 0xc0){
						first_byte+=3;
						second_byte+=3;
					}else{
						first_byte++;
						second_byte++;
					}
				}
				else if(first_val == 0xe9){
					long sym_offset = process_0xe9_instr(first_byte);
					sym_offset &= 0xfff;


					/*iterate through .rela.plt*/
					int rela_plt_count = 0;
					rela_plt_count = relaplt_shdr->sh_size/sizeof(Elf64_Rela);
					unsigned long symbol_index = 0;
					int k;
					for(k = 0; k < rela_plt_count; k++){
						long relaVal = relaplt[k].r_offset&0xfff;
						if(relaVal == sym_offset){
							symbol_index = ELF64_R_SYM(relaplt[k].r_info);
						}
					}
					char* index_name = strs+syms[symbol_index].st_name;
					printf("  (%s)\n",index_name);
					first_byte+=5;
					second_byte+=5;
					break;
				}
				else if(first_val == 0xeb){
					char offset = *(first_byte+1);
					char* next_func = offset + first_byte + 2;
					int next_offset = get_four_values(next_func+3);
					long dynsym_index = next_offset + (unsigned long)next_func + 7;
					dynsym_index &= 0xfff;
					/*iterate through .rela.plt*/
					int rela_dyn_count = 0;
					rela_dyn_count = reladyn_shdr->sh_size/sizeof(Elf64_Rela);
					unsigned long symbol_index = 0;
					int k;
					for(k = 0; k < rela_dyn_count; k++){
						//unsigned long relaVal = reladyn[j].r_offset & 0xfff;
						long relaVal = reladyn[k].r_offset&0xfff;
						if(relaVal == dynsym_index){
							symbol_index = ELF64_R_SYM(reladyn[k].r_info);
							//symbol_found = 1;
						}
					}
					char* index_name = strs+syms[symbol_index].st_name;
					printf("  %s\n",index_name);
					first_byte +=2;
					second_byte +=2;
				}
				else if(first_val == 0x66 && second_val == 0x66){
					break;
				}
				else{
					first_byte+=1;
					second_byte+=1;
				}
			}
		}
	}
}

static void check_for_shared_object(Elf64_Ehdr *ehdr) {
  if ((ehdr->e_ident[EI_MAG0] != ELFMAG0)
      || (ehdr->e_ident[EI_MAG1] != ELFMAG1)
      || (ehdr->e_ident[EI_MAG2] != ELFMAG2)
      || (ehdr->e_ident[EI_MAG3] != ELFMAG3))
    fail("not an ELF file", 0);

  if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    fail("not a 64-bit ELF file", 0);
  
  if (ehdr->e_type != ET_DYN)
    fail("not a shared-object file", 0);
}

static void fail(char *reason, int err_code) {
  fprintf(stderr, "%s (%d)\n", reason, err_code);
  exit(1);
}
