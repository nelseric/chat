#ifndef _SERVER_H_
#define _SERVER_H_

typedef struct {
	int socket;
	char *userid;
} chat_user_t;

typedef struct {
	int num_clients;
	int size;
	chat_user_t * clients;
} client_store_t;

void cs_init(client_store_t * cstore);

int add_client(client_store_t * cs, int sock, const char *userid);

chat_user_t * get_user_by_id(client_store_t *cs, const char *userid);

void cs_iterate(void (*action)(chat_user_t *));

void delete_client(client_store_t *cs, int sock);

void cs_free(client_store_t *cs);


#endif