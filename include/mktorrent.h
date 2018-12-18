#ifndef MKTORRENT_H
#define MKTORRENT_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <err.h>
#include <openssl/sha.h>

int mktorrent(char *path);

#endif /* ! MKTORRENT_H */
