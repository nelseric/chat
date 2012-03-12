#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#include "../common/chat.h"
#include "server.h"

void diep(char *s){
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv){

	int listen_sock;

	struct sockaddr_in listen_addr;
	memset((char *) &listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(CHAT_PORT);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    	diep("listen socket");
    if(bind(listen_sock, (struct sockaddr*) &listen_addr, sizeof(listen_addr)) == -1)
    	diep("listen bind");
    if(listen(listen_sock, 1024) == -1)
    	diep("listen");
    fd_set sockets;
    FD_ZERO(&sockets);

    while(1){
    	struct sockaddr_in caddr;
    	socklen_t caddr_size = sizeof(struct sockaddr_in);
    	int cfd = accept(listen_sock, (struct sockaddr *)&caddr, &caddr_size);
    	if(cfd == -1)
    		diep("accept");
    	puts("Connected!");

    }
}

void cs_init(client_store_t * cstore){
    cstore->size = 10;
    cstore->num_clients = 0;
    cstore->clients = malloc(10 * sizeof(chat_user_t));
}

int add_client(client_store_t * cs, int sock, const char *userid){
    
    if(cs->size == cs->num_clients){
        cs->size = (cs->size * 3 / 2) + 5;
        cs->clients = realloc(cs->clients, cs->size);
    }
	return 0;
}

chat_user_t * get_user_by_id(client_store_t *cs, const char *userid){
    return NULL;
}

void cs_iterate(void (*action)(chat_user_t *)){

}

void delete_client(client_store_t *cs, int sock){
    
}

void cs_free(client_store_t *cs){
    for(int i = 0; i < cs->num_clients; i++){
        free(cs->clients[i].userid);
    }
    free(cs->clients);
    free(cs);
}

