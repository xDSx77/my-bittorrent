#include "contact_tracker.h"
#include <stdlib.h>
#include <string.h>

size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata)
{
    size = size;
    if (!nmemb)
        return 0;
    //maybe check the if the len of userdata is sufficient

    size_t i = 0;

    for (; ptr[i]; i++)
        userdata[i] = ptr[i];
    userdata[i + 1] = '\0';

    return i;   //have to return the number of bytes written
}

int contact(struct be_node *node)
{
    if (!node)
        return 1;

    CURL *handle = curl_easy_init();

    char *url = node->element.dict[0]->val->element.str->content;
    char data[4096] =
    {
        0
    };
    char errbuf[CURL_ERROR_SIZE] =
    {
        0
    };

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, &errbuf);

    int c = curl_easy_perform(handle);
    if (c)
    {
        printf("CURL ERROR : %d\n", c);
        return 1;
    }

    puts(errbuf);
    puts(data);

    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return 0;
}
