/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 *
 * and
 *
 */
/* Embedded XINU, Copyright (C) 2010, 2014.  All rights reserved. */

#include <xinu.h>

ulong rand(void)
{
    ulong a = 1664525UL;
    ulong b = 1013904223UL;
    static ulong c = 0;

    c = a * c + b;
    return c;
}

syscall sleep(int time)
{
    /* Dumbest sleep ever. */
    int i = 0, j = 0;
    for (i = 0; i < time; i++)
        for (j = 0; j < 1000; j++)
            ;
    return 0;
}

/* BEGIN Textbook code from Ch 5 Programming Project 3, Silberschatz p. 254 */
typedef int buffer_item;
#define BUFFER_SIZE 5

struct boundedbuffer
{
    buffer_item buffer[BUFFER_SIZE];
    int bufferhead;
    int buffertail;

    semaphore empty;
    semaphore full;
    semaphore mutex;
};

int insert_item(struct boundedbuffer *bb, buffer_item item)
{
    // TODO:
    /* insert item into buffer
     * return 0 if successful, otherwise
     * return SYSERR indicating an error condition */
    wait(bb->empty);
    wait(bb->mutex);

    bb->buffer[bb->buffertail] = item;
    bb->buffertail = (bb->buffertail + 1) % BUFFER_SIZE;

    signal(bb->mutex);
    signal(bb->full);
    return 0;
}

int remove_item(struct boundedbuffer *bb, buffer_item * item)
{
    // TODO:
    /* remove an object from buffer
     * placing it in item
     * return 0 if successful, otherwise
     * return SYSERR indicating an error condition */
    wait(bb->full);
    wait(bb->mutex);

    *item = bb->buffer[bb->bufferhead];
    bb->buffer[bb->bufferhead] = 0;
    bb->bufferhead = (bb->bufferhead + 1) % BUFFER_SIZE;

    signal(bb->mutex);
    signal(bb->empty);
    return 0;
}

void producer(struct boundedbuffer *bb)
{
    buffer_item item;
    enable();
    while (1)
    {
        /* sleep for a random period of time */
        sleep(rand() % 100);
        /* generate a random number */
        item = rand();
        if (insert_item(bb, item))
            kprintf("report error condition\r\n");
        else
            kprintf("producer %d produced %d\r\n", currpid, item);
    }
}

void consumer(struct boundedbuffer *bb)
{
    buffer_item item;
    enable();
    while (1)
    {
        /* sleep for a random period of time */
        sleep(rand() % 100);
        if (remove_item(bb, &item))
            kprintf("report error condition\r\n");
        else
            kprintf("consumer %d consumed %d\r\n", currpid, item);
    }
}

/* END Textbook code from Ch 5 Programming Project 3, Silberschatz p. 254 */

void initbbuff(struct boundedbuffer *bbuff)
{
    int i;
    for(i = 0; i < BUFFER_SIZE; i++)
    {
       bbuff->buffer[i] = 0;
    }
    bbuff->bufferhead = 0;
    bbuff->buffertail = 0;
    bbuff->empty = semcreate(BUFFER_SIZE);
    bbuff->full = semcreate(0);
    bbuff->mutex = semcreate(1);
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    struct boundedbuffer bbuff;

    kprintf("0) Test 1 producer, 1 consumer, same priority\r\n");

    kprintf("===TEST BEGIN===");

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
        // TODO:
        // Initialize bbuff, and create producer and consumer processes
        initbbuff(&bbuff);
        ready(create((void *)producer, INITSTK, 5, "PRODUCER", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 5, "CONSUMER", 1, &bbuff), 0);
        break;

    default:
        break;
    }

    while (numproc > 2)
    {
        resched();
    }
    kprintf("\r\n===TEST END===\r\n");
    return;
}
