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

void dosomething(int a, int b, int c, int d, int e, int f, int g, int h)
{
    kprintf("\r\nProcess arguments are: %d, %d, %d, %d, %d, %d, %d, %d", a, b, c, d, e, f, g, h);
}

void printFreeList(void)
{
    memblk *current = &freelist;
    int count = 0;
    while((current = current->next) != NULL)
    {
        kprintf("\r\nAddress of block %d: %d, Size of block %d: %d, Address of next block %d: %d", count, (uint)current, count, current->length, count, (uint)current->next);
        count++;
    }
}

void printAccInfo(void *block)
{
    memblk *accinfo = (memblk *)((uint)block - sizeof(memblk));
    kprintf("\r\nAddress of accounting block: %d, Address of next pointer: %d, Size of memory block: %d", (uint)accinfo, accinfo->next, accinfo->length);
}

void printProcInfo(pid_typ pid)
{
    pcb *ppcb = &proctab[pid];
    kprintf("\r\nProcess stack base: %d, process stack length: %d", (uint)ppcb->stkbase, ppcb->stklen);
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;
    
    kprintf("\r\n0)Test that printing the free list works correctly");
    kprintf("\r\n1)Test getting a small amount of memory");
    kprintf("\r\n2)Test getting a small amount of memory and then freeing it");
    kprintf("\r\n3)Tests simple compaction, getting two 8 byte blocks of memory, frees the first and then the second, compacting the free list back together upon the second free");
    kprintf("\r\n4)Tests malloc with a small amount of memory");
    kprintf("\r\n5)Tests free by getting a small amount of memory and then freeing it");
    kprintf("\r\n6)Tests that create.c correctly utilizes getmem and kill.c correctly uses freemem");
    kprintf("\r\n7)Attempts to allocate a large amount of small memory blocks until there is no space left");
    kprintf("\r\n8)Attempts to malloc everything in one fell swoop");
    kprintf("");

    c = kgetc();

    kprintf("\r\n===TEST BEGIN===");

    // TODO: Test your operating system!

    switch (c)
    {
        case '0':
        {
            printFreeList();
            break;
        }

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
            break;
        }
        case '6':
        {
            printFreeList();
            pid_typ pid = create((void *)dosomething , INITSTK, 5, "GETMEM_TEST", 8, 1, 2, 3, 4, 5, 6, 7, 8);
            printProcInfo(pid);
            printFreeList();
            kill(pid);
            printFreeList();
            break;
        }
        case '7':
        {
            while(1)
            {
                if(malloc(1024) == (void *)SYSERR)
                {
                    kprintf("There is no memory left");
                    break;
                }
            }
            printFreeList();
            break;
        }
        case '8':
        {
            printFreeList();
            void *placeholder = malloc((uint)(truncmb((freelist.next->length) - 8)));
            printAccInfo(placeholder);
            printFreeList();
            free(placeholder);
            printFreeList();
            break;
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
