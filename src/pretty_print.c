#include "pretty_print.h"

void print_list(struct be_node *node, int indent)
{
    printf("\n");
    int i = 0;
    if (indent == 1)
        printf("    [");
    else if (indent == 2)
        printf("        [");
    else if (indent == 3)
        printf("            [");
    if (node->element.list[i]->type == BE_STR)
    {
        printf("%.*s", (int)node->element.list[i]->element.str->length,
            node->element.list[i]->element.str->content);
        i++;
    }
    while (node->element.list[i] != NULL)
    {
        if (node->element.list[i]->type == BE_STR)
            printf(", %.*s", (int)node->element.list[i]->element.str->length,
                node->element.list[i]->element.str->content);
        else if (node->element.list[i]->type == BE_DICT)
            print_dict(node->element.list[i], 3);
        i++;
    }
    if (indent == 1)
        printf("            ]\n");
    else if (indent == 2)
        printf("        ]\n");
    else if (indent == 3)
        printf("]\n");

}

void print_dict(struct be_node *node, int indent)
{
    if (indent == 1)
        printf("\n    {\n");
    else if (indent == 2)
        printf("\n        {\n");
    else if (indent == 3)
        printf("\n            {\n");
    if (indent == 1)
        print_data(node, 2);
    else if (indent == 3)
        print_data(node, 3);
    if (indent == 1)
        printf("    }\n");
    else if (indent == 2)
        printf("        }\n");
    else if (indent == 3)
        printf("            }\n");
}

void print_data(struct be_node *node, int indent)
{
    if (node == NULL)
        return;
    int i = 0;
    while (node->element.dict[i] != NULL)
    {
        if (indent == 1)
            printf("    ");
        else if (indent == 2)
            printf("        ");
        else if (indent == 3)
            printf("            ");
        printf("\"%.*s\": ", (int)node->element.dict[i]->key->length,
            node->element.dict[i]->key->content);
        if (node->element.dict[i]->val->type == BE_INT)
            printf("%llu,\n", node->element.dict[i]->val->element.num);
        else if (node->element.dict[i]->val->type == BE_STR)
        {
            printf("\"");
            for (int j = 0;
                j < (int)node->element.dict[i]->val->element.str->length; j++)
            {
                if (node->element.dict[i]->val->element.str->content[j] >= 32
                    && node->element.dict[i]->val->element.str->content[j] <= 126)
                    printf("%c", node->element.dict[i]->val->element.str->content[j]);
                else
                    printf("U+00%x", node->element.dict[i]->val->element.str->content[j]);
            }
            printf("\"\n");
        }
        else if (node->element.dict[i]->val->type == BE_LIST)
            print_list(node->element.dict[i]->val, indent);
        else if (node->element.dict[i]->val->type == BE_DICT)
            print_dict(node->element.dict[i]->val, indent);
        i++;
    }
}

void pretty_print(struct be_node *node)
{
    printf("{\n");
    print_data(node, 1);
    printf("}\n");
}
