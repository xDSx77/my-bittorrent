#define _POSIX_C_SOURCE 201112L
#include "contact_tracker.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata)
{
    size = size;
    if (!nmemb)
        return 0;
    //maybe check the if the len of userdata is sufficient

    CURL *handle = curl_easy_init();
    char *ptr_esc = curl_easy_escape(handle,ptr,size * nmemb);
    size_t i = 0;

    for (; ptr_esc[i]; i++)
        userdata[i] = ptr_esc[i];
    userdata[i + 1] = '\0';

    free_curl(handle, NULL, NULL, NULL);
    return size * nmemb;   //have to return the number of bytes written
}

void setopt_handle(CURL *handle, char *url, char *data, char *errbuf)
{
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errbuf);
}

int contact(struct be_node *node, char *buf, int len_buf)
{
    if (!node)
        return 1;

    CURL *handle = curl_easy_init();

    char *url = calloc(290,1);
    if (!url)
    {
        printf("Malloc failed\n");
        return 1;
    }
    compact(node, url, buf, len_buf);
    puts(url);  //debug

    //get_socket(url);

    //manage error of calloc maybe//
    char *data = calloc(50000, 1);
    char *errbuf = calloc(CURL_ERROR_SIZE, 1);

    setopt_handle(handle, url, data, errbuf);

    int c = curl_easy_perform(handle);
    if (c)
    {
        printf("CURL ERROR : %d\n", c);
        free_curl(handle, url, data, errbuf);
        return 1;
    }

    puts(data);

    free_curl(handle, url, data, errbuf);

    return 0;
}

char *compact(struct be_node *node, char *str, char *buf, int len_buf)
{
    CURL *handle = curl_easy_init();
    memset(str, 0, 290);
    strcat(str, node->element.dict[0]->val->element.str->content);

    // --peer_id-- //
    char *peer_id_esc = peer_id(node, str, handle);

    // --info_hash-- //
    char *buf_escaped = find_info(buf, len_buf, handle);
    char *sha1_esc = info_hash(str, buf_escaped, handle);

    // --other--//
    strcat(str, "&port=1174");
    strcat(str, "&left=50");
    strcat(str, "&downloaded=0");
    strcat(str, "&uploaded=0");
    strcat(str, "&event=started");
    strcat(str, "&compact=1");

    free_curl(handle, buf_escaped, peer_id_esc, sha1_esc);

    return str;
}

char *peer_id(struct be_node *node, char *str, CURL *handle)
{
    strcat(str, "?peer_id=-MB2021-aaaaaaaaaaaa");
    node = node;
    str = str;
    handle = handle;
    /*
    char *peer_id = node->element.dict[1]->val->element.str->content;
    char *peer_id_esc = curl_easy_escape(handle, peer_id, strlen(peer_id));
    if (strlen("-MB2021-") + strlen(peer_id_esc) < 20)
    {
        size_t pad = 20 - (strlen("-MB2021-") + strlen(peer_id_esc));
        for (; pad > 0; pad--)
            strcat(peer_id_esc, "a");
    }
    else if (strlen("-MB2021-") + strlen(peer_id_esc) > 20)
    {
        size_t pad = (strlen("-MB2021-") + strlen(peer_id_esc)) - 20;
        peer_id_esc[strlen(peer_id_esc) - pad] = '\0';
    }
    strcat(str, peer_id_esc);
    */

    return NULL;//peer_id_esc;
}

char *find_info(char *buf, int len_buf, CURL *handle)
{
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

    return buf_escaped;
}

char *info_hash(char *str, char *buf_escaped, CURL *handle)
{
    strcat(str, "&info_hash=");
    unsigned char *sha1 = calloc(SHA_DIGEST_LENGTH, 1);
    union cast_str cast1;
    condensat(buf_escaped, strlen(buf_escaped), sha1);
    cast1.u_str = sha1;
    char *sha1_esc = curl_easy_escape(handle, cast1.str, SHA_DIGEST_LENGTH);
    strcat(str, sha1_esc);

    free(sha1);
    return sha1_esc;
}

void free_curl(CURL *handle, char *str1, char *str2, char *str3)
{
    curl_free(str1);
    curl_free(str2);
    curl_free(str3);
    curl_easy_cleanup(handle);
    curl_global_cleanup();
}

unsigned char *condensat(char *str, size_t len_str, unsigned char *cond)
{
    SHA_CTX c;
    SHA1_Init(&c);
    SHA1_Update(&c, str, len_str);
    SHA1_Final(cond, &c);

    return cond;
}
