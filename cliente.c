#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"

#define BUFSZ 500

int typeRequest = 0;

void usage(int argc, char **argv)
{
    printf("usage: %s <server IP> <server port>", argv[0]);
    printf("example: %s 127.0.0.1 51511", argv[0]);
    exit(EXIT_FAILURE);
}

char *prepareCommand(char **request) {
    char *command = malloc(BUFSZ * sizeof(char));
    if (command == NULL) {
        return "ERROR: Memory allocation failed";
    }
    
    if (strcmp(request[0], "register") == 0) {
        snprintf(command, BUFSZ, "CAD_REQ %s", request[1]);
        typeRequest = 1;
    } else if ((strcmp(request[0], "init") == 0) && (strcmp(request[1], "info") == 0)) {
        snprintf(command, BUFSZ, "INI_REQ %s %s %s %s %s %s %s", request[2], request[3], request[4], request[5], request[6], request[7], request[8]);
    } else if (strcmp(request[0], "shutdown") == 0) {
        snprintf(command, BUFSZ, "DES_REQ %s", request[1]);
    } else if ((strcmp(request[0], "update") == 0) && (strcmp(request[1], "info") == 0)) {
        snprintf(command, BUFSZ, "ALT_REQ %s %s %s %s %s %s %s", request[2], request[3], request[4], request[5], request[6], request[7], request[8]);
    } else if ((strcmp(request[0], "load") == 0) && (strcmp(request[1], "info") == 0)) {
        snprintf(command, BUFSZ, "SAL_REQ %s", request[2]);
    } else if ((strcmp(request[0], "load") == 0) && (strcmp(request[1], "rooms\n") == 0)) {
        snprintf(command, BUFSZ, "INF_REQ");
    } else {
        free(command);
        printf("Comando invalido\n");
        return "ERROR";
    }

    return command;
}

char *checkResponse(char *response)
{
    if (strcmp(response, "OK 01") == 0)
    {
        return "sala instanciada com sucesso";
    }

    if (strcmp(response, "OK 02") == 0)
    {
        return "sensores inicializados com sucesso";
    }

    if (strcmp(response, "OK 03") == 0)
    {
        return "sensores desligados com sucesso";
    }

    if (strcmp(response, "OK 04") == 0)
    {
        return "informações atualizadas com sucesso";
    }

    if (strcmp(response, "ERROR 01") == 0)
    {
        return "sala inválida";
    }

    if (strcmp(response, "ERROR 02") == 0)
    {
        return "sala já existe";
    }

    if (strcmp(response, "ERROR 03") == 0)
    {
        return "sala inexistente";
    }

    if (strcmp(response, "ERROR 04") == 0)
    {
        return "sensores inválidos";
    }

    if (strcmp(response, "ERROR 05") == 0)
    {
        return "sensores já instalados";
    }

    if (strcmp(response, "ERROR 06") == 0)
    {
        return "sensores não instalados";
    }
    return response;
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

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    fgets(buf, BUFSZ - 1, stdin);
    char resp[600];
    sprintf(resp, "%s", prepareCommand(split(buf, " ")));
      
    size_t count = send(s, resp, strlen(resp) + 1, 0);
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

    if (strcmp(resp,"ERROR") != 0)
    {
        puts(checkResponse(buf));
    }

    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}