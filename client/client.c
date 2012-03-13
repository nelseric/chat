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
#include <readline/readline.h>

#include "../common/chat.h"
#include "client.h"


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

    int srv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(srv_socket < 0)
        diep("socket");

    if(connect(srv_socket, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in)))
        diep("connect");

    //set up connection
    //get motd

    struct chat_packet p;
    p.username = username;
    p.opcode = OP_HELLO;
    size_t len;
    char *data = cpack(&p, &len);
    write(srv_socket, data, len);

    struct chat_buffer cbuf;
    cbuf_init(&cbuf);
    fd_set fds;
    int nfds = srv_socket + 1;

    while(1) {
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(srv_socket, &fds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0; 

        int sr = select(nfds, &fds, NULL, NULL, &tv);
        if(sr == -1)
            diep("select");
        if(FD_ISSET(srv_socket, &fds)){
            char buff[500];
            struct chat_packet * pktReceived;
            size_t received  =  recv(srv_socket, buff,500, 0);
            if(received == 0){
                puts("Connection terminated by server.");
                exit(EXIT_SUCCESS);
            } else if(received == -1){
                diep("");
            }
            pktReceived = cunpack(buff, received);
            switch(pktReceived->opcode){
                case OP_CMSG:
                    {
                        printf("%s: %s\n", 
                                pktReceived->username, 
                                pktReceived->body.cm.message );
                        break;
                    }
                case OP_PMSG:
                    {
                        printf("!FROM %s: %s\n", 
                                pktReceived->username, 
                                pktReceived->body.pm.message );
                        break;
                    }
                case OP_ERROR:
                    {
                        printf("Error: %s: %s", 
                                pktReceived->username, 
                                pktReceived->body.error.message);
                        break;
                    }
            }
        }
        if(FD_ISSET(0, &fds)){
            char buf[100];
            fgets(buf, 100, stdin);
            parse_command(srv_socket, username, buf);
            free(cmd);
        }
    }
    freeaddrinfo(servinfo);
}


/***
 * This chat buffer stuff was from my initial design of using ncurses in two threads, but that was a pain.
 */
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

void parse_command(int sock, char * username, char * input ){
    if(input[0] == '/' && input[1] == 'p'){
        char *msg;
        char *rcp = strtok_r(input, " ", &msg);

        struct chat_packet p;
        p.opcode = OP_PMSG;
        p.username = username;
        p.body.pm.recipient = rcp;
        p.body.pm.message = msg;
        size_t len;
        char * data = cpack(&p, &len);
        write(sock, data, len);
    } else {
        struct chat_packet p;
        p.opcode = OP_CMSG;
        p.username = username;
        p.body.cm.message = input;
        size_t len;
        char * data = cpack(&p, &len);
        write(sock, data, len);
    }
}
