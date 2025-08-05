#include "memory.h"
#include "state.h"
#include <getopt.h>


void init_sdb();
void init_log(const char *log_file);
void init_ringbuf();
void init_disasm();
void read_elf_symbols(const char *elf_path);
void init_difftest(char *ref_so_file, long img_size, int port);

char *img_file = NULL;
 long load_img() {
	unsigned int offset = ysyx_25020047_RESET_VECTOR - ysyx_25020047_INITADDR;
	if (img_file == NULL) {
	  printf(ANSI_FG_BLUE "No image is given. Use the built-in image.\n" ANSI_NONE);
	  memcpy(&rom[offset], img, ysyx_25020047_IMG_SIZE);
	  return ysyx_25020047_IMG_SIZE; 
	}
  
	FILE *fp = fopen(img_file, "rb");
	if (!fp) {
        perror("Error opening bin file");
        exit(1);
    }
  
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
  
	printf(ANSI_FG_BLUE "The image is %s, size = %ld\n" ANSI_NONE, img_file, size);
	fseek(fp, 0, SEEK_SET);
	int ret = fread(&rom[offset], size, 1, fp);
	assert(ret == 1);
  
	fclose(fp);
	return size;
  }

#define required_argument 1
#define no_argument 0
char *log_file = NULL;
char *elf_file = NULL;
char *diff_so_file = NULL;
static int difftest_port = 1234;
bool is_batch_mode = false;

int parse_args(int argc, char *argv[]) {
	const struct option table[] = {
	  {"elf"      , required_argument, NULL, 'e'},  // csf added
	  {"batch"    , no_argument      , NULL, 'b'},
	  {"log"      , required_argument, NULL, 'l'},
	  {"diff"     , required_argument, NULL, 'd'},
	 // {"port"     , required_argument, NULL, 'p'},
	  {"help"     , no_argument      , NULL, 'h'},
	  {0          , 0                , NULL,  0 },
	};
	int o;
	while ( (o = getopt_long(argc, argv, "-l:e:d:b", table, NULL)) != -1) {
	  switch (o) {
		case 'e': elf_file = optarg; break;
		case 'b': is_batch_mode = true; break;
		//case 'p': sscanf(optarg, "%d", &difftest_port); break;
		case 'l': log_file = optarg; break;
		case 'd': diff_so_file = optarg; break;
		case 1: img_file = optarg; return 0;
		default:
		  printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
		  printf("\t-e,--elf=FILE           load the elf file\n");
		  printf("\t-b,--batch              run with batch mode\n");
		  printf("\t-l,--log=FILE           output log to FILE\n");
		  printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
		  printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
		  printf("\n");
		  exit(0);
	  }
	}
	return 0;
  }


void init_monitor()
{
	init_sdb();

	init_disasm();

	init_ringbuf();

	init_log(log_file);

	#ifdef CONFIG_FTRACE
  	if (elf_file != NULL) {
    printf(ANSI_FG_RED "[DEBUG] ELF file passed: %s\n" ANSI_NONE, elf_file);
    read_elf_symbols(elf_file);
  	}
	#endif


	#ifdef CONFIG_DIFFTEST
	init_difftest(diff_so_file, img_size, difftest_port);
	#endif
}