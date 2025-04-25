#include <common.h>


#define RINGBUF_SIZE 16

typedef struct {
    char buffer[128][RINGBUF_SIZE];
    int index;
    int count;
} RingBuffer;

static RingBuffer ringbuf;


void init_ringbuf()
{
    ringbuf.index = 0;
    for (int i = 0; i < RINGBUF_SIZE; i++) memset(ringbuf.buffer[i], 0, 128);
}


void ringbuf_push(char *log)
{
    snprintf(ringbuf.buffer[ringbuf.index], 128, "%s", log);
    ringbuf.index = (ringbuf.index + 1) % RINGBUF_SIZE;
    ringbuf.count++;
}

void ringbuf_print()
{
    printf("=== " ANSI_FG_YELLOW"IRINGBUFFER"ANSI_NONE " ===\n");
    if(ringbuf.count > RINGBUF_SIZE) for(int i = 0; i < RINGBUF_SIZE; i++) printf("%s\n", ringbuf.buffer[i]);
    else for(int i = 0; i < ringbuf.count; i++) printf("%s\n", ringbuf.buffer[2]); 
}   