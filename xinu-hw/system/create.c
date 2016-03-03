/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

/*Made by Nathan Arpin and April Song
 * Contact: nathan.arpin@marquette.edu, april.song@marquette.edu
 * This code may have been influenced by TA bot
 */
/*TA-BOT:MAILTO nathan.arpin@marquette.edu arpil.song@marquette.edu */

#include <xinu.h>

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */
    void INITRET(void);

    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
    /* round up to even boundary    */
    saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
    pid = newpid();
    /* a little error checking      */
    if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
    {
        return SYSERR;
    }

    numproc++;
    ppcb = &proctab[pid];
    /* setup PCB entry for new proc */
    ppcb->state = PRSUSP;

    // TODO: Setup PCB entry for new process.
    strncpy(ppcb->name, name, PNMLEN); //Copies the name to the process control block using a method from string.h
    ppcb->stkbase = (ulong *)((ulong)saddr - ssize); //Finds the location of the stack base by subtracting the stack size from the address of the stack
    ppcb->stklen = ((ulong)saddr) - (ulong)ppcb->stkbase; //Finds the length of the stack by subtracting the stack address by the base of the stack

    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;

    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / 4) * 4;
    }
    /* If more than 4 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }

    // TODO: Initialize process context.
    for (i = 0; i < PREGS; i++) { //Initializes the registers to 0
        ppcb->regs[i] = 0;
    }
    ppcb->regs[PREG_PC] = (int)funcaddr; //Sets the program counter to the address of the function that was passed in
    ppcb->regs[PREG_LR] = (int)INITRET; //Sets the link register
    ppcb->regs[PREG_SP] = (int)saddr; //Sets the stack pointer to the address of the stack

    // TODO:  Place arguments into activation record.
    //        See K&R 7.3 for example using va_start, va_arg and
    //        va_end macros for variable argument functions.
    va_start(ap, nargs);
    for (i = 0; i < nargs; i++) { //This is where the process deals with the arguments passed in.
        if (i < 4)
            ppcb->regs[i] = va_arg(ap, ulong); //Saves the first four arguments in the process control block
        else
            saddr[i - 4] = va_arg(ap, ulong); //Saves the remaining arguments on the stack
    }
    va_end(ap);

    return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    kill(currpid);
}
