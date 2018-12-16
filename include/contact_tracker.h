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

union cast
{
    struct sockaddr *sock;
    struct sockaddr_in *sockin;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata);
int contact(struct be_node *node);
int get_ip(char *url);

#endif  /* !CONTACT_TRACKER_ */
