#include <common.h>

#define RINGBUF_SIZE 16

typedef struct {
    char *buffer[RINGBUF_SIZE];
    int index;
    int count;
    int crash;
} RingBuffer;

static RingBuffer ringbuf;


void init_ringbuf()
{
    ringbuf.index = 0;
    ringbuf.count = 0;
    ringbuf.crash = 0;
    for (int i = 0; i < RINGBUF_SIZE; i++) ringbuf.buffer[i] = NULL;
}


void ringbuf_push(char *log)
{
    if (ringbuf.count >= RINGBUF_SIZE) {
        free(ringbuf.buffer[ringbuf.index]); // 释放旧数据
        ringbuf.count--; // 防止 count 溢出
    }
    ringbuf.buffer[ringbuf.index] = log;
    ringbuf.crash = ringbuf.index;
    ringbuf.index = (ringbuf.index + 1) % RINGBUF_SIZE;
    ringbuf.count++;
}

void ringbuf_print()
{
    printf("=== " ANSI_FG_YELLOW"IRINGBUFFER"ANSI_NONE " ===\n");
    printf(ANSI_FG_CYAN"you should open Itrace to output the diassembly\n"ANSI_NONE);
    printf("\n");
    //if(ringbuf.count > RINGBUF_SIZE) for(int i = 0; i < RINGBUF_SIZE; i++) printf("%s\n", ringbuf.buffer[i]);
    for(int i = 0; i < ringbuf.count; i++)
    {
        if(i == ringbuf.crash) printf(ANSI_BG_RED"--> "ANSI_NONE);
        else printf("    ");
        printf("%s\n", ringbuf.buffer[i]); 
    }
}   