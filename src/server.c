/**
 * Simple chat server
 * Extra features: each client gets a color,
 *                 a client may exit without shutting down the whole server,
 *                 and there can be more than five clients at once
 *
 * Created by Nathan Arpin and April Song
 * Used Dr. Brylow's simple server example as a basis
 *
 * Contact: nathan.arpin@marquette.edu or april.song@marquette.edu
 */



#include <chat.h>

#define REJECT "Error: There are too many clients on the server"

void sendtoAll(int, char *, int, int, char *, char *);
struct client * getFromName(char *);
int checkDst(char *);
void sendtoOne(int, char *, int, int, char *, struct client *);
struct client * getName(char *, char *);

int client_total = 0;

struct client
{
    char name[NAME_MAX];
    char host[NAME_MAX];
    char color[16];
    int their_addr_len;
    struct sockaddr_in *their_addr;
    struct client *next;
    struct client *prev;
} head;

void cpysa(struct sockaddr_in *dst, struct sockaddr_in *src)
{
    dst->sin_family = src->sin_family;
    dst->sin_port = src->sin_port;
    dst->sin_addr.s_addr = src->sin_addr.s_addr;
    memset(dst->sin_zero, '\0', 8);
}

int lower(int first, int sec)
{
    if(first < sec) 
        return first;
    else 
        return sec;
}

int color = 0;
char * getColor()
{
    color = (color + 1) % 6;
    return colorlist[color];
}

int addClient(struct sockaddr_in *clientaddr, char *clienthost, int len, int sockfd, char *username)
{
    if(client_total >= MAX_CLIENT)
    {
	    return FALSE;
    }
    client_total++;
    struct client *current = &head;
    while(current-> next != NULL)
    {
	    current = current->next;
	}

    struct client *newclient = malloc(sizeof(struct client));
    memset(newclient->name, '\0', NAME_MAX);
    memcpy(newclient->name, username, lower(NAME_MAX, strlen(username) - 1));
    memset(username, '\0', BUF_SIZE);
    strcpy(newclient->host, clienthost);
    strcpy(newclient->color, getColor());
    newclient->their_addr_len = len;
    newclient->their_addr = malloc(sizeof(struct sockaddr_in));
    cpysa(newclient->their_addr, clientaddr);
    newclient->next = NULL;
    newclient->prev = current;
    current->next = newclient;
    
    printf("Added new client : %s : %s\n", clienthost, newclient->name);
    
    if(sendto(sockfd, TITLE, strlen(TITLE), 0, (struct sockaddr *)newclient->their_addr, newclient->their_addr_len) != strlen(TITLE))
    {
        fprintf(stderr, "There was an error sending the title");
    }
    
    char restofmessage[22] = " has joined the chat\n\0";
    char welcome[strlen(newclient->name) + strlen(restofmessage)];
    strcpy(welcome, newclient->name);
    strcat(welcome, restofmessage);
    sendtoAll(sockfd, welcome, strlen(welcome), 0, "Server", DEFAULT);

    return TRUE;
}

void initHead()
{
    head = *((struct client *)(malloc(sizeof(struct client))));
    memset(head.name, '\0', NAME_MAX);
    memset(head.host, '\0', NAME_MAX);
    head.their_addr_len = 0;
    head.their_addr = NULL;
    head.next = NULL;
    head.prev = NULL;
}

struct client * get(char *clienthost)
{
    struct client *current = &head;
    while(current->next != NULL)
    {
	    current = current->next;
        if(strcmp(clienthost, current->host) == 0)
	        return current;
	}
    return NULL;
}

int contains(char *clienthost)
{    
    struct client *current = &head;
    while(current->next != NULL)
    {
	    current = current->next;
        if(strcmp(clienthost, current->host) == 0)
	        return TRUE;
	}
    return FALSE;
}

int server_on = 1;

void exitServer(int sockfd)
{
    server_on = 0;
    char goodbye[29] = "The server is disconnecting\n\0";
    sendtoAll(sockfd, goodbye, strlen(goodbye), 0, "Server", DEFAULT);
}

void closeClient(char *host, int sockfd)
{
    struct client *current = get(host);
    char goodbye[29] = " is leaving the chat\n\0";
    char messege[BUF_SIZE];
    memset(messege, '\0', BUF_SIZE);
    strncpy(messege, current->name, strlen(current->name));
    strcat(messege, goodbye);
    sendtoAll(sockfd, messege, strlen(messege), 0, "Server", DEFAULT);
    client_total--;
    current->prev->next = current->next;
    if(current->next != NULL)
        current->next->prev = current->prev;
    memset(goodbye, '\0', strlen(goodbye));
    memset(messege, '\0', BUF_SIZE);
}

int main (int argc, char *argv[])
{
    int sockfd, myport, nread, their_addr_len, numclients;
    struct sockaddr_in my_addr, their_addr;
    char buf[BUF_SIZE];

    initHead();

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

    while(server_on)
    {
        memset(buf, '\0', BUF_SIZE);
        nread = recvfrom (sockfd, buf, BUF_SIZE, 0, (struct sockaddr *)&their_addr, &their_addr_len);

        char host[NI_MAXHOST], service[NI_MAXSERV];
        int result;
        result = getnameinfo ((struct sockaddr *) &their_addr, their_addr_len,
                                host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

        int addedClient = 0;

        if (result == 0)
        {
            if(contains(host))
            {
                printf ("Received %d bytes from %s:%s\n", (long) nread, host, service);
            }
            else if(addClient(&their_addr, host, their_addr_len, sockfd, buf) == 0)
            {
                printf("Rejected %d bytes from %s:%s\n", (long)nread, host, service);
                if(sendto(sockfd, REJECT, strlen(REJECT), 0, (struct sockaddr *) &their_addr, their_addr_len) != strlen(REJECT))
                    fprintf(stderr, "Error sending reject response\n");
                continue;
            }
            else
            {
                addedClient = 1;
            }
        }
        else
        {
            fprintf (stderr, "getnameinfo: %s\n", gai_strerror (result));
        }
        if(!addedClient)
        {
            switch(checkDst(buf))
            {
                case 0:
                {
                    sendtoAll(sockfd, buf, nread, 0, (get(host))->name, (get(host))->color);
                    break;
                }
                case 2:
                {
                    exitServer(sockfd);
                    break;
                }
                case 3:
                {
                    closeClient(host, sockfd);
                    break;
                }
                default:
                    break;
            }
        }
    }
  close (sockfd);
  return 0;
}

int checkDst(char *buf)
{
    if(strcmp(buf, "exit\n") == 0)
        return 2;
    if(strcmp(buf, "close\n") == 0)
        return 3;
    else
        return 0;
}

void sendtoAll(int sockfd, char *buf, int nread, int flags, char *name, char *color)
{
    struct client *current = &head;
    while(current->next != NULL)
    {
        char bufbuffer[BUF_SIZE];
        strcpy(bufbuffer, buf);
        
        char namebuf[NAME_MAX];
        strcpy(namebuf, name);
        
	    current = current->next;
        char temp[BUF_SIZE];
        strcpy(temp, DIM);
        strcat(temp, color);
        strcat(temp, namebuf);
        strcat(temp, ": ");
        strcat(temp, RESETDIM);
        strncat(temp, bufbuffer, strlen(bufbuffer) - 1);
        strcat(temp, RESETALL "\n");
        if (sendto (sockfd, temp, strlen(temp), flags, (struct sockaddr *) current->their_addr, current->their_addr_len) != strlen(temp))
        {
            fprintf (stderr, "Error sending response\n");
        }
        memset(temp, '\0', BUF_SIZE);
        memset(bufbuffer, '\0', BUF_SIZE);
	}

}
