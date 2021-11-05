#include "Buffer.h"
#include <sys/uio.h>
#include <errno.h>
ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extraBuf[65535];
    struct iovec vec[2];
    const size_t writable = writeableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof extraBuf;

    const ssize_t n = readv(fd, vec, 2);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        writeIndex_ += n;
    }
    else
    {   // buffer_已经写满了，得更新指针
        writeIndex_ = buffer_.size();
        append(extraBuf,n - writeIndex_);
    }
    return n;
}