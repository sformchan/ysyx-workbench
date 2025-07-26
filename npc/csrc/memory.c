#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>



//rom
#define ysyx_25020047_MEM_SIZE 0x7ffffff
#define ysyx_25020047_INITADDR 0x80000000


// uint8_t rom[ysyx_25020047_ROM_SIZE] = {
//     0xb7, 0x00, 0x24, 0x1e,
//     0x03, 0x21, 0x30, 0x00,
//     0x23, 0x00, 0x10, 0x00,
//     0x03, 0x21, 0x00, 0x00,
//     0x93, 0x80, 0x00, 0x7d,
//     0x33, 0x81, 0x00, 0x00,
//     0x73, 0x00, 0x10, 0x00 //ebreak
// };
// lui x1 123456
// lw x2 3(x0)
// sb x1 0(x0)
// lw x2 0(x0)
// addi x1 x1 2000
// add x2 x1 x0  

uint8_t rom[ysyx_25020047_MEM_SIZE];


extern "C" int pmem_read(int raddr)
{
    raddr &= ~(0x3u);
    uint32_t offset = raddr - ysyx_25020047_INITADDR;
    if(offset + 3 >= ysyx_25020047_MEM_SIZE)
    {
        printf("\033[31mError: read_address 0x%08x is out of MEM range.\033[0m\n", raddr);
        return 0xFFFFFFFF;
    }
    return ((uint32_t)rom[offset]) |
           ((uint32_t)rom[offset + 1] << 8) |
           ((uint32_t)rom[offset + 2] << 16) |
           ((uint32_t)rom[offset + 3] << 24);
}


extern "C" void pmem_write(int waddr, int wdata, int wmask)
{
    waddr &= ~(0x3u);
    uint32_t offset = waddr - ysyx_25020047_INITADDR;
    if(offset + 3 >= ysyx_25020047_MEM_SIZE)
    {
        printf("\033[31mError: write_address 0x%08x is out of MEM range.\033[0m\n", waddr);
        return;
    }
	//wmask & 0x1 means to check if the lowest bit of wmask is set, if not, DO NOT write.
    if (wmask & 0x1) rom[offset + 0] = (wdata >> 0) & 0xFF;
    if (wmask & 0x2) rom[offset + 1] = (wdata >> 8) & 0xFF;
    if (wmask & 0x4) rom[offset + 2] = (wdata >> 16) & 0xFF;
    if (wmask & 0x8) rom[offset + 3] = (wdata >> 24) & 0xFF;
}



//load hex file
void load_verilog_hex(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening hex file");
        exit(1);
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        // Skip empty lines
        if (strlen(line) < 2) continue;

        // Parse address prefix
        unsigned int word_addr;
        if (sscanf(line, "%x:", &word_addr) != 1) {
            continue;  // malformed line
        }

        // Find the colon and point to the hex data part
        char *p = strchr(line, ':');
        if (!p) continue;
        p++;  // move past the colon

        // Parse 32-bit words from the line
        while (*p) {
            unsigned int word;
            int chars_read;

            // Try to read one 32-bit hex word
            if (sscanf(p, "%x%n", &word, &chars_read) == 1) {
                // Calculate byte address
                unsigned int byte_addr = word_addr * 4;
                if (byte_addr + 3 >= ysyx_25020047_MEM_SIZE) {
                    fprintf(stderr, "Out of ROM bounds at addr 0x%x\n", byte_addr);
                    break;
                }

                // Write in little endian
                rom[byte_addr + 0] = (word >> 0) & 0xFF;
                rom[byte_addr + 1] = (word >> 8) & 0xFF;
                rom[byte_addr + 2] = (word >> 16) & 0xFF;
                rom[byte_addr + 3] = (word >> 24) & 0xFF;

                word_addr++;  // move to next word address
                p += chars_read;  // move forward in the line
            } else {
                break;  // no more words on this line
            }
        }
    }

    fclose(fp);
    printf("MEM loaded successfully.\n");
}


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