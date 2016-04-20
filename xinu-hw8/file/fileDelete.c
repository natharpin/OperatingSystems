/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Nathan Arpin                                                    */
/* and                                                             */
/* April Song                                                      */
/*                                                                 */

/* TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <kernel.h>
#include <memory.h>
#include <file.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    if((NULL == supertab) || (NULL == filetab) || (filetab[fd].fn_state == FILE_FREE))
    {
        return SYSERR;
    }

    wait(supertab->sb_dirlock);
    filetab[fd].fn_length = 0;
    filetab[fd].fn_cursor = 0;
    int i;
    for(i = 0; i < FNAMLEN; i++)
    {
        filetab[fd].fn_name[i] = 0;
    }
    filetab[fd].fn_name[FNAMLEN] = '\0';
    filetab[fd].fn_blocknum = 0;

    if((free((void *)filetab[fd].fn_data)) == SYSERR)
    {
        signal(supertab->sb_dirlock);
        return SYSERR;
    }

    signal(supertab->sb_dirlock);

    if(SYSERR == sbFreeBlock(supertab, fd))
    {
        return SYSERR;
    }

    wait(supertab->sb_dirlock);
    filetab[fd].fn_state = FILE_FREE;
    signal(supertab->sb_dirlock);

    return OK;
}
