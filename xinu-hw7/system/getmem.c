/**
 * @file getmem.c
 * @provides getmem                                                       
 *
 * COSC 3250 / COEN 4820 malloc project.
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

/**
 * Authors: Nathan Arpin and April Song
 */

/*TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <xinu.h>

/**
 * Allocate heap storage, returning pointer to assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, SYSERR on failure
 */
void *getmem(uint nbytes)
{
    // TODO: Search free list for first chunk large enough to fit.
    //       Break off region of requested size; return pointer
    //       to new memory region, and add any remaining chunk
    //       back into the free list.
    
    //Rounds the number of bytes requested 
    //to the nearest mulitiple of the size 
    //of a memory block
    nbytes = (uint)(roundmb(nbytes));

    //Creates two memory blocks to find 
    //the memory block that has enough 
    //bytes for the request and the one 
    //just behind that
    memblk *current = &freelist;
    memblk *prev = &freelist;

    //Loops through the list of memory 
    //blocks until it either finds one 
    //that fits the size or it reaches 
    //the end and returns a SYSERR
    while((current = current->next) != NULL)
    {
        if(current->length >= nbytes)
        {
            //If the memory block it has is 
            //greater than the amount requested, 
            //this will cut it into two parts, 
            //one to return to the list, and one 
            //to give to the caller
            if(current->length - nbytes >= 8)
            {
                //Moves the address of the memory block 
                //to the end of where the memory is 
                //currently being given away
                prev->next = (memblk *)(((uint)prev->next) + nbytes);
                //Sets the next point of the memory 
                //block to what it was before
                prev->next->next = current->next;
                //Updates the length of the memory block to fit
                prev->next->length = current->length - nbytes;
                //Updates the total amount of free memory available
                freelist.length -= nbytes;
                return (void *)current;
            }
            //If the memory block is a perfect match
            else
            {
                //Skip over where this whole memory block was
                prev->next = current->next;
                //Making sure the caller function knows how 
                //much is being given to it
                nbytes = current->length;
                //Updates the total amount of free memory available
                freelist.length -= nbytes;
                return (void *)current;
            }
        }
        //If the current memory block is not a 
        //match, update the previous block and 
        //continue through the list
        prev = current;
    }
    //If nothing matches, throw a SYSERR
    return (void *)SYSERR;
}
