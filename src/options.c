#include "options.h"

void init_options(struct options *options)
{
    options->v = false;
    options->d = false;
    options->m = false;
    options->c = false;
    options->p = false;
    options->s = false;
    options->activate = 0;
    options->end_index = 0;
    options->data = NULL;
}

void print_options(struct options *options)
{
    printf("option v: %d\n", options->v);
    printf("option d: %d\n", options->d);
    printf("option m: %d\n", options->m);
    printf("option c: %d\n", options->c);
    printf("option p: %d\n", options->p);
    printf("option s: %d\n", options->s);
    printf("number of options activated: %d\n", options->activate);
    printf("index of the last option: %d\n", options->end_index);
    printf("data: %s\n", options->data);
}

static int check_error(int argc, char **argv, struct options *options, int i)
{
    if (options->activate > 3)
    {
        printf("my-bittorrent: too many arguments\n");
        return -1;
    }
    if (options->activate == 3)
    {
        if (options->d == false || options->v == false)
        {
            printf("my-bittorrent: too many arguments\n");
            return -1;
        }
        return 0;
    }
    if (options->activate == 2)
    {
        if (options->d == false || options->v == false)
        {
            printf("my-bittorrent: '%s' and '%s': incompatible options\n",
                argv[1], argv[2]);
            return -1;
        }
    }
    if (i + 1 >= argc)
    {
        printf("my-bittorrent: %s: option requires an argument\n", argv[i]);
        return -1;
    }
    return 0;
}

static int fill_options(int argc, char **argv, struct options *options, int *i,
    bool *option)
{
    *option = true;
    options->activate++;
    if (check_error(argc, argv, options, *i) == -1)
        return -1;
    if (strncmp(argv[*i + 1], "-", 1) == 0)
        return 0;
    options->data = argv[++*i];
    return 0;
}

static int parse_long(int argc, char **argv, struct options *options, int i)
{
    while (i < argc && strncmp("--", argv[i], 2) == 0)
    {
        options->end_index = i;
        if (strcmp("--pretty-print-torrent-file", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->p) == -1)
                return -1;
        }
        else if (strcmp("--mktorrent", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->m) == -1)
                return -1;
        }
        else if (strcmp("--check-integrity", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->c) == -1)
                return -1;
        }
        else if (strcmp("--dump-peers", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->d) == -1)
                return -1;
        }
        else if (strcmp("--verbose", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->v) == -1)
                return -1;
        }
        else if (strcmp("--seed", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->s) == -1)
                return -1;
        }
        else
        {
            printf("my-bittorrent: %s: unknown long option\n", argv[i]);
            return -1;
        }
        i++;
    }
    return i;
}

static int parse_short(int argc, char **argv, struct options *options, int i)
{
    while (i < argc && strncmp("-", argv[i], 1) == 0)
    {
        options->end_index = i;
        if (strcmp("-p", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->p) == -1)
                return -1;
        }
        else if (strcmp("-m", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->m) == -1)
                return -1;
        }
        else if (strcmp("-c", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->c) == -1)
                return -1;
        }
        else if (strcmp("-d", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->d) == -1)
                return -1;
        }
        else if (strcmp("-v", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->v) == -1)
                return -1;
        }
        else if (strcmp("-s", argv[i]) == 0)
        {
            if (fill_options(argc, argv, options, &i, &options->s) == -1)
                return -1;
        }
        else
        {
            printf("my-bittorrent: %s: unknown short option\n", argv[i]);
            return -1;
        }
        i++;
    }
    return i;
}

int parse_options(int argc, char **argv, struct options *options)
{
    if (argc == 1)
        return 0;

    int i = 1;

    i = parse_long(argc, argv, options, i);
    if (i == -1)
        return -1;

    i = parse_short(argc, argv, options, i);
    if (i == -1)
        return -1;

    return 0;
}
