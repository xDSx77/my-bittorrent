#define _POSIX_C_SOURCE 201112L
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

    return i;   //have to return the number of bytes written
}

int contact(struct be_node *node)
{
    if (!node)
        return 1;

    CURL *handle = curl_easy_init();

    char *url = malloc(290);
    memset(url, 0, 290);
    strcat(url, node->element.dict[0]->val->element.str->content);
    strcat(url, "?peer_id=-MB2021-");
    strcat(url, node->element.dict[1]->val->element.str->content);
    strcat(url, "&info_hash=");
    strcat(url, "&port=1174");
    strcat(url, "&left=");
    strcat(url, "&downloaded=");
    strcat(url, "&uploaded=");
    strcat(url, "&compact=1");
    puts(url);
    //get_socket(url);
    char data[50000] =
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

unsigned char *condensat(char *str, size_t len_str, unsigned char *cond)
{
    SHA_CTX c;
    SHA1_Init(&c);
    SHA1_Update(&c, str, len_str);
    SHA1_Final(cond, &c);

    return cond;
}


int get_socket(char *url)   //does not get the ip, but get the sockaddr struct
{//not url but ip in our case (maybe)
    struct addrinfo hints;
    struct addrinfo *rp;
    struct addrinfo *res;

    int status;
    char ip[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  //IPv4
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(url, NULL, &hints, &res);
    if (status)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    union cast cast;

    for (rp = res; rp; rp = rp->ai_next)
    {
        cast.sock = rp->ai_addr;
        void *addr = &(cast.sockin->sin_addr);
        inet_ntop(rp->ai_family, addr, ip, sizeof(ip));
        printf("IPv4: %s\n\n", ip);
    }

    freeaddrinfo(res);

    return 0;
}
