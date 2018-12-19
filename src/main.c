#define _POSIX_C_SOURCE 201112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <jansson.h>
#include <sys/mman.h>
#include "pretty_print.h"
#include "bencode/bencode.h"
#include "options.h"
#include "contact_tracker.h"
#include "mktorrent.h"
#include "check_integrity.h"

int main(int argc, char **argv)
{
    if (argc == 1)
        errx(1, "Usage: ./my-bittorrent [options] [files]");

    struct options options;
    init_options(&options);
    int nb_options = parse_options(argc, argv, &options);
    if (nb_options == -1)
        return 1;

    if (options.m)
    {
        if (mktorrent(options.data) == 0)
            return 0;
        else
            return 1;
    }

    FILE *torrent = fopen(options.data, "r");
    if (torrent == NULL)
        errx(1, "Error: Cannot open '%s'", options.data);

    int fd = fileno(torrent);
    int length = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    if (!buf)
    {
        fclose(torrent);
        errx(1, "Error: Cannot map '%s' in memory", options.data);
    }
    
    struct be_node *node = be_decode(buf, length);
    if (!node)
    {
        fclose(torrent);
        errx(1, "Error: Buffer of '%s' is incorrect", options.data);
    }

    if (options.c)
    {
        if (check_integrity(node, options.data) == 0)
        {
            be_free(node);
            fclose(torrent);
            return 0;
        }
        else
        {
            be_free(node);
            fclose(torrent);
            return 1;
        }
    }

    if (options.d)
        contact(node);

    if (options.p)
        pretty_print(node);
    
    be_free(node);
    fclose(torrent);
    return 0;
}
