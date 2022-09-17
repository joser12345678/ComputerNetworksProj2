#include "ssl_socket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//opens a tcp client connection and returns an int with the result
// -1 if socket was created incorrectly
// 0 if no error occured
int open_clientside_tcp_connection(int* tcp_socket, char* port_string, char* url)
{
    *tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket< 0)
    {
        return -1;
    }

    //target is used to hold the target, so the actual ip and port we are targeting
    struct addrinfo* target;

    //hints to help getaddrinfo resolve hostname
    //gets an ip4 address, a tcp socket
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //perform hostname resolution for the url
    int status;
    if ((status = getaddrinfo(url, port_string, &hints, &target)) != 0)
    {
        printf("Error resolving hostname: %s\n", gai_strerror(status));
        return -1;
    }

    //now that the hostname resolution works we need to parse the linked list
    void *addr;

    struct sockaddr_in *ipv4 = (struct sockaddr_in *) target->ai_addr;
    addr = &(ipv4->sin_addr);

    //print ip string
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(target->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s", ipstr);

    connect(*(tcp_socket), (struct sockaddr*)&target, sizeof(target));

    freeaddrinfo(target);
    
    return 0;

}

//initialize the ssl library, error strings, and algorithms.
//Not sure if error strings is needed but is included.
void init_ssl_ctx()
{
    SSL_library_init();
    SSL_load_error_strings();   //MAY NOT BE NECESSARY
    OpenSSL_add_all_algorithms();
}