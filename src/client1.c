#include "utils.h"

void send_number(int sockfd)
{
    uint16_t n = create_random();
    char *instr;
    char *buffer = malloc(sizeof(char) * 4 + sizeof(uint16_t));
    int offset = 0;

    if (buffer == NULL)
    {
        perror("ERROR Buffer NULL");
        exit(EXIT_FAILURE);
    }

    instr = "INT ";
    memmove(buffer, instr, 4);
    offset += 4;
    n = htons(n);
    memmove(buffer + offset, &n, 2);

    int s = send(sockfd, buffer, sizeof(buffer), 0);
    if (s == -1)
    {
        perror("Error during sending int");
        exit(EXIT_FAILURE);
    }

    buffer = realloc(buffer, sizeof(char) * 5);
    if (buffer == NULL)
    {
        perror("Failling during realloc buffer for INTOK");
        exit(EXIT_FAILURE);
    }

    int reponse = recv(sockfd, buffer, sizeof(buffer), 0);
    if (reponse == -1)
    {
        perror("Cannot receive reponse");
        exit(EXIT_FAILURE);
    }
    free(buffer);
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

    for (int i = 0; i < 5; i++)
    {
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

        if (connect(sockfd, (struct sockaddr *)&adress_stock, sizeof(struct sockaddr_in)) == 0)
        {
            pseudo(sockfd);
            send_number(sockfd);
            close(sockfd);
        }
    }

    return 0;
}