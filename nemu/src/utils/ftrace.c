#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

#define MAX_FUNCS 1024

typedef struct {
  uint32_t addr;
  uint32_t size;
  const char *name;
} FuncInfo;

FuncInfo func_table[MAX_FUNCS];
int func_cnt = 0;


void read_elf_symbols(const char *elf_path) {
  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    perror("Cannot open ELF file");
    exit(1);
  }

  // Read ELF header
  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    perror("Failed to read ELF header");
    exit(1);
  }
  assert(*(uint32_t *)ehdr.e_ident == 0x464C457F && "Not a valid ELF");

  // Read section headers
  Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
  assert(shdrs);
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  if (fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, fp) != ehdr.e_shnum) {
    perror("Failed to read section headers");
    exit(1);
  }

  // Read .shstrtab
  Elf32_Shdr shstr = shdrs[ehdr.e_shstrndx];
  char *shstrtab = malloc(shstr.sh_size);
  assert(shstrtab);
  fseek(fp, shstr.sh_offset, SEEK_SET);
  if (fread(shstrtab, 1, shstr.sh_size, fp) != shstr.sh_size) {
    perror("Failed to read .shstrtab");
    exit(1);
  }

  // Find .symtab and .strtab
  Elf32_Shdr symtab_hdr = {0}, strtab_hdr = {0};
  for (int i = 0; i < ehdr.e_shnum; i++) {
    const char *name = &shstrtab[shdrs[i].sh_name];
    if (strcmp(name, ".symtab") == 0) symtab_hdr = shdrs[i];
    if (strcmp(name, ".strtab") == 0) strtab_hdr = shdrs[i];
  }

  assert(symtab_hdr.sh_size > 0 && strtab_hdr.sh_size > 0);

  // Read .strtab
  char *strtab = malloc(strtab_hdr.sh_size);
  assert(strtab);
  fseek(fp, strtab_hdr.sh_offset, SEEK_SET);
  if (fread(strtab, 1, strtab_hdr.sh_size, fp) != strtab_hdr.sh_size) {
    perror("Failed to read .strtab");
    exit(1);
  }

  // Read .symtab
  int num_syms = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
  Elf32_Sym *syms = malloc(symtab_hdr.sh_size);
  assert(syms);
  fseek(fp, symtab_hdr.sh_offset, SEEK_SET);
  if (fread(syms, symtab_hdr.sh_entsize, num_syms, fp) != num_syms) {
    perror("Failed to read .symtab");
    exit(1);
  }

  // Store function symbols
  func_cnt = 0;
  for (int i = 0; i < num_syms && func_cnt < MAX_FUNCS; i++) {
    Elf32_Sym sym = syms[i];
    if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC && sym.st_size > 0) {
      func_table[func_cnt].addr = sym.st_value;
      func_table[func_cnt].size = sym.st_size;
      func_table[func_cnt].name = strdup(&strtab[sym.st_name]);
      func_cnt++;
    }
  }

  printf("Loaded %d functions from ELF file.\n", func_cnt);

  free(shstrtab);
  free(shdrs);
  free(strtab);
  free(syms);
  fclose(fp);
}




static int call_depth = 0;

// Helper: find function name given an address
const char *find_func_name_by_addr(uint32_t addr, uint32_t *func_addr_out) {
    for (int i = 0; i < func_cnt; i++) {
      uint32_t start = func_table[i].addr;
      uint32_t end = start + func_table[i].size;
      if (addr >= start && addr < end) {
        if (func_addr_out) *func_addr_out = start;
        return func_table[i].name;
      }
    }
    return NULL;
  }
  

// Main ftrace logic
void ftrace_exec(uint32_t pc, uint32_t target, uint32_t inst) {
  uint32_t opcode = inst & 0x7f;

  // Function call: jal or jalr
  if (opcode == 0b1101111 || opcode == 0b1100111) {
    const char *func_name = find_func_name_by_addr(target, NULL);
    if (func_name) {
      for (int i = 0; i < call_depth; i++) printf("  ");
      printf("Call %s@0x%08x\n", func_name, target);
      call_depth++;
    }
  }

  // Function return: jalr x0, ra, 0
  if (opcode == 0b1100111) {
    int rd  = (inst >> 7) & 0x1f;
    int rs1 = (inst >> 15) & 0x1f;
    int imm = (int32_t)inst >> 20;

    if (rd == 0 && rs1 == 1 && imm == 0) { // ret
      call_depth--;
      if (call_depth < 0) call_depth = 0; // safety
      uint32_t func_addr = 0;
      const char *func_name = find_func_name_by_addr(pc, &func_addr);
      if (func_name) {
        for (int i = 0; i < call_depth; i++) printf("  ");
        printf("Return %s@0x%08x\n", func_name, func_addr);
      }
    }
  }
}