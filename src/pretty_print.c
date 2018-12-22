#include "pretty_print.h"

static void print_element(struct be_node *node, int *i, int index)
{
    int len = node->element.list[*i]->element.str->length;
    if (index == 1)
    {
        printf("\"%.*s\"", len, node->element.list[*i]->element.str->content);
        (*i)++;
    }
    else
        printf(", \"%.*s\"", len, node->element.list[*i]->element.str->content);
}

void print_list(struct be_node *node, int indent)
{
    printf("\n");
    int i = 0;
    printf("%*s%s", indent * 4, " ", "[");
    if (node->element.list[i]->type == BE_STR)
        print_element(node, &i, 1);
    while (node->element.list[i] != NULL)
    {
        if (node->element.list[i]->type == BE_STR)
            print_element(node, &i, 2);
        else if (node->element.list[i]->type == BE_DICT)
        {
            if (node->element.list[i + 1] == NULL)
                print_dict(node->element.list[i], 3, 0);
            else
                print_dict(node->element.list[i], 3, 1);
        }
        i++;
    }
    if (indent == 1)
        printf("            ]\n");
    else if (indent == 2)
        printf("        ],\n");
    else if (indent == 3)
        printf("]\n");
}

void print_dict(struct be_node *node, int indent, int comma)
{
    printf("%s%*s%s", "\n", indent * 4, " ", "{\n");
    if (indent == 1)
        print_data(node, 2);
    else if (indent == 3)
        print_data(node, 3);
    printf("%*s%s", indent * 4, " ", "}");
    if (indent == 3 && comma == 1)
        printf(",");
    printf("\n");
}

void print_string(struct be_node *node, int i)
{
    long long int len = node->element.dict[i]->val->element.str->length;
    for (long long int j = 0; j < len; j++)
    {
        if (node->element.dict[i]->val->element.str->content[j] >= 32
            && node->element.dict[i]->val->element.str->content[j] <= 126)
        {
            if (node->element.dict[i]->val->element.str->content[j] == 34
            || node->element.dict[i]->val->element.str->content[j] == 92)
                printf("\\%c",
                node->element.dict[i]->val->element.str->content[j]);
            else
                printf("%c",
                node->element.dict[i]->val->element.str->content[j]);
        }
        else
            printf("U+00%hhX",
            node->element.dict[i]->val->element.str->content[j]);
    }
    if (node->element.dict[i+1] == NULL)
        printf("\"\n");
    else
        printf("\",\n");
}

void print_data(struct be_node *node, int indent)
{
    if (node == NULL)
        return;
    int i = 0;
    while (node->element.dict[i] != NULL)
    {
        printf("%*s", indent * 4, " ");
        int len = node->element.dict[i]->key->length;
        printf("\"%.*s\": ", len, node->element.dict[i]->key->content);
        if (node->element.dict[i]->val->type == BE_INT)
            printf("%llu,\n", node->element.dict[i]->val->element.num);
        else if (node->element.dict[i]->val->type == BE_STR)
        {
            printf("\"");
            print_string(node, i);
        }
        else if (node->element.dict[i]->val->type == BE_LIST)
            print_list(node->element.dict[i]->val, indent);
        else if (node->element.dict[i]->val->type == BE_DICT)
            print_dict(node->element.dict[i]->val, indent, 0);
        i++;
    }
}

void pretty_print(struct be_node *node)
{
    printf("{\n");
    print_data(node, 1);
    printf("}\n");
}
