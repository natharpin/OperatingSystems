/**
 * @file malloc.c
 * @provides malloc                                                       
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

/**
 * Made by Nathan Arpin and April Song
 */
/*TA-BOT: MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <xinu.h>

/**
 * Allocate heap storage plus accounting block, returning pointer to
 * assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *malloc(uint nbytes)
{
    // TODO: Pad request size with room for accounting info.
    //       Call getmem() to allocate region.
    //       Store accounting block at head of region, including size
    //         of request.  Return pointer to space above accounting
    //         block.

    //Rounds the request size up 
    //to the nearest multiple of 
    //the size of a memory block
    uint request = (uint)(roundmb(nbytes + sizeof(memblk)));
    
    //Calls getmem in a controlled environment
    mutexAcquire();
    void *block = getmem(request);
    mutexRelease();

    //Error checking
    if(block == (void *)SYSERR)
    {
        return (void *)SYSERR;
    }

    memblk *accinfo = block; //Creates the accounting 
                             //information block at the 
                             //beginning of the memory space
    accinfo->length = request; //Sets the length variable 
                               //in the memory block to 
                               //the size requested from getmem
    accinfo->next = accinfo; //Sets the next pointer to its 
                             //own address in memory to allow 
                             //free to check if it is safe to free

    block = (void *)(((uint)block) + sizeof(memblk)); //Makes block point 
                                                      //just ahead of the 
                                                      //accounting information

    return block;  
}
