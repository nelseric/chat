#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include <pthread.h>

#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>

#include "../common/chat.h"
#include "client.h"

void usage(char *argv[]) {
    
} 
void diep(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    if(argc < 3 || argc > 4){
        fprintf(stderr, "Usage: %s <username> <host> (<port>)\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *host = argv[2];
    char *username = argv[1];
    char * port = CHAT_PORT_S;

    puts(username);

    if(argc == 4){
        port = argv[3];
    }

    struct addrinfo hints;
    struct addrinfo *servinfo;  // will point to the results
    
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // get ready to connect
    if(getaddrinfo(host, port, &hints, &servinfo))
        diep("addr");

    struct sockaddr_in *server_addr = (struct sockaddr_in *) servinfo->ai_addr;

    int srv_socket;
    if((srv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) < 0))
        diep("socket");
    if(connect(srv_socket, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in)))
        diep("connect");

    //set up connection
    //get motd
    
    // initscr();

    // printw("Hello %s", username);
    // getch();

    // endwin();

    freeaddrinfo(servinfo);
}

void cbuf_init(struct chat_buffer *cbuf){
    cbuf->bsize = CBUF_SIZE_DEFAULT;
    cbuf->msgbuf = malloc(sizeof(char *) * cbuf->bsize);
    cbuf->nmsg = 0;
    pthread_mutex_init(&cbuf->lock, NULL);
}
void cbuf_destroy(struct chat_buffer *cbuf){
    pthread_mutex_lock(&cbuf->lock);
    for(int i = 0; i < cbuf->nmsg; i++){
        free(cbuf->msgbuf[i]);
    }
    free(cbuf->msgbuf);
    pthread_mutex_unlock(&cbuf->lock);
    pthread_mutex_destroy(&cbuf->lock);
}

void cbuf_add(struct chat_buffer *cbuf, const char *msg){
    pthread_mutex_lock(&cbuf->lock);

    pthread_mutex_unlock(&cbuf->lock);
}

