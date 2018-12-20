#define _XOPEN_SOURCE 500
#include "mktorrent.h"
#include "contact_tracker.h"

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

void handle_file(char *path, char *path2, FILE *file, DIR *dir)
{
    file = fopen(path, "r");
    if (!file)
        error(file, dir, path2, NO_FILE, path);

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
        error(file, dir, path2, FILE_EXISTS, path);

    int fd = fileno(file);
    size_t length = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    if (!buf)
        error(file, dir, path2, CANT_MAP, path2);

    FILE *torrent = fopen(path, "a+");
    if (!torrent)
        error(file, dir, path2, CANT_CREATE, path);

    fprintf(torrent, "d8:announce30:http://localhost:6969/announce");
    fprintf(torrent, "10:created by12:allan.cantin");
    time_t *d = date();
    fprintf(torrent, "13:creation datei%lde", *d);
    fprintf(torrent, "4:infod6:lengthi%lde", strlen(buf));
    fprintf(torrent, "4:name%ld:%s", strlen(path2), path2);
    fprintf(torrent, "12:piece lengthi262144e");
    unsigned char *sha1 = malloc(20);
    condensat(buf, strlen(buf), sha1);
    fprintf(torrent, "6:pieces20:%.*see", 20, sha1);
    free_all(file, torrent, sha1, d, path2);
}

int mktorrent(char *path)
{
    char *path2 = strdup(path);
    struct stat statbuf;
    stat(path, &statbuf);
    struct dirent *dirent = NULL;
    FILE *file = NULL;
    DIR *dir = NULL;

    if (S_ISREG(statbuf.st_mode))
        handle_file(path, path2, file, dir);

    if (S_ISDIR(statbuf.st_mode))
    {
        dir = opendir(path2);
        if (!dir)
            error(file, dir, path2, NO_FILE, path);
        int nb_file = 0;

        strcat(path, ".torrent");
        if (access(path, F_OK) != -1)
            error(file, dir, path2, FILE_EXISTS, path);

        FILE *torrent = fopen(path, "a+");
        if (!torrent)
            error(file, dir, path2, CANT_CREATE, path);

        fprintf(torrent, "d8:announce30:http://localhost:6969/announce");
        fprintf(torrent, "10:created by12:allan.cantin");
        time_t *d = date();
        fprintf(torrent, "13:creation datei%lde", *d);
        fprintf(torrent, "4:infod5:filesl");
        char *bigbuf = calloc(50000, 1);
        chdir(path2);
        while ((dirent = readdir(dir)) != NULL)
        {
            nb_file++;
            if (strcmp(dirent->d_name, ".") == 0 ||
                strcmp(dirent->d_name, "..") == 0)
                continue;
            file = fopen(dirent->d_name, "r");
            if (!file)
                error(file, dir, path2, NO_FILE, dirent->d_name);

            int f = fileno(file);
            size_t l = lseek(f, 0, SEEK_END);
            char *b = mmap(NULL, l, PROT_WRITE | PROT_READ, MAP_PRIVATE, f, 0);
            strncat(bigbuf, b, l);
            if (!b)
                error(file, dir, path2, CANT_MAP, dirent->d_name);

            fprintf(torrent, "d6:lengthi%lde", strlen(b));
            fprintf(torrent, "4:pathl%ld:%see", strlen(dirent->d_name), dirent->d_name);
            fclose(file);
        }
        if (nb_file == 2)
            error(file, dir, path2, EMPTY_DIR, path);
        fprintf(torrent, "e4:name%ld:%s", strlen(path), path);
        fprintf(torrent, "12:piece lengthi262144e");
        unsigned char *sha1 = malloc(20);
        condensat(bigbuf, strlen(bigbuf), sha1);
        fprintf(torrent, "6:pieces20:%.*see", 20, sha1);
        closedir(dir);
        free(bigbuf);
        free_all(NULL, torrent, sha1, d, path2);
    }

    return 0;
}
