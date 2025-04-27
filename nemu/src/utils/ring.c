#include <common.h>

#define RINGBUF_SIZE 16

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
    char *dest = strdup(log);
    ringbuf.buffer[ringbuf.index] = dest;
    if (ringbuf.count >= RINGBUF_SIZE) {
        free(ringbuf.buffer[ringbuf.index]);
        ringbuf.buffer[ringbuf.index] = NULL;
    }
    else if (ringbuf.count < RINGBUF_SIZE) {
        ringbuf.count++;
    }
    ringbuf.index = (ringbuf.index + 1) % RINGBUF_SIZE;
    ringbuf.count++;
}

void ringbuf_print()
{
    printf("=== " ANSI_FG_YELLOW"IRINGBUFFER"ANSI_NONE " ===\n");
    if(ringbuf.count > RINGBUF_SIZE) for(int i = 0; i < RINGBUF_SIZE; i++) printf("%s\n", ringbuf.buffer[i]);
    else for(int i = 0; i < ringbuf.count; i++) printf("%s\n", ringbuf.buffer[i]); 
}   