#include "platform.h"

#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "socket.h"

#include "udp.h"

struct socket {
    int type;
    int desc;
};

struct file*
socket_alloc(int domain, int type, int protocol)
{
    struct file *f;
    struct socket *s;

    if (domain != AF_INET || protocol != 0) {
        return NULL;
    }
    f = filealloc();
    if (!f) {
        return NULL;
    }
    s = (struct socket *)kalloc();
    if (!s) {
        fileclose(f);
        return NULL;
    }
    s->type = type;
    switch(type) {
    case SOCK_DGRAM:
        s->desc = udp_open();
        break;
    default:
        fileclose(f);
        memory_free(s);
        return NULL;
    }
    f->type = FD_SOCKET;
    f->readable = 1;
    f->writable = 1;
    f->socket = s;
    return f;
}

int
socket_close(struct socket *s)
{
    switch (s->type) {
    case SOCK_DGRAM:
        udp_close(s->desc);
    default:
        return -1;
    }
    memory_free(s);
    return 0;
}

int
socket_bind(struct socket *s, struct sockaddr *addr, int addrlen)
{
    struct ip_endpoint local;

    local.addr = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
    local.port = ((struct sockaddr_in *)addr)->sin_port;
    switch (s->type) {
    case SOCK_DGRAM:
        return udp_bind(s->desc, &local);
    default:
        return -1;
    }
}

int
socket_recvfrom(struct socket *s, char *buf, int n, struct sockaddr *addr, int *addrlen)
{
    struct ip_endpoint foreign;
    int ret;

    if (s->type != SOCK_DGRAM) {
        return -1;
    }
    ret = udp_recvfrom(s->desc, (uint8_t *)buf, n, &foreign);
    if (addr) {
        ((struct sockaddr_in *)addr)->sin_family = AF_INET;
        ((struct sockaddr_in *)addr)->sin_addr.s_addr = foreign.addr;
        ((struct sockaddr_in *)addr)->sin_port = foreign.port;
    }
    return ret;
}

int
socket_sendto(struct socket *s, char *buf, int n, struct sockaddr *addr, int addrlen)
{
    struct ip_endpoint foreign;

    if (s->type != SOCK_DGRAM) {
        return -1;
    }
    foreign.addr = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
    foreign.port = ((struct sockaddr_in *)addr)->sin_port;
    return udp_sendto(s->desc, (uint8_t *)buf, n, &foreign);
}
