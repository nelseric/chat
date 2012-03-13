#ifndef __CHAT_H__
#define __CHAT_H__

#include <stdint.h>
#include <stdlib.h>

#define CHAT_PORT 8899
#define CHAT_PORT_S "8899"

#define OP_HELLO    0 //Greet the server, so it can tell other people who you are
#define OP_CMSG     1 //Regular Message
#define OP_PMSG     2 //Private Message
#define OP_ERROR    3 //Error packet

struct chat_message {
    char * message;
};
struct chat_pm{
    char * recipient;
    char * message;   
};

struct chat_error{
    uint16_t errorcode;
    char * message;
};

struct chat_packet {
    uint16_t opcode;
    char *username;
    union {
        struct chat_message cm;
        struct chat_pm pm;
        struct chat_error error;
    } body;
};

char * cpack(const struct chat_packet *p, size_t *length);
struct chat_packet * cunpack(const char * buffer, size_t len);



#endif
