#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "proc.h"

#include "socket.h"

int
sys_socket(void)
{
  int fd, domain, type, protocol;
  struct file *f;

  argint(0, &domain);
  argint(1, &type);
  argint(2, &protocol);
  if ((f = socket_alloc(domain, type, protocol)) == 0 || (fd = fdalloc(f)) < 0){
    if (f)
      fileclose(f);
    return -1;
  }
  return fd;
}

int
sys_bind(void)
{
  struct file *f;
  uint64 addr_p;
  struct sockaddr_in addr;
  int addrlen;
  struct proc *p = myproc();

  if (argfd(0, 0, &f) < 0 || f->type != FD_SOCKET){
    return -1;
  }
  argaddr(1, &addr_p);
  argint(2, &addrlen);
  if (!addr_p || addrlen < 0) {
    return -1;
  }
  if (copyin(p->pagetable, (char *)&addr, addr_p, addrlen) < 0) {
    return -1;
  }
  return socket_bind(f->socket, (struct sockaddr *)&addr, addrlen);
}

int
sys_recvfrom(void)
{
  struct file *f;
  uint64 buf_p;
  char buf[2048];
  int buflen;
  uint64 addr_p;
  struct sockaddr_in addr;
  uint64 addrlen_p;
  int addrlen = 0;
  int ret;
  struct proc *p = myproc();

  if (argfd(0, 0, &f) < 0 || f->type != FD_SOCKET){
    return -1;
  }
  argaddr(1, &buf_p);
  argint(2, &buflen);
  if (!buf_p || buflen < 0 || buflen > sizeof(buf)) {
    return -1;
  }
  argaddr(3, (void*)&addr_p);
  argaddr(4, (void*)&addrlen_p);
  if (addrlen_p) {
    if (copyin(p->pagetable, (char *)&addrlen, addrlen_p, sizeof(addrlen)) < 0) {
      return -1;
    }
    if (addrlen && addrlen != sizeof(addr)) {
      return -1;
    }
  }
  if (addrlen && !addr_p) {
    return -1;
  }
  ret = socket_recvfrom(f->socket, buf, buflen, (struct sockaddr *)&addr, &addrlen);
  if (copyout(p->pagetable, buf_p, buf, buflen) < 0) {
    return -1;
  }
  if (addr_p) {
    if (copyout(p->pagetable, addr_p, (char *)&addr, addrlen) < 0) {
      return -1;
    }
    if (copyout(p->pagetable, addrlen_p, (char *)&addrlen, sizeof(addrlen)) < 0) {
      return -1;
    }
  }
  return ret;
}

int
sys_sendto(void)
{
  struct file *f;
  uint64 buf_p;
  char buf[2048];
  int buflen;
  uint64 addr_p;
  struct sockaddr_in addr;
  int addrlen;
  struct proc *p = myproc();

  if (argfd(0, 0, &f) < 0 || f->type != FD_SOCKET){
    return -1;
  }
  argaddr(1, &buf_p);
  argint(2, &buflen);
  if (!buf_p || buflen < 0 || buflen > sizeof(buf)) {
    return -1;
  }
  if (copyin(p->pagetable, buf, buf_p, buflen) < 0) {
    return -1;
  }
  argaddr(3, &addr_p);
  argint(4, &addrlen);
  if (!addr_p || addrlen < 0) {
    return -1;
  }
  if (copyin(p->pagetable, (char *)&addr, addr_p, addrlen) < 0) {
    return -1;
  }
  return socket_sendto(f->socket, buf, buflen, (struct sockaddr *)&addr, addrlen);
}
