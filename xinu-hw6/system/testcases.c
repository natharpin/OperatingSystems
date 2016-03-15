/**
 * @file testcases.c
 * @provides testcases
 *
 * $Id: testcases.c 175 2008-01-30 01:18:27Z brylow $
 *
 * Modified by:
 * Nathan Arpin
 * and
 * April Song
 */
/* Embedded XINU, Copyright (C) 2010, 2014.  All rights reserved. */

/*TA-BOT: MAILTO nathan.arpin@marquette.edu april.song@marquette.edu*/

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
    if(wait(bb->empty) == SYSERR || wait(bb->mutex) == SYSERR)
        return SYSERR;

    bb->buffer[bb->buffertail] = item;
    bb->buffertail = (bb->buffertail + 1) % BUFFER_SIZE;

    if(signal(bb->mutex) == SYSERR || signal(bb->full) == SYSERR)
        return SYSERR;
    return 0;
}

int remove_item(struct boundedbuffer *bb, buffer_item * item)
{
    // TODO:
    /* remove an object from buffer
     * placing it in item
     * return 0 if successful, otherwise
     * return SYSERR indicating an error condition */
    if(wait(bb->full) == SYSERR || wait(bb->mutex) == SYSERR)
        return SYSERR;

    *item = bb->buffer[bb->bufferhead];
    bb->buffer[bb->bufferhead] = 0;
    bb->bufferhead = (bb->bufferhead + 1) % BUFFER_SIZE;

    if(signal(bb->mutex) == SYSERR || signal(bb->empty) == SYSERR)
        return SYSERR;
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
        {
            mutexAcquire();
            kprintf("\r\nreport error condition");
            mutexRelease();
        }
        else
        {
            mutexAcquire();
            kprintf("\r\nproducer %d produced %d", currpid, item);
            mutexRelease();
        }
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
        {
            mutexAcquire();
            kprintf("\r\nreport error condition");
            mutexRelease();
        }
        else
        {
            mutexAcquire();
            kprintf("\r\nconsumer %d consumed %d", currpid, item);
            mutexRelease();
        }
    }
}

/* END Textbook code from Ch 5 Programming Project 3, Silberschatz p. 254 */

void deadlock(semaphore *first, semaphore *second)
{
    enable();
    kprintf("\r\nProcess %d attempting to acquire semaphore %d", currpid, *first);
    wait(*first);
    kprintf("\r\nProcess %d acquired semaphore %d and attempting to acquire semaphore %d", currpid, *first, *second);
    resched();
    wait(*second);
    kprintf("This line should never be run");
}

void mutexTest(void)
{
    enable();
    kprintf("\r\nProcess %d is attempting to acquire the mutex", currpid);
    mutexAcquire();
    int i;
    for(i = 0; i < 10; i++)
        kprintf("\r\nProcess %d has acquired the mutex", currpid);
    mutexRelease();
    kprintf("\r\nProcess %d has realeased the mutex", currpid);
}

void printSemaphoreQueue(qid_typ q)
{
    pid_typ current = queuehead(q);
    int i = 0;
    while((current = queuetab[current].next) != queuetail(q))
    {
        kprintf("\r\nProcess %d is at index %d of  queue %d", current, i, q);
        i++;
    }
}

void printPCB(pid_typ pid)
{
    pcb *ppcb = &proctab[pid];
    kprintf("\r\n");
    kprintf(ppcb->name);
    kprintf(" is in state ");
    switch(ppcb->state)
    {
        case 0:
            kprintf("PRFREE");
            break;
        
        case 1:
            kprintf("PRCURR");
            break;

        case 2:
            kprintf("PRSUSP");
            break;

        case 3:
            kprintf("PRREADY");
            break;

        case 4:
            kprintf("PRWAIT");
            break;

        default:
            break;
    }
}

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

void initbadbbuff(struct boundedbuffer *bbuff)
{
    bbuff->empty = -1;
    bbuff->full = -1;
    bbuff->mutex = -1;
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    struct boundedbuffer bbuff;
    semaphore first = semcreate(1);
    semaphore second = semcreate(1);

    kprintf("0) Test 1 producer, 1 consumer, same priority\r\n");
    kprintf("1) Test 1 producer, 1 consumer, producer with a lower priority\r\n");
    kprintf("2) Test 1 producer, 1 consumer, consumer with a lower priority\r\n");
    kprintf("3) Test 2 producers, 1 consumer, same priority\r\n");
    kprintf("4) Test 1 producer, 2 consumers, same priority\r\n");
    kprintf("5) Test mutexes, no process should be able to aquire the mutex while another process has it\r\n");
    kprintf("6) Test deadlock\r\n");
    kprintf("7) Test insert_item semaphores\r\n");
    kprintf("8) Test remove_item semaphores\r\n");
    kprintf("9) Test producer and consumer with bad semaphores\r\n");

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

    case '1':
        initbbuff(&bbuff);
        ready(create((void *)producer, INITSTK, 1, "PRODUCER_LOWPRIOR", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 5, "CONSUMER_HIGHPRIOR", 1, &bbuff), 0);
        break;

    case '2':
        initbbuff(&bbuff);
        ready(create((void *)producer, INITSTK, 5, "PRODUCER_HIGHPRIOR", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 1, "CONSUMER_LOWPRIOR", 1, &bbuff), 0);
        break;
    
    case '3':
        initbbuff(&bbuff);
        ready(create((void *)producer, INITSTK, 5, "PRODUCER_A", 1, &bbuff), 0);
        ready(create((void *)producer, INITSTK, 5, "PRODUCER_B", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 5, "CONSUMER_A", 1, &bbuff), 0);
        break;

    case '4':
        initbbuff(&bbuff);
        ready(create((void *)producer, INITSTK, 5, "PRODUCER_A", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 5, "CONSUMER_A", 1, &bbuff), 0);
        ready(create((void *)consumer, INITSTK, 5, "CONSUMER_B", 1, &bbuff), 0);
        break;

    case '5':
        ready(create((void *)mutexTest, INITSTK, 1, "MUTEX_TEST_A", 0), 0);
        ready(create((void *)mutexTest, INITSTK, 2, "MUTEX_TEST_B", 0), 0);
        ready(create((void *)mutexTest, INITSTK, 3, "MUTEX_TEST_C", 0), 0);
        ready(create((void *)mutexTest, INITSTK, 5, "MUTEX_TEST_D", 0), 0);
        ready(create((void *)mutexTest, INITSTK, 5, "MUTEX_TEST_E", 0), 0);
        break;
    
    case '6':
    {
        pid_typ firstproc = create((void *)deadlock, INITSTK, 5, "DEADLOCK_A", 2, &first, &second);
        pid_typ secondproc = create((void *)deadlock, INITSTK, 5, "DEADLOCK_B", 2, &second, &first);
        ready(firstproc, 0);
        ready(secondproc, 0);
        int i;
        for(i = 0; i < 20; i++)
        {
            resched();
        }
        printSemaphoreQueue(semtab[first].queue);
        printSemaphoreQueue(semtab[second].queue);
        kill(firstproc);
        kill(secondproc);
        kprintf("\r\nTerminated deadlocked processes");
        break;
    }

    case '7':
    {
        initbbuff(&bbuff);
        pid_typ proc1 = create((void *)producer, INITSTK, 5, "PRODUCER_A", 1, &bbuff);
        pid_typ proc2 = create((void *)producer, INITSTK, 5, "PRODUCER_B", 1, &bbuff);
        pid_typ proc3 = create((void *)producer, INITSTK, 5, "PRODUCER_C", 1, &bbuff);
        ready(proc1, 0);
        ready(proc2, 0);
        ready(proc3, 0);
        int i;
        for(i = 0; i < 10; i++)
            resched();
        printSemaphoreQueue(semtab[bbuff.empty].queue);
        printPCB(proc1);
        printPCB(proc2);
        printPCB(proc3);
        kill(proc1);
        kill(proc2);
        kill(proc3);
        break;
    }

    case '8':
    {
        initbbuff(&bbuff);
        pid_typ proc1 = create((void *)consumer, INITSTK, 5, "CONSUMER_A", 1, &bbuff);
        pid_typ proc2 = create((void *)consumer, INITSTK, 5, "CONSUMER_B", 1, &bbuff);
        pid_typ proc3 = create((void *)consumer, INITSTK, 5, "CONSUMER_C", 1, &bbuff);
        ready(proc1, 0);
        ready(proc2, 0);
        ready(proc3, 0);
        int i;
        for(i = 0; i < 10; i++)
            resched();
        printSemaphoreQueue(semtab[bbuff.full].queue);
        printPCB(proc1);
        printPCB(proc2);
        printPCB(proc3);
        kill(proc1);
        kill(proc2);
        kill(proc3);
        break;
    }

    case '9':
        initbadbbuff(&bbuff);
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
