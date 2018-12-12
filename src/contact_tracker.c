#include "contact_tracker.h"

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

    return i + 1;   //have to return the number of bytes written
}

int contact(struct be_node *node)
{
    if (!node)
        return 1;

    curl_global_init(CURL_GLOBAL_ALL);  //init to avoid problems
    CURL *handle = curl_easy_init();

    char *url = node->element.dict[0]->val->element.str->content;
    char *data = NULL;
    char errbuf[CURL_ERROR_SIZE];

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, &errbuf);

    if (curl_easy_perform(handle))
    {
        printf("ERROR PERFORMING HANDLE\n");
        return 1;
    }

    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return 0;
}
