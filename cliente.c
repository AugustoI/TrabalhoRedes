#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

void usage(int argc, char **argv)
{
    printf("usage: %s <server IP> <server port>", argv[0]);
    printf("example: %s 127.0.0.1 51511", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 500

char *prepareCommand(char **command)
{
    printf("prepare 1\n");
    if (strcmp(command[0], "install") == 0)
    {
        char *aux = malloc(BUFSZ * sizeof(char));
        sprintf(aux, "INS_REQ %s %s %s %s", command[3], command[1], command[4], command[5]);
        return aux;
    }
    if (strcmp(command[0], "remove") == 0)
    {
        char *aux = malloc(BUFSZ * sizeof(char));
        sprintf(aux, "REM_REQ %s %s", command[3], command[1]);
        return aux;
    }
    if (strcmp(command[0], "change") == 0)
    {
        char *aux = malloc(BUFSZ * sizeof(char));
        sprintf(aux, "CH_REQ %s %s %s %s", command[3], command[1], command[4], command[5]);
        return aux;
    }
    if ((strcmp(command[0], "show") == 0) && (strcmp(command[2], "in") == 0))
    {
        char *aux = malloc(BUFSZ * sizeof(char));
        sprintf(aux, "LOC_REQ %s", command[3]);
        return aux;
    }
    if ((strcmp(command[0], "show") == 0))
    {
        char *aux = malloc(BUFSZ * sizeof(char));
        sprintf(aux, "DEV_REQ %s %s", command[3], command[1]);
        return aux;
    }
    return "ERROR";
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != connect(s, addr, sizeof(storage)))
    {
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    printf("mensagem> ");
    fgets(buf, BUFSZ - 1, stdin);
    char resp[600];
    sprintf(resp, "%s", prepareCommand(split(buf, " ")));
    size_t count = send(s, resp, strlen(resp) + 1, 0);
    printf("Resposta: %s", resp);
    if (count != strlen(resp) + 1)
    {
        logexit("send");
    }

    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while (1)
    {
        count = recv(s, buf + total, BUFSZ - total, 0);
        if (count == 0)
        {
            break;
        }
        total += count;
    }

    printf("received %u bytes\n", total);
    puts(buf);

    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}