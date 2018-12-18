#ifndef CONTACT_TRACKER_
#define CONTACT_TRACKER_

#include "bencode/bencode.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <openssl/sha.h>

union cast_sock
{
    struct sockaddr *sock;
    struct sockaddr_in *sockin;
};

union  cast_str
{
    char *str;
    unsigned char *u_str;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata);
int contact(struct be_node *node, char *buf, int len_buf);
char *compact(struct be_node *node, char *str, char *buf, int len_buf);
unsigned char *condensat(char *str, size_t len_str, unsigned char *cond);
int get_socket(char *url);

#endif  /* !CONTACT_TRACKER_ */
