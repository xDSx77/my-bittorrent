#include "check_integrity.h"
#include "mktorrent.h"
#include "useful.h"

static int cut_path(char *path, char *torrent)
{
    size_t l1 = strlen(path);
    size_t l2 = strlen(torrent);
    size_t i = 0;
    while (path[l1 - i] != '.' && i <= l2)
    {
        if (path[l1 - i] != torrent[l2 - i])
            return 1;
        path[l1 - i] = '\0';
        i++;
    }
    path[l1 - i] = '\0';
    return 0;
}

static unsigned char *create_sha(char *buf, union cast_str *cast)
{
    unsigned char *sha2 = calloc(20, 1);
    condensat(buf, strlen(buf), sha2);
    cast->u_str = sha2;
    return sha2;
}

static void free_all(DIR *dir, char *bigbuf, struct dirent **dirent, int n)
{
    closedir(dir);
    free(bigbuf);
    for (int j = 0; j < n; j++)
        free(dirent[j]);
    free(dirent);
}

static int handle_file(FILE *file, char *path, char *sha1, union cast_str *cast)
{
    size_t len;
    file  = fopen(path, "r");
    if (!file)
        return 1;
    char *buf = map(file, &len);
    unsigned char *sha2 = create_sha(buf, cast);
    if (strncmp(sha1, cast->str, strlen(sha1)))
    {
        free(sha2);
        fclose(file);
        return 1;
    }
    free(sha2);
    fclose(file);
    return 0;
}

static int handle_dir(FILE *file, char *path, char *sha1, union cast_str *cast)
{
    size_t len;
    struct dirent **dirent;
    char *buf = NULL;

    DIR *dir = opendir(path);
    if (!dir)
        return 1;

    char *bigbuf = calloc(50000, 1);
    chdir(path);
    int n = scandir(".", &dirent, NULL, alphasort);
    for (int i = 2; i < n; i++)
    {
        file = fopen(dirent[i]->d_name, "r");
        if (!file)
        {
            free_all(dir, bigbuf, dirent, n);
            return 1;
        }
        buf = map(file, &len);
        strncat(bigbuf, buf, len);
        fclose(file);
    }
    unsigned char *sha2 = create_sha(bigbuf, cast);
    free_all(dir, bigbuf, dirent, n);
    if (strncmp(sha1, cast->str, strlen(sha1)))
    {
        free(sha2);
        return 1;
    }
    free(sha2);
    return 0;
}

int check_integrity(struct be_node *node, char *path)
{
    char *torrent = ".torrent";
    if (strlen(path) <= strlen(torrent))
        return 1;

    if (cut_path(path, torrent))
        return 1;

    struct stat statbuf;
    init_stat(&statbuf);
    stat(path, &statbuf);
    if (statbuf.st_mode == 0)
        return 1;

    union cast_str cast;
    FILE *file = NULL;
    struct be_dict *dict2 = node->element.dict[3]->val->element.dict[3];
    char *sha1 = dict2->val->element.str->content;

    if (S_ISREG(statbuf.st_mode))
    {
        if (handle_file(file, path, sha1, &cast))
            return 1;
        else
            return 0;
    }

    if (S_ISDIR(statbuf.st_mode))
    {
        if (handle_dir(file, path, sha1, &cast))
            return 1;
        else
            return 0;
    }

    return 1;
}
