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

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
        //If aging and preemption are off, each process will run to completion in strictly the order of their priority
        //If aging and preemption are on, the processes will run in a slightly different order, most likely bigargs-4-4-4-3-5-4-2-4-3-5 or something similar
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
	//This prints the queue in order of its current priority, this gets messed up if preemption is on so that got turned off for the time being
        disable();
        ready(create((void *)donothing, INITSTK, 1, "PRIORITY-A", 0), 0);
	ready(create((void *)donothing, INITSTK, 3, "PRIORITY-B", 0), 0);
	ready(create((void *)donothing, INITSTK, 3, "PRIORITY-C", 0), 0);
	ready(create((void *)donothing, INITSTK, 4, "PRIORITY-D", 0), 0);
	ready(create((void *)donothing, INITSTK, 5, "PRIORITY-E", 0), 0);
	printqueuetab();
	break;

    case 'A':
	//This runs one infinite loop that will only stop when the second process runs, the second process will only ever run if aging is on
        ready(create((void *)aginginf, INITSTK, 35, "INFINITE-LOOP", 0), 0);
	ready(create((void *)agingproof, INITSTK, 5, "AGING-PROOF", 0), 0);
	break;

    case 'P':
	//If preempt is off, each process will count to ten then pass control to the next process
        //If preempt is on, each process will attempt to count to ten but will probably be interrupted mid print for another process to start counting
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
