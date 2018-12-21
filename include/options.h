#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

struct options
{
    bool v;
    bool d;
    bool m;
    bool c;
    bool p;
    bool s;
    bool h;
    int activate;
    int end_index;
    char *data;
};

void init_options(struct options *options);
void print_options(struct options *options);
int parse_options(int argc, char **argv, struct options *options);

#endif /* ! OPTIONS_H_ */
