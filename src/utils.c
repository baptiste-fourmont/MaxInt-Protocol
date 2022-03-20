#include "utils.h"

int convert(char ch)
{
    int ascii = ch;
    return ascii;
}

char convert_to_char(int num)
{
    char ch = num;
    return ch;
}

int create_random()
{
    return rand();
}

// Question quand le serveur recoit le pseudo: doit - il le reconvertir en ascii pour que le client le décode?

char *get_name(int sockfd)
{
    int ascii[MAX_NAME];
    char *name = malloc(sizeof(char) * MAX_NAME + 1);
    if (name == NULL)
    {
        perror("ERROR during malloc");
        exit(1);
    }
    int rcv = recv(sockfd, &ascii, sizeof(int) * MAX_NAME, 0);
    if (rcv == -1)
    {
        perror("Error when getting name");
        exit(EXIT_FAILURE);
    }
    rcv = rcv / 4;
    int k = 0;
    for (int j = 0; j < rcv; j++)
    {

        name[k] = convert_to_char(ascii[j]);
        k += 1;
    }
    name[k] = '\0';
    return name;
}

void pseudo(int sockfd)
{

    const char *list[11];
    list[0] = "0123456789";
    list[1] = "aaaaaaaaaa";
    list[2] = "baaaaaab37";
    list[3] = "neutronjim";
    list[4] = "boobajkaar";
    list[5] = "niskamoio1";
    list[6] = "bidouapap0";
    list[7] = "jimmypayet";
    list[8] = "bobditlane";
    list[9] = "henrytuazo";
    list[10] = "michelinea";


    char name[MAX_NAME];
    int ascii[MAX_NAME];
    strcpy(name, list[create_random() % 10]);

    // On transforme en un tableau de ascii
    if (strlen(name) == 10)
    {
        int i = 0;
        while (name[i] != '\0')
        {
            ascii[i] = convert(name[i]);
            i += 1;
        }
        int s = send(sockfd, &ascii, sizeof(int) * i, 0);
        if (s == -1)
        {
            perror("ERROR during sending name");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        perror("Le pseudo ne contient pas exactement MAX_NAME char");
        exit(EXIT_FAILURE);
    }

    char buff[17];
    int rcv = recv(sockfd, &buff, sizeof(buff), 0);
    if (rcv == 0)
    {
        perror("Doesn't receive name");
    }
    buff[rcv] = '\0';
    printf("%s\n", buff);
}
