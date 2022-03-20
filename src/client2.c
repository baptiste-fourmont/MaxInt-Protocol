#include "utils.h"

void send_max(int sockfd)
{
    char *msg = "MAX";
    char *buffer = malloc(sizeof(char) * 3);
    char ip[INET_ADDRSTRLEN + 1];
    uint32_t ipu;
    uint16_t max;

    if (buffer == NULL)
    {
        perror("Error during malloc buffer during receive max");
        exit(EXIT_FAILURE);
    }

    int s = send(sockfd, msg, sizeof(char) * 3, 0);
    if (s == -1)
    {
        perror("Error during sending Max");
        exit(EXIT_FAILURE);
    }

    int rcv = recv(sockfd, buffer, sizeof(char) * 3, 0);
    if (rcv == -1)
    {
        perror("Error during receive NOP or REP");
        exit(EXIT_FAILURE);
    }
    buffer[rcv] = '\0';

    if (strcmp(buffer, "NOP") == 0)
    {
        printf("%s\n", buffer);
    }
    else
    {
        char *name = get_name(sockfd);

        rcv = recv(sockfd, &ipu, sizeof(uint32_t), 0);
        if (rcv == -1)
        {
            perror("Error during receive ip in uint32");
            exit(EXIT_FAILURE);
        }

        if (inet_ntop(AF_INET, &ipu, ip, sizeof(ip)) == NULL)
        {
            perror("Error during inet_ntop");
            exit(EXIT_FAILURE);
        }

        rcv = recv(sockfd, &max, sizeof(uint16_t), 0);
        if (rcv == -1)
        {
            perror("Error during receive max");
            exit(EXIT_FAILURE);
        }
        max = ntohs(max);
        printf("%s %s %s %d\n", buffer, name, ip, max);
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    if (argc != 3)
    {
        perror("You don't have the arguments require [PORT] [IP]");
        exit(EXIT_FAILURE);
    }

    int PORT = atoi(argv[1]);

    struct sockaddr_in adress_stock;
    adress_stock.sin_family = AF_INET;
    adress_stock.sin_port = htons(PORT);

    if (inet_aton(argv[2], &adress_stock.sin_addr) == 0)
    {
        perror("Invalid adress");
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    int cn = connect(sockfd, (struct sockaddr *)&adress_stock,
                     sizeof(struct sockaddr_in));
    if (cn == -1)
    {
        perror("Cannot connect");
        exit(EXIT_FAILURE);
    }

    pseudo(sockfd);
    send_max(sockfd);
    close(sockfd);
    return 0;
}