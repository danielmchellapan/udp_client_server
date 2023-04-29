#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>


#define PORT 5000
#define BUF_LEN 1024

//client
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char *argv[])
{
    struct sockaddr_in addr;
    struct sockaddr_in to_addr;
    int result;
    int fd;
    char data[BUF_LEN];
    ssize_t nwrote;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if(fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(to_addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if(result == -1)
    {
        close(fd);
        perror("bind");
        exit(EXIT_FAILURE);
    }

    to_addr.sin_family = AF_INET;
    to_addr.sin_port = htons(PORT);
    to_addr.sin_addr.s_addr = inet_addr(argv[2]);

    if(to_addr.sin_addr.s_addr ==  (in_addr_t)-1)
    {
        perror("inet_addr");
        exit(EXIT_FAILURE);
    }

    nwrote = sendto(fd, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&to_addr, sizeof(to_addr));

    if(nwrote == -1)
    {
        close(fd);
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    printf("wrote %ld\n", nwrote);
    write(STDOUT_FILENO, data, nwrote);
    close(fd);

    return EXIT_SUCCESS;
}
#pragma GCC diagnostic pop
