#ifndef DUMP_PEERS_H_
#define DUMP_PEERS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "1174"

int get_socket(char *url);

#endif /* !DUMP_PEERS_H_ */
