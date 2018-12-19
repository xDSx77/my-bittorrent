#ifndef CONTACT_TRACKER_
#define CONTACT_TRACKER_

#include "bencode/bencode.h"
#include "dump_peers.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

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
char *peer_id(struct be_node *node, char *str, CURL *handle);
char *find_info(char *buf, int len_buf, CURL *handle);
char *info_hash(char *str, char *buf_escaped, CURL *handle);
void free_curl(CURL *handle, char *str1, char *str2, char *str3);
unsigned char *condensat(char *str, size_t len_str, unsigned char *cond);

#endif  /* !CONTACT_TRACKER_ */
