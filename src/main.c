#include <stdio.h>

int main(int argc, char **argv)
{
    argv = argv;
    if (argc == 1)
    {
        fprintf(stderr,
            "my-bittorrent: Usage: ./my-bittorrent [options] [files]\n");
        return 1;
    }
}
