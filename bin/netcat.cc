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
    int nr = 0;
    while ( (nr = ::read(STDIN_FILENO, buf, sizeof(buf))) > 0 ) {
        int nw = stream->sendAll(buf, nr);
        if (nw < nr) {
            break;
        }
    }
    stream->shutdownWrite();
    thr.join();
}

int main(int argc, const char* argv[]) 
{
    if (argc < 3) {
        printf("Usage:\n  %s hostname port\n  %s -l port\n", argv[0], argv[0]);
        return 0;
    }
    
    int port = atoi(argv[2]);
    // if server side mode
    if (strcmp(argv[1], "-l") == 0) {
        std::unique_ptr<Acceptor> acceptor(new Acceptor(InetAddress(port)));
        TcpStreamPtr stream(acceptor->accept());
        if (stream) {
            acceptor.reset();
            run(std::move(stream));
        }
        else {
            perror("accept connection error");
        }
    }
    // client side mode 
    else {
        const char* hostname = argv[1];
        InetAddress addr(port);
        // if successful resolve a inet address
        if (InetAddress::resolve(hostname, &addr)) {
            TcpStreamPtr stream(TcpStream::connect(addr));
            if (stream) {
                run(std::move(stream));
            }
            // fail to connect server side
            else {
                printf("Not able to connect %s\n", addr.toIpPort().c_str());
                perror("");
            }
        }
        // fail to resolve inet address
        else {
            printf("Not able to resolve Inet Address: %s\n", hostname);
        }
    }
    return 0;
}