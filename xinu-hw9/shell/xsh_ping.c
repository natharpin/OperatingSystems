/**
 * @file     xsh_ping.c
 * @provides xsh_ping, echoRequest
 *
 */
/* Embedded XINU, Copyright (C) 2009, 2016.  All rights reserved. */

/**
 *  Made by Nathan Arpin and April Song
 */

/* TA-BOT:MAILTO nathan.arpin@marquette.edu april.song@marquette.edu */

#include <xinu.h>

int icmpPrep(void *buf, ushort id, char *dst);
int icmpPrint(void *buf, int length);
void setupEther(struct ethergram *, char *, ushort, ushort);
void printResults(char *, int, int, int);

/**
 * Provides a client that sends out a series of ping requests.
 * @param dst Dotted decimal destination string.
 * @return OK for success, SYSERR for syntax error
 */
int echoRequest(char *dst)
{
    char requestpkt[REQUEST_PKTSZ];
    char receivepkt[PKTSZ];
    struct ethergram *ether, *request;
    struct ipv4gram *ippkt;
    struct icmpgram *icmp;
    struct arpgram *argram;
    int length;
    int attempts, totalsent, dropped;
    ushort id, seq;
    bool endLoop;

    totalsent = 0;
    dropped = 0;
    id = 0;
    seq = 0;
    request = (struct ethergram *)requestpkt;
    ether = (struct ethergram *)receivepkt;

    // TODO: Zero out memory for receiving packets.
    //  Construct an ICMP echo request packet.  (See icmpPrep() for help)
    //  Write the constructed packet to the ethernet device.
    //  Read from the ethernet device and sleep.
    //  Reply to ARP requests using arp_reply() if appropriate.
    //  Print reply packets (icmpPrint()) and keep stats.


    attempts = 0;
    while(attempts < 20)
    {
	attempts++;

	bzero(request, REQUEST_PKTSZ);
	bzero(ether, PKTSZ);
	setupEther(request, dst, id, seq);

	write(ETH0, (void *)request, REQUEST_PKTSZ);
	sleep(1000);
	
	int i = 0;
	int found = 0;
	while(i < MAX_READ_ATTEMPTS && found == 0)
	{
	    while ((length = read(ETH0, (void *)ether, PKTSZ)) == 0)
                ;

	    if(ntohs(ether->type) == ETYPE_ARP)
	    {
                ether_swap(ether);
                argram = (struct arpgram *)ether->data;
	        if(ntohs(argram->oper) == ARP_REQUEST)
	        {
		    arp_reply(ether->data);
		    write(ETH0, (void *)receivepkt, length);
	        }
	    }
	    else
	    {
		i++;
	    }
	    if(ntohs(ether->type) == ETYPE_IPv4)
	    {
	        struct ipv4gram *ipgram = (struct ipv4gram *)ether->data;
	        if(ipgram->protocol == IP_ICMP)
	        {
		    struct icmpgram *icmpRep = (struct icmpgram *)ipgram->data;
		    if(icmpRep->type == ICMP_REPLY)
		    {
		        totalsent++;
			found = 1;
		        icmpPrint((void *)ipgram, ntohs(ipgram->length));
		    }
	        }
    	    }
	}
	if(found == 0) dropped++;
        seq++;
    }
    printf("Dropped %d packets of all %d requests sent\n", dropped, attempts);
    return OK;
}

void setupEther(struct ethergram *request, char *dst, ushort id, ushort seq)
{
    struct ipv4gram *ippkt;
    struct icmpgram *icmp;
    icmpPrep(request, id, dst);
    ippkt = (struct ipv4gram *)request->data;
    icmp = (struct icmpgram *)ippkt->data;
    icmp->type = ICMP_REQUEST;
    icmp->code = 0;
    icmp->id = 0;
    icmp->cksum = 0;
    icmp->seq = htons(seq);
    icmp->cksum = checksum(icmp ,(REQUEST_PKTSZ - ETHER_SIZE - IPv4_SIZE));
}
/**
 * Shell command (ping) sends echo requests to provided IP.
 * @param args array of arguments
 * @return OK for success, SYSERR for syntax error
 */
command xsh_ping(ushort nargs, char *args[])
{
    /* Output help, if '--help' argument was supplied */
    if ((nargs == 2 && strncmp(args[1], "--help", 6) == 0) || (nargs < 2))
    {
        printf("Usage: ping [IP address]\n");
        printf("Sends echo requests to provided IP address.\n");
        printf("\t--help\t display this help and exit\n");

        return OK;
    }

    echoRequest(args[1]);

    return OK;
}
