#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)


int main(void) {
    printf("Configuring local address...\n");
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo* bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating socket...\n");
    SOCKET socket_listen = socket(  bind_address->ai_family, 
                                    bind_address->ai_socktype, 
                                    bind_address->ai_protocol);
    if(!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...");
    if ( listen(socket_listen, 10) < 0 ) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept( socket_listen, (struct sockaddr*)&client_address, &client_len );
    if( !ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Client is connected...\n");
    char address_buffer[100] = {0};
    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST); 
    printf("%s\n", address_buffer );

    printf("Reading request...\n");
    char request[1024] = {0};
    int bytes_received = recv(socket_client, request, sizeof(request), 0);
    printf("%.*s", bytes_received, request);

    printf("Sending response...\n");
    char response[1024] = {0};

    time_t timer;
    time(&timer);
    char* time_msg = ctime(&timer);

    int count = sprintf(response,
                        "HTTP/1.1 200 OK\r\n"
                        "Connection: close\r\n"
                        "Content-Type: text/plain\r\n\r\n"
                        "Local time is: ");
    count += sprintf( &response[count], "%s", time_msg);

    int bytes_sent = send(socket_client, response, count, 0 );
    printf("Sent %d of %d bytes.\n", bytes_sent, count );

    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("Finished...\n");

    return 0;
}
