#define _POSIX_C_SOURCE 201112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <jansson.h>
#include <sys/mman.h>
#include "pretty_print.h"
#include "bencode/bencode.h"
#include "options.h"
#include "contact_tracker.h"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr,
            "my-bittorrent: Usage: ./my-bittorrent [options] [files]\n");
        return 1;
    }
    struct options options;
    init_options(&options);
    int nb_options = parse_options(argc, argv, &options);
    if (nb_options == -1)
        return 1;

    FILE *torrent = fopen(options.data, "r");
    if (torrent == NULL)
    {
        printf("my-bittorrent: Cannot open %s\n", options.data);
        return 1;
    }
    int fd = fileno(torrent);
    int length = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    struct be_node *node = be_decode(buf, length);
    if (!node)
        return 1;

    if (options.d)
        contact(node);

    if (options.p)
        pretty_print(node);
    munmap(buf, length);
    fclose (torrent);
    return 0;
}
