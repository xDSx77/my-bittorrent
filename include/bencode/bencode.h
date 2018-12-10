#ifndef BENCODE_H
#define BENCODE_H

#include <stddef.h>

enum be_type
{
    BE_STR,
    BE_INT,
    BE_LIST,
    BE_DICT
};

struct be_string
{
    long long int length;
    char *content;
};

struct be_dict
{
    struct be_string *key;
    struct be_node *val;
};

struct be_node
{
    enum be_type type;
    union
    {
        long long int num;
        struct be_string *str;
        struct be_node **list; /* A `NULL` terminated array of `be_node`. */
        struct be_dict **dict; /* A `NULL` terminated array of `be_dict`. */
    } element;
};

/**
** @brief Decode a buffer and return it as a `be_node`.
**
** @param buf The buffer to decode.
** @param size The size of the buffer to decode.
**
** @return On succes, the `be_node` corresponding to `buf`, `NULL` on failure.
**         A failure can occur if the size is not correct or if the input is
**         ill-formed, in which case `errno` is set to `EINVAL`, or if an
**         allocation error occurs, in which case `errno` is set to `ENOMEM`.
*/
struct be_node *be_decode(char *buf, size_t size);

/**
** @brief Encode the content of `node` and return it in the allocated buffer.
**
** @param node The `be_node` to encode.
** @param size The size of the resulting buffer.
**
** @result The allocated buffer filled with the encoded node.
**         Return `NULL` and set `errno` to `ENOMEM` on allocation error.
*/
char *be_encode(struct be_node *node, size_t *size);

/**
** @brief Allocate and initialize a `be_node` of type `type`.
**
** @param type The type of the `be_node`.
**
** @return the initialized `be_node`, or NULL in case of error.
*/
struct be_node *be_alloc(enum be_type type);

/**
** @brief Free a `be_node`.
**
** @param node The `be_node` to free.
*/
void be_free(struct be_node *node);

#endif /* !BENCODE_H */
