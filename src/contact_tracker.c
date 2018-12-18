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

int contact(struct be_node *node, char *path, char *buf, int len_buf)
{
    path = path;
    if (!node)
        return 1;

    CURL *handle = curl_easy_init();

    char *url = malloc(290);
    compact(node, url, buf, len_buf);
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
        free(url);
        curl_easy_cleanup(handle);
        curl_global_cleanup();
        return 1;
    }

    puts(errbuf);
    puts(data);

    curl_easy_cleanup(handle);
    curl_global_cleanup();
    free(url);

    return 0;
}

char *compact(struct be_node *node, char *str, char *buf, int len_buf)
{
    CURL *handle = curl_easy_init();
    memset(str, 0, 290);
    strcat(str, node->element.dict[0]->val->element.str->content);
    strcat(str, "?peer_id=-MB2021-");
    char *peer_id = node->element.dict[1]->val->element.str->content;
    char *peer_id_esc = curl_easy_escape(handle, peer_id, strlen(peer_id));
    strcat(str, peer_id_esc);
    if (strlen("-MB2021-") + strlen(peer_id_esc) != 20)
    {
        size_t pad = 20 - (strlen("-MB2021-") + strlen(peer_id_esc));
        for (; pad > 0; pad--)
            strcat(str, "a");
    }
    strcat(str, "&info_hash=");
    //make sha1 of info
    int i = 0;
    char *info = NULL;
    while (buf[i])
    {
        if (buf[i] == ':' && buf[i + 1] == 'i' && buf[i + 2] == 'n')
            if (buf[i + 3] == 'f' && buf[i + 4] == 'o' && buf[i + 5] == 'd')
            {
                i += 5;
                info = buf + i;
                break;
            }
        i++;
    }
    info[len_buf - i - 1] = '\0';   //delete the final e of the first dict
    char *buf_escaped = curl_easy_escape(handle, info, len_buf - i);

    unsigned char *sha1 = calloc(SHA_DIGEST_LENGTH, 1);
    union cast_str cast1;
    condensat(buf_escaped, len_buf - i, sha1);
    cast1.u_str = sha1;
    char *sha1_esc = curl_easy_escape(handle, cast1.str, SHA_DIGEST_LENGTH);
    strcat(str, sha1_esc);
    /*
    if (strlen(cast1.str) != 20)
    {
        for (size_t pad = strlen(cast1.str); pad > 0; pad--)
            strcat(str, "a");
    }
    */

    strcat(str, "&port=1174");
    strcat(str, "&left=0");
    strcat(str, "&downloaded=0");
    strcat(str, "&uploaded=0");
    strcat(str, "&compact=1");

    free(sha1);
    curl_free(buf_escaped);
    curl_free(peer_id_esc);
    curl_free(sha1_esc);
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    return str;
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

    union cast_sock cast;

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
