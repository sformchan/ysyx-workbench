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
  char *name;
} FuncInfo;

static FuncInfo func_table[MAX_FUNCS];
static int func_cnt = 0;

void read_elf_symbols(const char *elf_path) {
  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    perror("Cannot open ELF file");
    exit(1);
  }

  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    perror("Failed to read ELF header");
    exit(1);
  }
  assert(*(uint32_t *)ehdr.e_ident == 0x464C457F && "Not a valid ELF");

  Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
  if (!shdrs) {
    perror("malloc failed");
    exit(1);
  }
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  if (fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, fp) != ehdr.e_shnum) {
    perror("Failed to read section headers");
    exit(1);
  }

  Elf32_Shdr shstr = shdrs[ehdr.e_shstrndx];
  char *shstrtab = malloc(shstr.sh_size);
  if (!shstrtab) {
    perror("malloc failed");
    exit(1);
  }
  fseek(fp, shstr.sh_offset, SEEK_SET);
  if (fread(shstrtab, 1, shstr.sh_size, fp) != shstr.sh_size) {
    perror("Failed to read section header string table");
    exit(1);
  }

  Elf32_Shdr symtab_hdr = {0}, strtab_hdr = {0};
  int found_symtab = 0, found_strtab = 0;
  for (int i = 0; i < ehdr.e_shnum; i++) {
    const char *name = &shstrtab[shdrs[i].sh_name];
    if (strcmp(name, ".symtab") == 0) {
      symtab_hdr = shdrs[i];
      found_symtab = 1;
    }
    if (strcmp(name, ".strtab") == 0) {
      strtab_hdr = shdrs[i];
      found_strtab = 1;
    }
  }
  if (!found_symtab || !found_strtab) {
    fprintf(stderr, "Missing .symtab or .strtab sections\n");
    exit(1);
  }

  char *strtab = malloc(strtab_hdr.sh_size);
  if (!strtab) {
    perror("malloc failed");
    exit(1);
  }
  fseek(fp, strtab_hdr.sh_offset, SEEK_SET);
  if (fread(strtab, 1, strtab_hdr.sh_size, fp) != strtab_hdr.sh_size) {
    perror("Failed to read string table");
    exit(1);
  }

  int num_syms = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
  Elf32_Sym *syms = malloc(symtab_hdr.sh_size);
  if (!syms) {
    perror("malloc failed");
    exit(1);
  }
  fseek(fp, symtab_hdr.sh_offset, SEEK_SET);
  if (fread(syms, symtab_hdr.sh_entsize, num_syms, fp) != num_syms) {
    perror("Failed to read symbol table");
    exit(1);
  }

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

static const char *find_func_name_by_addr(uint32_t addr, uint32_t *func_addr_out) {
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

void ftrace_exec(uint32_t pc, uint32_t target, uint32_t inst) {
    uint32_t opcode = inst & 0x7f;
  
    int indent = call_depth;
    if (indent > 20) indent = 20; // max indent limit
  
    // Function call: jal or jalr
    if (opcode == 0x6f || opcode == 0x67) {
      const char *func_name = find_func_name_by_addr(target, NULL);
      if (func_name) {
        printf("[depth=%d] ", call_depth);
        for (int i = 0; i < indent; i++) printf("  ");
        printf("Call %s@0x%08x\n", func_name, target);
        call_depth++;
      }
    }
  
    // Function return: jalr x0, ra, 0
    if (opcode == 0x67) {
      int rd  = (inst >> 7) & 0x1f;
      int rs1 = (inst >> 15) & 0x1f;
      int imm = (int32_t)inst >> 20;
  
      if (rd == 0 && rs1 == 1 && imm == 0) { // ret
        call_depth--;
        if (call_depth < 0) call_depth = 0;
        uint32_t func_addr = 0;
        const char *func_name = find_func_name_by_addr(pc, &func_addr);
        if (func_name) {
          indent = call_depth;
          if (indent > 20) indent = 20;
          printf("[depth=%d] ", call_depth);
          for (int i = 0; i < indent; i++) printf("  ");
          printf("Return %s@0x%08x\n", func_name, func_addr);
        }
      }
    }
  }
