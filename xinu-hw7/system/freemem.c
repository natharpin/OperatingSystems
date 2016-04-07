/**
 * @file freemem.c
 * @provides freemem                                                     
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

/**
 * Made by Nathan Arpin and April Song
 */
/*TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu*/

#include <xinu.h>

//A quick helper method to merge two memory blocks together
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
    
    //Creates two memory blocks to help 
    //find where in the memory space this 
    //block is supposed to go
    memblk *current = freelist.next;
    memblk *prev = &freelist;

    //While the current block's address is 
    //less than the block that is being searched 
    //for and the current is not null, keep 
    //looping through the list
    while((uint)pmem > (uint)current && (uint)current != NULL)
    {
        prev = current;
        current = current->next;
    }
    
    //Sanity checks
    //If the previous block is the freelist, ignore that
    //If the address is within the space of the previous block, throw an error
    //If the end of the block is within the next block, throw an error
    //If the next pointer is null, ignore that
    if(((uint)prev != (uint)&freelist) && (((uint)pmem < (((uint)prev) + prev->length)) || ((uint)pmem) + nbytes > (uint)current) && (uint)current != NULL)
        return SYSERR;

    //Increase the total number of available free space
    freelist.length += nbytes;

    //Creates the memory block that 
    //will be put back into the list
    memblk *newblk = pmem;
    newblk->length = nbytes;
    
    //Sets places the block into the list
    prev->next = newblk;
    newblk->next = current;

    //If the end of the new block and the 
    //address of the next block are next 
    //to each other, merge them together
    if(((uint)newblk) + newblk->length == (uint)current)
        merge(newblk, current);

    //If the end of the previous block and 
    //the address of the new block are next 
    //to each other, merge them together
    if(((uint)prev) + prev->length == (uint)newblk)
        merge(prev, newblk);

    //If everything succeeded, return OK!
    return OK;
}
