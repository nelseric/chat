#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct {
	int socket;
	char *username;
} chat_user_t;

typedef struct {
	int num_clients;
	int size;
	chat_user_t * clients;
} client_store_t;

void cs_init(client_store_t * cstore);

void send_all(client_store_t *cs, const struct chat_packet *packet);

void clear_fds(client_store_t *cs, fd_set *fds);

int add_client(client_store_t * cs, int sock, const char *username);

chat_user_t * get_user_by_name(client_store_t *cs, const char *username);

void delete_client(client_store_t *cs, int sock);

void cs_free(client_store_t *cs);


#endif
