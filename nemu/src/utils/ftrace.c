#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>
#include <assert.h>

#define MAX_FUNCS 1024
#define MAX_CALL_DEPTH 100

typedef struct {
  uint32_t addr;
  uint32_t size;
  char *name;
} FuncInfo;

static FuncInfo func_table[MAX_FUNCS];
static int func_cnt = 0;

static int call_depth = 0;

// Load ELF symbols (functions) from ELF file
void read_elf_symbols(const char *elf_path) {
  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    perror("Cannot open ELF file");
    exit(1);
  }

  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    perror("Failed to read ELF header");
    fclose(fp);
    exit(1);
  }
  assert(*(uint32_t *)ehdr.e_ident == 0x464C457F && "Not a valid ELF");

  Elf32_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
  if (!shdrs) {
    perror("malloc failed");
    fclose(fp);
    exit(1);
  }
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  if (fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, fp) != ehdr.e_shnum) {
    perror("Failed to read section headers");
    free(shdrs);
    fclose(fp);
    exit(1);
  }

  Elf32_Shdr shstr = shdrs[ehdr.e_shstrndx];
  char *shstrtab = malloc(shstr.sh_size);
  if (!shstrtab) {
    perror("malloc failed");
    free(shdrs);
    fclose(fp);
    exit(1);
  }
  fseek(fp, shstr.sh_offset, SEEK_SET);
  if (fread(shstrtab, 1, shstr.sh_size, fp) != shstr.sh_size) {
    perror("Failed to read section header string table");
    free(shstrtab);
    free(shdrs);
    fclose(fp);
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
    free(shstrtab);
    free(shdrs);
    fclose(fp);
    exit(1);
  }

  char *strtab = malloc(strtab_hdr.sh_size);
  if (!strtab) {
    perror("malloc failed");
    free(shstrtab);
    free(shdrs);
    fclose(fp);
    exit(1);
  }
  fseek(fp, strtab_hdr.sh_offset, SEEK_SET);
  if (fread(strtab, 1, strtab_hdr.sh_size, fp) != strtab_hdr.sh_size) {
    perror("Failed to read string table");
    free(strtab);
    free(shstrtab);
    free(shdrs);
    fclose(fp);
    exit(1);
  }

  int num_syms = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
  Elf32_Sym *syms = malloc(symtab_hdr.sh_size);
  if (!syms) {
    perror("malloc failed");
    free(strtab);
    free(shstrtab);
    free(shdrs);
    fclose(fp);
    exit(1);
  }
  fseek(fp, symtab_hdr.sh_offset, SEEK_SET);
  if (fread(syms, symtab_hdr.sh_entsize, num_syms, fp) != num_syms) {
    perror("Failed to read symbol table");
    free(syms);
    free(strtab);
    free(shstrtab);
    free(shdrs);
    fclose(fp);
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

  free(syms);
  free(strtab);
  free(shstrtab);
  free(shdrs);
  fclose(fp);
}

const char *find_func(uint32_t addr, uint32_t *start_out) {
    for (int i = 0; i < func_cnt; i++) {
      uint32_t start = func_table[i].addr;
      uint32_t end = start + func_table[i].size;
      if (addr >= start && addr < end) {
        if (start_out) *start_out = start;
        return func_table[i].name;
      }
    }
    return "???";
  }
  
  void ftrace_exec(uint32_t pc, uint32_t target, uint32_t rd, uint32_t rs1, int32_t imm) {
    if(rd == 1)
    {
      call_depth++;
      for (int i = 0; i < call_depth; i++)
      {
        printf(" ");
      }
      printf("CALL  %s@0x%08x   [%d]\n", find_func(target, NULL), target, call_depth);
    }
    if(rd == 0 && rs1 == 1 && imm == 0)
    {
      call_depth--;
      for (int i = 0; i < call_depth; i++)
      {
        printf(" ");
      }
      printf("RET   %s@0x%08x   [%d]\n", find_func(target, NULL), target, call_depth);
    }
    //else printf("%u\n", rd);
//     // Extract instruction fields
//     int rd = (inst >> 7) & 0x1f;   // Destination register
//     int rs1 = (inst >> 15) & 0x1f; // Source register
//     int imm = (int32_t)(inst >> 20); // Sign-extend 12-bit immediate
// 1
//     // Check for JAL (opcode 0x6f)
//     if ((inst & 0x7f) == 0x6f) {
//         // JAL is typically a call, stores return address in rd (usually x1 or x5)
//         if (rd == 1 || rd == 5) { // Link registers
//             call_depth++;
//             printf("[depth=%d] Call %s@0x%08x\n", call_depth, find_func(target, NULL) ? find_func(target, NULL) : "unknown", target);
//         } else {
//             // Non-call JAL (e.g., jump)
//             printf("[depth=%d] Jump %s@0x%08x\n", call_depth, find_func(target, NULL) ? find_func(target, NULL) : "unknown", target);
//         }
//     }
//     // Check for JALR (opcode 0x67)
//     else if ((inst & 0x707f) == 0x67) {
//         // Detect return: jalr x0, rs1, imm (rs1 typically x1 or x5, imm usually 0)
//         if (rd == 0 && (rs1 == 1 || rs1 == 5) && imm == 0) {
//             call_depth--;
//             if (call_depth < 0) {
//                 call_depth = 0;
//                 fprintf(stderr, "Warning: Call depth underflow at PC=0x%08x\n", pc);
//             }
//             printf("[depth=%d] Return %s@0x%08x\n", call_depth, find_func(pc, NULL) ? find_func(pc, NULL) : "unknown", pc);
//         }
//         // Detect call: jalr rd, rs1, imm (rd typically x1 or x5)
//         else if (rd == 1 || rd == 5) {
//             call_depth++;
//             printf("[depth=%d] Call %s@0x%08x\n", call_depth, find_func(target, NULL) ? find_func(target, NULL) : "unknown", target);
//         }
//         // Non-call/return JALR (e.g., computed jump)
//         else {
//             printf("[depth=%d] Jump %s@0x%08x\n", call_depth, find_func(target, NULL) ? find_func(target, NULL) : "unknown", target);
//         }
//     }
//     // Ignore other instructions
}

  
  
  

