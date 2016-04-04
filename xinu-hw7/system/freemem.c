/**
 * @file freemem.c
 * @provides freemem                                                     
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

void merge(memblk *base, memblk *addon)
{
    base->next = addon->next;
    base->length += addon->length;
}

/**
 * Free a memory block, returning it to free list.
 * @param pmem pointer to memory block
 * @param nbytes size of memory block
 * @return OK on success, SYSERR on failure
 */
syscall	freemem(void *pmem, uint nbytes)
{
    // TODO: Insert back into free list, and compact with adjacent blocks.
    
    memblk *current = freelist.next;
    memblk *prev = &freelist;

    while((uint)pmem > (uint)current)
    {
        prev = current;
        current = current->next;
    }

    if(((uint)prev != (uint)&freelist) && (((uint)pmem < ((uint)prev + prev->length)) || (uint)pmem + nbytes > (uint)current))
        return SYSERR;

    memblk *newblk = pmem;
    newblk->length = nbytes;

    prev->next = newblk;
    newblk->next = current;

    if((uint)newblk + newblk->length == (uint)current)
        merge(newblk, current);

    if((uint)prev + prev->length == (uint)newblk)
        merge(prev, newblk);

    return OK;
}
