#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "common.h"

#define BUFSZ 500

typedef struct sala_s sala_t;

struct sala_s
{
    int id;
    int temperatura;
    int humidade;
    int estadoVent1;
    int estadoVent2;
    int estadoVent3;
    int estadoVent4;

    sala_t *prox;
};

char *cadastrarSala(sala_t *salas, int id)
{
    if ((id < 0) || (id > 7))
    {
        return "ERROR 01";
    }

    sala_t *s = salas;

    while (s)
    {
        if ((s->id == id))
        {
            return "ERROR 02";
        }

        s = s->prox;
    }

    sala_t *c = malloc(sizeof(sala_t));
    c->id = id;
    c->temperatura = -1;
    c->humidade = -1;
    c->estadoVent1 = -1;
    c->estadoVent2 = -1;
    c->estadoVent3 = -1;
    c->estadoVent4 = -1;
    c->prox = NULL;

    sala_t *p = salas;
    if (!p)
    {
        salas = c;
        return "OK 01";
    }

    while (p->prox)
        p = p->prox;
    p->prox = c;
    return "OK 01";
}

char *iniSensorSala(sala_t *disps, int id, int temp, int humidade, int estadoVent1, int estadoVent2, int estadoVent3, int estadoVent4)
{
    if ((id < 0) || (id > 7))
    {
        return "ERROR 01";
    }

    sala_t *s = disps;
    while (s)
    {
        if ((s->id == id))
        {
            s->temperatura = temp;
            s->humidade = humidade;
            s->estadoVent1 = estadoVent1;
            s->estadoVent2 = estadoVent2;
            s->estadoVent3 = estadoVent3;
            s->estadoVent4 = estadoVent4;
            return "OK 02";
        }
        s = s->prox;
    }
    return "ERROR 03";
}

char *desSensorSala(sala_t *disps, int id)
{
    if ((id < 0) || (id > 7))
    {
        return "ERROR 01";
    }

    sala_t *s = disps;
    while (s)
    {
        if ((s->id == id))
        {
            if (s->humidade == -1)
            {
                return "ERROR 06";
            }
            s->temperatura = -1;
            s->humidade = -1;
            s->estadoVent1 = -1;
            s->estadoVent2 = -1;
            s->estadoVent3 = -1;
            s->estadoVent4 = -1;
            return "OK 03";
        }
        s = s->prox;
    }
    return "ERROR 03";
}

char *altSensorSala(sala_t *disps, int id, int temp, int humidade, int estadoVent1, int estadoVent2, int estadoVent3, int estadoVent4)
{
    if ((id < 0) || (id > 7))
    {
        return "ERROR 01";
    }

    sala_t *s = disps;
    while (s)
    {
        if ((s->id == id))
        {
            if (s->humidade == -1)
            {
                return "ERROR 06";
            }            
            s->temperatura = temp;
            s->humidade = humidade;
            s->estadoVent1 = estadoVent1;
            s->estadoVent2 = estadoVent2;
            s->estadoVent3 = estadoVent3;
            s->estadoVent4 = estadoVent4;
            return "OK 04";
        }
        s = s->prox;
    }
    return "ERROR 03";
}

char *infoSala(sala_t *disps, int id)
{
    if ((id < 0) || (id > 7))
    {
        return "ERROR 01";
    }

    char *sala = malloc(500 * sizeof(char));
    
    sala_t *s = disps;
    while (s)
    {
        if ((s->id == id))
        {
            if (s->humidade == -1)
            {
                return "ERROR 06";
            }
            sprintf(sala, "sala %d: %d %d 1%d 2%d 3%d 4%d", s->id, s->temperatura, s->humidade, s->estadoVent1, s->estadoVent2, s->estadoVent3, s->estadoVent4);
            return sala;
        }
        s = s->prox;
    }
    return "ERROR 03";
}

char *infoTodasSalas(sala_t *disps)
{
    char *sala = malloc(500 * sizeof(char));
    char *result = malloc(500 * sizeof(char));
    sprintf(sala, "salas: ");
    
    sala_t *s = disps;
    while (s)
    {
        sprintf(result, "%s %d: %d %d 1%d 2%d 3%d 4%d", sala, s->id, s->temperatura, s->humidade, s->estadoVent1, s->estadoVent2, s->estadoVent3, s->estadoVent4);
        strcpy(sala, result);
        s = s->prox;
    }
    free(result);
    return sala;
}


void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

char *executeCommand(sala_t *salas, char **values)
{
    if (strcmp(values[0], "CAD_REQ") == 0)
    {
        return cadastrarSala(salas, atoi(values[1]));
    }
    if (strcmp(values[0], "INI_REQ") == 0)
    {
        return iniSensorSala(salas, atoi(values[1]), atoi(values[2]), atoi(values[3]), atoi(values[4]), atoi(values[5]), atoi(values[6]), atoi(values[7]));
    }
    if (strcmp(values[0], "DES_REQ") == 0)
    {
        return desSensorSala(salas, atoi(values[1]));
    }
    if (strcmp(values[0], "ALT_REQ") == 0)
    {
        return altSensorSala(salas, atoi(values[1]), atoi(values[2]), atoi(values[3]), atoi(values[4]), atoi(values[5]), atoi(values[6]), atoi(values[7]));
    }
    if (strcmp(values[0], "SAL_REQ") == 0)
    {
        return infoSala(salas, atoi(values[1]));
    }
    if (strcmp(values[0], "INF_REQ") == 0)
    {
        return infoTodasSalas(salas);
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
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }

    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    sala_t dispositivos;
    dispositivos.prox = NULL;

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int cSock = accept(s, caddr, &caddrlen);
        if (cSock == -1)
        {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(cSock, buf, BUFSZ, 0);
        printf("[msg] %s, %d bytes %s\n", caddrstr, (int)count, buf);
        char resp[600];
        sprintf(resp, "%s", executeCommand(&dispositivos, split(buf, " ")));
        printf("%s\n", resp);
        printf("Executed\n");
        count = send(cSock, resp, strlen(resp) + 1, 0);
        if (count != strlen(resp) + 1)
        {
            logexit("send");
        }
        close(cSock);
    }

    exit(EXIT_SUCCESS);
    return 0;
}