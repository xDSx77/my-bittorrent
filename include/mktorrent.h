#ifndef MKTORRENT_H
#define MKTORRENT_H

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>
#include <openssl/sha.h>
#include <dirent.h>

void init_stat(struct stat *statbuf);
int mktorrent(char *path);

#endif /* ! MKTORRENT_H */
