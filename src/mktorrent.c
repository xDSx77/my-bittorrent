#define _POSIX_C_SOURCE 200809L
#include "mktorrent.h"
#include "dump_peers.h"
#include "useful.h"

enum err
{
    NO_FILE,
    CANT_MAP,
    FILE_EXISTS,
    CANT_CREATE,
    EMPTY_DIR
};

static time_t *date(void)
{
    time_t *t = calloc(20, sizeof(time_t));
    time(t);
    return t;
}

static void error(char *path, enum err err)
{
    if (err == NO_FILE)
        printf("my-bittorrent: Error: '%s': No such file or directory\n", path);
    if (err == CANT_MAP)
        printf("my-bittorrent: Error: Cannot map '%s' in memory\n", path);
    if (err == FILE_EXISTS)
        printf("my-bittorrent: Error creating '%s': File exists\n", path);
    if (err == CANT_CREATE)
        printf("my-bittorrent: Error: Cannot create '%s'\n", path);
    if (err == EMPTY_DIR)
        printf("my-bittorrent: Error: '%s': Empty directory\n", path);
}

static void free_all(FILE *file, FILE *torrent, unsigned char *sha1, time_t *d,
    char *path2)
{
    if (file)
        fclose(file);
    if (torrent)
        fclose(torrent);
    free(sha1);
    free(d);
    free(path2);
    return;
}

static void print_single_file(FILE *torrent, char *buf, char *path2, time_t **d,
    unsigned char **sha1)
{
    fprintf(torrent, "d8:announce30:http://localhost:6969/announce");
    fprintf(torrent, "10:created by12:allan.cantin");
    *d = date();
    fprintf(torrent, "13:creation datei%lde", **d);
    fprintf(torrent, "4:infod6:lengthi%lde", strlen(buf));
    fprintf(torrent, "4:name%ld:%s", strlen(path2), path2);
    fprintf(torrent, "12:piece lengthi262144e");
    *sha1 = calloc(20, 1);
    if (!*sha1)
        return;
    condensat(buf, strlen(buf), *sha1);
    fprintf(torrent, "6:pieces20:%.*see", 20, *sha1);
}

static int handle_file(char *path, FILE *file)
{
    char *path2 = strdup(path);
    time_t *d = NULL;
    unsigned char *sha1 = NULL;
    size_t len;
    FILE *torrent = NULL;

    file = fopen(path, "r");
    if (!file)
    {
        error(path2, NO_FILE);
        free_all(file, torrent, sha1, d, path2);
        return 1;
    }

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
    {
        error(path, FILE_EXISTS);
        free_all(file, torrent, sha1, d, path2);
        return 1;
    }

    char *buf = map(file, &len);
    if (!buf)
    {
        error(path2, CANT_MAP);
        free_all(file, torrent, sha1, d, path2);
        return 1;
    }

    torrent = fopen(path, "a+");
    if (!torrent)
    {
        error(path, CANT_CREATE);
        free_all(file, torrent, sha1, d, path2);
        return 1;
    }

    print_single_file(torrent, buf, path2, &d, &sha1);
    free_all(file, torrent, sha1, d, path2);
    return 0;
}

static void print_dir_first(FILE *torrent, time_t *d)
{
    fprintf(torrent, "d8:announce30:http://localhost:6969/announce");
    fprintf(torrent, "10:created by12:allan.cantin");
    fprintf(torrent, "13:creation datei%lde", *d);
    fprintf(torrent, "4:infod5:filesl");
}

static void print_dir_second(FILE *torrent, char *buf, struct dirent *dirent)
{
    fprintf(torrent, "d6:lengthi%lde", strlen(buf));
    fprintf(torrent, "4:pathl%ld:%see", strlen(dirent->d_name), dirent->d_name);
}

static void print_dir_third(FILE *torrent, char *path, char *bigbuf,
    unsigned char **sha1)
{
    fprintf(torrent, "e4:name%ld:%s", strlen(path), path);
    fprintf(torrent, "12:piece lengthi262144e");
    *sha1 = calloc(20, 1);
    condensat(bigbuf, strlen(bigbuf), *sha1);
    fprintf(torrent, "6:pieces20:%.*see", 20, *sha1);
}

static void free_all_2(FILE *torrent, unsigned char *sha1, time_t *d,
    char *path2, struct dirent **dirent, DIR *dir, int n, char *bigbuf)
{
    if (torrent)
        fclose(torrent);
    if (dir)
        closedir(dir);
    free(sha1);
    free(d);
    free(path2);
    free(bigbuf);
    for (int i = 0; i < n; i++)
        free(dirent[i]);
    free(dirent);
    return;
}

static int handle_dir(char *path, FILE *file, DIR *dir)
{
    char *path2 = strdup(path);
    char *bigbuf = calloc(50000, 1);
    time_t *d = date();
    unsigned char *sha1 = NULL;
    size_t len;
    struct dirent **dirent;
    FILE *torrent = NULL;
    int n = 0;

    dir = opendir(path2);
    if (!dir)
    {
        error(path2, NO_FILE);
        free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
        return 1;
    }

    n = scandir(path2, &dirent, NULL, alphasort);
    if (n == 2)
    {
        error(path2, EMPTY_DIR);
        free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
        return 1;
    }

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
    {
        error(path, FILE_EXISTS);
        free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
        return 1;
    }

    torrent = fopen(path, "a+");
    if (!torrent)
    {
        error(path, CANT_CREATE);
        free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
        return 1;
    }

    chdir(path2);
    print_dir_first(torrent, d);
    for (int i = 2; i < n; i++)
    {
        file = fopen(dirent[i]->d_name, "r");
        if (!file)
        {
            error(dirent[i]->d_name, NO_FILE);
            free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
            return 1;
        }
        char *buf = map(file, &len);
        if (!buf)
        {
            error(dirent[i]->d_name, CANT_MAP);
            free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
            return 1;
        }
        strncat(bigbuf, buf, len);
        print_dir_second(torrent, buf, dirent[i]);
        fclose(file);
    }
    print_dir_third(torrent, path, bigbuf, &sha1);
    free_all_2(torrent, sha1, d, path2, dirent, dir, n, bigbuf);
    return 0;
}

void init_stat(struct stat *statbuf)
{
    statbuf->st_dev = 0;
    statbuf->st_ino = 0;
    statbuf->st_mode = 0;
    statbuf->st_nlink = 0;
    statbuf->st_uid = 0;
    statbuf->st_gid = 0;
    statbuf->st_rdev = 0;
    statbuf->st_size = 0;
    statbuf->st_blksize = 0;
    statbuf->st_blocks = 0;
}

int mktorrent(char *path)
{
    struct stat statbuf;
    init_stat(&statbuf);
    stat(path, &statbuf);
    if (statbuf.st_mode == 0)
    {
        error(path, NO_FILE);
        return 1;
    }

    FILE *file = NULL;
    DIR *dir = NULL;

    if (S_ISREG(statbuf.st_mode))
    {
        if (handle_file(path, file))
            return 1;
        else
            return 0;
    }

    if (S_ISDIR(statbuf.st_mode))
    {
        if (handle_dir(path, file, dir))
            return 1;
        else
            return 0;
    }

    error(path, NO_FILE);
    return 1;
}
