#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


enum
{
    PORT = 5000,
};
//client
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char *argv[])
{
    // Declares an IPV4 socket address structure to store the clients local IP and port
    struct sockaddr_in addr;

    // Declares an IPV4 socket address structure to store the servers destination IP and port
    struct sockaddr_in to_addr;

    //int to hold the return status code of bind()
    int result;

    //int value to hold the sockets file descriptor
    int fd;

    // Declares a signed integer to store the byte count returned by sendto()
    ssize_t nwrote;

    //Verifies that 3 user arguments were passed
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <client_ip> <server_ip> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Requests an IPv4 (AF_INET) connectionless UDP (SOCK_DGRAM) socket from the OS using default protocol (0)
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    // check if socket creation failed
    if(fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Fills the entire addr struct with 0s to clear any uninitialized memory
    memset(&addr, 0, sizeof(addr));

    // sets the client address to IPv4
    addr.sin_family = AF_INET;

    //Tells the OS to pick any free port for the client
    addr.sin_port = 0;

    //Converts the client IP string (argv[1]) to a 32-bit binary IPv4 address in network byte order
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    // checks to see if inet_addr failed on an invalid IP string
    if(addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        close(fd);
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    //binds the socket fd to the local IP address and OS assigned port specified in addr
    result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));


    //checks if binding failed
    if(result == -1)
    {
        close(fd);
        perror("bind");
        exit(EXIT_FAILURE);
    }
    // Zeroes out the destination to_addr structure
    memset(&to_addr, 0, sizeof(to_addr));
    // Sets the destination address family to IPv4
    to_addr.sin_family = AF_INET;
    //converts port 5000 from host byte order to big-endian network byte order
    to_addr.sin_port = htons(PORT);
    //Converts the target servers IP string to a 32-byte binary IPv4 address in network byte order
    to_addr.sin_addr.s_addr = inet_addr(argv[2]);

    //checks if the servers IP string is invalid
    if(to_addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        close(fd);
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    // Transmits the message string argv[3] of length strlen(argv[3]) bytes as a UDP datagram to to_addr
    // storing the number of bytes sent in nwrote
    nwrote = sendto(fd, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&to_addr, sizeof(to_addr));

    //checks if sendto failed
    if(nwrote == -1)
    {
        close(fd);
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    // Prints the number of bytes transmitted to standard output
    printf("wrote %ld\n", nwrote);
    // Writes the exact sent message bytes to standard output (STDOUT_FILENO)
    write(STDOUT_FILENO, argv[3], nwrote);
    //Writes a newline character to standard output
    write(STDOUT_FILENO, "\n", 1);
    close(fd);

    return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop
