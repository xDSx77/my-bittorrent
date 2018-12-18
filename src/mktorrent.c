#define _XOPEN_SOURCE 500
#include "mktorrent.h"
#include "contact_tracker.h"

time_t *date(void)
{
    time_t *t = malloc(sizeof(time_t));
    time(t);
    return t;
}

int mktorrent(char *path)
{
    char *file = strdup(path);
    FILE *p = fopen(path, "r");
    if (!p)
    {
        free(file);
        errx(1, "Error: '%s': No such file or directory", path);
    }

    int fd = fileno(p);
    int length = lseek(fd, 0, SEEK_END);
    char *buf = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
    if (!buf)
    {
        fclose(p);
        free(file);
        errx(1, "Error: Cannot map '%s' in memory", path);
    }

    strcat(path, ".torrent");
    if (access(path, F_OK) != -1)
    {
        fclose(p);
        free(file);
        errx(1, "Error creating '%s': File exists", path);
    }

    FILE *torrent = fopen(path, "a+");
    if (!torrent)
    {
        fclose(p);
        free(file);
        errx(1, "Error: Cannot create '%s'", path);
    }

    fprintf(torrent, "%s", "d8:announce30:http://localhost:6969/announce");
    fprintf(torrent, "%s", "10:created by12:allan.cantin");
    time_t *d = date();
    fprintf(torrent, "%s%ld%s", "13:creation datei", *d, "e");
    fprintf(torrent, "%s%ld%s", "4:infod6:lengthi", strlen(buf), "e");
    fprintf(torrent, "%s%ld%s%s", "4:name", strlen(file), ":", file);
    fprintf(torrent, "%s", "12:piece lengthi262144e");
    unsigned char *sha1 = malloc(20 * sizeof(char));
    condensat(buf, strlen(buf), sha1);
    fprintf(torrent, "%s%.*s%s", "6:pieces20:", 20, sha1, "ee");
    fclose(p);
    fclose(torrent);
    free(sha1);
    free(d);
    free(file);
    return 0;
}
