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
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

int agingtest = 1;

void aginginf(void)
{
    char message[] = {'I', 'n', 'f', 'i', 'n', 'i','t', 'e',
		      ' ', 'l', 'o', 'o', 'p', '\r', '\n'};
    int i;
    for(i = 0; agingtest;i++)
    {
	kprintf("%c", message[(i % 15)]);
	resched();
    }
    kprintf("The infinite loop is no longer infinite.");
}

void agingproof(void)
{
    kprintf("\r\nThe operating system uses aging\r\n");
    agingtest = 0;
    resched();
}

void preemptcounting(void)
{
    enable();
    int i;
    for(i = 1; i < 11; i++)
    {
	kprintf("Process %d count: %d\r\n", currpid, i);
    }
}

void printqueuetab(void)
{
    pid_typ printingproc = queuehead(readylist);
    while((printingproc = queuetab[printingproc].next) != queuetail(readylist))
    {
	kprintf("Process %d with effective key of %d\r\n", printingproc, queuetab[printingproc].key);
    }
}

void donothing(void)
{
   //Do nothing!
}

void bigargs(int a, int b, int c, int d, int e, int f)
{
    kprintf(" bigargs(%d, %d, %d, %d, %d, %d) == %d\r\n",
            a, b, c, d, e, f, a + b + c + d + e + f);
}

void printpid(int times)
{
    int i = 0;

    enable();
    for (i = 0; i < times; i++)
    {
        kprintf("This is process %d\r\n", currpid);
        resched();
    }
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;

    kprintf("===TEST BEGIN===\r\n");

    kprintf("0) Test priority scheduling\r\n");
    kprintf("1) Test the queue order using priority scheduling\r\n");
    kprintf("A) Test aging -- if TRUE it will loop forever, otherwise the second process will get the chance to run at a certain point and will stop the loop\r\n");
    kprintf("P) Test preemption -- if enabled, each loop will not complete in one go, otherwise each loop will finish and the next loop will run\r\n");
    kprintf("3) Test some spec, eventually\r\n");


    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
        ready(create((void *)printpid, INITSTK, 2, "PRINTER-A", 1, 5), 0);
        ready(create((void *)printpid, INITSTK, 5, "PRINTER-B", 1, 5), 0);
        ready(create((void *)printpid, INITSTK, 10, "PRINTER-C", 1, 5),
              0);
        ready(create((void *)printpid, INITSTK, 5, "PRINTER-D", 1, 5), 0);

        ready(create
              ((void *)bigargs, INITSTK, 20, "BIGARGS", 6, 10, 20, 30, 40,
               50, 60), 0);
        break;

    case '1':
	ready(create((void *)donothing, INITSTK, 1, "PRIORITY-A", 0), 0);
	ready(create((void *)donothing, INITSTK, 3, "PRIORITY-B", 0), 0);
	ready(create((void *)donothing, INITSTK, 3, "PRIORITY-C", 0), 0);
	ready(create((void *)donothing, INITSTK, 4, "PRIORITY-D", 0), 0);
	ready(create((void *)donothing, INITSTK, 5, "PRIORITY-E", 0), 0);
	printqueuetab();
	break;

    case 'A':
	ready(create((void *)aginginf, INITSTK, 35, "INFINITE-LOOP", 0), 0);
	ready(create((void *)agingproof, INITSTK, 5, "AGING-PROOF", 0), 0);
	break;

    case 'P':
	ready(create((void *)preemptcounting, INITSTK, 5, "COUNTER-A", 0), 0);
	ready(create((void *)preemptcounting, INITSTK, 5, "COUNTER-B", 0), 0);
	ready(create((void *)preemptcounting, INITSTK, 5, "COUNTER-C", 0), 0);
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
