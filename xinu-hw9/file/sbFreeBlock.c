/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Nathan Arpin                                                    */
/* and                                                             */
/* April Song                                                      */
/*                                                                 */

/* TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <kernel.h>
#include <device.h>
#include <memory.h>
#include <disk.h>
#include <file.h>

void emptyArray(int *array,  int max);
struct freeblock * createFreeblock(int block);
devcall writeFreeblock(int diskfd, struct freeblock *freeblk);
devcall writeSuperblock(int diskfd, struct superblock *psuper);

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.

    struct freeblock *current;
    struct dentry *phw;
    int diskfd;

    if(NULL == psuper || block < 0 || block > 255)
    {
        return SYSERR;
    }
    phw = psuper->sb_disk;
    if(NULL == phw)
    {
        return SYSERR;
    }
    diskfd = phw - devtab;

    current = psuper->sb_freelst;

    wait(psuper->sb_freelock);

    if(current == NULL)
    {
        current = (struct freeblock *)createFreeblock(block);
        psuper->sb_freelst = current;
        writeSuperblock(diskfd, psuper);
        writeFreeblock(diskfd, current);
        signal(psuper->sb_freelock);
        return OK;
    }

    while(current->fr_next != NULL)
    {
        current = current->fr_next;
    }

    if((current == psuper->sb_freelst) || (current->fr_count == FREEBLOCKMAX))
    {
        struct freeblock *newblk = (struct freeblock *)createFreeblock(block);
        current->fr_next = newblk;
        writeFreeblock(diskfd, current);
        writeFreeblock(diskfd, newblk);
    } 
    else if(current->fr_count < FREEBLOCKMAX)
    {
        current->fr_free[current->fr_count] = block;
        current->fr_count++;
        writeFreeblock(diskfd, current);
    }

    signal(psuper->sb_freelock);

    return OK;
}

void emptyArray(int *array, int max)
{
    int i;
    for(i = 0; i < max; i++)
    {
        array[i] = 0;
    }
}

struct freeblock* createFreeblock(int block)
{ 
    struct freeblock *collector = (struct freeblock *)malloc(sizeof(struct freeblock));
    collector->fr_blocknum = block;
    collector->fr_count = 0;
    collector->fr_next = NULL;
    emptyArray(collector->fr_free, FREEBLOCKMAX);
    return collector;
}

devcall writeFreeblock(int diskfd, struct freeblock *freeblk)
{
    struct freeblock *free2 = freeblk->fr_next;
    if(NULL == freeblk->fr_next)
    {
        freeblk->fr_next = 0;
    }
    else
    {
        freeblk->fr_next =
            (struct freeblock *)freeblk->fr_next->fr_blocknum;
    }
    seek(diskfd, freeblk->fr_blocknum);
    if(SYSERR == write(diskfd, freeblk, sizeof(struct freeblock)))
    {
        return SYSERR;
    }
    freeblk->fr_next = free2;
    return OK;
}

devcall writeSuperblock(int diskfd, struct superblock *psuper)
{
    struct dirblock *swizzle = psuper->sb_dirlst;
    psuper->sb_dirlst = (struct dirblock *)swizzle->db_blocknum;
    seek(diskfd, psuper->sb_blocknum);
    if(SYSERR ==
        write(diskfd, psuper, sizeof(struct superblock)))
    {
        return SYSERR;
    }
    psuper->sb_dirlst = swizzle;
    return OK;
}
