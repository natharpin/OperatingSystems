/**
 * @file malloc.c
 * @provides malloc                                                       
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

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
    
    uint request = (uint)(roundmb(nbytes + sizeof(memblk)));
    
    mutexAcquire();
    void *block = getmem(request);
    mutexRelease();

    if(block == (void *)SYSERR)
    {
        return (void *)SYSERR;
    }

    memblk *accinfo = block;
    accinfo->length = request;
    accinfo->next = accinfo;

    block = (void *)(((uint)block) + sizeof(memblk));

    return block;  
}
