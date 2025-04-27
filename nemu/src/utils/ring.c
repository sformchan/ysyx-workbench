#include <common.h>

#define RINGBUF_SIZE 3

typedef struct {
    char *buffer[RINGBUF_SIZE];
    int index;
    int count;
} RingBuffer;

static RingBuffer ringbuf;


void init_ringbuf()
{
    ringbuf.index = 0;
    ringbuf.count = 0;
    for (int i = 0; i < RINGBUF_SIZE; i++) ringbuf.buffer[i] = NULL;
}


void ringbuf_push(char *log)
{
    if (ringbuf.count >= RINGBUF_SIZE) {
        free(ringbuf.buffer[ringbuf.index]); // 释放旧数据
        ringbuf.count--; // 防止 count 溢出
    }
    ringbuf.buffer[ringbuf.index] = log;
    ringbuf.index = (ringbuf.index + 1) % RINGBUF_SIZE;
    ringbuf.count++;
}

void ringbuf_print()
{
    printf("=== " ANSI_FG_YELLOW"IRINGBUFFER"ANSI_NONE " ===\n");
    //if(ringbuf.count > RINGBUF_SIZE) for(int i = 0; i < RINGBUF_SIZE; i++) printf("%s\n", ringbuf.buffer[i]);
    for(int i = 0; i < ringbuf.count; i++) printf("%s\n", ringbuf.buffer[i]); 
}   