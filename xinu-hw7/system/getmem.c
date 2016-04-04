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

/*TA-BOT: MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

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
        
        memblk *current = &freelist;
        memblk *prev = &freelist;

        while((current = current->next) != NULL)
        {
            if(current->length >= nbytes)
            {
                if(current->length - nbytes >= 8)
                {
                    prev->next = (memblk *)((uint)prev->next + nbytes);
                    prev->next->next = current->next;
                    prev->next->length = current->length - nbytes;
                    return (void *)current;
                }
                else
                {
                    prev->next = current->next;
                    return (void *)current;
                }
            }
            prev = current;
        }
	return (void *)SYSERR;
}
