#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

enum
{
    PORT = 5000,
    BUF_LEN = 1024
};

//server
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char *argv[])
{
    //stores the servers destination IPV4 address and port
    struct sockaddr_in addr;

    //stores client source address
    struct sockaddr_storage from_addr;

    //holds byte size of from_addr_len
    socklen_t from_addr_len;

    //return status for bind()
    int result;

    //boolean flag
    int option;

    //socket file descriptor
    int fd;

    //1024 buffer for incoming message bytes
    char data[BUF_LEN];

    //stores signed number of bytes, -1 if an error occurs
    ssize_t nread;

    // checks if user supplied at least one argument (IP address for server to bind to)
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // creates an IPV4(AF_INET) connectionless UDP (SOCK_DGRAM) socket using default UDP protocol (0)
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    // if creation fails, exit program
    if(fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // zero out all bytes of addr so unused struct padding contains no leftover garbage memory
    memset(&addr, 0, sizeof(addr));

    // set address family to IPv4
    addr.sin_family = AF_INET;

    // Set listening port to htons() host to network short to convert the number to big-endian byte order
    addr.sin_port = htons(PORT);

    // checks if inet_addr() failed due to invalid IP string
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    //close and exit if invalid
    if(addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        close(fd);
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    // set value to option 1
    option = 1;

    // Tell the OS to allow immediate rebinding to port 5000, if server starts quickly to avoid "addr in use" errors
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // binds the IP address and port 5000 to the socket fd so the OS knows to deliver UDP packets arriving on port 5000
    result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    // check if binding failed
    if(result == -1)
    {
        close(fd);
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //Initialize from_addr_len with maximum capacity of from_addr so recvfrom() knows how much space is available
    // to write to the senders address
    from_addr_len = sizeof(from_addr);

    //waits until a UDP packet arrives on port 5000, copies up to BUF_LEN bytes into data
    //records the senders address in from_addr and returns the number of bytes received
    nread = recvfrom(fd, data, BUF_LEN, 0, (struct sockaddr *)&from_addr, &from_addr_len);

    //checks if receiving failed, close if so
    if(nread == -1)
    {
        close(fd);
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    // print the total number of bytes received
    printf("read %ld\n", nread);

    // writes the exact received payload bytes directly to the terminal
    write(STDOUT_FILENO, data, nread);

    //writes a newline character so the shell prompt appears cleanly on the next line.
    write(STDOUT_FILENO, "\n", 1);

    //close the UDP socket
    close(fd);

    return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop
