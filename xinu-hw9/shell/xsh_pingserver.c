/**
 * @file     xsh_pingserver.c
 * @provides xsh_pingserver, echoReply
 *
 */
/* Embedded XINU, Copyright (C) 2009, 2016.  All rights reserved. */

#include <xinu.h>

/**
 * Process replies to incoming echo requests.
 */
process echoReply(void)
{
    char packet[PKTSZ];
    struct ethergram *ether;
    struct ipv4gram *ipgram;
    struct icmpgram *icmp;
    struct arpgram *argram;
    int length;

    ether = (struct ethergram *)packet;

    enable();

    while (TRUE)
    {
        bzero(ether, PKTSZ);

        while ((length = read(ETH0, (void *)ether, PKTSZ)) == 0)
            ;

        // TODO: Decode and respond to ARP Request and ICMP Echo Requests.
        //  Replies can be sent with
        //  write(ETH0, (void *)packet, length);

        if(ntohs(ether->type) == ETYPE_ARP)
	{
            ether_swap(ether);
            argram = (struct arpgram *)ether->data;
	    if(ntohs(argram->oper) == ARP_REQUEST)
	    {
		arp_reply(ether->data);
		write(ETH0, (void *)packet, PKTSZ);
	    }
	}
	if(ntohs(ether->type) == ETYPE_IPv4)
	{
	    ipgram = (struct ipv4gram *)ether->data;
	    if(ipgram->protocol == IP_ICMP)
	    {
		icmp = (struct icmpgram *)ipgram->data;
		if(icmp->type == ICMP_REQUEST)
		{
		    ether_swap(ether);
		    ipv4_swap(ipgram, ntohs(ipgram->length));
		    icmp->cksum = 0;
		    icmp->cksum = checksum(icmp ,(PKTSZ - ETHER_SIZE - IPv4_SIZE));
		    icmp->type = ICMP_REPLY;
		    write(ETH0, (void *)packet, PKTSZ);
		}
	    }
	}
    }

    return 0;
}



/**
 * Shell command (pingserver) provides a simple ping server.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_pingserver(ushort nargs, char *args[])
{
    ready(create((void *)echoReply, INITSTK, 19, "EchoReply", 0),
          RESCHED_YES);
    return 0;
}
