#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <stdio.h>
#include "bencode/bencode.h"

void print_list(struct be_node *node, int indent);
void print_dict(struct be_node *node, int indent);
void print_data(struct be_node *node, int indent);
void pretty_print(struct be_node *node);

#endif /* !PRETTY_PRINT_H */
