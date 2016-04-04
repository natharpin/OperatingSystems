/**
 * @file free.c
 * @provides free                                                     
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Free a memory block, returning it to free list.
 * @param *pmem pointer to memory block
 * @return OK on success, SYSERR on failure
 */
syscall	free(void *pmem)
{
    memblk *block = (memblk *)((uint)pmem - sizeof(memblk));

    // TODO: Perform some sanity checks to see if pmem is feasible and
    //       could be from a malloc() request:
    //       1) is ptr within heap region of memory?
    //       2) is ptr + length within heap region of memory?
    //       3) does accounting block mnext field point to itself?
    //       4) is accounting block mlen field nonzero?
    //       Call freemem() to put back into free list.
    
    if((uint)pmem < (ulong)memheap || ((uint)pmem + block->length) > (ulong)platform.maxaddr)
        return SYSERR;

    if((uint)block != (uint)block->next)
        return SYSERR;

    if(block->length == 0)
        return SYSERR;

    return freemem((void *)block, block->length);
}
