
int kpselect(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, const struct timespec *timeout,
                            const sigset_t *sigmask);

ssize_t kread(int fd, void *buf, size_t count);
