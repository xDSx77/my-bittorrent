#ifndef CHECK_INTEGRITY_H
#define CHECK_INTEGRITY_H

#define _POSIX_C_SOURCE 201112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include "bencode/bencode.h"
#include "dump_peers.h"

int check_integrity(struct be_node *node, char *path);

#endif /* ! CHECK_INTEGRITY_H */
