#define _POSIX_C_SOURCE 201112L
#include "dump_peers.h"
#include "contact_tracker.h"

struct sockaddr *get_socket(char *url)
{
    struct addrinfo hints;
    struct addrinfo *res;

    int status;
    char ip[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  //IPv4
    hints.ai_socktype = SOCK_STREAM;

    url = url;

    status = getaddrinfo(NULL, PORT, &hints, &res);
    if (status)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        return NULL;
    }

    /*
    union cast_sock cast;

    for (rp = res; rp; rp = rp->ai_next)
    {
        cast.sock = rp->ai_addr;
        void *addr = &(cast.sockin->sin_addr);
        inet_ntop(rp->ai_family, addr, ip, sizeof(ip));
        printf("IPv4: %s\n\n", ip);
    }

    freeaddrinfo(res);
    */

    return res;
}

int check_socket(struct sockaddr *sock)
{
    struct addrinfo *rp;
    int sockfd;

    for (rp = sock; rp; rp = rp->ai_next)
    {
        if ((sockfd = socket(rp->ai_family, rp->socktype, rp->protocol) == -1))
        {
            continue;
        }
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
        close(sockfd);
    }

    freeaddrinfo(sock);
}
