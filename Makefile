all:
	gcc -Wall -c common.c
	gcc -Wall cliente.c common.o -o cliente
	gcc -Wall servidor.c common.o -o servidor