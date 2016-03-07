#include <xinu.h>

pid_typ insert(pid_typ, qid_typ, ulong);

/**
 * Prioritizes processes in the ready queue
 * @param pid    id of process to be added
 * @param q      queue to add the process to
 * @param key    priority of process
 * @return       the queue the process was added to
 */
qid_typ prioritize(pid_typ pid, qid_typ q, ulong key)
{
    if(isbadqueue(q) || isbadpid(pid))
    {
	return SYSERR;
    }

    queuetab[pid].key = key;
    insert(pid, q, key);
    //enqueue(pid, q);
    //while(queuetab[queuetab[pid].prev].key < key && queuetab[queuetab[pid].prev].key != 0)
    //{
    //    prioritize(remove(pid), q, queuetab[pid].key);
    //}

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
	enqueue(pid, q);
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
    return SYSERR;
}
