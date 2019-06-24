#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpStream.h"

#include <thread>

#include <string.h>
#include <unistd.h>

// write_n writes "length" bytes from buf to the fd
// return bytes written
int write_n(int fd, const void* buf, int length) {
    int written = 0;
    // if write not done
    while (written < length) {
        int nw = ::write(fd, static_cast<const void*>(buf) + written, length - written);
        // reading not done
        if (nw  > 0) {
            written += nw;
        }
        // EOF
        else if (nw == 0) {
            break;
        }
        // errno is not EINTR
        else if (errno != EINTR) {
            perror("write");
            break;
        }
    }
    return written;
}