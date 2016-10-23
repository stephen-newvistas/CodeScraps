#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int 
kpselect(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, const struct timespec *timeout,
                            const sigset_t *sigmask)
{
      return  pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
}

ssize_t 
kread(int fd, void *buf, size_t count)
{
  return read(fd, buf, count);
}
