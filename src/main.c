#include <stdio.h>
#include "options.h"

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
}
