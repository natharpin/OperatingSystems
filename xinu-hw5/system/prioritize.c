/**
 * @file priority.c
 * @provides priority
 * 
 * COSC 3250 / COEN 4820 Assignment 5
 *
 * Made by Nathan Arpin and April Song
 * Contact: nathan.arpin@marquette.edu and april.song@marquette.edu
 */

/*TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu*/

#include <xinu.h>

pid_typ insert(pid_typ, qid_typ, ulong);

/**
 * Prioritizes processes in the ready queue
 * @param pid    id of process to be added
 * @param q      queue to add the process to
 * @param key    priority of process
 * @return       the queue the process was added to
 */
int prioritize(pid_typ pid, qid_typ q, ulong key)
{
    if(isbadqueue(q) || isbadpid(pid))
    {
	return SYSERR;
    }

    queuetab[pid].key = key;
    insert(pid, q, key);

    return q;
}

pid_typ insert(pid_typ pid, qid_typ q, ulong key)
{
    if(isbadqueue(q) || isbadpid(pid))
    {
	return SYSERR;
    }

    if(isempty(q))
    {
	return enqueue(pid, q);
    }

    pid_typ currentproc = queuehead(q);
    while((currentproc = queuetab[currentproc].next) != queuetail(q))
    {
	if (queuetab[currentproc].key < queuetab[pid].key)
	{
	    queuetab[queuetab[currentproc].prev].next = pid;
	    queuetab[pid].prev = queuetab[currentproc].prev;
	    queuetab[pid].next = currentproc;
	    queuetab[currentproc].prev = pid;
	    return pid;
	}
    }
    return enqueue(pid, q);
}
