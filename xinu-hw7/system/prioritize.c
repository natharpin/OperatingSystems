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

    //Sets the key of the process to the key that was passed in to the method
    queuetab[pid].key = key;
    insert(pid, q, key);

    return q;
}

//Inserts a process into the ready list directly before the next lowest priority process
pid_typ insert(pid_typ pid, qid_typ q, ulong key)
{
    //If either the queue or process is incorrect, throw a system error
    if(isbadqueue(q) || isbadpid(pid))
    {
	return SYSERR;
    }

    //If the queue is empty just enqueue the process normally
    if(isempty(q))
    {
	return enqueue(pid, q);
    }

    //Iterates through the queue to find the first place that the process is higher priority than the comparison and places the process there
    pid_typ currentproc = queuehead(q);
    while((currentproc = queuetab[currentproc].next) != queuetail(q))
    {
	if (queuetab[currentproc].key < queuetab[pid].key)
	{
            //The four necessary assignments to correctly insert an item in a double linked list
            queuetab[queuetab[currentproc].prev].next = pid;
	    queuetab[pid].prev = queuetab[currentproc].prev;
	    queuetab[pid].next = currentproc;
	    queuetab[currentproc].prev = pid;
	    return pid;
	}
    }
    //If the process has the lowest priority out of any current process, enqueue it at the very back
    return enqueue(pid, q);
}
