#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//rom
#define ysyx_25020047_ROM_SIZE 1024 * 1024
#define ysyx_25020047_INITADDR 0x00000000



// uint8_t rom[ysyx_25020047_ROM_SIZE] = {
//     0x03, 0x21, 0x00, 0x00,
//     0x23, 0x00, 0x10, 0x00,
//     0x03, 0x21, 0x00, 0x00,
//     0x73, 0x00, 0x10, 0x00 //ebreak
//   };

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

uint8_t rom[ysyx_25020047_ROM_SIZE] = {
    0xb7, 0x00, 0x24, 0x1e,
    0x33, 0x81, 0x20, 0x00,
    0x73, 0x00, 0x10, 0x00 //ebreak
};


extern "C" int pmem_read(int raddr)
{
    raddr &= ~(0x3u);
    if(raddr + 3 > ysyx_25020047_ROM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", raddr);
        return 0xFFFFFFFF; // 返回错误码
    }
    return ((uint32_t)rom[raddr]) |
           ((uint32_t)rom[raddr + 1] << 8) |
           ((uint32_t)rom[raddr + 2] << 16) |
           ((uint32_t)rom[raddr + 3] << 24);
}

extern "C" void pmem_write(int waddr, int wdata, int wmask)
{
    int byte_offset = waddr & 0x3;
    waddr &= ~(0x3u);
    if(waddr + 3 > ysyx_25020047_ROM_SIZE)
    {
        printf("\033[31mError: address 0x%08x is out of RAM range.\033[0m\n", waddr);
        return;
    }
    //wmask & 0x1 means to check if the lowest bit of wmask is set, if not, DO NOT write.
    if (wmask & 0x1) rom[waddr + 0] = (wdata >> 0) & 0xFF /*, printf("byte0 written to 0x%08x: 0x%02x\n", waddr,rom[waddr + 0])*/; 
    if (wmask & 0x2) rom[waddr + 1] = (wdata >> 8) & 0xFF /*, printf("byte1 written to 0x%08x: 0x%02x\n", waddr + 1, rom[waddr + 1])*/; 
    if (wmask & 0x4) rom[waddr + 2] = (wdata >> 16) & 0xFF /*, printf("byte2 written to 0x%08x: 0x%02x\n", waddr + 2, rom[waddr + 2])*/; 
    if (wmask & 0x8) rom[waddr + 3] = (wdata >> 24) & 0xFF /*, printf("byte3 written to 0x%08x: 0x%02x\n", waddr + 3, rom[waddr + 3])*/; 
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
                if (byte_addr + 3 >= ysyx_25020047_ROM_SIZE) {
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
    printf("ROM loaded successfully.\n");
}
