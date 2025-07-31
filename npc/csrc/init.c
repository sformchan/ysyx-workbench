#include <stdio.h>
#include "memory.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "utils.h"


//load bin file


#define ysyx_25020047_RESET_VECTOR 0x80000000

char *img_file = NULL;
 long load_img() {
	if (img_file == NULL) {
	  perror("No image is given.\n");
	  exit(1); 
	}
  
	FILE *fp = fopen(img_file, "rb");
	if (!fp) {
        perror("Error opening bin file");
        exit(1);
    }
  
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
  
	printf("The image is %s, size = %ld\n", img_file, size);
  
	fseek(fp, 0, SEEK_SET);
	unsigned int offset = ysyx_25020047_RESET_VECTOR - ysyx_25020047_INITADDR;
	int ret = fread(&rom[offset], size, 1, fp);
	assert(ret == 1);
  
	fclose(fp);
	return size;
  }


//  int parse_args(int argc, char *argv[]) {
// 	const struct option table[] = {
// 	  {0          , 0                , NULL,  0 },
// 	};
// 	int o;
// 	while ( (o = getopt_long(argc, argv, "-", table, NULL)) != -1) {
// 	  switch (o) {
// 		case 1: img_file = optarg; return 0;
// 		default:
// 		  printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
// 		  printf("\n");
// 		  exit(0);
// 	  }
// 	}
// 	return 0;
//   }


int parse_args(int argc, char *argv[]) {
	if (argc < 2) {
	  printf("Usage: %s IMAGE\n", argv[1]);
	  exit(1);
	}
	img_file = argv[1];
	return 0;
}

int init_monitor()
{

}