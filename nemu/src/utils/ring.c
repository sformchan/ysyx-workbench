#include <common.h>


#define RINGBUF_SIZE 16

typedef struct {
    char buffer[128][RINGBUF_SIZE];
    int index;
} RingBuffer;

static RingBuffer ringbuf;


void init_ringbuf()
{
    ringbuf.index = 0;
    for (int i = 0; i < RINGBUF_SIZE; i++) memset(ringbuf.buffer[i], 0, 128);
}


void ringbuf_push(const char *log)
{
    snprintf(ringbuf.buffer[ringbuf.index], 128, "%s", log);
    ringbuf.index = (ringbuf.index + 1) % RINGBUF_SIZE;
}

void ringbuf_print()
{
    printf(ANSI_FG_YELLOW"IRINGBUFFER:\n"ANSI_NONE);
    for(int i = 0; i < RINGBUF_SIZE; i++) printf("%s\n", ringbuf.buffer[i]);
}