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

void run(TcpStreamPtr stream) {
    std::thread thr([&stream] () {
        // 8KB of data
        char buf[8192];
        // keep track of data read from socket
        int nr = 0;
        while ( (nr = stream->receiveSome(buf, sizeof(buf))) > 0 ) {
            // write to stdout
            int nw = write_n(STDOUT_FILENO, buf, nr);
            if (nw < nr) {
                break;
            }
        }
        ::exit(0);
    });
    char buf[8192];
}