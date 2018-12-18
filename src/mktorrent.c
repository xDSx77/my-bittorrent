#include "mktorrent.h"

char *date(void)
{
    time_t *t = malloc(sizeof(time_t));
    time(t);
    struct tm *tm = localtime(t);
    char *date = calloc(14, sizeof(char));
    char *year = calloc(4, sizeof(char));
    char *month = calloc(2, sizeof(char));
    char *day = calloc(2, sizeof(char));
    char *hour = calloc(2, sizeof(char));
    char *min = calloc(2, sizeof(char));
    char *sec = calloc(2, sizeof(char));
    sprintf(year, "%d", tm->tm_year + 1900);
    sprintf(month, "%d", tm->tm_mon + 1);
    sprintf(day, "%d", tm->tm_mday);
    sprintf(hour, "%d", tm->tm_hour);
    sprintf(min, "%d", tm->tm_min);
    sprintf(sec, "%d", tm->tm_sec);
    strcat(date, year);
    strcat(date, month);
    strcat(date, day);
    strcat(date, hour);
    strcat(date, min);
    strcat(date, sec);
    free(t);
    return date;
}

int mktorrent(char *path)
{
    char *file = path;
    strcat(file, ".torrent");
    FILE *torrent = fopen(file, "a+");
    if (!torrent)
    {
        puts("mybittorrent: error when opening the file");
        return 1;
    }
    fprintf(torrent, "%s", "d8:announce30:http://localhost:6969/announce");
    fprintf(torrent, "%s", "7:comment6:coucou");
    fprintf(torrent, "%s", "10:created by12:Allan Cantin");
    fprintf(torrent, "%s%s%s", "13:creation datei", date(), "e");
    fprintf(torrent, "%s", "4:infod6:lengthi15e");
    fprintf(torrent, "%s%ld%s%s", "4:name", strlen(path), ":", path);
    fprintf(torrent, "%s", "12:piece lengthi123456e");
    fprintf(torrent, "%s", "6:pieces32:a&'5rnKT;PW^èOP8u$@{¤è+,gRZ:<7£²ee");
    fclose(torrent);
    return 0;
}
