#include "chat.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>


char * cpack(const struct chat_packet * p, size_t *length){
    char *buf;
    switch(p->opcode){
        case OP_HELLO:
            {
                size_t uname_len = strlen(p->username)+1;
                *length = 2 + uname_len;
                buf = malloc(2 + uname_len);
                *(uint16_t*)buf = htons(p->opcode);

                strncpy(buf + 2, p->username, uname_len);
            }
            break;
        case OP_CMSG:
            {
                size_t uname_len = strlen(p->username)+1;
                size_t msg_len = strlen(p->body.cm.message)+1;
                *length = 2 + uname_len + msg_len;
                buf = malloc(2 + uname_len + msg_len);
                *(uint16_t*)buf = htons(p->opcode);

                strncpy(buf + 2, p->username, uname_len);
                strncpy(buf + 2 + uname_len, p->body.cm.message, msg_len);
            }
            break;
        case OP_PMSG:
            {
                size_t uname_len = strlen(p->username)+1;
                size_t rcp_len = strlen(p->body.pm.recipient)+1;
                size_t msg_len = strlen(p->body.pm.message)+1;

                *length = 2 + uname_len + msg_len + rcp_len;
                buf = malloc(2 + uname_len + msg_len + rcp_len);
                *(uint16_t*)buf = htons(p->opcode);

                strncpy(buf + 2, p->username, uname_len);
                strncpy(buf + 2 + uname_len, p->body.pm.recipient, rcp_len);
                strncpy(buf + 2 + uname_len + rcp_len, p->body.pm.message, msg_len);
            }
            break;
        case OP_ERROR:
            {
                size_t uname_len = strlen(p->username)+1;
                size_t emsg_len = strlen(p->body.error.message)+1;

                *length = 4 + uname_len + emsg_len;
                buf = malloc(4 + uname_len + emsg_len);
                *(uint16_t*)buf = htons(p->opcode);

                strncpy(buf + 2, p->username, uname_len);
                *(uint16_t*)(buf + 2 + uname_len) = htons(p->body.error.errorcode);
                strncpy(buf + 4 + uname_len, p->body.error.message, emsg_len);
            }
            break;
    }
    return buf;
}

struct chat_packet * cunpack(const char *buf, size_t len){
    struct chat_packet *p = malloc(sizeof(struct chat_packet));
    p->opcode = ntohs(*(uint16_t *) buf);
    size_t uname_len = strlen(buf+2) + 1;
    p->username = calloc(sizeof(char), uname_len);
    strcpy(p->username, buf+2);
    switch(p->opcode){
        case OP_HELLO:
            {

            }
        case OP_CMSG:
            {
                size_t msg_len = strlen(buf + 2 + uname_len) + 1;
                p->body.cm.message = calloc(msg_len, sizeof(char));
                strcpy(p->body.cm.message, buf + 2 + uname_len);
            }
            break;
        case OP_PMSG:
            {
                size_t rcp_len = strlen(buf + 2 + uname_len) + 1;

                p->body.pm.recipient = calloc(rcp_len, sizeof(char));
                strcpy(p->body.pm.recipient, buf + 2 + uname_len);

                size_t msg_len = strlen(buf + 2 + uname_len + rcp_len) + 1;

                p->body.pm.message = calloc(msg_len, sizeof(char));
                strcpy(p->body.pm.message, buf + 2 + uname_len + rcp_len);
            }
            break;
        case OP_ERROR:
            {
                p->body.error.errorcode = ntohs(*(uint16_t*)( buf + 2 + uname_len));

                size_t msg_len = strlen(buf + 4 + uname_len) + 1;
                p->body.error.message = calloc(msg_len, 0);
                strcpy(p->body.error.message, buf + 4 + uname_len);

            }
            break;
    }

    return p;
}

