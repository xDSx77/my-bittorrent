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
#include "useful.h"
#include "contact_tracker.h"
#include "mktorrent.h"
#include "check_integrity.h"

static struct options options;

static void free_all(struct be_node *node, FILE *torrent)
{
    be_free(node);
    fclose(torrent);
}

static int init_and_parse_options(int argc, char **argv)
{
    init_options(&options);
    if (parse_options(argc, argv, &options))
        return 1;
    return 0;
}

char *map(FILE *file, size_t *len)
{
    int fd = fileno(file);
    *len = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, *len, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    return buf;
}

static struct be_node *create_node(size_t *len, FILE **torrent, char **buf)
{
    *torrent = fopen(options.data, "r");
    if (*torrent == NULL)
        return NULL;

    *buf = map(*torrent, len);
    if (!buf)
    {
        fclose(*torrent);
        return NULL;
    }

    struct be_node *node = be_decode(*buf, *len);
    if (!node)
    {
        fclose(*torrent);
        return NULL;
    }
    return node;
}

static void help(void)
{
    printf("Usage:   ./my-bittorrent [options] [files]\n");
    printf("Options: -p|--pretty-print-torrent-file <.torrent file>\n");
    printf("         -m|--mktorrent <path>\n");
    printf("         -c|--check-integrity <.torrent file>\n");
    printf("         -d|--dump-peers <.torrent file>\n");
    printf("         -v|--verbose <.torrent file>\n");
    printf("         -s|--seed <.torrent file>\n");
    printf("         -h|--help\n");
}

int main(int argc, char **argv)
{
    if (argc == 1)
        errx(1, "Usage: ./my-bittorrent [options] [files]");

    if (init_and_parse_options(argc, argv))
        return 1;

    if (options.h)
        help();

    if (options.m)
    {
        if (mktorrent(options.data) == 0)
            return 0;
        else
            return 1;
    }

    size_t len;
    FILE *torrent = NULL;
    char *buf = NULL;
    struct be_node *node = NULL;
    if ((node = create_node(&len, &torrent, &buf)) == NULL)
        return 1;

    if (options.c)
    {
        if (check_integrity(node, options.data))
        {
            free_all(node, torrent);
            return 1;
        }
    }

    if (options.d)
        contact(node, buf, len);

    if (options.p)
        pretty_print(node);

    free_all(node, torrent);
    return 0;
}
