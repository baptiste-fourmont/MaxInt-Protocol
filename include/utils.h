#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define MAX_NAME 10

#ifndef UTIL_H
#define UTIL_H

struct user
{
    char *name;
    uint32_t ip;
    uint16_t max;
};

int convert(char ch);
char convert_to_char(int num);
char *get_name(int sockfd);
void pseudo(int sockfd);
int create_random();

#endif