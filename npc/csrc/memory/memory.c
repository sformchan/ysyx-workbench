#include "memory.h"
#include <common.h>
#include <unistd.h>
#include "device.h"
#include "utils.h"
#include "state.h"
#include "difftest.h"


/////////MEM//////////
uint8_t rom[ysyx_25020047_MEM_SIZE];

uint8_t img[28] = {
    0xb7, 0x00, 0xf4, 0x01,
    0x93, 0x80, 0x00, 0x7d,
    0x33, 0x81, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
    0xb3, 0x06, 0xf7, 0x00,
    0x33, 0x81, 0x00, 0x00,
    0x73, 0x00, 0x10, 0x00 //ebreak
};





/////////MEM_VISIT/////////
extern "C" int pmem_read(int raddr, int flag)
{
	uint32_t high = 0;
	if(raddr == RTC_ADDR) 
	{
		difftest_skip_ref();
		uint64_t uptime = get_uptime_64bit();
		//printf("%lu\n", uptime);
		high = uptime >> 32;
		return (uint32_t)(uptime & 0xFFFFFFFF); 
	}
	if(raddr == RTC_ADDR + 4)
	{
		uint32_t uptime = high;
		return uptime;
	}
	//printf("raddr: %08x\n", raddr);
	int raddr1 = raddr & ~(0x3u);
	if(flag)
	{
		#ifdef CONFIG_MTRACE
		printf("[MTARCE] " ANSI_FG_CYAN "READ  " ANSI_NONE "addr=0x%08x"  "  len=4\n", raddr);
		#endif
	}
    uint32_t offset = raddr1 - ysyx_25020047_INITADDR;
    if(offset + 3 >= ysyx_25020047_MEM_SIZE)
    {
        printf("\033[31mError: read_address 0x%08x is out of MEM range.\033[0m\n", raddr1);
        return 0xFFFFFFFF;
    }
    return ((uint32_t)rom[offset]) |
           ((uint32_t)rom[offset + 1] << 8) |
           ((uint32_t)rom[offset + 2] << 16) |
           ((uint32_t)rom[offset + 3] << 24);
}



extern "C" void pmem_write(int waddr, int wdata, int wmask)
{

	
	if ((uint32_t)waddr == SERIAL_ADDR) {
		difftest_skip_ref();
		char ch = (char)(wdata & 0xFF);
		if (ch == '\n') fputc('\r', stderr);  // optional: 兼容终端换行
		fputc(ch, stderr);
		return;
	}
	uint32_t waddr1 = (uint32_t)waddr & ~(0x3u);
    //waddr &= ~(0x3u);
    uint32_t offset = waddr1 - ysyx_25020047_INITADDR;
    if(offset + 3 >= ysyx_25020047_MEM_SIZE)
    {
		printf("\n");
		//printf("\033[31mPC: 0x%08x\033[0m\n", pc);
		//printf("\033[31mINST: 0x%08x\033[0m\n", inst);
        printf("\033[31mError: write_address 0x%08x is out of MEM range.\033[0m\n", waddr1);
		printf("\033[31mError: origin address: 0x%08x.\033[0m\n", waddr);
		printf("\033[31mError: offset: 0x%08x\033[0m\n", offset);
		exit(1);
        return;
    }
	int len = 0;
	//wmask & 0x1 means to check if the lowest bit of wmask is set, if not, DO NOT write.
    if (wmask & 0x1) rom[offset + 0] = (wdata >> 0) & 0xFF, len++;
    if (wmask & 0x2) rom[offset + 1] = (wdata >> 8) & 0xFF, len++;
    if (wmask & 0x4) rom[offset + 2] = (wdata >> 16) & 0xFF, len++;
    if (wmask & 0x8) rom[offset + 3] = (wdata >> 24) & 0xFF, len++;
	#ifdef CONFIG_MTRACE
  	printf("[MTARCE] " ANSI_FG_MAGENTA "WRITE " ANSI_NONE "addr=0x%08x"  "  len=%d\n", waddr1, len);
  	#endif
}

uint8_t *guest_to_host(uint32_t paddr) {
	uint32_t offset = paddr - ysyx_25020047_INITADDR;
	if (offset >= ysyx_25020047_MEM_SIZE) {
	  printf("\033[31mError: guest_to_host() access out of range: 0x%08x\033[0m\n", paddr);
	  exit(1); // 或者 assert(0);
	}
	return &rom[offset];
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


