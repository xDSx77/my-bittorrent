#define _POSIX_C_SOURCE 201112L
#define _DEFAULT_SOURCE
#include "contact_tracker.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, char *userdata)
{
    if (!nmemb)
        return 0;

    struct be_node *node = be_decode(ptr, size * nmemb);
    char *peers;
    size_t len;

    for (int i = 0; node->element.dict; i++)
    {
        if (!strcmp(node->element.dict[i]->key->content, "peers"))
        {
            peers = node->element.dict[i]->val->element.str->content;
            len = node->element.dict[i]->val->element.str->length;
            break;
        }
    }

    dump_peers(peers, len);

    userdata = userdata;
    be_free(node);

    return size * nmemb;   //have to return the number of bytes written
}

void setopt_handle(CURL *handle, char *url, char *data, char *errbuf)
{
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, data);
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errbuf);
}

int contact(struct be_node *node, int len_buf)
{
    if (!node)
        return 1;

    CURL *handle = curl_easy_init();

    char *url = calloc(290,1);

    compact(node, url, len_buf);

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

    //function to exploit data

    free_curl(handle, url, data, errbuf);

    return 0;
}

char *compact(struct be_node *node, char *str, int len_buf)
{
    CURL *handle = curl_easy_init();
    memset(str, 0, 290);
    strcat(str, node->element.dict[0]->val->element.str->content);

    // --info_hash-- //
    char *buf_escaped = find_info(node, len_buf);
    char *sha1_esc = info_hash(str, buf_escaped, handle);

    // --peer_id-- //
    char *peer_id_esc = peer_id(node, str, handle);

    // --other--//
    strcat(str, "&port=1174");
    strcat(str, "&left=57");
    strcat(str, "&downloaded=0");
    strcat(str, "&uploaded=0");
    strcat(str, "&event=started");
    strcat(str, "&compact=1");

    free_curl(handle, buf_escaped, peer_id_esc, sha1_esc);

    return str;
}

char *peer_id(struct be_node *node, char *str, CURL *handle)
{
    strcat(str, "&peer_id=-MB2021-");
    char *peer_id = calloc(20,1);
    size_t len = node->element.dict[1]->val->element.str->length;
    memcpy(peer_id, node->element.dict[1]->val->element.str->content, len);

    if (strlen("-MB2021-") + strlen(peer_id) < 20)
    {
        size_t pad = 20 - (strlen("-MB2021-") + strlen(peer_id));
        for (; pad > 0; pad--)
            strcat(peer_id, "a");
    }
    else if (strlen("-MB2021-") + strlen(peer_id) > 20)
    {
        size_t pad = (strlen("-MB2021-") + strlen(peer_id)) - 20;
        peer_id[strlen(peer_id) - pad] = '\0';
    }

    char *peer_id_esc = curl_easy_escape(handle, peer_id, strlen(peer_id));
    strcat(str, peer_id_esc);
    free(peer_id);

    return peer_id_esc;
}

char *find_info(struct be_node *node, int len_buf)
{
    struct be_node *info;
    size_t len = len_buf;
    for (int i = 0; node->element.dict[i]; i++)
    {
        if (!strcmp(node->element.dict[i]->key->content, "info"))
        {
            info = node->element.dict[i]->val;
            break;
        }
    }

    char *info_str = be_encode(info, &len);

    return info_str;
}

char *info_hash(char *str, char *buf_escaped, CURL *handle)
{
    strcat(str, "?info_hash=");
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

void dump_peers(char *peers, size_t len_peers)
{
    char ip[50];
    union cast_char cast1;
    union cast_char cast2;
    union cast_char cast3;
    union cast_char cast4;
    union cast_uint16 cast_port;

    for (size_t i = 0; i < len_peers; i += 6)
    {
        cast1.char_ = peers[i];
        cast2.char_ = peers[i + 1];
        cast3.char_ = peers[i + 2];
        cast4.char_ = peers[i + 3];
        sprintf(ip, "%d.%d.%d.%d",
                cast1.uchar, cast2.uchar, cast3.uchar, cast4.uchar);
        cast_port.str = peers + i + 4;
        printf("%s:%d\n", ip, ntohs(*(cast_port.uint16)));
    }
}
