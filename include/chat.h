#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Application definitions */
#define BUF_SIZE   500
#define MAX_CLIENT 99
#define NAME_MAX   64
#define TRUE       1
#define FALSE      0

/* Text colors and modifiers */
#define DIM        "\033[2m"
#define RESETDIM   "\033[22m"
#define RESETALL   "\033[0m"

#define DEFAULT    "\033[39m"
#define RED        "\033[31m"
#define GREEN      "\033[32m"
#define YELLOW     "\033[33m"
#define BLUE       "\033[34m"
#define MAGENTA    "\033[35m"
#define CYAN       "\033[36m"

char *colorlist[6] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};

/* Title messege */

#define TITLE "  __  __           ___     ___ _         _   \n |  \\/  |__ _ _ _ / _ \\   / __| |_  __ _| |_ \n | |\\/| / _` | '_| (_) | | (__| ' \\/ _` |  _| \n |_|  |_\\__,_|_|  \\__\\_\\  \\___|_||_\\__,_|\\__|\n                                         \n"
