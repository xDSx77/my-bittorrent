#include "check_integrity.h"

union cast_str
{
    char *str;
    unsigned char *u_str;
};

int check_integrity(struct be_node *node, char *path)
{
    struct be_dict *dict2 = node->element.dict[3]->val->element.dict[3];
    char *sha1 = dict2->val->element.str->content;
    char *torrent = ".torrent";
    size_t l1 = strlen(path);
    size_t l2 = strlen(torrent);
    if (l1 <= l2)
        return 1;
    size_t i = 1;
    while (path[l1 - i] != '.' && i <= l2)
    {
        if (path[l1 - i] != torrent[l2 - i])
            return 1;
        path[l1 - i] = '\0';
        i++;
    }
    path[l1 - i] = '\0';

    FILE *file  = fopen(path, "r");
    if (!file)
        return 1;
    int fd = fileno(file);
    int length = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    unsigned char *sha2 = malloc(20);
    condensat(buf, strlen(buf), sha2);
    union cast_str cast;
    cast.u_str = sha2;
    if (strncmp(sha1, cast.str, strlen(sha1)))
    {
        free(sha2);
        fclose(file);
        return 1;
    }
    free(sha2);
    fclose(file);
    return 0;
}
