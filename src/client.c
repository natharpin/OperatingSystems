
#include <chat.h>

#define CLEAR "\33[1A\33[2K"

/* Help command string indeces*/
#define HELPCMD 0

/* Help command messages */
#define HELP "Type 'close' to sign out of the server\nType 'exit' to shut the whole server down\n"
#define HELP_ERROR "That was not the help command\n"

void * receiveMsg(void *);
int check(char *);

int is_open;
char *cmd[] = {"$help\n"};

void closeThis()
{
    is_open = 0;
}


int main(int argc, char *argv[])
{
    is_open = 1;
    int sockfd, myport, nread, their_addr_len;
    struct sockaddr_in my_addr, their_addr;
    char buf[BUF_SIZE];

    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s <IP address> <portnum> <username>\n", argv[0]);
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

    pthread_t printer;
    if(pthread_create(&printer, NULL, receiveMsg, &sockfd))
    {
	    fprintf(stderr, "Error creating thread");
        exit(EXIT_FAILURE);
    }

    printf("The username is %s\n", argv[3]);
    
    char username[NAME_MAX];
    strncpy(username, argv[3], NAME_MAX);
    
    memset(buf, '\0', BUF_SIZE);
    strcpy(buf, username);
    strcat(buf, "\n");
    if(sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, their_addr_len) != strlen(buf))
    {
	    fprintf(stderr, "Error sending username");
        exit(EXIT_FAILURE);
    }
    

    while(is_open)
    {
        memset(buf, '\0', BUF_SIZE);
        fgets(buf, BUF_SIZE, stdin);
	    printf(CLEAR);
        if(check(buf))
        {    
            if(sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&their_addr, their_addr_len) != strlen(buf))
            {
	            fprintf(stderr, "Error sending message");
                exit(EXIT_FAILURE);
            }
        }
    }

    pthread_join(printer, NULL);
    close(sockfd);

    return 0;

}

int check(char *buf)
{
    char *temp = malloc(strlen(buf));
    strcpy(temp, buf);
    if(strchr(buf, (int)'$'))
    {
        if(strcmp(temp, cmd[HELPCMD]) == 0)
        {
            printf(HELP);
            return 0;
        }
        else
        {
            printf(HELP_ERROR);
            return 0;
        }
    }
    
    if(strcmp(buf, "close\n") == 0)
        closeThis();
    
    return 1;
}

void *receiveMsg(void *socket)
{
    int sockfd = *((int *)socket), i = 0;
    char rcv_buf[BUF_SIZE];
    while(is_open)
    {
        memset(rcv_buf, '\0', BUF_SIZE);
        recvfrom(sockfd, rcv_buf, BUF_SIZE, 0, NULL, NULL);
        printf("%s", rcv_buf);
    }
}

