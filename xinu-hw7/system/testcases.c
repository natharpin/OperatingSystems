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

void printFreeList(void)
{
    memblk *current = &freelist;
    int count = 0;
    while((current = current->next) != NULL)
    {
        kprintf("\r\nSize of block %d: %d, Address of next block %d: %d", count, current->length, count, (uint)current->next);
        count++;
    }
}

void printAccInfo(void *block)
{
    memblk *accinfo = (memblk *)((uint)block - sizeof(memblk));
    kprintf("\r\nAddress of accounting block: %d, Address of next pointer: %d, Size of memory block: %d", (uint)accinfo, accinfo->next, accinfo->length);
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    kprintf("===TEST BEGIN===");

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
        case '1':
        {
            printFreeList();
            getmem(15);
            printFreeList();
            break;
        }

        case '2':
        {
            printFreeList();
            void *placeholder = getmem(8);
            printFreeList();
            freemem(placeholder, 8);
            printFreeList();
            break;
        }

        case '3':
        {
            printFreeList();
            void *placeholder1 = getmem(8);
            printFreeList();
            void *placeholder2 = getmem(8);
            printFreeList();
            freemem(placeholder1, 8);
            printFreeList();
            freemem(placeholder2, 8);
            printFreeList();
            break;
        }

        case '4':
        {
            printFreeList();
            void *placeholder = malloc(4);
            printAccInfo(placeholder);
            printFreeList();
            break;
        }

        case '5':
        {
            printFreeList();
            void *placeholder = malloc(4);
            printAccInfo(placeholder);
            printFreeList();
            if(free(placeholder) == SYSERR)
                kprintf("You dun goofed");
            printFreeList();
        }

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
