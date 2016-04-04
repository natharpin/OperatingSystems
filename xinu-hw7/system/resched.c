/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
    irqmask im;
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */
    pid_typ head, tail;

    oldproc = &proctab[currpid];

    im = disable();

    /* TODO: Add queue aging to the system. */
    //Iterate through the ready list, incrementing each key by one
    if(AGING){
        head = queuehead(readylist);
        tail = queuetail(readylist);
        pid_typ agingproc = head;
        while((agingproc = queuetab[agingproc].next) != tail)
        {
	    queuetab[agingproc].key++;
	}
    }

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        //Insert the process into the list in the correct spot, using its original priority, meaning aging is reset for a process when that process is rescheduled
        prioritize(currpid, readylist, oldproc->priority);
    }

    /* remove first process in ready queue */
    currpid = dequeue(readylist);
    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt = QUANTUM;          /* reset preemption counter     */
#endif
    ctxsw(&oldproc->regs, &newproc->regs);

    /* The OLD process returns here when resumed. */
    restore(im);
    return OK;
}