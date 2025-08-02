#include "memory.h"
#include "state.h"
#include <string.h>
#include <getopt.h>

#define ysyx_25020047_RESET_VECTOR 0x80000000

char *img_file = NULL;
 long load_img() {
	unsigned int offset = ysyx_25020047_RESET_VECTOR - ysyx_25020047_INITADDR;
	if (img_file == NULL) {
	  printf(ANSI_FG_BLUE "No image is given. Use the built-in image.\n" ANSI_NONE);
	  memcpy(&rom[offset], img, 24);
	  return 24; 
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
int parse_args(int argc, char *argv[]) {
	const struct option table[] = {
	  //{"elf"      , required_argument, NULL, 'e'},  // csf added
	 // {"batch"    , no_argument      , NULL, 'b'},
	  {"log"      , required_argument, NULL, 'l'},
	 // {"diff"     , required_argument, NULL, 'd'},
	 // {"port"     , required_argument, NULL, 'p'},
	  {"help"     , no_argument      , NULL, 'h'},
	  {0          , 0                , NULL,  0 },
	};
	int o;
	while ( (o = getopt_long(argc, argv, "-l:", table, NULL)) != -1) {
	  switch (o) {
		//case 'e': elf_file = optarg; break; // csf added
		//case 'b': sdb_set_batch_mode(); break;
		//case 'p': sscanf(optarg, "%d", &difftest_port); break;
		case 'l': log_file = optarg; break;
		//case 'd': diff_so_file = optarg; break;
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


// int parse_args(int argc, char *argv[]) 
// {
// 	// if (argc < 2) {
// 	//   printf("Usage: %s IMAGE\n", argv[1]);
// 	//   exit(1);
// 	// }
// 	img_file = argv[1];
// 	return 0;
// }