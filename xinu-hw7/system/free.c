/**
 * @file free.c
 * @provides free                                                     
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

/**
 * Made by Nathan Arpin and April Song
 */
/*TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <xinu.h>

/**
 * Free a memory block, returning it to free list.
 * @param *pmem pointer to memory block
 * @return OK on success, SYSERR on failure
 */
syscall	free(void *pmem)
{
    memblk *block = (memblk *)(((uint)pmem) - sizeof(memblk));

    // TODO: Perform some sanity checks to see if pmem is feasible and
    //       could be from a malloc() request:
    //       1) is ptr within heap region of memory?
    //       2) is ptr + length within heap region of memory?
    //       3) does accounting block mnext field point to itself?
    //       4) is accounting block mlen field nonzero?
    //       Call freemem() to put back into free list.
    
    //If the address of the block is greater 
    //than the total amount of memory or less 
    //than the bottom of the heap size, throw a SYSERR
    if((uint)block < (ulong)memheap || (((uint)block) + block->length) > (ulong)platform.maxaddr)
        return SYSERR;

    //If the block's next pointer does not point to 
    //itself, the program did not create this memory 
    //block and should not free it out of hand
    if((uint)block != (uint)block->next)
        return SYSERR;
    
    //If the memory block's length is garbage 
    //or not possible, throw a SYSERR
    if(block->length <= 0)
        return SYSERR;
    //Call freemem on the block in a controlled environment
    mutexAcquire();
    syscall result = freemem((void *)block, block->length);
    mutexRelease();
    return result;
}
