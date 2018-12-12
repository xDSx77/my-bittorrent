#ifndef CONTACT_TRACKER_
#define CONTACT_TRACKER_

#include "bencode/bencode.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stddef.h>


size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata);
int contact(struct be_node *node);

#endif  /* ! CONTACT_TRACKER_ */
