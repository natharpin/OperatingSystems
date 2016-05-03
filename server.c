/* Simple echo server demonstrating listening to, reading from,
 * and writing to a network socket. */
/* DWB */
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

#define BUF_SIZE 500
#define MAX_CLIENT 5
#define TRUE 1
#define FALSE 0

int client_total = 0;

struct client
{
    char *name;
    char *host;
    struct sockaddr_in *their_addr;
    struct client *next;
    struct client *prev;
} *head;

int addClient(struct sockaddr_in *clientaddr, char *clienthost)
{
    if(client_total >= MAX_CLIENT)
    {
	return FALSE;
    }
    client_total++;
    struct client *current = head;
    while(current-> next != NULL)
	current = current->next;

    struct client *newclient;
    newclient->name = NULL;
    newclient->host = clienthost;
    newclient->their_addr = clientaddr;
    newclient->next = NULL;
    newclient->prev = current;
    current->next = newclient;
    return TRUE;
}

int contains(char *clienthost)
{
    struct client *current = head;
    while(current->next != NULL)
	current = current->next;
    if(strcmp(clienthost, current->host))
	return FALSE;
    return TRUE;
}

int
main (int argc, char *argv[])
{
    int sockfd, myport, nread, their_addr_len, numclients;
    struct sockaddr_in my_addr, their_addr;
    char buf[BUF_SIZE];

    head->name = NULL;
    head->their_addr = NULL;
    head->next = NULL;

    if (argc != 2)
    {
        fprintf (stderr, "Usage: %s portnum\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    numclients = 0;

    myport = atoi (argv[1]);

    sockfd = socket (AF_INET, SOCK_DGRAM, 0);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons (myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset (&(my_addr.sin_zero), '\0', 8);

    if (bind (sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)))
    {
        close (sockfd);
        fprintf (stderr, "Failed to bind socket!\n");
        exit (EXIT_FAILURE);
    }
    else
    {
      printf ("Server listening on port %d\n", myport);
    }

    their_addr_len = sizeof (struct sockaddr_in);

    while(1)
    {
        nread = recvfrom (sockfd, buf, BUF_SIZE, 0,
                    (struct sockaddr *) &their_addr, &their_addr_len);

        char host[NI_MAXHOST], service[NI_MAXSERV];
        int result;
        result = getnameinfo ((struct sockaddr *) &their_addr, their_addr_len,
                                host, NI_MAXHOST, service, NI_MAXSERV,
			        NI_NUMERICSERV);

        if (result == 0)
            printf ("Received %d bytes from %s:%s\n", 
                    (long) nread, host, service);
        else
            fprintf (stderr, "getnameinfo: %s\n", gai_strerror (result));

        if (sendto (sockfd, buf, nread, 0, (struct sockaddr *) &their_addr,
                their_addr_len) != nread)
        {
            fprintf (stderr, "Error sending response\n");
        }
    }
  close (sockfd);
  return 0;
}
