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

/* Nathan Arpin and April Song
 * Contact: nathan.arpin@marquette.edu, april.song@marquette.edu
 */
/*TA-BOT:MAILTO nathan.arpin@marquette.edu arpil.song@marquette.edu */

#include <xinu.h>

extern void main(int, char *);

int testmain(int argc, char **argv)
{
    int i = 0;
    kprintf("Hello XINU World!\r\n");

    for (i = 0; i < 10; i++)
    {
        kprintf("This is process %d\r\n", currpid);

        /* Uncomment the resched() line for cooperative scheduling. */
        resched();
    }
    return 0;
}

void testbigargs(int a, int b, int c, int d, int e, int f, int g, int h)
{
    //Prints all arguments of a process with 8 arguments
    kprintf("Testing bigargs...\r\n");
    kprintf("a = 0x%08X\r\n", a);
    kprintf("b = 0x%08X\r\n", b);
    kprintf("c = 0x%08X\r\n", c);
    kprintf("d = 0x%08X\r\n", d);
    kprintf("e = 0x%08X\r\n", e);
    kprintf("f = 0x%08X\r\n", f);
    kprintf("g = 0x%08X\r\n", g);
    kprintf("h = 0x%08X\r\n", h);
}

void testquick(void)
{
    //Prints the number of the process when it is created or readied and prints the number again before it finishes, reschudling in between to run other processes
    kprintf("This is process %d\r\n", currpid);
    resched();
    kprintf("Process %d done!\r\n", currpid);
}

void testfour(int a, int b, int c, int d)
{
	//Prints four arguments of a process
	kprintf("Testing four args.\r\n");
	kprintf("a = 0x%08X\r\n", a);
	kprintf("b = 0x%08X\r\n", b);
	kprintf("c = 0x%08X\r\n", c);
	kprintf("d = 0x%08X\r\n", d);
}

void testtwo(int a, int b)
{
	//Prints two arguments of a process
	kprintf("Testing two args.\r\n");
	kprintf("a = 0x%08X\r\n", a);
	kprintf("b = 0x%08X\r\n", b);
}

void printpcb(int pid)
{
    pcb *ppcb = NULL;

    /* Using the process ID, access it in the PCB table. */
    ppcb = &proctab[pid];

    /* Printing PCB */
    kprintf("Process name		  : %s \r\n", ppcb->name);

    switch (ppcb->state)
    {
    case PRFREE:
        kprintf("State of the process	  : FREE \r\n");
        break;
    case PRCURR:
        kprintf("State of the process 	  : CURRENT \r\n");
        break;
    case PRSUSP:
        kprintf("State of the process	  : SUSPENDED \r\n");
        break;
    case PRREADY:
        kprintf("State of the process	  : READY \r\n");
        break;
    default:
        kprintf("ERROR: Process state not correctly set!\r\n");
        break;
    }

    /* Print PCB contents and registers */
    kprintf("Base of run time stack    : 0x%08X \r\n", ppcb->stkbase);
    kprintf("Stack pointer of process  : 0x%08X \r\n",
            ppcb->regs[PREG_SP]);
    kprintf("Stack length of process   : %8u \r\n", ppcb->stklen);
}

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c = 0;
    int i, pid;

    while(c != 4){
    	kprintf("0) Test creation of one process\r\n");
    	kprintf("1) Test passing of many args\r\n");
    	kprintf("2) Create many processes and run them\r\n");
    	kprintf("3) Create one process with 0 arguments\r\n");
    	kprintf("4) Create a process with 4 arguments\r\n");
    	kprintf("5) Create a process with 2 arguments\r\n");
	kprintf("6) Create 3 processes and let them run\r\n");

	c = kgetc();

    	kprintf("===TEST BEGIN===\r\n");

    	// TODO: Test your operating system!

    	switch (c)
    	{
    	case '0':
		//Creates a suspended process and prints the process control block information of that process
        	pid = create((void *)testmain, INITSTK, "MAIN1", 2, 0, NULL);
        	printpcb(pid);
        	break;

    	case '1':
		//Print out the stack info and extra arguments of a process
        	pid = create((void *)testbigargs, INITSTK, "MAIN1", 8,
                     	0x11111111, 0x22222222, 0x33333333, 0x44444444,
                     	0x55555555, 0x66666666, 0x77777777, 0x88888888);
        	printpcb(pid);
        	// TODO: print out stack with extra args
        	pcb *ppcb = NULL;
		ppcb = &proctab[pid];
		ulong *saddr = (ulong *)ppcb -> regs[PREG_SP];

		while(*saddr != STACKMAGIC){
		    kprintf("Address: 0x%08x Contains: 0x%08x\r\n", saddr, *saddr);
		    *saddr++;
		}
        	// TODO: ready(pid, 0);
		ready(pid, 0);
        	break;

    	case '2':
		//Creates 50 processes, expecting that the last two fail to create
        	for(i = 0; i < 51; i++){
	    	    pid = create((void *)testquick, INITSTK, "quickTest", 2, 0, NULL);
	    	    if(pid != SYSERR) { ready(pid, 0); }
	    	    else kprintf("Create process %d failed\r\n", i);
        	}
		break;
    	case '3':
		//Creates a process with zero arugments
		pid = create((void *)testmain, INITSTK, "lalala", 0);
		printpcb(pid);
		ready(pid, 0);
		break;
    	case '4':
		//Creates a process with four arguments
		pid = create((void *)testfour, INITSTK, "testfour", 4, 0x01234567, 0x09876543, 0x00000000, 0x99999999);
		printpcb(pid);
		ready(pid, 0);
		break;
    	case '5':
		//Creates a process with two arguments
		pid = create((void *)testtwo, INITSTK, "testint", 2, 1, 10);
		printpcb(pid);
		ready(pid, 0);
		break;
    	default:
        	// Create three copies of a process, and let them play.
        	ready(create((void *)main, INITSTK, "MAIN1", 2, 0, NULL), 0);
        	ready(create((void *)main, INITSTK, "MAIN2", 2, 0, NULL), 0);
        	ready(create((void *)main, INITSTK, "MAIN3", 2, 0, NULL), 0);
       		while (numproc > 1)
        	    resched();
   	     break;
   	 }

	//While there are still other processes running, reschedule to them to clear the way for the next test	
	int limit = 0;
	while(numproc > 1 && limit < 1000) {
	    resched();
	    limit++;
	}
 
   	kprintf("\r\n===TEST END===\r\n");     
    }
    return;
}
