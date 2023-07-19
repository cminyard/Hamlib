#include <hamlib/rig.h>
#include <stdio.h>
#include "fifo.h"


void initFIFO(FIFO_RIG *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

void resetFIFO(FIFO_RIG *fifo)
{
    fifo->head = fifo->tail;
    fifo->flush = 1;
}

// returns RIG_OK if added
// return -RIG error if overflow
int push(FIFO_RIG *fifo, const char *msg)
{
    int len = strlen(msg);

    for (int i = 0; i < len; ++i)
    {
        fifo->data[fifo->tail] = msg[i];
        rig_debug(RIG_DEBUG_VERBOSE, "%s: push %c (%d,%d)\n", __func__, msg[i],
                  fifo->head, fifo->tail);

        if (fifo->tail + 1 == fifo->head) { return -RIG_EDOM; }

        fifo->tail = (fifo->tail + 1) % HAMLIB_FIFO_SIZE;
    }

    return RIG_OK;
}

int peek(FIFO_RIG *fifo)
{
    if (fifo->tail < 0 || fifo->head < 0) return -1;
    if (fifo->tail > 1023 || fifo->head > 1023) return -1;
    if (fifo->tail == fifo->head) { return -1; }
    char c = fifo->data[fifo->head];
    rig_debug(RIG_DEBUG_VERBOSE, "%s: peek %c (%d,%d)\n", __func__, c, fifo->head,
              fifo->tail);
    return c;
}

int pop(FIFO_RIG *fifo)
{
    if (fifo->tail == fifo->head) { return -1; }

    char c = fifo->data[fifo->head];
    rig_debug(RIG_DEBUG_VERBOSE, "%s: pop %c (%d,%d)\n", __func__, c, fifo->head,
              fifo->tail);
    fifo->head = (fifo->head + 1) % HAMLIB_FIFO_SIZE;
    return c;
}

#ifdef TEST
int main()
{
    FIFO_RIG fifo;
    initFIFO(&fifo);

    const char *str = "Hello, World!\n";

    // Pushing the string onto the FIFO
    push(&fifo, str);

    // Popping and printing one character at a time
    int c;

    while ((c = pop(&fifo)) != -1)
    {
        printf("%c", c);
    }

    return 0;
}
#endif
