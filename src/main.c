#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    //print_options(&options);
    if (nb_options == -1)
        return 1;

    FILE *torrent = fopen(options.data, "r");
    if (torrent == NULL)
    {
        printf("my-bittorrent: Cannot open %s\n", options.data);
        return 1;
    }
    char *buf = malloc(1024);
    int i = 0;
    int c;
    while ((c = fgetc(torrent)) != EOF)
    {
        buf[i] = c;
        i++;
    }
    buf[i] = '\0';
    //printf("%s\n", buf);
    struct be_node *node = be_decode(buf, strlen(buf));
    if (options.d)
        contact(node);
    printf("node 0 key length: %llu\n", node->element.dict[0]->key->length);
    printf("node 0 key content: %s\n", node->element.dict[0]->key->content);
    printf("node 0 val length: %llu\n", node->element.dict[0]->val->element.str->length);
    printf("node 0 val content: %s\n", node->element.dict[0]->val->element.str->content);
    printf("node 1 key length: %llu\n", node->element.dict[1]->key->length);
    printf("node 1 key content: %s\n", node->element.dict[1]->key->content);
    printf("node 1 val length: %llu\n", node->element.dict[1]->val->element.str->length);
    printf("node 1 val content: %s\n", node->element.dict[1]->val->element.str->content);
    fclose (torrent);
    return 0;
}
