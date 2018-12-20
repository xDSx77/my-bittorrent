#define _POSIX_C_SOURCE 200809L
#include "mktorrent.h"
#include "contact_tracker.h"
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
    time_t *t = malloc(sizeof(time_t));
    time(t);
    return t;
}

static void error(FILE *file, DIR *dir, char *path2, enum err err, char *path)
{
    if (file)
        fclose(file);
    if (dir)
        closedir(dir);
    if (err == NO_FILE)
        errx(1, "Error: '%s': No such file or directory", path);
    if (err == CANT_MAP)
        errx(1, "Error: Cannot map '%s' in memory", path);
    if (err == FILE_EXISTS)
        errx(1, "Error creating '%s': File exists", path);
    if (err == CANT_CREATE)
        errx(1, "Error: Cannot create '%s'", path);
    if (err == EMPTY_DIR)
        errx(1, "Error: '%s': Empty directory", path);
    free(path2);
}

static void free_all(FILE *file, FILE *torrent, unsigned char *sha1, time_t *d,
    char *path2)
{
    if (file)
        fclose(file);
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
    *sha1 = malloc(20);
    if (!*sha1)
        return;
    condensat(buf, strlen(buf), *sha1);
    fprintf(torrent, "6:pieces20:%.*see", 20, *sha1);
}

static void handle_file(char *path, char *path2, FILE *file, DIR *dir)
{
    time_t *d = NULL;
    unsigned char *sha1 = NULL;
    size_t len;

    file = fopen(path, "r");
    if (!file)
        error(file, dir, path2, NO_FILE, path);

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
        error(file, dir, path2, FILE_EXISTS, path);

    char *buf = map(file, &len);
    if (!buf)
        error(file, dir, path2, CANT_MAP, path2);

    FILE *torrent = fopen(path, "a+");
    if (!torrent)
        error(file, dir, path2, CANT_CREATE, path);

    print_single_file(torrent, buf, path2, &d, &sha1);
    free_all(file, torrent, sha1, d, path2);
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
    *sha1 = malloc(20);
    condensat(bigbuf, strlen(bigbuf), *sha1);
    fprintf(torrent, "6:pieces20:%.*see", 20, *sha1);
}

static void handle_dir(char *path, char *path2, FILE *file, DIR *dir)
{
    int nb_file = 0;
    char *bigbuf = calloc(50000, 1);
    time_t *d = date();
    unsigned char *sha1 = NULL;
    size_t len;
    struct dirent **dirent;

    dir = opendir(path2);
    if (!dir)
        error(file, dir, path2, NO_FILE, path);

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
        error(file, dir, path2, FILE_EXISTS, path);

    FILE *torrent = fopen(path, "a+");
    if (!torrent)
        error(file, dir, path2, CANT_CREATE, path);

    print_dir_first(torrent, d);
    chdir(path2);
    for (int i = 2; i < scandir(".", &dirent, NULL, alphasort); i++)
    {
        nb_file++;
        file = fopen(dirent[i]->d_name, "r");
        if (!file)
            error(file, dir, path2, NO_FILE, dirent[i]->d_name);

        char *buf = map(file, &len);
        strncat(bigbuf, buf, len);
        if (!buf)
            error(file, dir, path2, CANT_MAP, dirent[i]->d_name);

        print_dir_second(torrent, buf, dirent[i]);
        fclose(file);
    }
    if (nb_file == 0)
        error(file, dir, path2, EMPTY_DIR, path);
    print_dir_third(torrent, path, bigbuf, &sha1);
    closedir(dir);
    free(bigbuf);
    free_all(NULL, torrent, sha1, d, path2);
}

int mktorrent(char *path)
{
    char *path2 = strdup(path);
    struct stat statbuf;
    stat(path, &statbuf);
    FILE *file = NULL;
    DIR *dir = NULL;

    if (S_ISREG(statbuf.st_mode))
        handle_file(path, path2, file, dir);

    if (S_ISDIR(statbuf.st_mode))
        handle_dir(path, path2, file, dir);

    return 0;
}
