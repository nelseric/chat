#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <pthread.h>

#define CBUF_SIZE_DEFAULT 20;

struct chat_buffer{
	int bsize;
	int nmsg;
	char **msgbuf; 
	pthread_mutex_t lock;
} chat_buffer;

void cbuf_init(struct chat_buffer *cbuf);
void cbuf_destroy(struct chat_buffer *cbuf);
void cbuf_add(struct chat_buffer *cbuf, const char *msg);







#endif
