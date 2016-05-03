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

void * receiveMsg(void *);

int main(int argc, char *argv[])
{
    int sockfd, myport, nread, their_addr_len;
    struct sockaddr_in my_addr, their_addr;
    char buf[BUF_SIZE];

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s, IP address, portnum \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    their_addr_len = sizeof(struct sockaddr_in);
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(atoi(argv[2]));
    if(!inet_aton(argv[1], &(their_addr.sin_addr)))
    {
        fprintf(stderr, "Incorrect IP adress");
        exit(EXIT_FAILURE);
    }
    memset(&(their_addr.sin_zero), '\0', 8);

    pthread_t rec_thread;
    pthread_create(&rec_thread, NULL, receiveMsg, &sockfd);

    memset(buf, '\0', BUF_SIZE);
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(1)
        {
            sendto(sockfd, buf, strlen(buf), 0, &their_addr, their_addr_len);
        }
    }

    pthread_join(rec_thread, NULL);
    close(sockfd);

    return 0;

}

void *receiveMsg(void *socket)
{
    int sockfd = *((int *)socket), i = 0;
    char rcv_buf[BUF_SIZE];
    while(1)
    {
        memset(rcv_buf, '\0', BUF_SIZE);
        recvfrom(sockfd, rcv_buf, BUF_SIZE, 0, NULL, NULL);
        printf("%s", rcv_buf);
    }
}

