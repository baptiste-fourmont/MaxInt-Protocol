#include "utils.h"
#include <time.h>
#include <pthread.h>

#define TRUE 1
#define USERS 1

struct args
{
    int *clients;
};

static pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

struct user user_max;
struct user user;

char *get_instr(int sockfd)
{
    char *buff = malloc(sizeof(char) * 3 + 1);
    if (buff == NULL)
    {
        perror("Error during create buff");
        exit(EXIT_FAILURE);
    }

    int rcv = recv(sockfd, buff, sizeof(char) * 3, 0);
    if (rcv == -1)
    {
        perror("Error cannot receive");
        exit(EXIT_FAILURE);
    }
    buff[rcv] = '\0';
    return buff;
}

void welcome(int sockfd)
{
    char *reponse = malloc(sizeof(char) * 18);
    if (reponse == NULL)
    {
        perror("ERROR during malloc");
        exit(EXIT_FAILURE);
    }

    char *name = get_name(sockfd);
    sprintf(reponse, "%s %s", "HELLO", name);
    int s = send(sockfd, reponse, strlen(reponse), 0);
    if (s == -1)
    {
        perror("Error during sending message");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&mutex_lock);
    user.name = name;
    pthread_mutex_unlock(&mutex_lock);

    printf("%s\n", reponse);
    free(reponse);
}

void end_communication(int sockfd)
{
    close(sockfd);
}

void send_int_reply(int sock)
{
    int offset = 0;
    char *buff;
    uint16_t number;

    buff = malloc(sizeof(char) + sizeof(uint16_t));
    if (buff == NULL)
    {
        perror("Error buf during malloc");
        exit(EXIT_FAILURE);
    }

    int rcv = recv(sock, buff, sizeof(char) + sizeof(uint16_t), 0);
    if (rcv == -1)
    {
        perror("ERROR during receive number");
        exit(EXIT_FAILURE);
    }
    offset = 1;
    memmove(&number, buff + offset, sizeof(uint16_t));
    number = ntohs(number);
    printf("RECEIVE: %d\n", number);
    pthread_mutex_lock(&mutex_lock);

    user.max = number;
    if (user.max > user_max.max)
    {
        user_max = user;
    }

    pthread_mutex_unlock(&mutex_lock);

    buff = realloc(buff, sizeof(char) * 5);
    if (buff == NULL)
    {
        perror("Error realloc failed");
        exit(EXIT_FAILURE);
    }
    memset(buff, 0, 5);
    buff = "INTOK";
    int s = send(sock, buff, sizeof(char) * 5, 0);
    if (s == -1)
    {
        perror("Error during sending INTOK");
        exit(EXIT_FAILURE);
    }
    printf("INTOK\n");
}

void send_max_reply(int sock)
{
    int offset = 0;
    int ascii[MAX_NAME];
    int i = 0;
    char *head = "REP";
    uint16_t max;

    char *buff = malloc(sizeof(char) * 3 + sizeof(int) * 10 + sizeof(uint16_t) + sizeof(uint32_t));
    if (buff == NULL)
    {
        perror("Error during malloc reply max");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&mutex_lock);

    while (user_max.name[i] != '\0')
    {
        ascii[i] = convert(user_max.name[i]);
        i += 1;
    }

    max = user_max.max;

    memmove(buff, head, 3);
    offset += 3;
    memmove(buff + offset, &ascii, sizeof(int) * MAX_NAME);
    offset += sizeof(int) * MAX_NAME;
    memmove(buff + offset, &user.ip, sizeof(uint32_t));
    // Déjà en big endian
    offset += sizeof(uint32_t);
    max = htons(max);
    memmove(buff + offset, &max, sizeof(uint16_t));
    pthread_mutex_unlock(&mutex_lock);
    int s = send(sock, buff, sizeof(char) * 3 + sizeof(int) * 10 + sizeof(uint16_t) + sizeof(uint32_t), 0);
    if (s == -1)
    {
        perror("Error during sending");
        exit(EXIT_FAILURE);
    }
    free(buff);
}

void check(int sockfd)
{

    char *instr = get_instr(sockfd);

    if (strcmp("INT", instr) == 0)
    {
        send_int_reply(sockfd);
    }
    else if (strcmp("MAX", instr) == 0)
    {
        pthread_mutex_lock(&mutex_lock);
        int max = user_max.max;
        pthread_mutex_unlock(&mutex_lock);

        // On vérifie si il a déjà été changé (0 de base)
        if (max == 0)
        {
            char *buff = malloc(strlen("NOP") + 1);
            if (buff == NULL)
            {
                perror("ERROR during malloc NOP");
                exit(EXIT_FAILURE);
            }
            strcpy(buff, "NOP");
            if (send(sockfd, buff, 3, 0) == -1)
            {
                perror("Error during sending NOP");
                exit(EXIT_FAILURE);
            }
            free(buff);
        }
        else
        {
            send_max_reply(sockfd);
        }
    }
}

void *play(void *tab)
{
    int *fd = (int *)tab;
    welcome(*fd);
    check(*fd);
    end_communication(*fd);
    free(fd);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Remains [PORT]");
    }

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in adress;
    adress.sin_port = htons(atoi(argv[1]));
    adress.sin_family = AF_INET;
    adress.sin_addr.s_addr = htonl(INADDR_ANY);

    int bin = bind(sockfd, (struct sockaddr *)&adress, sizeof(struct sockaddr));
    if (bin == -1)
    {
        perror("Can't create bind");
        exit(EXIT_FAILURE);
    }

    int lis = listen(sockfd, 0);
    if (lis == -1)
    {
        perror("Can't listen");
        exit(EXIT_FAILURE);
    }

    // On assigne à 0 pour vérifier si on envoie un NOP ou pas
    user_max.max = 0;

    struct sockaddr_in caller;
    socklen_t size = sizeof(caller);
    int *clients;
    while (TRUE)
    {
        clients = (int *)malloc(sizeof(int));
        if (clients == NULL)
        {
            perror("ERROR during malloc clients");
            exit(EXIT_FAILURE);
        }
        *clients = accept(sockfd, (struct sockaddr *)&caller, &size);
        if (*clients == -1)
        {
            perror("Error during accepting client");
            exit(EXIT_FAILURE);
        }

        user.ip = caller.sin_addr.s_addr;
        pthread_t th1;
        pthread_create(&th1, NULL, play, clients);
    }

    close(sockfd);
    close(*clients);
    pthread_exit(clients);
    free(clients);
    return 0;
}